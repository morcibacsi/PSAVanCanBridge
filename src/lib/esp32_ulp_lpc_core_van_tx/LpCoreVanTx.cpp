#include "lib/esp32_ulp_lpc_core_van_tx/LpCoreVanTx.hpp"

#ifdef CONFIG_IDF_TARGET_ESP32C6
#include <ulp_lp_core.h>
#include "esp_clk_tree.h"
#include "esp_timer.h"
#include "soc/lp_aon_reg.h"
#include "soc/lp_io_reg.h"
#include "soc/soc.h"
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "VanEManchesterDecoder.hpp"
#include "VanFrameBuilder.hpp"
#include "ulp_main.h"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[] asm("_binary_ulp_main_bin_end");
static portMUX_TYPE vanApiLock = portMUX_INITIALIZER_UNLOCKED;

// LP RAM is uncached on C6. Aligned 32-bit publication plus hardware fences;
// volatile alone would not order the preceding frame/configuration writes.
static inline volatile uint32_t& SharedWord(uint32_t& word) { return word; }
static inline void SharedFence() { __asm__ __volatile__("fence rw,rw" ::: "memory"); }

LpCoreVanTx::LpCoreVanTx(gpio_num_t rxPin, gpio_num_t txPin, LP_VAN_NETWORK_SPEED networkSpeed)
{
    _rxPin = rxPin;
    _txPin = txPin;
    _networkSpeed = networkSpeed;

    esp_err_t err = rtc_gpio_init(_txPin);
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(rtc_gpio_set_level(_txPin, 1));
    err = rtc_gpio_set_direction(_txPin, RTC_GPIO_MODE_OUTPUT_ONLY);
    ESP_ERROR_CHECK(err);

    rtc_gpio_init(_rxPin);
    rtc_gpio_set_direction(_rxPin, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(_rxPin);
    rtc_gpio_pullup_dis(_rxPin);
}


LpCoreVanTx::~LpCoreVanTx() = default;

void LpCoreVanTx::Start()
{
    ESP_ERROR_CHECK(VanBusTiming::IsSupported(_networkSpeed) ? ESP_OK : ESP_ERR_INVALID_ARG);
    uint32_t lpClockHz = 0;
    ESP_ERROR_CHECK(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_RTC_FAST, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &lpClockHz));
    const uint32_t sliceCycles = VanBusTiming::TimeSliceCycles(lpClockHz, _networkSpeed);
    ESP_ERROR_CHECK(ulp_lp_core_load_binary(ulp_main_bin_start, ulp_main_bin_end - ulp_main_bin_start));
    ulp_lp_core_cfg_t cfg = { .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU };
    ESP_ERROR_CHECK(ulp_lp_core_run(&cfg));
    _configuration = {};
    volatile VanLpRxQueue* rxQueue = reinterpret_cast<volatile VanLpRxQueue*>(&ulp_VAN_RX_QUEUE);
    volatile uint32_t* rxWords = reinterpret_cast<volatile uint32_t*>(rxQueue);
    for (unsigned i = 0; i < sizeof(VanLpRxQueue) / 4; ++i) rxWords[i] = 0;
    SharedWord(ulp_SET_VAN_RX_PIN) = _rxPin;
    SharedWord(ulp_SET_VAN_TX_PIN) = _txPin;
    SharedWord(ulp_VAN_TS_CYCLES) = sliceCycles;
    printf("VAN LP clock: %u Hz, %u bps, slice=%u cycles\n",
           static_cast<unsigned>(lpClockHz),
           static_cast<unsigned>(VanBusTiming::BitRate(_networkSpeed)),
           static_cast<unsigned>(sliceCycles));
    // Query ACK defaults on; the legacy setter can explicitly disable it.
    _configuration.queryAckEnabled = 1;
    volatile VanLpConfig* banks = reinterpret_cast<volatile VanLpConfig*>(&ulp_VAN_CONFIG);
    const uint32_t* source = reinterpret_cast<const uint32_t*>(&_configuration);
    volatile uint32_t* target = reinterpret_cast<volatile uint32_t*>(&banks[0]);
    for (unsigned i = 0; i < sizeof(VanLpConfig) / 4; ++i) target[i] = source[i];
    SharedFence();
    SharedWord(ulp_VAN_START_APP) = 1;
    _started = true;
}

bool LpCoreVanTx::IsConfigurationReady() const
{
    return _started && SharedWord(ulp_VAN_CONFIG_PUBLISHED) == SharedWord(ulp_VAN_CONFIG_APPLIED);
}

