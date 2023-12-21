#include "VanCrcCalculator.h"

bool VanCrcCalculator::IsCrcOk(const uint8_t message[], uint8_t messageLength)
{
    uint8_t crcByte1 = message[messageLength - 2];
    uint8_t crcByte2 = message[messageLength - 1];

    uint8_t msgLenghtWithoutCrc = messageLength - 2;

    uint16_t crcValueInMessage = crcByte1 << 8 | crcByte2;

    uint16_t crc = 0x7FFF;
    uint8_t  high8Bits;

    uint8_t i = 0;

    // if the message contains the SOF then we exclude that from the calculation
    if (message[0] == 0x0E)
    {
        i = 1;
        msgLenghtWithoutCrc--;
    }

    while (msgLenghtWithoutCrc--)
    {
        high8Bits = crc >> 7;

        crc <<= 8;

        crc ^= VAN_CRC15_TABLE[high8Bits ^ message[i]];
        i++;
    }
    crc ^= 0x7FFF;
    crc <<= 1;
    return crc == crcValueInMessage;
}
