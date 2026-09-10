#define VAN_LP_HOST_TEST
#include "../../ulp/main.c"

VanLpResult test_receive(const VanLpConfig* config, uint32_t edge)
{
    return receive_frame(config, edge);
}
VanLpResult test_request(const VanLpFrame* frame, uint32_t edge, int ack)
{
    return transmit_prepared(frame->words, frame->frameWordCount, 0, 9, edge, true, ack != 0);
}
void test_monitor_reset(void)
{
    monitor.config = &VAN_CONFIG[0];
    monitor.highSince = cycles();
    monitor.wasHigh = 0;
    monitor.retries = 0;
}
void test_monitor_step(void) { monitor_once(); }
