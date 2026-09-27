#ifndef UlpVanTx_h
#define UlpVanTx_h

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32

#include "driver/gpio.h"
#include "esp32/ulp.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "soc/rtc_periph.h"
#include <stdint.h>
#include <hulp_arduino.h>

static RTC_SLOW_ATTR ulp_var_t ulp_command[1 + 33];
enum {
    ULP_ACK_CONFIGURED_IDENTIFIER,
    ULP_ACK_LAST_SAMPLED_IDENTIFIER,
    ULP_ACK_LAST_SAMPLED_COM,
    ULP_ACK_LAST_EOD_CANDIDATE_GROUP,
    ULP_ACK_COUNT,
    ULP_ACK_STATE_WORD_COUNT
};
// One persistent configured identifier, receive diagnostics, and a 16-bit ACK
// completion counter. CPU/ULP accesses are asynchronous, so do not cache them.
static RTC_SLOW_ATTR volatile ulp_var_t ulp_ack_state[ULP_ACK_STATE_WORD_COUNT];

typedef enum {
    ULP_VAN_62K5BPS  = 0,
    ULP_VAN_125KBPS  = 1,
} ULP_VAN_NETWORK_SPEED;

class UlpVanTx
{
    private:
        gpio_num_t _rxPin;
        gpio_num_t _txPin;
        ULP_VAN_NETWORK_SPEED _networkSpeed;
        uint16_t _ulpProgramInstructionCount;

        uint16_t Crc15(const uint8_t data[], const uint8_t length);
        void InternalSendFrame(const uint8_t data[], const uint8_t length);
    public:
        UlpVanTx(gpio_num_t rxPin, gpio_num_t txPin, ULP_VAN_NETWORK_SPEED networkSpeed);
        ~UlpVanTx();
        void Start();
        bool SetAckIdentifier(uint16_t identifier, bool enabled);
        uint16_t GetConfiguredAckIdentifier() const;
        uint16_t GetAckCount() const;
        uint16_t GetLastSampledIdentifier() const;
        uint8_t GetLastSampledCom() const;
        uint8_t GetLastEodCandidateGroup() const;
        uint16_t GetUlpProgramInstructionCount() const;
        void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck);
        void SendReplyRequestFrame(const uint16_t identifier);
};

#endif
#endif
