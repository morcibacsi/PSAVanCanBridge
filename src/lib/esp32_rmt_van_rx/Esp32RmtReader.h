// Esp32RmtReader.h
#pragma once

#ifndef _Esp32RmtReader_h
    #define _Esp32RmtReader_h

#include <stdint.h>
#include "driver/rmt_rx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"

/*
    This is a base class around the RMT peripheral of the ESP32. Can't be used directly, you need to create a derived class
    and implemement the virtual methods.
*/

typedef struct {
    rmt_symbol_word_t* symbols;
    size_t num_symbols;
} RmtSymbolBuffer;

class Esp32RmtReader
{
    private:
        static bool IRAM_ATTR StaticRmtRxDoneCallback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
        {
            // Cast user_data back to the class instance
            auto *instance = static_cast<Esp32RmtReader*>(user_data);
            // Delegate the handling to the specific instance's member function
            return instance->RmtRxDoneCallback(channel, edata);
        }
        bool RmtRxDoneCallback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata);

    #define RMT_READER_MAX_DATA 255
    protected:
        size_t _num_symbols;
        uint32_t _minSignal;
        uint32_t _maxSignal;
        uint8_t _rxPin;
        int8_t _ledPin;
        uint8_t _messageLength;
        uint8_t _message[RMT_READER_MAX_DATA];
        //QueueHandle_t _receive_queue = xQueueCreate(50, sizeof(rmt_rx_done_event_data_t));
        QueueHandle_t _receive_queue = xQueueCreate(50, sizeof(RmtSymbolBuffer));
        RingbufHandle_t _ringbuf = xRingbufferCreate(1024 * sizeof(rmt_symbol_word_t), RINGBUF_TYPE_NOSPLIT);

        rmt_symbol_word_t _raw_symbols[255];
        rmt_rx_done_event_data_t _rx_data;
        rmt_receive_config_t _receive_config = {};
        rmt_channel_handle_t _rx_channel = NULL;

        Esp32RmtReader(uint32_t minSignal, uint32_t maxSignal, uint8_t rxPin, int8_t ledPin);
        void InitLed();
        void SwitchLed(uint8_t state);

        /*
        It is called before the ProcessSignal is executed. You can initialize variables, prepare for incoming data etc...
        The derived class should implement it.
        */
        virtual void BeforeProcessSignal() = 0;
        /*
        It is called after the ProcessSignal is executed. The derived class should implement it.
        */
        virtual void AfterProcessSignal() = 0;
        // Used to convert the level and duration to real data. It is up to the derived class to implement the parsing logic.
        virtual void ProcessSignal(uint32_t level, uint32_t duration) = 0;
    public:
        // Start receiving RMT data
        int32_t Start();
        // Stop receiving RMT data
        void Stop();
        // This should be called regularly from a loop to get the parsed data from the RMT peripheral.
        void ReceiveData(uint8_t *messageLength, uint8_t message[]);
};

#endif