void LpCoreVanTx::PublishConfiguration()
{
    // Caller holds vanApiLock and checked that the previous bank was applied.
    const uint32_t next = SharedWord(ulp_VAN_CONFIG_PUBLISHED) + 1;
    volatile VanLpConfig* banks = reinterpret_cast<volatile VanLpConfig*>(&ulp_VAN_CONFIG);
    volatile uint32_t* target = reinterpret_cast<volatile uint32_t*>(&banks[next & 1]);
    const uint32_t* source = reinterpret_cast<const uint32_t*>(&_configuration);
    SharedFence(); // Acquire LP acknowledgement before reusing the old bank.
    for (unsigned i = 0; i < sizeof(VanLpConfig) / 4; ++i) target[i] = source[i];
    SharedFence();
    SharedWord(ulp_VAN_CONFIG_PUBLISHED) = next;
}

bool LpCoreVanTx::ConfigureAckFrame(uint8_t slot, uint16_t identifier, bool enabled)
{
    portENTER_CRITICAL(&vanApiLock);
    const bool ok = IsConfigurationReady() && VanFrameBuilder::SetAck(_configuration, slot, identifier, 0xc, enabled);
    if (ok) PublishConfiguration();
    portEXIT_CRITICAL(&vanApiLock);
    return ok;
}

bool LpCoreVanTx::ConfigureReplyFrame(uint8_t slot, uint16_t identifier, const uint8_t* data, uint8_t length, bool enabled)
{
    portENTER_CRITICAL(&vanApiLock);
    const bool ok = IsConfigurationReady() && VanFrameBuilder::SetReply(_configuration, slot, identifier, data, length, enabled);
    if (ok) PublishConfiguration();
    portEXIT_CRITICAL(&vanApiLock);
    return ok;
}

// Compatibility setters are task-context configuration operations, never parse/
// generate fast paths. Bounded waiting leaves the LP monitoring continuously.
static bool WaitForConfiguration(LpCoreVanTx& tx)
{
    const TickType_t start = xTaskGetTickCount();
    while (!tx.IsConfigurationReady())
    {
        if (xTaskGetTickCount() - start >= pdMS_TO_TICKS(50)) return false;
        vTaskDelay(1);
    }
    return true;
}

void LpCoreVanTx::SetAckIdentifiers(const uint16_t* identifiers, uint8_t count)
{
    if (count > VAN_LP_ENTRY_COUNT || (count && !identifiers)) return;
    for (unsigned i = 0; i < count; ++i) if (identifiers[i] > 0xfff) return;
    if (!WaitForConfiguration(*this)) return;
    portENTER_CRITICAL(&vanApiLock);
    if (IsConfigurationReady())
    {
        for (unsigned i = 0; i < VAN_LP_ENTRY_COUNT; ++i)
            VanFrameBuilder::SetAck(_configuration, i, i < count ? identifiers[i] : 0, 0xc, i < count);
        PublishConfiguration();
    }
    portEXIT_CRITICAL(&vanApiLock);
}

void LpCoreVanTx::SetQueryRequesterAckEnabled(bool enabled)
{
    if (!WaitForConfiguration(*this)) return;
    portENTER_CRITICAL(&vanApiLock);
    if (IsConfigurationReady())
    {
        _configuration.queryAckEnabled = enabled;
        PublishConfiguration();
    }
    portEXIT_CRITICAL(&vanApiLock);
}

void LpCoreVanTx::SetRequestedReplyFrame(uint8_t slot, uint16_t identifier, const uint8_t* data, uint8_t length, bool enabled)
{
    if (slot >= VAN_LP_ENTRY_COUNT) return;
    if (WaitForConfiguration(*this)) ConfigureReplyFrame(slot, identifier, data, length, enabled);
}

bool LpCoreVanTx::TrySendFrame(uint16_t identifier, const uint8_t* data, uint8_t length, uint8_t command, bool query)
{
    if ((query && (command != 0xf || length != 0))
        || (!query && command != 0x8 && command != 0xc)) return false;
    VanLpFrame frame = {};
    if (!VanFrameBuilder::Build(identifier, command, data, length, frame)) return false;
    portENTER_CRITICAL(&vanApiLock);
    if (!_started || !IsTxPossible())
    {
        portEXIT_CRITICAL(&vanApiLock);
        return false;
    }
    SharedFence();
    volatile uint32_t* target = &ulp_VAN_DATA;
    for (unsigned i = 0; i < frame.frameWordCount; ++i) target[i] = frame.words[i];
    SharedWord(ulp_VAN_DATA_LENGTH) = frame.frameWordCount;
    SharedWord(ulp_VAN_FRAME_TYPE) = query ? 1 : 0;
    SharedWord(ulp_VAN_RETRY_COUNT) = 1;
    SharedWord(ulp_VAN_TX_FINISHED) = 0;
    SharedFence();
    SharedWord(ulp_VAN_START_TX) = 1;
    portEXIT_CRITICAL(&vanApiLock);
    return true;
}

