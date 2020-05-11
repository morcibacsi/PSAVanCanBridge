#pragma once

#ifndef _AbstractVanMessageSender_h
    #define _AbstractVanMessageSender_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <tss463_channel_registers_struct.h>

/// <summary> An abstract class around the library in case if you want to write custom logic </summary>
class AbstractVanMessageSender {
public:

     /// <summary> Sets up the channel for transmitting a message </summary>
     /// <param name="channelId"> Channel identifier (0-14) </param>
     /// <param name="identifier"> Message identifier </param>
     /// <param name="values"> Data to be transmitted </param>
     /// <param name="messageLength"> Length of the data array to be transmitted </param>
     /// <param name="requireAck"> Indicates whether if we require an acknowledge to be set by the receiver or not </param>
     /// <returns> True if channel was set up otherwise False </returns>
    virtual bool set_channel_for_transmit_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t requireAck) = 0;

    /// <summary> Sets up the channel for receiving a message </summary>
    /// <param name="channelId"> Channel identifier (0-14) </param>
    /// <param name="identifier"> Message identifier </param>
    /// <param name="messageLength"> Length of the data array to be transmitted </param>
    /// <param name="setAck"> Indicates whether if we set up an acknowledge or not </param>
    /// <returns> True if channel was set up otherwise False </returns>
    virtual bool set_channel_for_receive_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t setAck) = 0;

    virtual bool set_channel_for_reply_request_message_without_transmission(uint8_t channelId, uint16_t identifier, uint8_t messageLength) = 0;

    /// <summary> Sets up the channel for a query type message </summary>
    /// <param name="channelId"> Channel identifier (0-14) </param>
    /// <param name="identifier"> Message identifier </param>
    /// <param name="messageLength"> Length of the message to be received </param>
    /// <param name="requireAck"> Indicates whether if we require an acknowledge to be set by the receiver or not </param>
    /// <returns> True if channel was set up otherwise False </returns>
    virtual bool set_channel_for_reply_request_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength, uint8_t requireAck) = 0;

    /// <summary> Sets up the channel for a reply to a query type message. This will be transmitted only when a query with the message identifier is received </summary>
    /// <param name="channelId"> Channel identifier (0-14) </param>
    /// <param name="identifier"> Message identifier </param>
    /// <param name="values"> Data to be transmitted </param>
    /// <param name="messageLength"> Length of the message to be replied for the query </param>
    /// <returns> True if channel was set up otherwise False </returns>
    virtual bool set_channel_for_immediate_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength) = 0;

    /// <summary> Sets up the channel for a reply to a query type message. This will be transmitted even without a query for the message identifier </summary>
    /// <param name="channelId"> Channel identifier (0-14) </param>
    /// <param name="identifier"> Message identifier </param>
    /// <param name="values"> Data to be transmitted </param>
    /// <param name="messageLength"> Length of the message to be replied for the query </param>
    /// <returns> True if channel was set up otherwise False </returns>
    virtual bool set_channel_for_deferred_reply_message(uint8_t channelId, uint16_t identifier, const uint8_t values[], uint8_t messageLength, uint8_t setAck) = 0;

    /// <summary> Sets up the channel for detecting a reply request frame. This channel won't transmit on the bus, just notifies the processor on reception</summary>
    /// <param name="channelId"> Channel identifier (0-14) </param>
    /// <param name="identifier"> Message identifier </param>
    /// <param name="messageLength"> Length of the message to be received </param>
    /// <returns> True if channel was set up otherwise False </returns>
    virtual bool set_channel_for_reply_request_detection_message(uint8_t channelId, uint16_t identifier, uint8_t messageLength) = 0;

    /// <summary> Activates a channel which was previously set by one of the set_channel_ prefixed method </summary>
    /// <returns> True if channel was reactivated otherwise False </returns>
    virtual bool reactivate_channel(uint8_t channelId) = 0;

    /// <summary> Checks if a message is available in a channel </summary>
    /// <param name="channelId"> Channel identifier (0-14) </param>
    virtual MessageLengthAndStatusRegister message_available(uint8_t channelId) = 0;

    /// <summary> Reads a message from a channel </summary>
    /// <param name="channelId"> Channel identifier (0-14) </param>
    /// <param name="length"> Message length will be written to this variable </param>
    /// <param name="buffer"> Message data will be written to this variable </param>
    virtual void read_message(uint8_t channelId, uint8_t* length, uint8_t buffer[]) = 0;

    /// <summary> Returns the channel which transferred or received a message last time </summary>
    virtual uint8_t get_last_channel() = 0;

    /// <summary> Starts the library </summary>
    virtual void begin() = 0;

    /// <summary> Resets all channels to their initial states </summary>
    virtual void reset_channels() = 0;

    /// <summary> Sets an individual byte inside an already active channel. If we have a long message it is much faster to send only the changed byte instad of the full packet </summary>
    /// <param name="channelId"> Channel identifier (0-14) </param>
    /// <param name="index0"> Zero based index of data to be set </param>
    /// <param name="value"> Value to be set </param>
    virtual void set_value_in_channel(uint8_t channelId, uint8_t index0, uint8_t value) = 0;
};

#endif
