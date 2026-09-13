#include "../../src/lib/esp32_ulp_lpc_core_van_tx/VanFrameBuilder.hpp"
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <utility>
#include <algorithm>

extern "C" {
VanLpResult test_receive(const VanLpConfig*, uint32_t);
VanLpResult test_request(const VanLpFrame*, uint32_t, int);
void test_monitor_reset(void);
void test_monitor_step(void);
int test_inverse(uint32_t*, uint32_t, uint32_t*);
uint32_t test_abort_detail(void);
uint32_t test_eod_ts(void);
uint32_t test_ack_edge_advance(void);
uint32_t test_ack_hold_extension(void);
void test_set_ts_cycles(uint32_t);
VanLpResult test_track_response(uint32_t);
extern volatile uint32_t VAN_DATA[], VAN_DATA_LENGTH, VAN_START_TX, VAN_TX_FINISHED;
extern volatile uint32_t VAN_FRAME_TYPE, VAN_TX_RESULT, VAN_BUS_RESULT;
extern volatile uint32_t VAN_TX_ABORT_DETAIL;
extern volatile uint32_t VAN_TX_EOD_TS;
extern volatile VanLpRxTrace VAN_TX_RX_TRACE[VAN_LP_RX_TRACE_COUNT];
extern volatile uint32_t VAN_TX_RX_TRACE_COUNT;
extern volatile uint32_t VAN_RETRY_COUNT;
extern volatile VanLpArbitrationTrace VAN_TX_ARBITRATION_TRACE;
extern volatile uint32_t VAN_CONFIG_PUBLISHED, VAN_CONFIG_APPLIED;
extern volatile VanLpConfig VAN_CONFIG[2];
}

static uint64_t now, origin;
static unsigned txLevel;
static std::vector<unsigned> incoming;
static std::vector<std::pair<uint64_t, unsigned>> writes;
static uint32_t injectedDelay;
static uint64_t delayAt;
static unsigned wireTsCycles = 128;
static bool publishDuringFrame;
static uint64_t publishAt;
static bool alignQueryOrigin;
static unsigned gpioReadCycles;
static unsigned txReleaseDelayCycles;
static uint64_t txRecessiveAt;

extern "C" uint32_t van_test_cycles()
{
    if (publishDuringFrame && now >= publishAt) { VAN_CONFIG_PUBLISHED = 1; publishDuringFrame = false; }
    if (injectedDelay && now >= delayAt) { now += injectedDelay; injectedDelay = 0; }
    return static_cast<uint32_t>(now++);
}
extern "C" uint32_t ulp_lp_core_gpio_get_level(unsigned)
{
    const uint64_t ts = now >= origin ? (now - origin) / wireTsCycles : UINT64_MAX;
    const unsigned localLevel = txLevel && now >= txRecessiveAt;
    const unsigned level = localLevel & (ts < incoming.size() ? incoming[ts] : 1u);
    // Model time spent completing a GPIO read/poll. This is a latency stress
    // model, not an instruction-accurate ESP32-C6 emulator.
    now += gpioReadCycles;
    return level;
}
extern "C" void ulp_lp_core_gpio_set_level(unsigned, uint8_t level)
{
    if (alignQueryOrigin && level == 0) { origin = now; alignQueryOrigin = false; }
    if (level != txLevel) writes.emplace_back(now, level);
    if (level && !txLevel) txRecessiveAt = now + txReleaseDelayCycles;
    else if (!level) txRecessiveAt = UINT64_MAX;
    txLevel = level;
}
extern "C" void ulp_lp_core_delay_us(uint32_t us) { now += 16 * us; }

static std::vector<unsigned> raw(const VanLpFrame& frame)
{
    std::vector<unsigned> bits;
    for (unsigned w = 0; w < frame.frameWordCount; ++w)
        for (int b = 9; b >= 0; --b) bits.push_back((frame.words[w] >> b) & 1);
    return bits;
}
static VanLpFrame frame(uint16_t id, uint8_t com, unsigned length = 3)
{
    uint8_t bytes[28];
    for (unsigned i = 0; i < 28; ++i) bytes[i] = static_cast<uint8_t>(i * 37);
    VanLpFrame result = {};
    assert(VanFrameBuilder::Build(id, com, bytes, length, result));
    return result;
}
static void reset(const std::vector<unsigned>& bits, uint64_t start = 128)
{
    now = start - 64;
    origin = start;
    txLevel = 1;
    txReleaseDelayCycles = 0;
    txRecessiveAt = 0;
    writes.clear();
    incoming = bits;
    injectedDelay = 0;
    publishDuringFrame = false;
    alignQueryOrigin = false;
    wireTsCycles = 128;
    test_set_ts_cycles(128);
    gpioReadCycles = 0;
}
static void check_ack(unsigned ackStartTs)
{
    assert(writes.size() == 2 && writes[0].second == 0 && writes[1].second == 1);
    const int64_t error = writes[0].first
                        - (origin + (ackStartTs + 1) * 128 - test_ack_edge_advance());
    assert(error >= -4 && error <= 8);
    assert(writes[1].first - writes[0].first == 128 + test_ack_hold_extension());
    assert(txLevel == 1);
}

