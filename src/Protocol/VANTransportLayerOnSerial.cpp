#include <algorithm>
#include "VANTransportLayerOnSerial.hpp"
#include <cstring>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/usb_serial_jtag.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"


VANTransportLayerOnSerial::VANTransportLayerOnSerial()
{
    usb_serial_jtag_driver_config_t usb_serial_jtag_config = {
        .tx_buffer_size = BUF_SIZE,
        .rx_buffer_size = BUF_SIZE,
    };
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usb_serial_jtag_config));
    ESP_LOGI("usb_serial_jtag echo", "USB_SERIAL_JTAG init done");
}

void VANTransportLayerOnSerial::SendMessage(const BusMessage& message, bool highPriority)
{
    printf("VAN >>: %03X ", (unsigned int)(message.id));
    for (size_t i = 0; i < message.dataLength; i++)
    {
        if (i != message.dataLength - 1)
        {
            printf("%02X ", message.data[i]);
        }
        else
        {
            printf("%02X", message.data[i]);
        }
    }
    printf("\n");

    if (!IsBusAvailable())
    {
        return;
    }

    switch (message.type)
    {
        case MessageType::Query:
        {
            break;
        }
        case MessageType::Normal:
        {
            break;
        }

        default:
            break;
    }
}

bool VANTransportLayerOnSerial::ReceiveMessage(BusMessage& message)
{
    uint8_t vanMessageLength;
    uint8_t vanMessage[32];

    vanMessageLength = usb_serial_jtag_read_bytes(vanMessage, (32 - 1), 20 / portTICK_PERIOD_MS);

    if (vanMessageLength < 6)
    {
        return false;
    }

    /*
    if (!_vanRx->IsCrcOk(vanMessage, vanMessageLength))
    {
        return false;
    }
    */
    if (vanMessage[0] != 'v')
    {
        return false;
    }
    /*
    for (int i = 1; i < vanMessageLength-1; i++)
    {
        printf("%02X ", vanMessage[i]);
    }
    printf("\n");
    */
    message.id = (vanMessage[2] << 8 | vanMessage[3]) >> 4;
    std::memcpy(message.data, vanMessage + 4, vanMessageLength-2); // -2 to remove CRC from the data
    //message.data.assign(vanMessage + 4, vanMessage + vanMessageLength-2); // -2 to remove CRC from the data
    message.crc = vanMessage[vanMessageLength - 2] << 8 | vanMessage[vanMessageLength - 1] << 0; // last two bytes of the data
    //message.crc.assign(vanMessage + vanMessageLength-2, vanMessage + vanMessageLength); // last two bytes of the data
    printf("VAN <<: %03X ", (unsigned int)message.id);
    for (size_t i = 0; i < message.dataLength; i++)
    {
        if (i != message.dataLength - 1)
        {
            printf("%02X ", message.data[i]);
        }
        else
        {
            printf("%02X", message.data[i]);
        }
    }
    printf("\n");

    return true;
}

bool VANTransportLayerOnSerial::IsBusAvailable()
{
    return true;
}
