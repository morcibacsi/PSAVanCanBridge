#if defined(__riscv) || defined(__riscv__) || defined(VAN_LP_HOST_TEST)
#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core_gpio.h"
#include "ulp_lp_core_utils.h"
#ifndef VAN_LP_HOST_TEST
#include "soc/lp_io_reg.h"
#endif
#include "../src/lib/esp32_ulp_lpc_core_van_tx/VanLpShared.h"

volatile uint32_t VAN_DATA[VAN_LP_FRAME_WORDS];
volatile uint32_t VAN_DATA_LENGTH;
volatile uint32_t VAN_START_TX;
volatile uint32_t VAN_TX_FINISHED = 1;
volatile uint32_t SET_VAN_RX_PIN, SET_VAN_TX_PIN, VAN_START_APP;
volatile uint32_t VAN_RETRY_COUNT, VAN_FRAME_TYPE;
volatile uint32_t VAN_TX_RESULT, VAN_BUS_RESULT;
volatile uint32_t VAN_TX_ABORT_DETAIL;
volatile uint32_t VAN_TX_EOD_TS;
volatile uint32_t VAN_TS_CYCLES = 128;
volatile VanLpRxTrace VAN_TX_RX_TRACE[VAN_LP_RX_TRACE_COUNT];
volatile uint32_t VAN_TX_RX_TRACE_COUNT;
volatile VanLpArbitrationTrace VAN_TX_ARBITRATION_TRACE;
static VanLpArbitrationTrace arbitrationTrace;
static uint32_t abortDetail;
static uint32_t responseEodTs;
volatile VanLpConfig VAN_CONFIG[2];
volatile uint32_t VAN_CONFIG_PUBLISHED, VAN_CONFIG_APPLIED;
volatile lp_io_num_t VAN_RX_PIN, VAN_TX_PIN;

// HP calibrates RTC_FAST before starting bus activity. The RC oscillator's
// nominal 16 MHz is not precise enough for absolute receive/TX deadlines.
// Cache once: no shared-RAM timing updates during an active transaction.
static uint32_t tsCycles = 128;
#define TS_CYCLES tsCycles
#define HALF_TS_CYCLES (tsCycles / 2u)
#define DEADLINE_SLACK_CYCLES 16u
#define INLINE static inline __attribute__((always_inline))

#ifndef VAN_LP_HOST_TEST
INLINE uint32_t cycles(void)
{
    uint32_t value;
    __asm__ __volatile__("csrr %0, mcycle" : "=r"(value));
    return value;
}
INLINE void shared_fence(void) { __asm__ __volatile__("fence rw,rw" ::: "memory"); }
#else
#define cycles van_test_cycles
#define shared_fence() ((void)0)
#endif

// Absolute deadlines absorb processing cost; delay_us(8) between samples would
// add the entire matcher cost to every slice. Signed subtraction handles wrap.
INLINE bool wait_until(uint32_t deadline)
{
    int32_t late;
    do { late = (int32_t)(cycles() - deadline); } while (late < 0);
    return late <= (int32_t)DEADLINE_SLACK_CYCLES;
}
#ifndef VAN_LP_HOST_TEST
static uint32_t rxMask, txMask;
// C6 LP GPIO W1TS/W1TC are full 32-bit registers with pins in bits 0..7.
// Direct MMIO avoids the IDF -Os bitfield helpers' calls and stack spills.
// All prepared TX modes use these operations.
INLINE uint32_t read_bus(void)
{
    return (*(volatile uint32_t*)LP_IO_IN_REG & rxMask) != 0;
}
INLINE uint32_t read_output(void)
{
    return (*(volatile uint32_t*)LP_IO_OUT_DATA_REG & txMask) != 0;
}
INLINE void drive_bus(uint32_t level)
{
    *(volatile uint32_t*)(LP_IO_OUT_DATA_W1TC_REG - (level << 2)) = txMask;
}
#else
INLINE uint32_t read_bus(void) { return ulp_lp_core_gpio_get_level(VAN_RX_PIN) != 0; }
INLINE uint32_t read_output(void) { return 1; }
INLINE void drive_bus(uint32_t level) { ulp_lp_core_gpio_set_level(VAN_TX_PIN, level); }
#endif
INLINE void release_bus(void) { drive_bus(1); }

