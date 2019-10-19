#pragma once

#ifndef _VanMessageSender_h
    #define _VanMessageSender_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <SPI.h>
#else
    #include "WProgram.h"
#endif

#include <tss463_van.h>
#include "AbstractVanMessageSender.h"


class VanMessageSender : public AbstractVanMessageSender {
TSS463_VAN *VAN;

  public:
      VanMessageSender(uint8_t vanPin, SPIClass *spi)
      {
          VAN = new TSS463_VAN(vanPin, spi);
      }

      virtual void set_channel_for_transmit_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t requireAck)
      {
          VAN->set_channel_for_transmit_message(channelId, identifier, values, messageLength, requireAck);
      }

      virtual void set_channel_for_receive_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t setAck)
      {
          VAN->set_channel_for_receive_message(channelId, identifier, messageLength, setAck);
      }

      virtual void set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint16_t identifier, uint8_t messageLength)
      {
          VAN->set_channel_for_reply_request_message_without_transmission(channelId, identifier, messageLength);
      }

      virtual void set_channel_for_reply_request_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t requireAck)
      {
          VAN->set_channel_for_reply_request_message(channelId, identifier, messageLength, requireAck);
      }

      virtual void set_channel_for_immediate_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength)
      {
          VAN->set_channel_for_immediate_reply_message(channelId, identifier, values, messageLength);
      }

      virtual void set_channel_for_deferred_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t setAck)
      {
          VAN->set_channel_for_deferred_reply_message(channelId, identifier, values, messageLength, setAck);
      }

      virtual void set_channel_for_reply_request_detection_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength)
      {
          VAN->set_channel_for_reply_request_detection_message(channelId, identifier, messageLength);
      }

      virtual MessageLengthAndStatusRegister message_available(uint8_t channelId)
      {
          return VAN->message_available(channelId);
      }

      virtual uint8_t readMsgBuf(const uint8_t channelId, uint8_t *len, uint8_t buf[])
      {
          return VAN->readMsgBuf(channelId, len, buf);
      }

      virtual uint8_t getlastChannel()
      {
          return VAN->getlastChannel();
      }

      virtual void begin()
      {
          VAN->begin();
      }

      virtual void disable_channel(uint8_t channelId)
      {
          VAN->disable_channel(channelId);
      }
 };

#endif
