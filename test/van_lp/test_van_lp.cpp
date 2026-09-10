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
extern volatile uint32_t VAN_DATA[], VAN_DATA_LENGTH, VAN_START_TX, VAN_TX_FINISHED;
extern volatile uint32_t VAN_FRAME_TYPE, VAN_TX_RESULT, VAN_BUS_RESULT;
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

extern "C" uint32_t van_test_cycles()
{
    if (publishDuringFrame && now >= publishAt) { VAN_CONFIG_PUBLISHED = 1; publishDuringFrame = false; }
    if (injectedDelay && now >= delayAt) { now += injectedDelay; injectedDelay = 0; }
    return static_cast<uint32_t>(now++);
}
extern "C" uint32_t ulp_lp_core_gpio_get_level(unsigned)
{
    const uint64_t ts = now >= origin ? (now - origin) / wireTsCycles : UINT64_MAX;
    return txLevel & (ts < incoming.size() ? incoming[ts] : 1u);
}
extern "C" void ulp_lp_core_gpio_set_level(unsigned, uint8_t level)
{
    if (alignQueryOrigin && level == 0) { origin = now; alignQueryOrigin = false; }
    if (level != txLevel) writes.emplace_back(now, level);
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
    writes.clear();
    incoming = bits;
    injectedDelay = 0;
    publishDuringFrame = false;
    alignQueryOrigin = false;
    wireTsCycles = 128;
}
static void check_ack(unsigned ackStartTs)
{
    assert(writes.size() == 2 && writes[0].second == 0 && writes[1].second == 1);
    const int64_t error = writes[0].first - (origin + (ackStartTs + 1) * 128);
    assert(error >= -4 && error <= 8);
    assert(writes[1].first - writes[0].first == 128);
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

    VanLpConfig config = {};
    reset(raw(frame(0x8c4, 0xc)));
    assert(test_receive(&config, origin) == VAN_LP_NONE && writes.empty());
    for (unsigned slot = 0; slot < 5; ++slot)
    {
        assert(VanFrameBuilder::SetAck(config, slot, 0x8c0 + slot, 0xc, true));
        const uint8_t data[] = {0x80, 0x01, 0x55};
        assert(VanFrameBuilder::SetReply(config, slot, 0x9c0 + slot, data, sizeof(data), true));
    }
    assert(config.enabledMask == 0x3ff);
    assert(!VanFrameBuilder::SetAck(config, 5, 0, 0xc, true));
    assert(!VanFrameBuilder::SetAck(config, 0, 0, 0x8, true));
    assert(!VanFrameBuilder::SetReply(config, 5, 0, nullptr, 0, true));

    for (unsigned slot = 0; slot < 5; ++slot)
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
    for (unsigned slot = 0; slot < 5; ++slot)
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
        assert(writes[writes.size()-2].first >= origin + (VanFrameBuilder::AckStartTs(response) + 1) * 128);
        assert(writes.back().first - writes[writes.size()-2].first == 128);
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
    }
    puts("VAN LP: encoding, all slots/lengths, ACK, RTR, malformed frames, deadlines, wrap, drift and pending TX passed");
}
