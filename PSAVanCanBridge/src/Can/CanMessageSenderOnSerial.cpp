#include "CanMessageSenderOnSerial.h"

void CanMessageSenderOnSerial::PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray)
{
    /*
    if (canId != 0x1e3)
    {
        return;
    }
    */
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

CanMessageSenderOnSerial::CanMessageSenderOnSerial(AbsSer *serialPort)
{
    _serialPort = serialPort;
}

void CanMessageSenderOnSerial::Init()
{
}

uint8_t CanMessageSenderOnSerial::SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray)
{
    PrintToSerial(canId, ext, sizeOfByteArray, byteArray);
    return 0;
}

void CanMessageSenderOnSerial::ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf)
{
}
