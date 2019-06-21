// VanMessageSender.h
#pragma once

#ifndef _VanMessageSender_h
    #define _VanMessageSender_h

//#include "tss463_van.h"
#include "AbstractVanMessageSender.h"


class VanMessageSender : public AbstractVanMessageSender {
//TSS463_VAN *VAN;

  public:
      VanMessageSender(uint8_t vanPin)
      {
          //VAN = new TSS463_VAN(vanPin);
      }

      virtual void set_channel_for_transmit_message(uint8_t channelId, uint8_t id1, uint8_t id2, const uint8_t values[], uint8_t messageLength, uint8_t requireAck)
      {
          //VAN->set_channel_for_transmit_message(channelId, id1, id2, values, messageLength, requireAck);
      }

      virtual void set_channel_for_receive_message(uint8_t channelId, uint8_t id1, uint8_t id2, uint8_t messageLength)
      {
          //VAN->set_channel_for_receive_message(channelId, id1, id2, messageLength);
      }

      virtual void set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint8_t id1, uint8_t id2, uint8_t messageLength)
      {
          //VAN->set_channel_for_reply_request_message_without_transmission(channelId, id1, id2, messageLength);
      }

      virtual void set_channel_for_reply_request_message(uint8_t channelId, uint8_t id1, uint8_t id2, uint8_t messageLength)
      {
          //VAN->set_channel_for_reply_request_message(channelId, id1, id2, messageLength);
      }

      virtual void set_channel_for_immediate_reply_message(uint8_t channelId, uint8_t id1, uint8_t id2, const uint8_t values[], uint8_t messageLength)
      {
          //VAN->set_channel_for_immediate_reply_message(channelId, id1, id2, values, messageLength);
      }

      virtual void set_channel_for_deferred_reply_message(uint8_t channelId, uint8_t id1, uint8_t id2, const uint8_t values[], uint8_t messageLength)
      {
          //VAN->set_channel_for_deferred_reply_message(channelId, id1, id2, values, messageLength);
      }

      virtual void set_channel_for_reply_request_detection_message(uint8_t channelId, uint8_t id1, uint8_t id2, uint8_t messageLength)
      {
          //VAN->set_channel_for_reply_request_detection_message(channelId, id1, id2, messageLength);
      }

      //virtual MessageLengthAndStatusRegister message_available(uint8_t channelId)
      //{
      //    return VAN->message_available(channelId);
      //}

      virtual uint8_t readMsgBuf(const uint8_t channelId, uint8_t *len, uint8_t buf[])
      {
          //return VAN->readMsgBuf(channelId, len, buf);
      }

      virtual uint8_t getlastChannel()
      {
          //return VAN->getlastChannel();
      }

      virtual void begin()
      {
          //VAN->begin();
      }

      virtual void disable_channel(uint8_t channelId)
      {
          //VAN->disable_channel(channelId);
      }
 };

#endif
