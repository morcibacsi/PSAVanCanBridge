#define VAN_LP_HOST_TEST
#include "../../ulp/main.c"

VanLpResult test_receive(const VanLpConfig* config, uint32_t edge)
{
    return receive_frame(config, edge);
}
VanLpResult test_request(const VanLpFrame* frame, uint32_t edge, int ack)
{
    return transmit_prepared(frame->words, frame->frameWordCount, 0, 9, edge, VAN_TX_REQUESTER, ack != 0);
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

uint32_t test_abort_detail(void) { return abortDetail; }
uint32_t test_eod_ts(void) { return responseEodTs; }
void test_set_ts_cycles(uint32_t value) { tsCycles = value; }
VanLpResult test_track_response(uint32_t center) { return track_response(center, true, true); }
