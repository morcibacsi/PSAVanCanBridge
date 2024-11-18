#pragma once

#ifndef _ICanMessageSender_h
    #define _ICanMessageSender_h

#include <inttypes.h>

class ICanMessageSender {
  public:
    virtual void Init() = 0; // The '= 0;' makes whole class "pure virtual"
    virtual uint8_t SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray) = 0;
    virtual bool ReadMessage(uint16_t* canId, uint8_t* len, uint8_t* buf) = 0;
    virtual void Reset() = 0;
 };

#endif