// Called only after normal TX has lost arbitration and released its output.
// No extra reads/timestamps on successful slices and no retry/re-sampling of
// the arbitration decision. Incoming response activity must not overwrite the
// published local-TX snapshot while HP is waiting to read completion.
static __attribute__((noinline)) void record_arbitration_loss(unsigned wordIndex, int bitIndex, uint32_t edge,
                                                              uint32_t sampleDone, uint32_t outputBefore,
                                                              uint32_t outputReadDone)
{
    const uint32_t released = cycles();
    uint32_t after = read_bus();
    const uint32_t initialAfter = after;
    const uint32_t readDone = cycles();
    const uint32_t output = read_output();
    uint32_t recessiveOffset = after ? readDone - edge : UINT32_MAX;
    // Failure-path-only observation. Do not retry or reinterpret the original
    // arbitration sample; just determine whether RX follows the released TX
    // latch before two complete slices have elapsed.
    while (!after && (uint32_t)(cycles() - edge) < 2 * TS_CYCLES)
    {
        after = read_bus();
        if (after) recessiveOffset = cycles() - edge;
    }
    arbitrationTrace = (VanLpArbitrationTrace){wordIndex * 10 + 9 - bitIndex,
                                            sampleDone - edge, outputBefore, outputReadDone - edge,
                                            released - edge, initialAfter, readDone - edge,
                                            output, recessiveOffset};
}

static VanLpResult abort_at(VanLpAbortStage stage, uint32_t rawTs)
{
    release_bus(); // Never lengthen a dominant pulse to record diagnostics.
    abortDetail = ((uint32_t)stage << 16) | (rawTs & 0xffffu);
    return VAN_LP_ABORT;
}

INLINE bool sample_at(uint32_t deadline, uint32_t* bit)
{
    if (!wait_until(deadline)) return false;
    *bit = read_bus();
    return true;
}

// At the mandatory inversion slot, find the edge and re-center the sample.
// This bounds drift to one five-TS group, rather than an entire 28-byte frame.
// No edge means a possible violation; only the caller may classify 00 as EOD.
INLINE bool sample_inverse(uint32_t* sampleCycle, uint32_t previous, uint32_t* bit)
{
    uint32_t now;
    uint32_t lastPreviousCycle = *sampleCycle - TS_CYCLES;
    // Stop GPIO edge polling a quarter-TS before the sample center. At EOD
    // there is deliberately no edge: polling all the way to the center could
    // overshoot it by one MMIO/poll iteration and falsely trip the 16-cycle
    // deadline guard. Finish with the same short timer-only wait as sample_at.
    // An edge in the final timer-only interval must ALSO correct phase. Merely
    // accepting its new level at the center leaves RX free-running, eventually
    // classifying DATA as EOD. Keep a bracket for that otherwise missed edge.
    const uint32_t edgeSearchEnd = *sampleCycle - HALF_TS_CYCLES / 2;
    while ((int32_t)((now = cycles()) - edgeSearchEnd) < 0)
    {
        if (read_bus() != previous)
        {
            // Reject an edge outside the middle half of the expected TS.
            int32_t error = (int32_t)(now - (*sampleCycle - HALF_TS_CYCLES));
            if (error < -(int32_t)(HALF_TS_CYCLES / 2) || error > (int32_t)(HALF_TS_CYCLES / 2)) return false;
            *sampleCycle = now + HALF_TS_CYCLES;
            return sample_at(*sampleCycle, bit);
        }
        lastPreviousCycle = now;
    }
    if (!sample_at(*sampleCycle, bit)) return false;
    if (*bit != previous)
    {
        // The transition lies between the last old-level observation and this
        // new-level observation. Use their midpoint, not the late sample as
        // the exact edge. The bit has already been sampled; move only future
        // deadlines. At EOD there is no transition and the phase is unchanged.
        const uint32_t observedCycle = cycles();
        const uint32_t edge = lastPreviousCycle + (observedCycle - lastPreviousCycle) / 2u;
        *sampleCycle = edge + HALF_TS_CYCLES;
    }
    return true;
}

