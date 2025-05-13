#include <algorithm>
#include "ITransportLayer.hpp"
#include "BusMessage.hpp"
#include "CANTransportLayer.hpp"
#include "../lib/CanMessageSender/CanMessageSenderEsp32Idf.h"
#include "../Helpers/IntUnions.h"
#include <cstring>

CANTransportLayer::CANTransportLayer(uint8_t rxPin, uint8_t txPin, uint8_t canController)
{
    _crcCalculator = new VanCrcCalculator();
    _canMessageSender = new CanMessageSenderEsp32Idf(rxPin, txPin, canController);
    _canMessageSender->Init();
}

void CANTransportLayer::SendMessage(const BusMessage& message, bool highPriority)
{
    _canMessageSender->SendMessage(message.id, 0, message.dataLength, message.data);
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
