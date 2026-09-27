#pragma once

#include "VanFrameBuilder.hpp"

namespace VanConfiguration
{
inline bool IsValidAckIdentifier(uint8_t slot, uint16_t identifier, bool enabled)
{
    return slot < VAN_LP_ENTRY_COUNT && (!enabled || identifier <= 0x0fff);
}

inline bool SetAckIdentifier(VanLpConfig& configuration, uint8_t slot,
                             uint16_t identifier, bool enabled)
{
    if (!IsValidAckIdentifier(slot, identifier, enabled)) return false;
    return VanFrameBuilder::SetAck(configuration, slot, identifier, 0x0c, enabled);
}

inline bool IsValidAckIdentifiers(const uint16_t* identifiers, uint8_t count)
{
    if (count > VAN_LP_ENTRY_COUNT || (count && !identifiers)) return false;
    for (uint8_t i = 0; i < count; ++i)
        if (identifiers[i] > 0x0fff) return false;
    return true;
}

inline bool SetAckIdentifiers(VanLpConfig& configuration,
                              const uint16_t* identifiers, uint8_t count)
{
    if (!IsValidAckIdentifiers(identifiers, count)) return false;

    for (uint8_t i = 0; i < VAN_LP_ENTRY_COUNT; ++i)
        if (!SetAckIdentifier(configuration, i, i < count ? identifiers[i] : 0,
                              i < count))
            return false;
    return true;
}

inline bool IsValidRequestedReplyFrame(uint8_t slot, uint16_t identifier,
                                       const uint8_t* data, uint8_t length,
                                       bool enabled)
{
    return slot < VAN_LP_ENTRY_COUNT
        && (!enabled || (identifier <= 0x0fff && length <= VAN_LP_MAX_DATA_BYTES
                         && (!length || data)));
}

inline bool SetRequestedReplyFrame(VanLpConfig& configuration, uint8_t slot,
                                   uint16_t identifier, const uint8_t* data,
                                   uint8_t length, bool enabled)
{
    if (!IsValidRequestedReplyFrame(slot, identifier, data, length, enabled)) return false;
    return VanFrameBuilder::SetReply(configuration, slot, identifier, data, length, enabled);
}
}