// Start with DATA[0] at raw index 30. The COM inverse has already been consumed.
// EOD is only legal at 48/49 + 10*N, N=0..28; never infer a nonexistent DLC.
// Trace writes occur after a pair (three skipped DATA slices are available),
// after an abort releases TX, or after ACK release. Never during ACK dominance.
static void record_rx_pair(unsigned rawTs, uint32_t pair, uint32_t start,
                           uint32_t planned, uint32_t adjusted,
                           uint32_t fourthDone, uint32_t inverseDone)
{
    const unsigned index = VAN_TX_RX_TRACE_COUNT;
    if (index >= VAN_LP_RX_TRACE_COUNT) return;
    volatile VanLpRxTrace* trace = &VAN_TX_RX_TRACE[index];
    trace->rawTs = rawTs;
    trace->pair = pair;
    trace->centerOffset = adjusted - start;
    trace->correction = (int32_t)(adjusted - planned);
    trace->fourthLate = (int32_t)(fourthDone - (planned - TS_CYCLES));
    trace->inverseLate = (int32_t)(inverseDone - adjusted);
    VAN_TX_RX_TRACE_COUNT = index + 1;
}

static VanLpResult track_response(uint32_t sampleCycle, bool ack, bool query)
{
    uint32_t bit, fourth;
    const uint32_t responseStart = sampleCycle;
    if (query) VAN_TX_RX_TRACE_COUNT = 0;
    unsigned pairParity = 0; // Even number of five-TS groups at byte boundary.
    // The HP receiver decodes DATA/FCS. Here only D,!D is needed to locate
    // EOD. Skip A,B,C so the D sample can flow directly into edge polling:
    // a per-slice loop spends the narrow D-center -> !D-edge window on
    // phase bookkeeping, branches and the next loop iteration instead.
    for (unsigned rawTs = 34; rawTs < 330; rawTs += 5)
    {
        sampleCycle += 5 * TS_CYCLES;
        const bool trace = query && rawTs < 34 + 5 * VAN_LP_RX_TRACE_COUNT;
        const uint32_t planned = sampleCycle;
        if (!sample_at(sampleCycle - TS_CYCLES, &fourth))
            return abort_at(VAN_LP_ABORT_RX_SAMPLE_DEADLINE, rawTs - 1);
        const uint32_t fourthDone = trace ? cycles() : 0;
        bit = 2;
        const bool inverseOk = sample_inverse(&sampleCycle, fourth, &bit);
        const uint32_t inverseDone = trace ? cycles() : 0;
        if (!inverseOk)
        {
            VanLpResult result = abort_at(VAN_LP_ABORT_RX_INVERSE_TIMING, rawTs);
            if (trace) record_rx_pair(rawTs, (fourth << 2) | bit, responseStart,
                                      planned, sampleCycle, fourthDone, inverseDone);
            return result;
        }
        if (bit == fourth)
        {
            // 11 is malformed, not EOD. A full 15-bit FCS must fit before
            // the 00, and DATA always has a whole number of bytes.
            if (bit || rawTs < 49 || !pairParity)
            {
                VanLpResult result = abort_at(VAN_LP_ABORT_RX_EOD_INVALID, rawTs);
                if (trace) record_rx_pair(rawTs, (fourth << 2) | bit, responseStart,
                                          planned, sampleCycle, fourthDone, inverseDone);
                return result;
            }
            if (ack)
            {
                // At second EOD slot center. ACK[0] starts +0.5 TS;
                // ACK[1] starts +1.5 TS. Drive only ACK[1], for ONE TS.
                if (!sample_at(sampleCycle + TS_CYCLES, &bit))
                    return abort_at(VAN_LP_ABORT_ACK_FIRST_DEADLINE, rawTs + 1);
                if (!bit) return abort_at(VAN_LP_ABORT_ACK_FIRST_DOMINANT, rawTs + 1);
                uint32_t ackEdge = sampleCycle + TS_CYCLES + HALF_TS_CYCLES;
                if (!wait_until(ackEdge)) return abort_at(VAN_LP_ABORT_ACK_DRIVE_DEADLINE, rawTs + 2);
                drive_bus(0);
                bool onTime = wait_until(ackEdge + TS_CYCLES);
                release_bus();
                if (!onTime) return abort_at(VAN_LP_ABORT_ACK_RELEASE_DEADLINE, rawTs + 3);
            }
            // Record only after releasing ACK; no diagnostic stores in the
            // sampling loop or dominant pulse. This is the detected boundary,
            // not proof that it was the real end of the reply.
            responseEodTs = rawTs;
            if (trace) record_rx_pair(rawTs, fourth << 2, responseStart,
                                      planned, sampleCycle, fourthDone, inverseDone);
            return query ? (ack ? VAN_LP_QUERY_RESPONSE_ACKED : VAN_LP_QUERY_RESPONSE_RECEIVED)
                         : (ack ? VAN_LP_NORMAL_FRAME_ACKED : VAN_LP_NONE);
        }
        if (trace) record_rx_pair(rawTs, (fourth << 2) | bit, responseStart,
                                  planned, sampleCycle, fourthDone, inverseDone);
        pairParity ^= 1;
    }
    return abort_at(VAN_LP_ABORT_RX_SCAN_LIMIT, 330);
}