void LpCoreVanTx::SendNormalFrame(uint16_t identifier, const uint8_t* data, uint8_t length, bool requireAck)
{
    TrySendFrame(identifier, data, length, requireAck ? 0xc : 0x8, false);
}

void LpCoreVanTx::SendReplyRequestFrame(uint16_t identifier)
{
    TrySendFrame(identifier, nullptr, 0, 0xf, true);
}

bool LpCoreVanTx::IsTxPossible() { return _started && SharedWord(ulp_VAN_TX_FINISHED) == 1; }
VanLpResult LpCoreVanTx::GetLastTxResult() const { return static_cast<VanLpResult>(SharedWord(ulp_VAN_TX_RESULT)); }
uint32_t LpCoreVanTx::GetLastTxAbortDetail() const { return SharedWord(ulp_VAN_TX_ABORT_DETAIL); }
bool LpCoreVanTx::GetLastTxArbitrationTrace(VanLpArbitrationTrace& trace) const
{
    if (SharedWord(ulp_VAN_TX_FINISHED) != 1 || SharedWord(ulp_VAN_FRAME_TYPE) != 0
        || SharedWord(ulp_VAN_TX_RESULT) != VAN_LP_ARBITRATION_LOST) return false;
    SharedFence();
    const volatile auto* source = reinterpret_cast<const volatile VanLpArbitrationTrace*>(&ulp_VAN_TX_ARBITRATION_TRACE);
    trace = {source->rawTs, source->sampleOffset, source->outputBeforeRelease,
             source->outputReadOffset, source->releaseOffset, source->rxAfterRelease,
             source->readOffset, source->outputAfterRelease, source->rxRecessiveOffset};
    return true;
}
void LpCoreVanTx::GetGpioState(VanLpGpioState& state) const
{
    state = {
        REG_READ(LP_IO_OUT_ENABLE_REG),
        REG_READ(LP_IO_OUT_DATA_REG),
        REG_READ(LP_IO_IN_REG),
        REG_READ(LP_AON_GPIO_MUX_REG),
        REG_READ(LP_IO_GPIO0_REG + static_cast<uint32_t>(_txPin) * sizeof(uint32_t)),
        REG_READ(LP_IO_GPIO0_REG + static_cast<uint32_t>(_rxPin) * sizeof(uint32_t))
    };
}
uint32_t LpCoreVanTx::GetLastTxEodTs() const { return SharedWord(ulp_VAN_TX_EOD_TS); }
bool LpCoreVanTx::GetLastTxRxTrace(uint8_t index, VanLpRxTrace& trace) const
{
    if (SharedWord(ulp_VAN_TX_FINISHED) != 1 || index >= VAN_LP_RX_TRACE_COUNT
        || index >= SharedWord(ulp_VAN_TX_RX_TRACE_COUNT)) return false;
    SharedFence();
    const volatile VanLpRxTrace* records = reinterpret_cast<const volatile VanLpRxTrace*>(&ulp_VAN_TX_RX_TRACE);
    trace = { records[index].rawTs, records[index].pair, records[index].centerOffset,
              records[index].correction, records[index].fourthLate, records[index].inverseLate };
    return true;
}
VanLpResult LpCoreVanTx::GetLastBusResult() const { return static_cast<VanLpResult>(SharedWord(ulp_VAN_BUS_RESULT)); }

void LpCoreVanTx::GetReceiveDiagnostics(VanLpRxDiagnostics& diagnostics) const
{
    const volatile VanLpRxQueue* queue = reinterpret_cast<const volatile VanLpRxQueue*>(&ulp_VAN_RX_QUEUE);
    SharedFence();
    diagnostics = {queue->receivedCount, queue->overflowCount,
                   queue->malformedCount, queue->maxOccupancy};
}

void LpCoreVanTx::ReceiveData(uint8_t* messageLength, uint8_t message[])
{
    *messageLength = 0;
    volatile VanLpRxQueue* queue = reinterpret_cast<volatile VanLpRxQueue*>(&ulp_VAN_RX_QUEUE);
    while (_started && queue->readIndex == queue->writeIndex) vTaskDelay(1);
    if (!_started) return;

    SharedFence();
    const uint32_t read = queue->readIndex;
    const volatile VanLpRxFrame* frame = &queue->frames[read % VAN_LP_RX_QUEUE_LENGTH];
    const uint32_t groupCount = frame->groupCount;
    if (groupCount > VAN_LP_RX_MAX_GROUPS)
    {
        SharedFence();
        queue->readIndex = read + 1;
        return;
    }
    uint8_t groups[VAN_LP_RX_MAX_GROUPS];
    for (uint32_t i = 0; i < groupCount; ++i) groups[i] = frame->groups[i];
    SharedFence();
    queue->readIndex = read + 1;
    VanEManchesterDecoder::Decode(groups, groupCount, message, messageLength);
}
#endif
