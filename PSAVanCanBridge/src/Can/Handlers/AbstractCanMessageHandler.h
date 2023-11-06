// AbstractCanMessageHandler.h
#pragma once

#ifndef _AbstractCanMessageHandler_h
    #define AbstractCanMessageHandler_h

class AbstractCanMessageHandler {
public:
    virtual bool ProcessMessage(
        const uint16_t canId,
        const uint8_t canMsgLength,
        const uint8_t canMsg[])

    = 0; // The '= 0;' makes whole class "pure virtual"
};
#endif