// Transmit using an existing full frame and an already established array index.
// Replies enter at word 2, bit 0 after driving RTR; local TX enters after SOF[0].
// No copying or buffer replacement at RTR. ACK occupies bits 9/8 of last word.
typedef enum { VAN_TX_NORMAL, VAN_TX_REQUESTER, VAN_TX_RESPONDER } VanTxRole;

INLINE VanLpResult transmit_prepared(const volatile uint32_t* words, unsigned wordCount,
                                     unsigned wordIndex, int bitIndex, uint32_t edgeCycle,
                                     VanTxRole role, bool queryAck)
{
    const bool requester = role == VAN_TX_REQUESTER;
    if (wordCount < 6 || wordCount > VAN_LP_FRAME_WORDS) return abort_at(VAN_LP_ABORT_TX_LENGTH, 0);
    uint32_t bit, bus;
    // Local TX enters after driving dominant SOF[0]. Responder takeover also
    // drives dominant RTR before entering here.
    uint32_t previousBit = 0;
    bool acknowledged = false;
    for (; wordIndex < wordCount; ++wordIndex, bitIndex = 9)
    {
        uint32_t word = words[wordIndex];
        for (; bitIndex >= 0; --bitIndex)
        {
            bit = (word >> bitIndex) & 1u;
            if (!wait_until(edgeCycle))
                return abort_at(VAN_LP_ABORT_TX_EDGE_DEADLINE, wordIndex * 10 + 9 - bitIndex);
            drive_bus(bit);
            if (!sample_at(edgeCycle + HALF_TS_CYCLES, &bus))
                return abort_at(VAN_LP_ABORT_TX_SAMPLE_DEADLINE, wordIndex * 10 + 9 - bitIndex);
            if (!bit && bus)
                return abort_at(VAN_LP_ABORT_TX_DOMINANT_NOT_SEEN, wordIndex * 10 + 9 - bitIndex);
            // Hardware traces show the C6 LP input can still report the old
            // dominant state at the ordinary sample after a local 0 -> 1
            // transition, even though OUT_DATA already contains 1. Confirm
            // only that mismatch late in the same slice. A real contender
            // remains dominant and still wins arbitration.
            if (role == VAN_TX_NORMAL && bit && !bus && !previousBit)
            {
                const uint32_t confirmCycle = edgeCycle + TS_CYCLES - TS_CYCLES / 4;
                if (!sample_at(confirmCycle, &bus))
                    return abort_at(VAN_LP_ABORT_TX_SAMPLE_DEADLINE, wordIndex * 10 + 9 - bitIndex);
            }
            if (requester && wordIndex == 2 && bitIndex == 1 && !bus)
            {
                release_bus(); // RTR recessive lost: stay inside this frame.
                uint32_t center = edgeCycle + HALF_TS_CYCLES + TS_CYCLES;
                if (!sample_inverse(&center, 0, &bus)) return abort_at(VAN_LP_ABORT_RTR_INVERSE_TIMING, 29);
                if (bus != 1) return abort_at(VAN_LP_ABORT_RTR_INVERSE_INVALID, 29);
                return track_response(center, queryAck, true);
            }
            if (wordIndex == wordCount - 1 && bitIndex == 8)
                acknowledged = !bus; // Bus was released by the prepared 1.
            else if (bit && !bus)
            {
                // The arbitration decision is already final. Capture whether
                // the original recessive write reached the TX latch before the
                // unconditional safety release can hide that distinction.
                const uint32_t sampleDone = cycles();
                const uint32_t outputBefore = role == VAN_TX_NORMAL ? read_output() : 0;
                const uint32_t outputReadDone = role == VAN_TX_NORMAL ? cycles() : sampleDone;
                release_bus();
                if (role == VAN_TX_NORMAL)
                    record_arbitration_loss(wordIndex, bitIndex, edgeCycle,
                                            sampleDone, outputBefore, outputReadDone);
                return VAN_LP_ARBITRATION_LOST;
            }
            previousBit = bit;
            edgeCycle += TS_CYCLES;
        }
    }
    if (!wait_until(edgeCycle)) return abort_at(VAN_LP_ABORT_TX_END_DEADLINE, wordCount * 10);
    release_bus();
    if (role == VAN_TX_NORMAL) return VAN_LP_NORMAL_TX_COMPLETED;
    return requester ? VAN_LP_QUERY_NO_RESPONSE
                     : (acknowledged ? VAN_LP_REPLY_ACKNOWLEDGED : VAN_LP_REPLY_NOT_ACKNOWLEDGED);
}

