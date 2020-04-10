// CanRadioRd4DiagHandler.h
#pragma once

#ifndef _CanRadioRd4DiagHandler_h
    #define _CanRadioRd4DiagHandler_h

#include "../AbstractCanMessageSender.h"
#include "../Structs/CanRadioRd4DiagStructs.h"

class CanRadioRd4DiagHandler
{
    CanRadioRd4DiagPacketSender* _packetSender;
    AbsSer* _serialPort;

    uint8_t vinNumber[17] = { 0 };
    uint8_t vinIndex = 0;

    void PrintArrayToSerialAsChar(const uint8_t vanMessage[], uint8_t vanMessageLength)
    {
        for (uint8_t i = 0; i < vanMessageLength; i++)
        {
            _serialPort->write(vanMessage[i]);
        }
        _serialPort->println();
    }

    public:
    CanRadioRd4DiagHandler(AbstractCanMessageSender* object, AbsSer* serialPort)
    {
        _packetSender = new CanRadioRd4DiagPacketSender(object);
        _serialPort = serialPort;
    }

    void GetVin()
    {
        _packetSender->EnterDiagMode();
    }

    void ProcessReceivedCanMessage(uint16_t canId, uint8_t length, uint8_t canMsg[])
    {
        _packetSender->ProcessReceivedCanMessage(canId, length, canMsg);

        if (length > 3)
        {
            uint8_t startPosition = 0;
            uint8_t endPosition = 0;
            if (canMsg[0] == 0x10)
            {
                vinIndex = 0;
                startPosition = 4;
                endPosition = 7;
            }
            if (canMsg[0] == 0x21)
            {
                startPosition = 1;
                endPosition = 7;
            }
            if (canMsg[0] == 0x22)
            {
                startPosition = 1;
                endPosition = 6;
            }

            for (uint8_t i = startPosition; i < endPosition + 1; i++)
            {
                vinNumber[vinIndex] = canMsg[i];
                vinIndex++;
            }

            if (vinIndex == 17)
            {
                PrintArrayToSerialAsChar(vinNumber, 17);
            }
        }
    }
};

#endif