// Independent bit-by-bit polynomial reference, deliberately not the builder's
// byte loop or packed-CRC EOD trick.
static std::vector<unsigned> reference(uint16_t id, uint8_t com, const uint8_t* data, unsigned count)
{
    std::vector<unsigned> logical;
    for (int i = 11; i >= 0; --i) logical.push_back((id >> i) & 1);
    for (int i = 3; i >= 0; --i) logical.push_back((com >> i) & 1);
    for (unsigned i = 0; i < count; ++i)
        for (int b = 7; b >= 0; --b) logical.push_back((data[i] >> b) & 1);
    unsigned crc = 0x7fff;
    for (unsigned bit : logical)
    {
        unsigned feedback = (crc >> 14) ^ bit;
        crc = (crc << 1) & 0x7fff;
        if (feedback) crc ^= 0xf9d;
    }
    crc ^= 0x7fff;
    for (int i = 14; i >= 0; --i) logical.push_back((crc >> i) & 1);
    std::vector<unsigned> result = {0,0,0,0,1,1,1,1,0,1};
    for (unsigned i = 0; i < logical.size(); ++i)
    {
        result.push_back(logical[i]);
        if (i % 4 == 3) result.push_back(!logical[i]);
    }
    result.insert(result.end(), {0,0,1,1,1,1,1,1,1,1,1,1});
    return result;
}

