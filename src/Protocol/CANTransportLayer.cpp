#include <algorithm>
#include <cstring>

#include "ITransportLayer.hpp"
#include "BusMessage.hpp"
#include "CANTransportLayer.hpp"
#include "../Helpers/IntUnions.h"

CANTransportLayer::CANTransportLayer(ICanMessageSender* canMessageSender)
{
    _crcCalculator = new VanCrcCalculator();
    _canMessageSender = canMessageSender;
    _canMessageSender->Init();
    serialSemaphore = xSemaphoreCreateMutex();
}

uint8_t CANTransportLayer::SendMessage(const BusMessage& message, bool highPriority)
{
    PrintToSerial(message.id, 0, message.dataLength, message.data);

    if (_loggerFunction)
    {
        _loggerFunction(_network, 2, message); // 2 for outgoing message
    }

    return _canMessageSender->SendMessage(message.id, 0, message.dataLength, message.data);
}

bool CANTransportLayer::ReceiveMessage(BusMessage& message)
{
    uint16_t canMessageId = 0;
    uint8_t canMessageLength;
    uint8_t canMessage[8];

    _canMessageSender->ReadMessage(&canMessageId, &canMessageLength, canMessage);

    if (canMessageLength == 0)
    {
        return false;
    }

    PrintToSerial(canMessageId, 0, canMessageLength, canMessage);
    if (_loggerFunction)
    {
        _loggerFunction(_network, 1, message); // 1 for incoming message
    }

    message.id = canMessageId;
    std::memcpy(message.data, canMessage, canMessageLength);
    message.dataLength = canMessageLength;
    //message.data.assign(canMessage, canMessage + canMessageLength);
    //UInt16 checksum {};
    ////checksum.asUint16 = FastChecksum(message.data.data(), message.data.size());
    //checksum.asUint16 = _crcCalculator->CalculateCrc(message.data, message.dataLength, 0);
    //message.crc.assign(reinterpret_cast<uint8_t*>(&checksum), reinterpret_cast<uint8_t*>(&checksum) + sizeof(checksum));
    message.crc = _crcCalculator->CalculateCrc(message.data, message.dataLength, 0);

    return true;
}

uint16_t CANTransportLayer::FastChecksum(const uint8_t *data, uint8_t length)
{
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;

    for (uint8_t i = 0; i < length; ++i) {
        sum1 = (sum1 + data[i]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}


void CANTransportLayer::PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, const uint8_t byteArray[])
{
    //if (!(canId == 0x2A1 || canId == 0x261 || canId == 0x221))
    {
        return;
    }

    char tmp[3];
    if (xSemaphoreTake(serialSemaphore, portMAX_DELAY) == pdTRUE)
    {
        //printf("%d - >> ID: %03X, Size: %d, Data: ", _handle, canId, sizeOfByteArray);
        uint8_t _handle = 0;
        printf("%d - >> Id: %03X ", _handle, canId);
        for (size_t i = 0; i < sizeOfByteArray; i++)
        {
            printf("%02X ", byteArray[i]);
        }
        printf("\n");
        xSemaphoreGive(serialSemaphore);
    }
}
