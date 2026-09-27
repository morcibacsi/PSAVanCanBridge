#pragma once

#include <string.h>
#include "VanFrame.h"
#include "VanLpShared.h"

namespace VanFrameCodec
{
inline bool FromLegacyBytes(const uint8_t* message, uint8_t messageLength,
                            uint8_t rawAck, VanFrame& frame)
{
    if (!message || messageLength < 5 || messageLength > VAN_LP_RX_MAX_MESSAGE_BYTES)
        return false;

    VanFrame decoded;
    decoded.sof = message[0];
    decoded.identifier = (uint16_t(message[1]) << 4) | (message[2] >> 4);
    decoded.com.raw = message[2] & 0x0fu;
    decoded.com.ext = (decoded.com.raw & 0x08u) != 0;
    decoded.com.rak = (decoded.com.raw & 0x04u) != 0;
    decoded.com.readWrite = (decoded.com.raw & 0x02u) != 0;
    decoded.com.rtr = (decoded.com.raw & 0x01u) != 0;
    decoded.dataLength = messageLength - 5;
    if (decoded.dataLength)
        memcpy(decoded.data.data(), message + 3, decoded.dataLength);
    decoded.fcs = (uint16_t(message[messageLength - 2]) << 8)
                | message[messageLength - 1];
    decoded.ack = rawAck & 0x03u;

    if (decoded.com.rtr)
        decoded.type = VanFrameType::ReplyRequest;
    else if (decoded.com.readWrite)
        decoded.type = VanFrameType::ReplyResponse;
    else if (decoded.com.rak)
        decoded.type = VanFrameType::Write;
    else
        decoded.type = VanFrameType::Broadcast;

    decoded.ackState = !decoded.com.rak ? VanAckState::NotRequested
                     : !(decoded.ack & 0x01u) ? VanAckState::Acknowledged
                                             : VanAckState::NotAcknowledged;
    frame = decoded;
    return true;
}

inline bool ToLegacyBytes(const VanFrame& frame, uint8_t message[], uint8_t* messageLength)
{
    if (!message || !messageLength || frame.identifier > 0x0fffu
        || frame.com.raw > 0x0fu || frame.dataLength > VAN_MAX_DATA_LENGTH)
        return false;

    message[0] = frame.sof;
    message[1] = static_cast<uint8_t>(frame.identifier >> 4);
    message[2] = static_cast<uint8_t>((frame.identifier << 4) | frame.com.raw);
    if (frame.dataLength)
        memcpy(message + 3, frame.data.data(), frame.dataLength);
    message[frame.dataLength + 3] = static_cast<uint8_t>(frame.fcs >> 8);
    message[frame.dataLength + 4] = static_cast<uint8_t>(frame.fcs);
    *messageLength = static_cast<uint8_t>(frame.dataLength + 5u);
    return true;
}
}
