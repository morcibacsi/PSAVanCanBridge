#include <algorithm>
#include <cstring>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/usb_serial_jtag.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"

#include "CANTransportLayerOnSerial.hpp"
#include "../Helpers/IntUnions.h"

CANTransportLayerOnSerial::CANTransportLayerOnSerial()
{
    usb_serial_jtag_driver_config_t usb_serial_jtag_config = {
        .tx_buffer_size = BUF_SIZE,
        .rx_buffer_size = BUF_SIZE,
    };
    ESP_ERROR_CHECK(usb_serial_jtag_driver_install(&usb_serial_jtag_config));
    ESP_LOGI("usb_serial_jtag echo", "USB_SERIAL_JTAG init done");
}

void CANTransportLayerOnSerial::SendMessage(const BusMessage& message, bool highPriority)
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

bool CANTransportLayerOnSerial::ReceiveMessage(BusMessage& message)
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
    if (vanMessage[0] != 'c')
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
    message.id = (vanMessage[1] << 8 | vanMessage[2] << 4) >> 4;
    std::memcpy(message.data, vanMessage + 3, vanMessageLength-3);
    message.crc =  FastChecksum(message.data, message.dataLength);

    /*
    UInt16 checksum {};
    checksum.asUint16 = FastChecksum(message.data.data(), message.data.size());
    message.crc.assign(reinterpret_cast<uint8_t*>(&checksum), reinterpret_cast<uint8_t*>(&checksum) + sizeof(checksum));
*/
    /*
    printf("CAN <<: %03X ", (unsigned int)message.id);
    for (size_t i = 0; i < message.data.size(); i++)
    {
        if (i != message.data.size() - 1)
        {
            printf("%02X ", message.data[i]);
        }
        else
        {
            printf("%02X", message.data[i]);
        }
    }
    printf("\n");
    */
    return true;
}

bool CANTransportLayerOnSerial::IsBusAvailable()
{
    return true;
}

uint16_t CANTransportLayerOnSerial::FastChecksum(const uint8_t *data, uint8_t length)
{
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;

    for (uint8_t i = 0; i < length; ++i) {
        sum1 = (sum1 + data[i]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}