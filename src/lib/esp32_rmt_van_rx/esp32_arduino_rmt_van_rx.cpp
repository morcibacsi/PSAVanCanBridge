#include "esp32_arduino_rmt_van_rx.h"
#include <string.h>

void ESP32_RMT_VAN_RX::BeforeProcessSignal()
{
    _bitCounter = 0;
    _mask = 1 << 7;
    _tempByte = 0;
    _messageLength = 0;
}

void ESP32_RMT_VAN_RX::AfterProcessSignal()
{
}

uint8_t ESP32_RMT_VAN_RX::RoundToNearest(uint8_t numToRound, uint8_t multiple)
{
    if (multiple == 0)
        return numToRound;

    uint8_t remainder = numToRound % multiple;

    // If the remainder is greater than half of the multiple, then round up. Otherwise, round down.
    if (remainder > multiple / 2)
        return numToRound + multiple - remainder;
    else
        return numToRound - remainder;
}

void ESP32_RMT_VAN_RX::ProcessSignal(uint32_t level, uint32_t duration)
{
    if (_vanLineLevel == VAN_LINE_LEVEL_LOW)
    {
        level = !level;
    }

    // on the bus the time slices are a little off from the multiple of the TS microseconds, so we round it to the nearest multiple of the TS before dividing
    uint8_t countOfTimeSlices = RoundToNearest(duration, _timeSliceDivisor) / _timeSliceDivisor;

    for (int i = 0; i < countOfTimeSlices; i++)
    {
        // every 5th bit is a manchester bit, we must skip them while we are building our byte
        bool isManchesterBit = (_bitCounter + 1) % 5 == 0;
        if (!isManchesterBit)
        {
            if (level == 1)
            {
                _tempByte |= _mask;
            }
            _mask = _mask >> 1;
        }
        else if (_bitCounter == 9)
        {
            // if we found the second manchester bit, then we have the full byte in the _tempByte variable, so we place it in _message array and we can start building the next byte
            _bitCounter = -1;
            _mask = 1 << 7;
            _message[_messageLength] = _tempByte;
            _messageLength++;
            _tempByte = 0;
        }

        _bitCounter++;
    }
}

uint16_t ESP32_RMT_VAN_RX::Crc15(uint8_t data[], uint8_t lengthOfData)
{
    const uint8_t order = 15;
    const uint16_t polynom = 0xF9D;
    const uint16_t xorValue = 0x7FFF;
    const uint16_t mask = 0x7FFF;

    uint16_t crc = 0x7FFF;

    for (uint8_t i = 0; i < lengthOfData; i++)
    {
        uint8_t currentByte = data[i];

        // rotate one data byte including crcmask
        for (uint8_t j = 0; j < 8; j++)
        {
            bool bit = (crc & (1 << (order - 1))) != 0;
            if ((currentByte & 0x80) != 0)
            {
                bit = !bit;
            }
            currentByte <<= 1;

            crc = ((crc << 1) & mask) ^ (-bit & polynom);
        }
    }

    // perform xor and multiply result by 2 to turn 15 bit result into 16 bit representation
    return (crc ^ xorValue) << 1;
}

bool ESP32_RMT_VAN_RX::IsCrcOk(uint8_t vanMessage[], uint8_t vanMessageLength)
{
    // The VAN message must contain the SOF, IDEN bytes and the CRC bytes
    if (vanMessageLength < 4)
    {
        return false;
    }

    // Extract the CRC value from the message
    uint8_t crcByte1 = vanMessage[vanMessageLength - 2];
    uint8_t crcByte2 = vanMessage[vanMessageLength - 1];
    uint16_t crcValueInMessage = crcByte1 << 8 | crcByte2;

    // Create a new array containing the message without the SOF and CRC bytes
    uint8_t vanMessageWithIdWithoutCrc[32];
    if(vanMessageLength - 3 <= 32){
        memcpy(vanMessageWithIdWithoutCrc, vanMessage + 1, vanMessageLength - 3);

        // Calculate the CRC of the message with the ID but without the CRC bytes
        uint16_t calculatedCrc = Crc15(vanMessageWithIdWithoutCrc, vanMessageLength - 3);

        // Return true if the calculated CRC matches the CRC in the message
        return crcValueInMessage == calculatedCrc;
    }

    return false;
}
