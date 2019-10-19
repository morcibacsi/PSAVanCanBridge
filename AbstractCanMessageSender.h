#pragma once

#ifndef _AbstractCanMessageSender_h
    #define _AbstractCanMessageSender_h

class AbstractCanMessageSender {
  public:
    virtual void Init() = 0; // The '= 0;' makes whole class "pure virtual"
    virtual void SendMessage(uint16_t canId, byte ext, byte sizeOfByteArray, unsigned char *byteArray) = 0; // The '= 0;' makes whole class "pure virtual"
    virtual void ReadMessage(uint16_t* canId, uint8_t* len, uint8_t* buf) = 0;
    //virtual unsigned long GetCanId(void) = 0;
    //virtual unsigned long Start(byte speedset, const byte clockset) = 0;
    //virtual byte CheckReceive(void) = 0;
 };

#endif
