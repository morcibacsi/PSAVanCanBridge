#include "CanMessageSenderEsp32Idf.h"
#include "driver/gpio.h"
#include "driver/can.h"

void CanMessageSenderEsp32Idf::PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray)
{
    //
    /*
    if (canId != 0x1e3)
    {
        return;
    }
    //*/
    char tmp[3];

    _serialPort->print(canId, HEX);
    _serialPort->print(",1,");
    _serialPort->print(sizeOfByteArray, DEC);
    _serialPort->print(",");

    for (size_t i = 0; i < sizeOfByteArray; i++)
    {
        snprintf(tmp, 3, "%02X", byteArray[i]);
        if (i != sizeOfByteArray - 1)
        {
            _serialPort->print(tmp);
            _serialPort->print("");
        }
    }
    _serialPort->println(tmp);
}

CanMessageSenderEsp32Idf::CanMessageSenderEsp32Idf(uint8_t rxPin, uint8_t txPin, bool enableThrottling, AbsSer *serialPort)
{
    _serialPort = serialPort;
    _enableThrottling = enableThrottling;
    _prevCanId = 0;

    can_general_config_t g_config = {.mode = CAN_MODE_NORMAL,
                                     .tx_io = (gpio_num_t)txPin, .rx_io = (gpio_num_t)rxPin,
                                     .clkout_io = CAN_IO_UNUSED, .bus_off_io = CAN_IO_UNUSED,
                                     .tx_queue_len = 10, .rx_queue_len = 10,
                                     .alerts_enabled = CAN_ALERT_NONE,  .clkout_divider = 0,
                                     .intr_flags = ESP_INTR_FLAG_LEVEL1};

    can_timing_config_t t_config = CAN_TIMING_CONFIG_125KBITS();
    can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

    esp_err_t result = can_driver_install(&g_config, &t_config, &f_config);

    canSemaphore = xSemaphoreCreateMutex();
}

void CanMessageSenderEsp32Idf::Init()
{
    esp_err_t result = can_start();
}

uint8_t CanMessageSenderEsp32Idf::SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray)
{
    //PrintToSerial(canId, ext, sizeOfByteArray, byteArray);

    can_message_t message;
    message.identifier = canId;
    message.flags = CAN_MSG_FLAG_SS;
    message.data_length_code = sizeOfByteArray;
    for (int i = 0; i < sizeOfByteArray; i++) {
        message.data[i] = byteArray[i];
    }

    //workaround to avoid weird errors on screen
    if (_enableThrottling)
    {
        unsigned long currentTime = millis();
        if (_prevCanId != canId)
        {
            unsigned long delayFromLastTransmission = currentTime - _prevCanIdTime;
            if (delayFromLastTransmission < 15)
            {
                delay(15 - delayFromLastTransmission);
            }
            _prevCanIdTime = currentTime;
            _prevCanId = canId;
        }
    }

    uint8_t result = -1;

    if (xSemaphoreTake(canSemaphore, portMAX_DELAY) == pdTRUE)
    {
        if (can_transmit(&message, pdMS_TO_TICKS(10)) == ESP_OK) {
            //_serialPort->println("Message queued for transmission");
            result = 0;
        } else {
            //_serialPort->println("Failed to queue message for transmission");
            //return -1;
            result = -1;
        }
        xSemaphoreGive(canSemaphore);
    }
    return result;
}

void CanMessageSenderEsp32Idf::ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf)
{
    can_message_t message;
    if (can_receive(&message, pdMS_TO_TICKS(10)) == ESP_OK) {
        if (message.flags == CAN_MSG_FLAG_NONE || message.flags == CAN_MSG_FLAG_SS)
        {
            *canId = message.identifier;
            *len = message.data_length_code;
            for (int i = 0; i < message.data_length_code; i++)
            {
                buf[i] = message.data[i];
            }
            //PrintToSerial(*canId, 0, *len, buf);
        }
    } else {
        //printf("Failed to receive message\n");
        return;
    }
}
