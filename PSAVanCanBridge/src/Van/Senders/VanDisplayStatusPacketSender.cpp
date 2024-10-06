#include "VanDisplayStatusPacketSender.h"
#include "../Structs/VanDisplayStatusStructs.h"

VanDisplayStatusPacketSender::VanDisplayStatusPacketSender(IVanMessageSender * object)
{
    vanMessageSender = object;
}

void VanDisplayStatusPacketSender::SendStatus(uint8_t channelId, uint8_t resetTotals, uint8_t resetCumulative, uint8_t keepVanComfortAlive)
{
    VanEmfBsiRequestByte1 field1;
    field1.data.request_for_keep_van_comfort_alive = keepVanComfortAlive;
    field1.data.request_to_reset_course_totals = resetTotals;
    field1.data.request_to_reset_cumulative = resetCumulative;

    uint8_t vanPacket[2];
    vanPacket[0] = field1.asByte;
    vanPacket[1] = 0x1E;

    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_DISPLAY_STATUS, vanPacket, 2, 1);
}

void VanDisplayStatusPacketSender::Disable(uint8_t channelId)
{
    vanMessageSender->disable_channel(channelId);
}