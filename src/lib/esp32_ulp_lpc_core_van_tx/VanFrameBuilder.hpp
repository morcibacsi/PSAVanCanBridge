#pragma once

#include "VanLpShared.h"
#include <string.h>

// HP CPU only. Also usable by deterministic host tests without ESP-IDF.
namespace VanFrameBuilder
{
constexpr uint32_t Stuff(uint8_t byte)
{
    return ((byte & 0xf0u) << 2) | ((~byte & 0x10u) << 1)
         | ((byte & 0x0fu) << 1) | (~byte & 1u);
}

inline uint16_t Crc15(const uint8_t* data, uint8_t length)
{
    uint16_t crc = 0x7fff;
    for (uint8_t i = 0; i < length; ++i)
    {
        uint8_t byte = data[i];
        for (uint8_t j = 0; j < 8; ++j)
        {
            const bool bit = ((crc & 0x4000) != 0) != ((byte & 0x80) != 0);
            byte <<= 1;
            crc = ((crc << 1) & 0x7fff) ^ (-bit & 0x0f9d);
        }
    }
    return (crc ^ 0x7fff) << 1;
}

inline bool Build(uint16_t identifier, uint8_t command, const uint8_t* data,
                  uint8_t dataBytes, VanLpFrame& frame)
{
    if (identifier > 0xfff || command > 0xf || dataBytes > VAN_LP_MAX_DATA_BYTES
        || (dataBytes && !data)) return false;
    uint8_t bytes[VAN_LP_MAX_DATA_BYTES + 5] = {};
    bytes[0] = 0x0e;
    bytes[1] = identifier >> 4;
    bytes[2] = (identifier << 4) | command;
    if (dataBytes) memcpy(bytes + 3, data, dataBytes);
    const uint16_t crc = Crc15(bytes + 1, dataBytes + 2);
    bytes[dataBytes + 3] = crc >> 8;
    bytes[dataBytes + 4] = crc;
    for (unsigned i = 0; i < dataBytes + 5u; ++i) frame.words[i] = Stuff(bytes[i]);
    // Preserve the proven builder: packed CRC's bit zero is always zero, so
    // clearing its stuffed inverse creates precisely the final 00 EOD pair.
    frame.words[dataBytes + 4] &= frame.words[dataBytes + 4] - 1u;
    frame.words[dataBytes + 5] = 0x3ff; // ACK[2] + EOF[8], not ten EOF slices.
    frame.frameWordCount = dataBytes + 6;
    return true;
}

// HP/test convenience only. LP already knows ACK occupies bits 9/8 of the
// last word, so no redundant per-frame ACK/EOF/RTR offsets live in LP RAM.
inline unsigned AckStartTs(const VanLpFrame& frame) { return (frame.frameWordCount - 1u) * 10; }

inline void RemoveCandidate(VanLpConfig& config, unsigned candidate)
{
    const uint32_t keep = ~(1u << candidate);
    config.enabledMask &= keep;
    for (unsigned ts = 0; ts < VAN_LP_PREFIX_TS; ++ts)
        for (unsigned level = 0; level < 2; ++level) config.matchMask[ts][level] &= keep;
}

inline void SetCandidate(VanLpConfig& config, unsigned candidate, const uint32_t* words)
{
    RemoveCandidate(config, candidate);
    for (unsigned ts = 0; ts < VAN_LP_PREFIX_TS; ++ts)
        config.matchMask[ts][(words[ts / 10] >> (9 - ts % 10)) & 1] |= 1u << candidate;
    config.enabledMask |= 1u << candidate;
}

inline bool SetAck(VanLpConfig& config, unsigned slot, uint16_t identifier, uint8_t command, bool enabled)
{
    // Only normal write/data frames requesting ACK; match the complete COM.
    if (slot >= VAN_LP_ENTRY_COUNT || (enabled && (identifier > 0xfff || command != 0xc))) return false;
    RemoveCandidate(config, slot);
    if (enabled)
    {
        const uint32_t prefix[3] = {Stuff(0x0e), Stuff(identifier >> 4), Stuff((identifier << 4) | command)};
        SetCandidate(config, slot, prefix);
    }
    return true;
}

inline bool SetReply(VanLpConfig& config, unsigned slot, uint16_t identifier,
                     const uint8_t* data, uint8_t dataBytes, bool enabled)
{
    if (slot >= VAN_LP_ENTRY_COUNT) return false;
    if (enabled && !Build(identifier, 0xe, data, dataBytes, config.replies[slot])) return false;
    RemoveCandidate(config, slot + VAN_LP_ENTRY_COUNT);
    if (enabled) SetCandidate(config, slot + VAN_LP_ENTRY_COUNT, config.replies[slot].words);
    else config.replies[slot].frameWordCount = 0;
    return true;
}
}
