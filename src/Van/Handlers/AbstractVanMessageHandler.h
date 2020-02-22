// AbstractVanMessageHandler.h
#pragma once

#ifndef _AbstractVanMessageHandler_h
    #define _AbstractVanMessageHandler_h

class AbstractVanMessageHandler {
public:
    virtual bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan& dataToBridge,
        VanIgnitionDataToBridgeToCan& ignitionDataToBridge,
        DoorStatus& doorStatus)

    = 0; // The '= 0;' makes whole class "pure virtual"
};


/* Concatenates the two bytes and removes the last digit and compares the result to the IDENT */
bool static IsVanIdent(uint8_t byte1, uint8_t byte2, uint16_t ident)
{
    bool result = false;
    uint16_t combined = byte1 << 8 | byte2;
    result = combined >> 4 == ident;
    return result;
}

uint16_t static SwapHiByteAndLoByte(int input)
{
    return ((input & 0xff) << 8) | ((input >> 8) & 0xff);
}
#endif