static VanLpResult receive_frame(const volatile VanLpConfig* config, uint32_t edgeCycle)
{
    uint32_t candidates = config->enabledMask;
    uint32_t bit = 0, previous = 0, sof = 0;
    uint32_t center = edgeCycle + HALF_TS_CYCLES;
    // One raw index continues from comparison into transmission. The HP's mask
    // table is derived from these same reply arrays, including SOF and COM=E.
    unsigned wordIndex = 0;
    int bitIndex = 9;
    const volatile VanLpFrame* selected = 0;
    unsigned selectedWordCount = 0;
    uint32_t selectedRtr = 0;
    for (unsigned rawTs = 0; rawTs < VAN_LP_PREFIX_TS; ++rawTs)
    {
        if (bitIndex == 5 || bitIndex == 0)
        {
            if (!sample_inverse(&center, previous, &bit)) goto abort;
        }
        else if (!sample_at(center, &bit)) goto abort;
        if (rawTs < 10)
        {
            sof = (sof << 1) | bit;
            if (rawTs == 9 && sof != 0x03d) goto abort;
        }
        candidates &= config->matchMask[rawTs][bit];
        if (!candidates) return VAN_LP_NONE;
        previous = bit;
        // Resolve duplicates and fetch metadata at RAK, one full TS BEFORE
        // R/W. Every reply candidate expects the same remaining R/W=1. This
        // keeps selection work out of the half-TS compare -> RTR deadline.
        if (rawTs == VAN_LP_RTR_TS - 2 && (candidates & VAN_LP_REPLY_MASK))
        {
            unsigned slot = 0;
            uint32_t replies = candidates >> VAN_LP_ENTRY_COUNT;
            while (!(replies & 1u)) { ++slot; replies >>= 1; }
            selected = &config->replies[slot];
            selectedWordCount = selected->frameWordCount;
            if (selectedWordCount < 6 || selectedWordCount > VAN_LP_FRAME_WORDS) goto abort;
            selectedRtr = (selected->words[wordIndex] >> (bitIndex - 2)) & 1u;
            if (selectedRtr != 0) goto abort;
        }
        if (rawTs == VAN_LP_RTR_TS - 1 && selected && (candidates & VAN_LP_REPLY_MASK))
        {
            // bitIndex is 2 for R/W. Drive the preloaded state from this SAME
            // array at RTR before setting up the remaining transmit loop.
            // Its setup now has a full TS instead of consuming the half-TS
            // R/W-center -> RTR-edge window. There is no buffer/index rebuild.
            --bitIndex;
            uint32_t rtrEdge = center + HALF_TS_CYCLES;
            if (!wait_until(rtrEdge)) goto abort;
            drive_bus(selectedRtr);
            return transmit_prepared(selected->words, selectedWordCount,
                                     wordIndex, bitIndex - 1, rtrEdge + TS_CYCLES, VAN_TX_RESPONDER, false);
        }
        if (--bitIndex < 0) { bitIndex = 9; ++wordIndex; }
        if (rawTs != VAN_LP_PREFIX_TS - 1) center += TS_CYCLES;
    }
    return track_response(center, (candidates & VAN_LP_ACK_MASK) != 0, false);
abort:
    release_bus();
    return VAN_LP_ABORT;
}

typedef struct {
    const volatile VanLpConfig* config;
    uint32_t highSince;
    uint32_t wasHigh;
    unsigned retries;
} VanMonitor;
static VanMonitor monitor;

