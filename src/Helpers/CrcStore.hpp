#pragma once

#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <vector>

struct InitItem {
    uint16_t VanId;
    uint8_t Command = 0;
    bool IgnoreCommand = true;
};

struct CrcEntry {
    uint16_t crc;
    uint64_t timestamp;
};

class CrcStore
{
public:
    CrcStore(std::vector<InitItem> initItems, bool automaticallyStoreNewIds = false);
    bool IsCrcSameAsPrevious(uint16_t vanId, uint8_t command, uint16_t crc, uint64_t currentTime);

private:
    const uint8_t CRC_TIMEOUT = 800; // 1 second
    struct CrcKey {
        uint16_t VanId;
        uint8_t Command;

        bool operator==(const CrcKey& other) const {
            return VanId == other.VanId && Command == other.Command;
        }
    };

    struct CrcKeyHasher {
        std::size_t operator()(const CrcKey& k) const {
            return std::hash<uint16_t>()(k.VanId) ^ (std::hash<uint8_t>()(k.Command) << 1);
        }
    };

    std::unordered_map<CrcKey, CrcEntry, CrcKeyHasher> CrcMap;
    std::unordered_map<uint16_t, CrcEntry> VanIdOnlyCrcMap; // For IgnoreCommand = true
    bool _automaticallyStoreNewIds;
};
