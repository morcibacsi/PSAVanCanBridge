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
volatile VanLpConfig VAN_CONFIG[2];
volatile uint32_t VAN_CONFIG_PUBLISHED, VAN_CONFIG_APPLIED;
volatile lp_io_num_t VAN_RX_PIN, VAN_TX_PIN;

// ESP-IDF's C6 LP delay implementation also assumes 16 MHz. These constants
// require the same clock; no HP clock/frequency scaling may alter the LP clock.
#define TS_CYCLES 128u
#define HALF_TS_CYCLES 64u
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
// The production normal TX function below deliberately still uses its original
// GPIO helpers; only the new deadline-driven paths use these operations.
INLINE uint32_t read_bus(void)
{
    return (*(volatile uint32_t*)LP_IO_IN_REG & rxMask) != 0;
}
INLINE void drive_bus(uint32_t level)
{
    *(volatile uint32_t*)(LP_IO_OUT_DATA_W1TC_REG - (level << 2)) = txMask;
}
#else
INLINE uint32_t read_bus(void) { return ulp_lp_core_gpio_get_level(VAN_RX_PIN) != 0; }
INLINE void drive_bus(uint32_t level) { ulp_lp_core_gpio_set_level(VAN_TX_PIN, level); }
#endif
INLINE void release_bus(void) { drive_bus(1); }

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
    while ((int32_t)((now = cycles()) - *sampleCycle) < 0)
    {
        if (read_bus() != previous)
        {
            // Reject an edge outside the middle half of the expected TS.
            int32_t error = (int32_t)(now - (*sampleCycle - HALF_TS_CYCLES));
            if (error < -32 || error > 32) return false;
            *sampleCycle = now + HALF_TS_CYCLES;
            return sample_at(*sampleCycle, bit);
        }
    }
    if ((int32_t)(now - *sampleCycle) > (int32_t)DEADLINE_SLACK_CYCLES) return false;
    *bit = read_bus();
    return true;
}

// Start with DATA[0] at raw index 30. The COM inverse has already been consumed.
// EOD is only legal at 48/49 + 10*N, N=0..28; never infer a nonexistent DLC.
static VanLpResult track_response(uint32_t sampleCycle, bool ack, bool query)
{
    uint32_t bit = 0, fourth = 0;
    unsigned phase = 0;
    unsigned pairParity = 0; // Even number of five-TS groups at byte boundary.
    for (unsigned rawTs = 30; rawTs < 330; ++rawTs)
    {
        sampleCycle += TS_CYCLES;
        if (phase == 4)
        {
            if (!sample_inverse(&sampleCycle, fourth, &bit)) goto abort;
            if (bit == fourth)
            {
                // 11 is malformed, not EOD. A full 15-bit FCS must fit before
                // the 00, and DATA always has a whole number of bytes.
                if (bit || rawTs < 49 || !pairParity) goto abort;
                if (ack)
                {
                    // At second EOD slot center. ACK[0] starts +0.5 TS;
                    // ACK[1] starts +1.5 TS. Drive only ACK[1], for ONE TS.
                    if (!sample_at(sampleCycle + TS_CYCLES, &bit) || !bit) goto abort;
                    uint32_t ackEdge = sampleCycle + TS_CYCLES + HALF_TS_CYCLES;
                    if (!wait_until(ackEdge)) goto abort;
                    drive_bus(0);
                    bool onTime = wait_until(ackEdge + TS_CYCLES);
                    release_bus();
                    if (!onTime) goto abort;
                }
                return query ? (ack ? VAN_LP_QUERY_RESPONSE_ACKED : VAN_LP_QUERY_RESPONSE_RECEIVED)
                             : (ack ? VAN_LP_NORMAL_FRAME_ACKED : VAN_LP_NONE);
            }
            phase = 0;
            pairParity ^= 1;
        }
        else
        {
            if (!sample_at(sampleCycle, &bit)) goto abort;
            if (phase == 3) fourth = bit;
            ++phase;
        }
    }
abort:
    release_bus();
    return VAN_LP_ABORT;
}

// Transmit using an existing full frame and an already established array index.
// Replies enter at word 2, bit 1 (= raw RTR index 28); requests enter at SOF.
// No copying or buffer replacement at RTR. ACK occupies bits 9/8 of last word.
INLINE VanLpResult transmit_prepared(const volatile uint32_t* words, unsigned wordCount,
                                     unsigned wordIndex, int bitIndex, uint32_t edgeCycle,
                                     bool requester, bool queryAck)
{
    if (wordCount < 6 || wordCount > VAN_LP_FRAME_WORDS) goto abort;
    uint32_t bit, bus;
    bool acknowledged = false;
    for (; wordIndex < wordCount; ++wordIndex, bitIndex = 9)
    {
        uint32_t word = words[wordIndex];
        for (; bitIndex >= 0; --bitIndex)
        {
            bit = (word >> bitIndex) & 1u;
            if (!wait_until(edgeCycle)) goto abort;
            drive_bus(bit);
            if (!sample_at(edgeCycle + HALF_TS_CYCLES, &bus)) goto abort;
            if (!bit && bus) goto abort;
            if (requester && wordIndex == 2 && bitIndex == 1 && !bus)
            {
                release_bus(); // RTR recessive lost: stay inside this frame.
                uint32_t center = edgeCycle + HALF_TS_CYCLES + TS_CYCLES;
                if (!sample_inverse(&center, 0, &bus) || bus != 1) goto abort;
                return track_response(center, queryAck, true);
            }
            if (wordIndex == wordCount - 1 && bitIndex == 8)
                acknowledged = !bus; // Bus was released by the prepared 1.
            else if (bit && !bus)
            {
                release_bus();
                return VAN_LP_ARBITRATION_LOST;
            }
            edgeCycle += TS_CYCLES;
        }
    }
    if (!wait_until(edgeCycle)) goto abort;
    release_bus();
    return requester ? VAN_LP_QUERY_NO_RESPONSE
                     : (acknowledged ? VAN_LP_REPLY_ACKNOWLEDGED : VAN_LP_REPLY_NOT_ACKNOWLEDGED);
abort:
    release_bus();
    return VAN_LP_ABORT;
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
                                     wordIndex, bitIndex - 1, rtrEdge + TS_CYCLES, false, false);
        }
        if (--bitIndex < 0) { bitIndex = 9; ++wordIndex; }
        if (rawTs != VAN_LP_PREFIX_TS - 1) center += TS_CYCLES;
    }
    return track_response(center, (candidates & VAN_LP_ACK_MASK) != 0, false);
