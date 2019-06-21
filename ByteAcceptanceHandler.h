// ByteAcceptanceHandler.h
#pragma once

#ifndef _ByteAcceptanceHandler_h
    #define _ByteAcceptanceHandler_h

class ByteAcceptanceHandler
{
    uint8_t AcceptanceCount;
    uint8_t LastByte;
    uint8_t AcceptanceCounter;

    public:
    ByteAcceptanceHandler(uint8_t acceptanceCount)
    {
        LastByte = 0x00;
        AcceptanceCounter = 0;
        AcceptanceCount = acceptanceCount;
    }

    uint8_t GetAcceptedByte(uint8_t byteToCheck)
    {
        if (LastByte == byteToCheck)
        {
            if (AcceptanceCounter == AcceptanceCount)
            {
                AcceptanceCounter = 0;
                return byteToCheck;
            }
            AcceptanceCounter++;
        }
        return LastByte;
    }
};

#endif
