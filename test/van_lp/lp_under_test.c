#define VAN_LP_HOST_TEST
#include "../../components/lp_core_van/ulp/main.c"

VanLpResult test_receive(const VanLpConfig* config, uint32_t edge)
{
    return receive_frame(config, edge);
}
VanLpResult test_request(const VanLpFrame* frame, uint32_t edge, int ack)
{
    VanRxCapture capture;
    capture_prepared_prefix(frame->words, &capture);
    return transmit_prepared(frame->words, frame->frameWordCount, 0, 9, edge,
                             VAN_TX_REQUESTER, ack != 0, &capture, 0);
}
VanLpResult test_request_capture(const VanLpFrame* frame, uint32_t edge, int ack)
{
    VanRxCapture capture;
    capture_prepared_prefix(frame->words, &capture);
    return transmit_prepared(frame->words, frame->frameWordCount, 0, 9, edge,
                             VAN_TX_REQUESTER, ack != 0, &capture, 0);
}
void test_monitor_reset(void)
{
    monitor.config = &VAN_CONFIG[0];
    monitor.highSince = cycles();
    monitor.wasHigh = 0;
    monitor.retries = 0;
}
void test_monitor_step(void) { monitor_once(); }

int test_inverse(uint32_t* center, uint32_t previous, uint32_t* bit)
{
    return sample_inverse(center, previous, bit);
}

void test_rx_reset(void) { VAN_RX_QUEUE = (VanLpRxQueue){0}; }
uint32_t test_rx_count(void) { return VAN_RX_QUEUE.writeIndex - VAN_RX_QUEUE.readIndex; }
uint32_t test_rx_pop(VanLpRxFrame* frame)
{
    if (VAN_RX_QUEUE.readIndex == VAN_RX_QUEUE.writeIndex) return 0;
    *frame = VAN_RX_QUEUE.frames[VAN_RX_QUEUE.readIndex % VAN_LP_RX_QUEUE_LENGTH];
    ++VAN_RX_QUEUE.readIndex;
    return 1;
}
uint32_t test_rx_overflow(void) { return VAN_RX_QUEUE.overflowCount; }
uint32_t test_abort_detail(void) { return abortDetail; }
uint32_t test_eod_ts(void) { return responseEodTs; }
uint32_t test_ack_edge_advance(void) { return ACK_EDGE_ADVANCE_CYCLES; }
uint32_t test_ack_hold_extension(void) { return ACK_HOLD_EXTENSION_CYCLES; }
void test_set_ts_cycles(uint32_t value) { tsCycles = value; }
VanLpResult test_track_response(uint32_t center)
{
    VanRxCapture capture;
    capture_reset(&capture);
    return track_response(center, true, true, &capture);
}
