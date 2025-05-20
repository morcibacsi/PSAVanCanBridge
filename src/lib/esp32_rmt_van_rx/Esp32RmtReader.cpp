#include "Esp32RmtReader.h"
#include <string.h>
#include <rom/gpio.h>
#include "driver/gpio.h"

Esp32RmtReader::Esp32RmtReader(uint32_t minSignal, uint32_t maxSignal, uint8_t rxPin, int8_t ledPin)
{
    _minSignal = minSignal;
    _maxSignal = maxSignal;
    _rxPin = rxPin;
    _ledPin = ledPin;
    _messageLength = 0;
}

void Esp32RmtReader::InitLed()
{
    if (_ledPin > -1)
    {
        gpio_pad_select_gpio(_ledPin);
        gpio_set_direction((gpio_num_t)_ledPin, GPIO_MODE_OUTPUT);
    }
}

void Esp32RmtReader::SwitchLed(uint8_t state)
{
    if (_ledPin > -1)
    {
        gpio_set_level((gpio_num_t)_ledPin, state);
    }
}

bool Esp32RmtReader::RmtRxDoneCallback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata)
{
    BaseType_t high_task_wakeup = pdFALSE;

    size_t num_bytes = edata->num_symbols * sizeof(rmt_symbol_word_t);

    void* dest = nullptr;

    // Attempt to acquire space in the ring buffer
    if (xRingbufferSendAcquire(_ringbuf, &dest, num_bytes, 0) == pdTRUE && dest != nullptr) {
        memcpy(dest, edata->received_symbols, num_bytes);
        xRingbufferSendComplete(_ringbuf, dest);
    }

    // restart RX
    rmt_receive(_rx_channel, _raw_symbols, sizeof(_raw_symbols), &_receive_config);

    return high_task_wakeup == pdTRUE;
}

int32_t Esp32RmtReader::Start()
{
    InitLed();

    _receive_config.signal_range_min_ns = _minSignal;
    _receive_config.signal_range_max_ns = _maxSignal;
    _receive_config.flags.en_partial_rx = 0; // we don't need partial receive

    rmt_rx_channel_config_t rx_channel_cfg;
    memset(&rx_channel_cfg, 0, sizeof(rx_channel_cfg));
    rx_channel_cfg.clk_src = RMT_CLK_SRC_DEFAULT;
    rx_channel_cfg.resolution_hz = 1000000;// 1MHz resolution, 1 tick = 1us;
    rx_channel_cfg.mem_block_symbols = 96;
    rx_channel_cfg.gpio_num = (gpio_num_t)_rxPin;
    rx_channel_cfg.intr_priority = 0;
    rx_channel_cfg.flags.invert_in = 0;
    rx_channel_cfg.flags.with_dma = 0;
    rx_channel_cfg.flags.io_loop_back = 0;

    ESP_ERROR_CHECK(rmt_new_rx_channel(&rx_channel_cfg, &_rx_channel));

    ESP_ERROR_CHECK(rmt_enable(_rx_channel));

    rmt_rx_event_callbacks_t callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.on_recv_done = StaticRmtRxDoneCallback;

    ESP_ERROR_CHECK(rmt_rx_register_event_callbacks(_rx_channel, &callbacks, this));

    // start receiving data
    esp_err_t result = rmt_receive(_rx_channel, _raw_symbols, sizeof(_raw_symbols), &_receive_config);
    ESP_ERROR_CHECK(result);
    return result;
}

void Esp32RmtReader::Stop()
{
    if (_rx_channel)
    {
        ESP_ERROR_CHECK(rmt_disable(_rx_channel));
        ESP_ERROR_CHECK(rmt_del_channel(_rx_channel));
        _rx_channel = nullptr;
    }

    if (_receive_queue)
    {
        RmtSymbolBuffer buffer;
        while (xQueueReceive(_receive_queue, &buffer, 0) == pdPASS)
        {
            free(buffer.symbols);
        }
        vQueueDelete(_receive_queue);
        _receive_queue = nullptr;
    }

    if (_ringbuf) {
        vRingbufferDelete(_ringbuf);
        _ringbuf = nullptr;
    }
}

void Esp32RmtReader::ReceiveData(uint8_t *messageLength, uint8_t message[])
{
    *messageLength = 0;
    _messageLength = 0;
    memset(_message, 0x00, RMT_READER_MAX_DATA * sizeof(*_message));

    size_t item_size;
    rmt_symbol_word_t* symbols = (rmt_symbol_word_t*) xRingbufferReceive(_ringbuf, &item_size, portMAX_DELAY);

    if (symbols != nullptr)
    {
        size_t num_symbols = item_size / sizeof(rmt_symbol_word_t);

        SwitchLed(1);
        BeforeProcessSignal();

        for (size_t i = 0; i < num_symbols; i++) {
            ProcessSignal(symbols[i].level0, symbols[i].duration0);
            ProcessSignal(symbols[i].level1, symbols[i].duration1);
        }

        *messageLength = _messageLength;
        memcpy(message, _message, _messageLength);

        AfterProcessSignal();
        SwitchLed(0);

        vRingbufferReturnItem(_ringbuf, (void*) symbols);
    }
}
