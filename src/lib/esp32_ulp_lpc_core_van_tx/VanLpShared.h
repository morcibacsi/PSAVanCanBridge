#pragma once

#include <stdint.h>

// All offsets below count raw 8 us time slices, starting at SOF[0].
#define VAN_LP_ENTRY_COUNT 5u
#define VAN_LP_MAX_DATA_BYTES 28u
#define VAN_LP_FRAME_WORDS 34u
#define VAN_LP_PREFIX_TS 30u
#define VAN_LP_RTR_TS 28u
#define VAN_LP_ACK_MASK 0x1fu
#define VAN_LP_REPLY_MASK 0x3e0u

typedef struct {
    uint32_t words[VAN_LP_FRAME_WORDS]; // Each word holds ten bus states, MSB first.
    uint32_t frameWordCount;           // Zero disables entry. Final word = ACK[2]+EOF[8].
} VanLpFrame;

typedef struct {
    // Bit-sliced prefix comparison accelerator for all ten candidates. Built
    // from ACK prefixes and the COMPLETE reply frames by the HP CPU. This small
    // shared table avoids ten loads/comparisons every 128 LP cycles; it is not a
    // second request/response bitstream. Reply array indexing remains SOF-based.
    uint32_t matchMask[VAN_LP_PREFIX_TS][2];
    VanLpFrame replies[VAN_LP_ENTRY_COUNT];
    uint32_t enabledMask;
    uint32_t queryAckEnabled;
} VanLpConfig;

#ifdef __cplusplus
static_assert(sizeof(VanLpFrame) == 140, "LP frame ABI must contain 35 words");
static_assert(sizeof(VanLpConfig) == 948, "HP/LP configuration layout must match");
#else
_Static_assert(sizeof(VanLpFrame) == 140, "LP frame ABI must contain 35 words");
_Static_assert(sizeof(VanLpConfig) == 948, "HP/LP configuration layout must match");
#endif

typedef enum {
    VAN_LP_NONE = 0,
    VAN_LP_NORMAL_TX_COMPLETED,
    VAN_LP_ARBITRATION_LOST,
    VAN_LP_NORMAL_FRAME_ACKED,
    VAN_LP_QUERY_NO_RESPONSE,
    VAN_LP_QUERY_RESPONSE_ACKED,
    VAN_LP_QUERY_RESPONSE_RECEIVED,
    VAN_LP_REPLY_ACKNOWLEDGED,
    VAN_LP_REPLY_NOT_ACKNOWLEDGED,
    VAN_LP_ABORT
} VanLpResult;
