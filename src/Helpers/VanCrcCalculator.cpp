#include "VanCrcCalculator.hpp"

bool VanCrcCalculator::IsCrcOk(const uint8_t message[], uint8_t messageLength)
{
    if (messageLength < 3) return false; // Not enough bytes to include CRC

    uint8_t crcByte1 = message[messageLength - 2];
    uint8_t crcByte2 = message[messageLength - 1];
    uint16_t crcValueInMessage = crcByte1 << 8 | crcByte2;

    uint8_t offset = (message[0] == 0x0E) ? 1 : 0;
    uint8_t dataLength = messageLength - 2 - offset;

    uint16_t calculatedCrc = CalculateCrc(message, dataLength, offset);

    return calculatedCrc == crcValueInMessage;
}

uint16_t VanCrcCalculator::CalculateCrc(const uint8_t message[], uint8_t length, uint8_t offset)
{
    uint16_t crc = 0x7FFF;

    for (uint8_t i = 0; i < length; ++i)
    {
        uint8_t high8Bits = crc >> 7;
        crc <<= 8;
        crc ^= VAN_CRC15_TABLE[high8Bits ^ message[offset + i]];
    }

    crc ^= 0x7FFF;
    crc <<= 1;

    return crc;
}