abort:
    release_bus();
    return VAN_LP_ABORT;
}

    #define NOP1()  __asm__ __volatile__("nop")
    #define NOP2()  NOP1(); NOP1()
    #define NOP4()  NOP2(); NOP2()
    #define NOP8()  NOP4(); NOP4()
    #define NOP10()  NOP4(); NOP4(); NOP2()
    #define DELAY_10_CYCLES() NOP8(); NOP1(); NOP1()
    #define DELAY_15_CYCLES() NOP8(); NOP1(); NOP1()
    #define DELAY_20_CYCLES() DELAY_10_CYCLES(); DELAY_10_CYCLES()
    #define DELAY_25_CYCLES() DELAY_10_CYCLES(); DELAY_10_CYCLES(); NOP1(); NOP1(); NOP1(); NOP1(); NOP1()
    #define DELAY_50_CYCLES() DELAY_25_CYCLES(); DELAY_25_CYCLES()
    #define DELAY_75_CYCLES() DELAY_50_CYCLES(); DELAY_25_CYCLES()
    #define DELAY_90_CYCLES() DELAY_50_CYCLES(); DELAY_25_CYCLES(); DELAY_15_CYCLES()
    #define DELAY_100_CYCLES() DELAY_50_CYCLES(); DELAY_50_CYCLES()

    bool transmit_data_125kbps()
    {
        // warning: this function is written by observing the logic analyzer output to match the timings of a 125kbps signal
        // in case of any changes in this function, please verify the timings with a logic analyzer (1 bit = 8us)
        // every instruction takes some cycle so in case of modification the ulp_lp_core_delay_cycles() should be adjusted
        uint8_t bitToWrite;
        uint8_t prevBit = 0xFF; // invalid state to force first write
        uint8_t busLevel;

        for (int i = 0; i < VAN_DATA_LENGTH; i++)
        {
            for (int bitCounter = 9; bitCounter >= 0; bitCounter--)
            {
                bitToWrite = (VAN_DATA[i] >> bitCounter) & 1;

                if (bitToWrite != prevBit)
                {
                    ulp_lp_core_gpio_set_level(VAN_TX_PIN, bitToWrite);
                    prevBit = bitToWrite;
                }
                else
                {
                    DELAY_20_CYCLES();
                }

                if (bitToWrite == 1)
                {
                    DELAY_10_CYCLES();
                    busLevel = ulp_lp_core_gpio_get_level(VAN_RX_PIN);

                    if (busLevel == 0)
                    {
                        // If the bus is low then we lost arbitration, we need to stop the transmission
                        // A receiver owns only the SECOND ACK slot. Do not treat it
                        // as arbitration loss. This branch is off the normal TX
                        // success path, preserving its calibrated NOP cadence.
                        if (i != VAN_DATA_LENGTH - 1 || bitCounter != 8)
                            return false;
                    }

                    //DELAY_75_CYCLES();
                    ///*
                    if (bitCounter != 0)
                    {
                        DELAY_75_CYCLES();
                    }
                    else
                    {
                        DELAY_50_CYCLES();
                        DELAY_10_CYCLES();
                    }
                    //*/
                }
                else
                {
                    //DELAY_100_CYCLES();
                    ///*
                    if (bitCounter != 0)
                    {
                        DELAY_90_CYCLES();
                        NOP8();
                    }
                    else
                    {
                        DELAY_90_CYCLES();
                    }
                    //*/
                }
            }
        }

        ulp_lp_core_gpio_set_level(VAN_TX_PIN, 1);
        return true;
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
    VanLpResult result;
    if (VAN_DATA_LENGTH < 6 || VAN_DATA_LENGTH > VAN_LP_FRAME_WORDS)
        result = VAN_LP_ABORT;
    else if (VAN_FRAME_TYPE == 1)
    {
        // Begin SOF immediately after the final idle check. Do not spend an
        // extra slice waiting on a TX deadline with incoming monitoring off.
        unsigned wordCount = VAN_DATA_LENGTH;
        bool queryAck = monitor.config->queryAckEnabled != 0;
        if (!read_bus()) return;
        drive_bus(0); // SOF[0], identical for every VAN frame.
        result = transmit_prepared(VAN_DATA, wordCount, 0, 8,
                                   cycles() + TS_CYCLES, true, queryAck);
    }
    else
        result = transmit_data_125kbps() ? VAN_LP_NORMAL_TX_COMPLETED : VAN_LP_ARBITRATION_LOST;
    release_bus();
    monitor.wasHigh = 0;
    VAN_TX_RESULT = result;
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