static void monitor_once(void)
{
    // Only this between-transaction point adopts a bank. Acknowledging it
    // lets HP recycle the OLD bank; the new bank stays pinned for the frame.
    uint32_t published = VAN_CONFIG_PUBLISHED;
    if (published != VAN_CONFIG_APPLIED)
    {
        shared_fence();
        monitor.config = &VAN_CONFIG[published & 1];
        shared_fence();
        VAN_CONFIG_APPLIED = published;
    }
    uint32_t high = read_bus();
    uint32_t now = cycles();
    if (!high)
    {
        // An EOF-sized recessive run is enough to arm SOF reception. Own
        // TX is stricter below (EOF+IFS). This also tolerates edge polling
        // latency at the exact end of IFS, without searching inside DATA.
        if (monitor.wasHigh && now - monitor.highSince >= 8 * TS_CYCLES - DEADLINE_SLACK_CYCLES)
            VAN_BUS_RESULT = receive_frame(monitor.config, now);
        monitor.wasHigh = 0;
        return;
    }
    if (!monitor.wasHigh) { monitor.highSince = now; monitor.wasHigh = 1; }
    if (!VAN_START_TX || now - monitor.highSince < 16 * TS_CYCLES) return;
    // Incoming SOF takes priority until the actual TX start. Arbitration
    // handles a competing start occurring after this final GPIO check.
    if (!read_bus()) return;
    shared_fence();
    abortDetail = 0;
    responseEodTs = UINT32_MAX;
    VAN_TX_RX_TRACE_COUNT = 0;
    VanLpResult result;
    if (VAN_DATA_LENGTH < 6 || VAN_DATA_LENGTH > VAN_LP_FRAME_WORDS)
        result = abort_at(VAN_LP_ABORT_TX_LENGTH, 0);
    else if (VAN_FRAME_TYPE == 1)
    {
        // Begin SOF immediately after the final idle check. Do not spend an
        // extra slice waiting on a TX deadline with incoming monitoring off.
        unsigned wordCount = VAN_DATA_LENGTH;
        bool queryAck = monitor.config->queryAckEnabled != 0;
        if (!read_bus()) return;
        drive_bus(0); // SOF[0], identical for every VAN frame.
        result = transmit_prepared(VAN_DATA, wordCount, 0, 8,
                                   cycles() + TS_CYCLES, VAN_TX_REQUESTER, queryAck);
    }
    else
    {
        const unsigned wordCount = VAN_DATA_LENGTH;
        if (!read_bus()) return;
        drive_bus(0); // Same immediate SOF start as the requester path.
        result = transmit_prepared(VAN_DATA, wordCount, 0, 8,
                                   cycles() + TS_CYCLES, VAN_TX_NORMAL, false);
    }
    release_bus();
    monitor.wasHigh = 0;
    VAN_TX_RESULT = result;
    if (result == VAN_LP_ARBITRATION_LOST && VAN_FRAME_TYPE == 0)
        VAN_TX_ARBITRATION_TRACE = arbitrationTrace;
    // Pin the local-TX failure detail before continuous monitoring resumes.
    // Incoming frames can change abortDetail but not this completion snapshot.
    VAN_TX_ABORT_DETAIL = result == VAN_LP_ABORT ? abortDetail : 0;
    VAN_TX_EOD_TS = responseEodTs;
    if (result == VAN_LP_ARBITRATION_LOST && VAN_FRAME_TYPE == 0 && ++monitor.retries < VAN_RETRY_COUNT)
        return; // Retain pending TX, resume incoming monitoring before IFS.
    monitor.retries = 0;
    VAN_START_TX = 0;
    shared_fence();
    VAN_TX_FINISHED = 1;
}

#ifndef VAN_LP_HOST_TEST
int main(void)
{
    while (!VAN_START_APP) {}
    shared_fence();
    // Continuous polling needs no LP interrupts; prevent interrupt latency from
    // stretching ACK or disturbing RTR. HP configuration uses shared RAM only.
    __asm__ __volatile__("csrci mstatus, 8" ::: "memory");
    tsCycles = VAN_TS_CYCLES;
    VAN_RX_PIN = SET_VAN_RX_PIN;
    VAN_TX_PIN = SET_VAN_TX_PIN;
    rxMask = 1u << VAN_RX_PIN;
    txMask = 1u << VAN_TX_PIN;
    ulp_lp_core_gpio_init(VAN_RX_PIN);
    ulp_lp_core_gpio_input_enable(VAN_RX_PIN);
    ulp_lp_core_gpio_init(VAN_TX_PIN);
    release_bus();
    ulp_lp_core_gpio_output_enable(VAN_TX_PIN);
    ulp_lp_core_gpio_set_output_mode(VAN_TX_PIN, RTCIO_LL_OUTPUT_NORMAL);
    monitor.config = &VAN_CONFIG[0];
    monitor.highSince = cycles();
    for (;;) monitor_once();
}
#endif
#endif
