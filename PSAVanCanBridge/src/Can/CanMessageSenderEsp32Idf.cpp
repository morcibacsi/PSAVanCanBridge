#include "CanMessageSenderEsp32Idf.h"
#include "driver/gpio.h"

void CanMessageSenderEsp32Idf::PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray)
{
    return;
    //    /*
    //if (!(canId == 0x760 || canId == 0x660 || canId == 0x297 || canId == 0x228))
    if (!(canId == 0x228))
    //if (!(canId == 0x168 || canId == 0x128 || canId == 0x0F6 || canId == 0x036))
    {
        return;
    }
    //*/
    char tmp[3];
    if (xSemaphoreTake(serialSemaphore, portMAX_DELAY) == pdTRUE)
    {
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
        xSemaphoreGive(serialSemaphore);
    }
}

CanMessageSenderEsp32Idf::CanMessageSenderEsp32Idf(uint8_t rxPin, uint8_t txPin, AbsSer *serialPort, uint8_t handle)
{
    _serialPort = serialPort;
    _prevCanId = 0;

    twai_general_config_t g_config = {.controller_id = handle,
                                     .mode = TWAI_MODE_NORMAL,
                                     .tx_io = (gpio_num_t)txPin, .rx_io = (gpio_num_t)rxPin,
                                     .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,
                                     .tx_queue_len = 10, .rx_queue_len = 10,
                                     .alerts_enabled = TWAI_ALERT_NONE,  .clkout_divider = 0,
                                     .intr_flags = ESP_INTR_FLAG_LEVEL1};

    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    esp_err_t result = twai_driver_install_v2(&g_config, &t_config, &f_config, &_twai);

    canSemaphore = xSemaphoreCreateMutex();
    serialSemaphore = xSemaphoreCreateMutex();
}

void CanMessageSenderEsp32Idf::Init()
{
    esp_err_t result = twai_start_v2(_twai);

    _alertInit = twai_reconfigure_alerts_v2(_twai, TWAI_ALERT_ABOVE_ERR_WARN | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_OFF, NULL);
}

uint8_t CanMessageSenderEsp32Idf::SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray)
{
    //PrintToSerial(canId, ext, sizeOfByteArray, byteArray);

    twai_message_t message;
    message.identifier = canId;
    message.flags = TWAI_MSG_FLAG_NONE;
    message.data_length_code = sizeOfByteArray;
    memcpy(message.data, byteArray, sizeOfByteArray);

    PrintToSerial(canId, ext, sizeOfByteArray, message.data);

    uint8_t result = 0;

    if (xSemaphoreTake(canSemaphore, portMAX_DELAY) == pdTRUE)
    {
        if (twai_transmit_v2(_twai, &message, pdMS_TO_TICKS(10)) == ESP_OK) {
            //_serialPort->println("Message queued for transmission");
            result = 1;
        } else {
            //_serialPort->println("Failed to queue message for transmission");
            //return -1;
            result = 0;
        }
        xSemaphoreGive(canSemaphore);
    }

    ///*
    uint32_t alerts;
    if (_alertInit == ESP_OK && twai_read_alerts_v2(_twai, &alerts, pdMS_TO_TICKS(0)) == ESP_OK)
    {
        if (alerts & TWAI_ALERT_BUS_OFF)
        {
            twai_initiate_recovery_v2(_twai);
        }
    }
    //*/

    return result;
}

bool CanMessageSenderEsp32Idf::ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf)
{
    twai_message_t message;
    if (twai_receive_v2(_twai, &message, pdMS_TO_TICKS(10)) == ESP_OK) {
        if (message.flags == TWAI_MSG_FLAG_NONE || message.flags == TWAI_MSG_FLAG_SS)
        {
            *canId = message.identifier;
            *len = message.data_length_code;
            for (int i = 0; i < message.data_length_code; i++)
            {
                buf[i] = message.data[i];
            }
            //PrintToSerial(*canId, 0, *len, buf);
        }
        return true;
    } else {
        //printf("Failed to receive message\n");
        return false;
    }
}

void CanMessageSenderEsp32Idf::Reset()
{

}