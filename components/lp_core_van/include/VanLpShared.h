#pragma once

#include <stdint.h>

#define VAN_LP_RX_TRACE_COUNT 8u
typedef struct {
    uint32_t rawTs;
    uint32_t sampleOffset; // Timestamp immediately after the failed arbitration sample.
    uint32_t outputBeforeRelease; // TX latch before the redundant safety release.
    uint32_t outputReadOffset; // Timestamp after reading that pre-release latch.
    uint32_t releaseOffset; // Failure-path timestamp after release, relative to scheduled slice edge.
    uint32_t rxAfterRelease; // A fresh read, not the original arbitration sample (which was 0).
    uint32_t readOffset; // Timestamp after that read, relative to scheduled slice edge.
    uint32_t outputAfterRelease; // TX output latch after release (1 means recessive was latched).
    uint32_t rxRecessiveOffset; // First observed recessive RX offset, or UINT32_MAX if absent for two slices.
} VanLpArbitrationTrace;

typedef struct {
    uint32_t outputEnable;
    uint32_t outputData;
    uint32_t inputData;
    uint32_t gpioMux;
    uint32_t txConfig;
    uint32_t rxConfig;
} VanLpGpioState;

typedef struct {
    uint32_t rawTs;
    uint32_t pair; // D in bits 3:2, inverse in 1:0; 2 means not sampled.
    uint32_t centerOffset; // Scheduled inverse center relative to COM inverse.
    int32_t correction; // Edge resynchronization applied to this pair.
    int32_t fourthLate; // Post-read timestamp minus scheduled D center.
    int32_t inverseLate; // Post-helper timestamp minus adjusted inverse center.
} VanLpRxTrace;

// All offsets below count raw 8 us time slices, starting at SOF[0].
#define VAN_LP_ENTRY_COUNT 5u
#define VAN_LP_MAX_DATA_BYTES 28u
#define VAN_LP_FRAME_WORDS 34u
#define VAN_LP_PREFIX_TS 30u
#define VAN_LP_RTR_TS 28u
#define VAN_LP_ACK_MASK 0x1fu
#define VAN_LP_REPLY_MASK 0x3e0u
#define VAN_LP_RX_QUEUE_LENGTH 4u
#define VAN_LP_RX_MAX_MESSAGE_BYTES 33u
#define VAN_LP_RX_MAX_GROUPS (VAN_LP_RX_MAX_MESSAGE_BYTES * 2u)

typedef struct {
    // Ten states fit in 16 bits. Keeping 32-bit words here exhausted LP RAM
    // once both configuration banks and the RX queue were linked.
    uint16_t words[VAN_LP_FRAME_WORDS]; // Each word holds ten bus states, MSB first.
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

typedef struct {
    uint32_t groupCount;
    // One raw five-state E-Manchester group per byte, MSB first in bits 4..0.
    // Keeping groups byte-aligned minimizes LP work; HP performs decoding.
    uint8_t groups[VAN_LP_RX_MAX_GROUPS];
    uint8_t ack; // Raw two-state ACK field, first state in bit 1.
    uint8_t padding;
} VanLpRxFrame;

// One LP producer and one HP consumer. The producer publishes writeIndex only
// after the selected entry is complete. A full queue drops the newest frame so
// receive bookkeeping can never delay ACK, reply or transmit timing.
typedef struct {
    VanLpRxFrame frames[VAN_LP_RX_QUEUE_LENGTH];
    uint32_t writeIndex;
    uint32_t readIndex;
    uint32_t receivedCount;
    uint32_t overflowCount;
    uint32_t malformedCount;
    uint32_t maxOccupancy;
    uint32_t sofCount;
    uint32_t eodCount;
} VanLpRxQueue;

#ifdef __cplusplus
static_assert(sizeof(VanLpFrame) == 72, "LP frame ABI must contain 34 packed words and a length");
static_assert(sizeof(VanLpConfig) == 608, "HP/LP configuration layout must match");
static_assert(sizeof(VanLpRxFrame) == 72, "LP receive frame ABI must match");
static_assert(sizeof(VanLpRxQueue) == 320, "LP receive queue ABI must match");
#else
_Static_assert(sizeof(VanLpFrame) == 72, "LP frame ABI must contain 34 packed words and a length");
_Static_assert(sizeof(VanLpConfig) == 608, "HP/LP configuration layout must match");
_Static_assert(sizeof(VanLpRxFrame) == 72, "LP receive frame ABI must match");
_Static_assert(sizeof(VanLpRxQueue) == 320, "LP receive queue ABI must match");
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

// Failure detail: stage in bits 31..16, zero-based raw 8 us slice in 15..0.
// Written only after releasing the bus on failure; no per-sample trace buffer.
typedef enum {
    VAN_LP_ABORT_UNSPECIFIED = 0,
    VAN_LP_ABORT_TX_LENGTH,
    VAN_LP_ABORT_TX_EDGE_DEADLINE,
    VAN_LP_ABORT_TX_SAMPLE_DEADLINE,
    VAN_LP_ABORT_TX_DOMINANT_NOT_SEEN,
    VAN_LP_ABORT_RTR_INVERSE_TIMING,
    VAN_LP_ABORT_RTR_INVERSE_INVALID,
    VAN_LP_ABORT_RX_SAMPLE_DEADLINE,
    VAN_LP_ABORT_RX_INVERSE_TIMING,
    VAN_LP_ABORT_RX_EOD_INVALID,
    VAN_LP_ABORT_RX_SCAN_LIMIT,
    VAN_LP_ABORT_ACK_FIRST_DEADLINE,
    VAN_LP_ABORT_ACK_FIRST_DOMINANT,
    VAN_LP_ABORT_ACK_DRIVE_DEADLINE,
    VAN_LP_ABORT_ACK_RELEASE_DEADLINE,
    VAN_LP_ABORT_TX_END_DEADLINE
} VanLpAbortStage;