int main()
{
    // Reported normal frames must run through the actual monitor/normal TX
    // branch to completion. Check GPIO transition order, not host NOP timing.
    const uint8_t normal5e4[] = {0x20, 0x1e};
    const uint8_t normal8a4[] = {0x8f, 0x47, 0xff, 0x38, 0x31, 0x0f, 0xff};
    for (bool longer : {false, true})
    {
        const uint16_t id = longer ? 0x8a4 : 0x5e4;
        const uint8_t com = longer ? 0x8 : 0xc;
        const uint8_t* payload = longer ? normal8a4 : normal5e4;
        const unsigned length = longer ? sizeof(normal8a4) : sizeof(normal5e4);
        VanLpFrame normal = {};
        assert(VanFrameBuilder::Build(id, com, payload, length, normal));
        auto expected = reference(id, com, payload, length);
        assert(raw(normal) == expected);
        std::vector<uint8_t> crcInput = {static_cast<uint8_t>(id >> 4),
                                       static_cast<uint8_t>((id << 4) | com)};
        crcInput.insert(crcInput.end(), payload, payload + length);
        assert(VanFrameBuilder::Crc15(crcInput.data(), crcInput.size()) == (longer ? 0x39fc : 0x8a50));
        reset({});
        for (unsigned i = 0; i < normal.frameWordCount; ++i) VAN_DATA[i] = normal.words[i];
        VAN_DATA_LENGTH = normal.frameWordCount;
        VAN_FRAME_TYPE = 0; VAN_START_TX = 1; VAN_TX_FINISHED = 0;
        test_monitor_reset();
        while (!VAN_TX_FINISHED && now < 100000) test_monitor_step();
        assert(VAN_TX_FINISHED && !VAN_START_TX);
        assert(VAN_TX_RESULT == VAN_LP_NORMAL_TX_COMPLETED && txLevel == 1);
        size_t transition = 0;
        unsigned previous = 1;
        for (unsigned level : expected)
            if (level != previous)
            {
                assert(transition < writes.size() && writes[transition].second == level);
                previous = level;
                ++transition;
            }
        assert(transition == writes.size());
    }
    // C6 hardware reports stale dominant RX after a commanded recessive
    // transition. The late same-slice confirmation must accept delayed local
    // readback without weakening genuine dominant arbitration below.
    {
        VanLpFrame delayed = {};
        assert(VanFrameBuilder::Build(0x5e4, 0xc, normal5e4, sizeof(normal5e4), delayed));
        reset({});
        txReleaseDelayCycles = 96;
        gpioReadCycles = 8;
        for (unsigned i = 0; i < delayed.frameWordCount; ++i) VAN_DATA[i] = delayed.words[i];
        VAN_DATA_LENGTH = delayed.frameWordCount;
        VAN_FRAME_TYPE = 0; VAN_START_TX = 1; VAN_TX_FINISHED = 0;
        test_monitor_reset();
        while (!VAN_TX_FINISHED && now < 100000) test_monitor_step();
        assert(VAN_TX_FINISHED && !VAN_START_TX && txLevel == 1);
        assert(VAN_TX_RESULT == VAN_LP_NORMAL_TX_COMPLETED);
    }
    // Preserve the exact arbitration location through asynchronous completion.
    // Exercise all recessive slices, especially the first SOF transition at 4.
    auto arbitrationFrame = frame(0x5e4, 0xc, 2);
    auto arbitrationBits = raw(arbitrationFrame);
    for (unsigned ts = 0; ts < arbitrationBits.size(); ++ts)
        if (arbitrationBits[ts] && ts != arbitrationBits.size() - 9)
        {
            std::vector<unsigned> competitor(arbitrationBits.size(), 1);
            competitor[ts] = 0;
            reset(competitor, UINT64_MAX / 2);
            now = 0;
            alignQueryOrigin = true;
            for (unsigned i = 0; i < arbitrationFrame.frameWordCount; ++i) VAN_DATA[i] = arbitrationFrame.words[i];
            VAN_DATA_LENGTH = arbitrationFrame.frameWordCount;
            VAN_FRAME_TYPE = 0; VAN_START_TX = 1; VAN_TX_FINISHED = 0;
            VAN_RETRY_COUNT = 1;
            test_monitor_reset();
            while (!VAN_TX_FINISHED && now < 100000) test_monitor_step();
            assert(VAN_TX_FINISHED && !VAN_START_TX && txLevel == 1);
            assert(VAN_TX_RESULT == VAN_LP_ARBITRATION_LOST);
            assert(VAN_TX_ARBITRATION_TRACE.rawTs == ts);
            assert(VAN_TX_ARBITRATION_TRACE.sampleOffset >= 64);
            assert(VAN_TX_ARBITRATION_TRACE.outputBeforeRelease == 1);
            assert(VAN_TX_ARBITRATION_TRACE.outputReadOffset > VAN_TX_ARBITRATION_TRACE.sampleOffset);
            assert(VAN_TX_ARBITRATION_TRACE.releaseOffset >= 64);
            assert(VAN_TX_ARBITRATION_TRACE.releaseOffset > VAN_TX_ARBITRATION_TRACE.outputReadOffset);
            assert(VAN_TX_ARBITRATION_TRACE.readOffset > VAN_TX_ARBITRATION_TRACE.releaseOffset);
            assert(VAN_TX_ARBITRATION_TRACE.rxAfterRelease == 0);
            assert(VAN_TX_ARBITRATION_TRACE.outputAfterRelease == 1);
            assert(VAN_TX_ARBITRATION_TRACE.rxRecessiveOffset >= 128);
            assert(VAN_TX_ARBITRATION_TRACE.rxRecessiveOffset < 256);
            VanLpConfig noMatches = {};
            reset(raw(frame(0x8c4, 0xc)));
            test_receive(&noMatches, origin);
            assert(VAN_TX_ARBITRATION_TRACE.rawTs == ts);
        }
    // Exact 27-byte immediate response supplied in the 0x564 hardware report.
    const uint8_t captured564[] = {
        0x80,0,0,0,0,0,0,0,0,0,0xe1,0x2a,0x2a,0x09,0x1c,0x94,
        0,0x46,0x1c,0x94,0,0x46,0x01,0x2c,0xff,0xff,0x80
    };
    VanLpFrame capturedResponse = {}, capturedRequest = {};
    assert(VanFrameBuilder::Build(0x564, 0xe, captured564, sizeof(captured564), capturedResponse));
    assert(VanFrameBuilder::Build(0x564, 0xf, nullptr, 0, capturedRequest));
    uint8_t crcInput[sizeof(captured564) + 2] = {0x56, 0x4e};
    memcpy(crcInput + 2, captured564, sizeof(captured564));
    assert(VanFrameBuilder::Crc15(crcInput, sizeof(crcInput)) == 0x9f82);
    auto capturedBus = raw(capturedResponse);
    std::fill(capturedBus.begin(), capturedBus.begin() + 28, 1);
    reset(capturedBus);
    assert(test_request(&capturedRequest, origin, 1) == VAN_LP_QUERY_RESPONSE_ACKED);

    // An inversion just after the edge-search cutoff is still visible at the
    // center sample. It must correct phase instead of silently free-running.
    // Small clock mismatch plus an early handover reproduces false EOD at 49.
    reset(capturedBus);
    wireTsCycles = 129;
    uint32_t earlyHandover = static_cast<uint32_t>(origin + 29 * 129 + 64 - 48);
    now = earlyHandover + 1;
    const VanLpResult lateEdgeResult = test_track_response(earlyHandover);
    if (lateEdgeResult != VAN_LP_QUERY_RESPONSE_ACKED || test_eod_ts() != 319)
        fprintf(stderr, "Late-edge regression: result=%u eod=%u detail=0x%08x\n",
               lateEdgeResult, test_eod_ts(), test_abort_detail());
    assert(lateEdgeResult == VAN_LP_QUERY_RESPONSE_ACKED && test_eod_ts() == 319);
    assert(VAN_TX_RX_TRACE[0].correction > 0); // Previously zero at every pair.
    for (unsigned period : {128u, 129u, 130u})
        for (unsigned early : {32u, 40u, 48u, 56u})
            for (unsigned latency : {0u, 8u, 16u, 24u})
            {
                reset(capturedBus);
                wireTsCycles = period;
                gpioReadCycles = latency;
                const uint32_t center = static_cast<uint32_t>(origin + 29 * period + period / 2 - early);
                now = center + 1;
                assert(test_track_response(center) == VAN_LP_QUERY_RESPONSE_ACKED);
                assert(test_eod_ts() == 319);
                const int64_t ackError = static_cast<int64_t>(writes.front().first)
                                       - static_cast<int64_t>(origin + 321 * period
                                                              - test_ack_edge_advance());
                assert(ackError >= -32 && ackError <= 32);
                assert(writes.size() == 2
                       && writes[1].first - writes[0].first == 128 + test_ack_hold_extension());
            }

    // EOD has NO inversion edge. A GPIO polling iteration straddling the
    // sample center must not create a spurious deadline failure.
    for (unsigned latency = 0; latency <= 24; ++latency)
        for (unsigned startOffset = 0; startOffset < 32; ++startOffset)
        {
            reset(std::vector<unsigned>(100, 0));
            now = origin + 49 * 128;
            uint32_t eodCenter = static_cast<uint32_t>(now + 64), eodBit = 1;
            now += startOffset;
            gpioReadCycles = latency;
            assert(test_inverse(&eodCenter, 0, &eodBit));
            assert(eodBit == 0);
            assert(eodCenter == origin + 49 * 128 + 64);
        }
    reset(capturedBus);
    gpioReadCycles = 20;
    assert(test_request(&capturedRequest, origin, 1) == VAN_LP_QUERY_RESPONSE_ACKED);
    assert(txLevel == 1);
    assert(writes.back().first - writes[writes.size()-2].first
           == 128 + test_ack_hold_extension());

    // Updated hardware capture: valid EOD is at 318/319, not 53/54.
    uint8_t captured564Latest[sizeof(captured564)];
    memcpy(captured564Latest, captured564, sizeof(captured564));
    captured564Latest[0] = captured564Latest[26] = 0x81;
    captured564Latest[10] = 0xe0;
    memcpy(crcInput + 2, captured564Latest, sizeof(captured564Latest));
    assert(VanFrameBuilder::Crc15(crcInput, sizeof(crcInput)) == 0xb048);
    VanLpFrame latestResponse = {};
    assert(VanFrameBuilder::Build(0x564, 0xe, captured564Latest, sizeof(captured564Latest), latestResponse));
    auto latestBus = raw(latestResponse);
    assert(latestBus[53] == 0 && latestBus[54] == 1);
    std::fill(latestBus.begin(), latestBus.begin() + 28, 1);
    for (unsigned latency = 0; latency <= 24; ++latency)
    {
        reset(latestBus);
        gpioReadCycles = latency;
        assert(test_request(&capturedRequest, origin, 1) == VAN_LP_QUERY_RESPONSE_ACKED);
        assert(test_eod_ts() == 319);
        const auto& ackWrite = writes[writes.size() - 2];
        assert(ackWrite.second == 0);
        assert(ackWrite.first >= origin + 321 * 128 - test_ack_edge_advance());
        assert(ackWrite.first < origin + 321 * 128 - test_ack_edge_advance() + 32);
        assert(writes.back().first - ackWrite.first
               == 128 + test_ack_hold_extension());
    }
    for (unsigned level : {0u, 1u})
    {
        auto malformed = latestBus;
        malformed[53] = malformed[54] = level;
        reset(malformed);
        assert(test_request(&capturedRequest, origin, 1) == VAN_LP_ABORT);
        assert(test_abort_detail() == ((VAN_LP_ABORT_RX_EOD_INVALID << 16) | 54));
        assert(txLevel == 1 && writes.back().first < origin + 30 * 128);
    }
    // Both latest replies decode cleanly. A successful software result alone
    // cannot tell whether an erroneous sampled pair was classified as EOD.
    for (const auto& capture : {std::make_pair(0x86u, 0xbfdcu), std::make_pair(0x83u, 0x5a94u),
                               std::make_pair(0x84u, 0x5500u), std::make_pair(0x81u, 0xeaecu),
                               std::make_pair(0x80u, 0xc526u)})
    {
        captured564Latest[0] = captured564Latest[26] = capture.first;
        captured564Latest[10] = capture.first == 0x83 || capture.first == 0x80 ? 0xe0 : 0xe1;
        memcpy(crcInput + 2, captured564Latest, sizeof(captured564Latest));
        assert(VanFrameBuilder::Crc15(crcInput, sizeof(crcInput)) == capture.second);
        assert(VanFrameBuilder::Build(0x564, 0xe, captured564Latest, sizeof(captured564Latest), latestResponse));
        auto bus = raw(latestResponse);
        std::fill(bus.begin(), bus.begin() + 28, 1);
        reset(bus);
        assert(test_request(&capturedRequest, origin, 1) == VAN_LP_QUERY_RESPONSE_ACKED);
        assert(test_eod_ts() == 319);
        assert(VAN_TX_RX_TRACE_COUNT == VAN_LP_RX_TRACE_COUNT);
        for (unsigned i = 0; i < VAN_LP_RX_TRACE_COUNT; ++i)
        {
            const unsigned ts = 34 + i * 5;
            assert(VAN_TX_RX_TRACE[i].rawTs == ts);
            assert(VAN_TX_RX_TRACE[i].pair == ((bus[ts-1] << 2) | bus[ts]));
            assert(VAN_TX_RX_TRACE[i].fourthLate >= 0 && VAN_TX_RX_TRACE[i].fourthLate <= 4);
            assert(VAN_TX_RX_TRACE[i].inverseLate >= 0 && VAN_TX_RX_TRACE[i].inverseLate <= 4);
        }
    }
    // A failed pair is retained, and incoming monitoring cannot overwrite it.
    auto traceFailure = latestBus;
    traceFailure[53] = traceFailure[54] = 0;
    reset(traceFailure);
    assert(test_request(&capturedRequest, origin, 1) == VAN_LP_ABORT);
    assert(VAN_TX_RX_TRACE_COUNT == 5);
    assert(VAN_TX_RX_TRACE[4].rawTs == 54 && VAN_TX_RX_TRACE[4].pair == 0);
    VanLpConfig traceConfig = {};
    assert(VanFrameBuilder::SetAck(traceConfig, 0, 0x564, 0xc, true));
    reset(raw(frame(0x564, 0xc)));
    assert(test_receive(&traceConfig, origin) == VAN_LP_NORMAL_FRAME_ACKED);
    assert(VAN_TX_RX_TRACE_COUNT == 5 && VAN_TX_RX_TRACE[4].rawTs == 54);
    // The RTC_FAST RC oscillator is calibrated by HP, not assumed 16 MHz.
    // Check requester timing and ACK location/width at each accepted cycle count.
    for (unsigned period = 96; period <= 192; ++period)
    {
        reset(latestBus);
        wireTsCycles = period;
        test_set_ts_cycles(period);
        assert(test_request(&capturedRequest, origin, 1) == VAN_LP_QUERY_RESPONSE_ACKED);
        assert(test_eod_ts() == 319);
        const uint64_t ackStart = writes[writes.size()-2].first;
        assert(ackStart >= origin + 321 * period - test_ack_edge_advance() - 1);
        assert(ackStart <= origin + 321 * period - test_ack_edge_advance() + 8);
        assert(writes.back().first - ackStart == period + test_ack_hold_extension());
    }
    // Start at an accurately known handover, then use the wrong 16 MHz
    // assumption for reception. A clean reply must not be reported as received.
    reset(latestBus);
    wireTsCycles = 144;
    uint32_t handoverCenter = static_cast<uint32_t>(origin + 29 * 144 + 72);
    now = handoverCenter + 1;
    assert(test_track_response(handoverCenter) == VAN_LP_ABORT);
    reset(latestBus);
    wireTsCycles = 144;
    test_set_ts_cycles(144);
    now = handoverCenter + 1;
    assert(test_track_response(handoverCenter) == VAN_LP_QUERY_RESPONSE_ACKED);
    assert(test_eod_ts() == 319);

    // Artificial early EOD followed by a recessive ACK[0]: exposes why result
    // 5 needs its detected boundary logged. This is not a hardware capture.
    auto falseEodBus = latestBus;
    falseEodBus[48] = falseEodBus[49] = 0;
    falseEodBus[50] = 1;
    reset(falseEodBus);
    assert(test_request(&capturedRequest, origin, 1) == VAN_LP_QUERY_RESPONSE_ACKED);
    assert(test_eod_ts() == 49);
    assert(writes[writes.size()-2].first < origin + 52 * 128);

    // Detailed failure reporting remains actionable if hardware still aborts.
    reset(capturedBus);
    delayAt = origin + 43 * 128;
    injectedDelay = 300;
    assert(test_request(&capturedRequest, origin, 1) == VAN_LP_ABORT);
    assert((test_abort_detail() >> 16) == VAN_LP_ABORT_RX_SAMPLE_DEADLINE);
    assert((test_abort_detail() & 0xffffu) == 43);
    assert(txLevel == 1);

    // Encoding, maximum buffer, offsets, CRC and normal-frame regression.
    for (unsigned byte = 0; byte < 256; ++byte)
    {
        unsigned expanded = 0;
        for (int bit = 7; bit >= 0; --bit)
        {
            expanded = (expanded << 1) | ((byte >> bit) & 1);
            if (bit == 4 || bit == 0) expanded = (expanded << 1) | !((byte >> bit) & 1);
        }
        assert(expanded == VanFrameBuilder::Stuff(byte));
    }
    for (unsigned length = 0; length <= 28; ++length)
        for (unsigned com : {8u, 12u, 14u, 15u})
        {
            uint8_t payload[28];
            for (unsigned i = 0; i < 28; ++i) payload[i] = (length * 47 + i * 73) & 255;
            VanLpFrame built = {};
            assert(VanFrameBuilder::Build(0x8c4, com, payload, length, built));
            assert(raw(built) == reference(0x8c4, com, payload, length));
            assert(built.frameWordCount == length + 6);
            assert(VanFrameBuilder::AckStartTs(built) == 50 + 10 * length);
        }
    const uint8_t sample[] = {0x8c, 0x4c, 0x8a, 0x21, 0x40};
    assert(VanFrameBuilder::Crc15(sample, sizeof(sample)) == 0x3d54);
    VanLpFrame invalid = {};
    assert(!VanFrameBuilder::Build(0x1000, 0xc, nullptr, 0, invalid));
    assert(!VanFrameBuilder::Build(0, 0xc, nullptr, 1, invalid));
    assert(!VanFrameBuilder::Build(0, 0xc, sample, 29, invalid));
    for (unsigned id = 0; id <= 0xfff; ++id)
    {
        VanLpFrame built = {};
        assert(VanFrameBuilder::Build(id, 0xc, nullptr, 0, built));
        assert(raw(built) == reference(id, 0xc, nullptr, 0));
    }

    // The application-facing slot model maps directly to the five fixed LP
    // entries: replacement/removal is explicit and never reallocates peers.
    VanLpConfig replySlots = {};
    const uint8_t initialReply[] = {0x80, 0x01, 0x55};
    for (unsigned slot = 0; slot < VAN_LP_ENTRY_COUNT; ++slot)
        assert(VanFrameBuilder::SetReply(replySlots, slot, 0x9c0 + slot,
                                         initialReply, sizeof(initialReply), true));

    VanLpConfig beforeUpdate = replySlots;
    const uint8_t replacementReply[] = {0x40, 0x02, 0xaa, 0x17};
    assert(VanFrameBuilder::SetReply(replySlots, 2, 0xa52, replacementReply,
                                     sizeof(replacementReply), true));
    const uint32_t updatedCandidate = 1u << (VAN_LP_ENTRY_COUNT + 2);
    for (unsigned slot = 0; slot < VAN_LP_ENTRY_COUNT; ++slot)
        if (slot != 2)
            assert(memcmp(&replySlots.replies[slot], &beforeUpdate.replies[slot],
                          sizeof(replySlots.replies[slot])) == 0);
    for (unsigned ts = 0; ts < VAN_LP_PREFIX_TS; ++ts)
        for (unsigned level = 0; level < 2; ++level)
            assert(((replySlots.matchMask[ts][level] ^ beforeUpdate.matchMask[ts][level])
                    & ~updatedCandidate) == 0);

    VanLpConfig beforeDisable = replySlots;
    assert(VanFrameBuilder::SetReply(replySlots, 3, 0, nullptr, 0, false));
    assert(replySlots.replies[3].frameWordCount == 0);
    const uint32_t disabledCandidate = 1u << (VAN_LP_ENTRY_COUNT + 3);
    for (unsigned slot = 0; slot < VAN_LP_ENTRY_COUNT; ++slot)
        if (slot != 3)
            assert(memcmp(&replySlots.replies[slot], &beforeDisable.replies[slot],
                          sizeof(replySlots.replies[slot])) == 0);
    for (unsigned ts = 0; ts < VAN_LP_PREFIX_TS; ++ts)
        for (unsigned level = 0; level < 2; ++level)
            assert(((replySlots.matchMask[ts][level] ^ beforeDisable.matchMask[ts][level])
                    & ~disabledCandidate) == 0);

    VanLpConfig beforeInvalidSlot = replySlots;
    assert(!VanFrameBuilder::SetReply(replySlots, VAN_LP_ENTRY_COUNT, 0x777,
                                      initialReply, sizeof(initialReply), true));
    assert(memcmp(&replySlots, &beforeInvalidSlot, sizeof(replySlots)) == 0);

    VanLpConfig config = {};
    reset(raw(frame(0x8c4, 0xc)));
    assert(test_receive(&config, origin) == VAN_LP_NONE && writes.empty());
    for (unsigned slot = 0; slot < VAN_LP_ENTRY_COUNT; ++slot)
    {
        assert(VanFrameBuilder::SetAck(config, slot, 0x8c0 + slot, 0xc, true));
        const uint8_t data[] = {0x80, 0x01, 0x55};
        assert(VanFrameBuilder::SetReply(config, slot, 0x9c0 + slot, data, sizeof(data), true));
    }
    assert(config.enabledMask == 0x3ff);
    assert(!VanFrameBuilder::SetAck(config, VAN_LP_ENTRY_COUNT, 0, 0xc, true));
    assert(!VanFrameBuilder::SetAck(config, 0, 0, 0x8, true));
    assert(!VanFrameBuilder::SetReply(config, VAN_LP_ENTRY_COUNT, 0, nullptr, 0, true));

    for (unsigned slot = 0; slot < VAN_LP_ENTRY_COUNT; ++slot)
        for (unsigned length = 0; length <= 28; ++length)
        {
            VanLpFrame incomingFrame = frame(0x8c0 + slot, 0xc, length);
            reset(raw(incomingFrame));
            assert(test_receive(&config, origin) == VAN_LP_NORMAL_FRAME_ACKED);
            check_ack(VanFrameBuilder::AckStartTs(incomingFrame));
        }
    for (unsigned com : {0u, 8u, 10u, 11u, 13u, 14u, 15u})
    {
        reset(raw(frame(0x8c4, com)));
        test_receive(&config, origin);
        assert(writes.empty());
    }
    reset(raw(frame(0x8cf, 0xc))); // Shared prefix then different last ID bits.
    assert(test_receive(&config, origin) == VAN_LP_NONE && writes.empty());
    assert(VanFrameBuilder::SetAck(config, 4, 0xaaa, 0xc, true));
    reset(raw(frame(0x8c4, 0xc)));
    assert(test_receive(&config, origin) == VAN_LP_NONE && writes.empty());
    assert(VanFrameBuilder::SetAck(config, 4, 0, 0xc, false));

    // Reply takeover from the same complete frame. Requester supplies ONLY the
    // prefix through RTR=1; no external transmitter supplies the response.
    for (unsigned slot = 0; slot < VAN_LP_ENTRY_COUNT; ++slot)
        for (bool ack : {false, true})
        {
            auto request = raw(frame(0x9c0 + slot, 0xf, 0));
            request.resize(config.replies[slot].frameWordCount * 10, 1);
            std::fill(request.begin() + 29, request.end(), 1);
            if (ack) request[VanFrameBuilder::AckStartTs(config.replies[slot]) + 1] = 0;
            reset(request);
            VanLpResult result = test_receive(&config, origin);
            assert(result == (ack ? VAN_LP_REPLY_ACKNOWLEDGED : VAN_LP_REPLY_NOT_ACKNOWLEDGED));
            assert(writes.front().second == 0);
            assert(writes.front().first - origin >= 28 * 128 && writes.front().first - origin <= 28 * 128 + 8);
            const auto expected = raw(config.replies[slot]);
            unsigned level = 1, pos = 0;
            for (unsigned ts = 28; ts < expected.size(); ++ts)
            {
                while (pos < writes.size() && writes[pos].first <= origin + ts * 128 + 64)
                    level = writes[pos++].second;
                assert(level == expected[ts]); // Includes release through ACK and EOF.
            }
            assert(txLevel == 1);
        }
    // Removing/replacing replies must not affect ACK entries or other replies.
    assert(VanFrameBuilder::SetReply(config, 0, 0xabc, nullptr, 0, true));
    reset(raw(frame(0x9c0, 0xf, 0)));
    test_receive(&config, origin); assert(writes.empty());
    assert(VanFrameBuilder::SetReply(config, 0, 0, nullptr, 0, false));

    // Requester: no responder; immediate response; ACK disabled; all lengths.
    auto request = frame(0x8c4, 0xf, 0);
    reset({});
    assert(test_request(&request, origin, 1) == VAN_LP_QUERY_NO_RESPONSE);
    for (unsigned length = 0; length <= 28; ++length)
    {
        auto response = frame(0x8c4, 0xe, length);
        auto bus = raw(response);
        std::fill(bus.begin(), bus.begin() + 28, 1);
        reset(bus);
        assert(test_request(&request, origin, 1) == VAN_LP_QUERY_RESPONSE_ACKED);
        assert(writes[writes.size()-2].first >= origin
               + (VanFrameBuilder::AckStartTs(response) + 1) * 128
               - test_ack_edge_advance());
        assert(writes.back().first - writes[writes.size()-2].first
               == 128 + test_ack_hold_extension());
        reset(bus);
        assert(test_request(&request, origin, 0) == VAN_LP_QUERY_RESPONSE_RECEIVED);
        assert(writes.back().first < origin + 30 * 128);
    }
    auto conflict = std::vector<unsigned>(60, 1);
    conflict[10] = 0; // Our identifier begins recessive: another sender wins.
    reset(conflict);
    assert(test_request(&request, origin, 1) == VAN_LP_ARBITRATION_LOST && txLevel == 1);
    VanLpConfig corrupt = config;
    corrupt.replies[1].frameWordCount = 35;
    reset(raw(frame(0x9c1, 0xf, 0)));
    assert(test_receive(&corrupt, origin) == VAN_LP_ABORT && writes.empty());
    // Invalid 11 inversion and premature 00 must never cause ACK.
    for (unsigned bit : {0u, 1u})
    {
        auto bad = raw(frame(0x8c0, 0xc));
        bad[33] = bad[34] = bit;
        reset(bad);
        assert(test_receive(&config, origin) == VAN_LP_ABORT && txLevel == 1 && writes.empty());
    }
    reset(raw(frame(0x8c0, 0xc)));
    delayAt = origin + 42 * 128;
    injectedDelay = 300;
    assert(test_receive(&config, origin) == VAN_LP_ABORT && txLevel == 1);
    auto badAck = frame(0x8c0, 0xc);
    auto badAckBits = raw(badAck);
    badAckBits[VanFrameBuilder::AckStartTs(badAck)] = 0;
    reset(badAckBits);
    assert(test_receive(&config, origin) == VAN_LP_ABORT && writes.empty());
    // Cycle counter wrap and clock mismatch: inversion edges re-center RX.
    reset(raw(frame(0x8c0, 0xc, 28)), UINT32_MAX - 1000ull);
    assert(test_receive(&config, static_cast<uint32_t>(origin)) == VAN_LP_NORMAL_FRAME_ACKED);
    for (unsigned period : {126u, 130u})
    {
        reset(raw(frame(0x8c0, 0xc, 28)));
        wireTsCycles = period;
        assert(test_receive(&config, origin) == VAN_LP_NORMAL_FRAME_ACKED);
    }

    // Actual monitor iteration: pending TX is untouched by ACK/reply traffic.
    for (bool reply : {false, true})
    {
        VanLpConfig active = {};
        VanFrameBuilder::SetAck(active, 0, 0x8c0, 0xc, true);
        VanFrameBuilder::SetReply(active, 0, 0x9c0, sample, sizeof(sample), true);
        auto input = raw(frame(reply ? 0x9c0 : 0x8c0, reply ? 0xf : 0xc));
        if (reply) { input.resize(active.replies[0].frameWordCount * 10, 1); std::fill(input.begin()+29, input.end(), 1); }
        reset(input, 1600); now = 0;
        const uint32_t* source = reinterpret_cast<const uint32_t*>(&active);
        volatile uint32_t* dest = reinterpret_cast<volatile uint32_t*>(&VAN_CONFIG[0]);
        for (unsigned i = 0; i < sizeof(active)/4; ++i) dest[i] = source[i];
        dest = reinterpret_cast<volatile uint32_t*>(&VAN_CONFIG[1]);
        for (unsigned i = 0; i < sizeof(active)/4; ++i) dest[i] = 0;
        VAN_CONFIG_PUBLISHED = VAN_CONFIG_APPLIED = 0;
        publishAt = origin + 15 * 128;
        publishDuringFrame = true;
        auto outgoing = frame(0x700, 8);
        for (unsigned i = 0; i < outgoing.frameWordCount; ++i) VAN_DATA[i] = outgoing.words[i];
        VAN_DATA_LENGTH = outgoing.frameWordCount;
        VAN_FRAME_TYPE = 0; VAN_START_TX = 1; VAN_TX_FINISHED = 0;
        test_monitor_reset();
        while (now < origin) test_monitor_step();
        test_monitor_step();
        assert(VAN_START_TX == 1 && VAN_TX_FINISHED == 0);
        assert(VAN_BUS_RESULT == (reply ? VAN_LP_REPLY_NOT_ACKNOWLEDGED : VAN_LP_NORMAL_FRAME_ACKED));
        for (unsigned i = 0; i < outgoing.frameWordCount; ++i) assert(VAN_DATA[i] == outgoing.words[i]);
        assert(VAN_CONFIG_PUBLISHED == 1 && VAN_CONFIG_APPLIED == 0);
        test_monitor_step();
        assert(VAN_CONFIG_APPLIED == 1); // Old bank can only now be recycled.
        // Avoid host NOP timing claims: wait through complete wire frame + IFS;
        // normal TX only checked for consumption/completion, not pulse widths.
        while (!VAN_TX_FINISHED && now < origin + 200000) test_monitor_step();
        assert(VAN_TX_FINISHED && !VAN_START_TX);
    }
    // Exercise query SOF start through the real monitor, including its immediate
    // GPIO start (the standalone transmitter tests use a scheduled start).
    for (bool responder : {false, true})
    {
        auto response = frame(0x8c4, 0xe, 28);
        auto bus = responder ? raw(response) : std::vector<unsigned>{};
        if (responder) std::fill(bus.begin(), bus.begin() + 28, 1);
        reset(bus, UINT64_MAX / 2); now = 0; alignQueryOrigin = true;
        volatile uint32_t* bank = reinterpret_cast<volatile uint32_t*>(&VAN_CONFIG[0]);
        for (unsigned i = 0; i < sizeof(VanLpConfig)/4; ++i) bank[i] = 0;
        VAN_CONFIG[0].queryAckEnabled = 1;
        VAN_CONFIG_PUBLISHED = VAN_CONFIG_APPLIED = 0;
        for (unsigned i = 0; i < request.frameWordCount; ++i) VAN_DATA[i] = request.words[i];
        VAN_DATA_LENGTH = request.frameWordCount;
        VAN_FRAME_TYPE = 1; VAN_START_TX = 1; VAN_TX_FINISHED = 0;
        test_monitor_reset();
        while (!VAN_TX_FINISHED && now < 100000) test_monitor_step();
        assert(VAN_TX_FINISHED && !VAN_START_TX);
        assert(VAN_TX_RESULT == (responder ? VAN_LP_QUERY_RESPONSE_ACKED : VAN_LP_QUERY_NO_RESPONSE));
        assert(VAN_TX_ABORT_DETAIL == 0);
        assert(VAN_TX_EOD_TS == (responder ? 329u : UINT32_MAX));
    }
    // Incoming frames must not replace the published local-TX boundary.
    const uint32_t completedEod = VAN_TX_EOD_TS;
    reset(raw(frame(0x8c0, 0xc, 3)));
    assert(test_receive(&config, origin) == VAN_LP_NORMAL_FRAME_ACKED);
    assert(test_eod_ts() == 79 && VAN_TX_EOD_TS == completedEod);
    // The TX task may read completion after LP has resumed receiving traffic.
    // A later incoming abort must not overwrite the failed query's detail.
    auto malformedReply = raw(frame(0x8c4, 0xe, 28));
    std::fill(malformedReply.begin(), malformedReply.begin() + 28, 1);
    malformedReply[33] = malformedReply[34] = 1;
    reset(malformedReply, UINT64_MAX / 2); now = 0; alignQueryOrigin = true;
    VAN_START_TX = 1; VAN_TX_FINISHED = 0;
    test_monitor_reset();
    while (!VAN_TX_FINISHED && now < 100000) test_monitor_step();
    assert(VAN_TX_FINISHED && VAN_TX_RESULT == VAN_LP_ABORT);
    const uint32_t queryDetail = (VAN_LP_ABORT_RX_EOD_INVALID << 16) | 34;
    assert(VAN_TX_ABORT_DETAIL == queryDetail);
    assert(VAN_TX_EOD_TS == UINT32_MAX);
    VanLpConfig incomingConfig = {};
    VanFrameBuilder::SetAck(incomingConfig, 0, 0x8c4, 0xc, true);
    auto malformedIncoming = raw(frame(0x8c4, 0xc, 28));
    malformedIncoming[43] = malformedIncoming[44] = 0;
    reset(malformedIncoming);
    assert(test_receive(&incomingConfig, origin) == VAN_LP_ABORT);
    assert((test_abort_detail() & 0xffffu) == 44);
    assert(VAN_TX_ABORT_DETAIL == queryDetail);
    puts("VAN LP: encoding, all slots/lengths, ACK, RTR, malformed frames, deadlines, wrap, drift and pending TX passed");
}
