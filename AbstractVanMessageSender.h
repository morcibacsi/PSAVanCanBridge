#pragma once

#ifndef _AbstractVanMessageSender_h
    #define _AbstractVanMessageSender_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include <tss463_channel_registers_struct.h>

class AbstractVanMessageSender {
  public:
      virtual void set_channel_for_transmit_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t requireAck) = 0;
      virtual void set_channel_for_receive_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t setAck) = 0;
      virtual void set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint16_t identifier, uint8_t messageLength) = 0;
      virtual void set_channel_for_reply_request_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t requireAck) = 0;
      virtual void set_channel_for_immediate_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength) = 0;
      virtual void set_channel_for_deferred_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t setAck) = 0;
      virtual void set_channel_for_reply_request_detection_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength) = 0;
      virtual MessageLengthAndStatusRegister message_available(uint8_t channelId) = 0;
      virtual uint8_t readMsgBuf(const uint8_t channelId, uint8_t *len, uint8_t buf[]) = 0;
      virtual uint8_t getlastChannel() = 0;
      virtual void begin() = 0;
      virtual void disable_channel(uint8_t channelId) = 0;
 };

#endif
