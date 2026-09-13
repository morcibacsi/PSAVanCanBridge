#pragma once

#include <stdint.h>
#include "VanLpShared.h"

// Main-CPU decoder for the raw five-state groups captured by the LP core.
// The LP still recognizes EOD for real-time ACK and frame publication, but it
// does not reconstruct logical bits or assemble application-facing bytes.
class VanEManchesterDecoder
{
    public:
        static bool Decode(const uint8_t groups[], uint32_t groupCount,
                           uint8_t message[], uint8_t* messageLength)
        {
            *messageLength = 0;
            if (!groups || !message || groupCount < 10 || groupCount > VAN_LP_RX_MAX_GROUPS
                || (groupCount & 1u))
                return false;

            uint8_t currentByte = 0;
            for (uint32_t i = 0; i < groupCount; ++i)
            {
                const uint8_t group = groups[i];
                if (group & 0xe0u) return false;

                const uint8_t nibble = (group >> 1) & 0x0fu;
                const uint8_t fourth = (group >> 1) & 1u;
                const uint8_t inverse = group & 1u;
                const bool violation = fourth == inverse;

                if (!(i & 1u))
                    currentByte = nibble << 4;
                else
                {
                    if (*messageLength >= VAN_LP_RX_MAX_MESSAGE_BYTES) return false;
                    message[(*messageLength)++] = currentByte | nibble;
                }

                if (!violation) continue;
                // EOD is the 00 violation in the second group of a byte. It
                // must terminate the capture and follow at least SOF, ID/COM
                // and the two FCS bytes.
                return !fourth && (i & 1u) && i + 1 == groupCount
                       && *messageLength >= 5;
            }
            return false; // Complete captures always include an EOD violation.
        }
};
