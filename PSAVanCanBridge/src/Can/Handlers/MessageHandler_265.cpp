#include "MessageHandler_265.h"
#include "../Structs/CAN_265.h"

void MessageHandler_265::SetData(uint8_t rds, uint8_t rds_ok, uint8_t ta, uint8_t ta_ok, uint8_t reg, uint8_t pty_not_selected)
{
    CAN_265_Byte1Struct field1;
    field1.data.af = rds;
    field1.data.af_ok = rds_ok;
    field1.data.ta = ta;
    field1.data.ta_ok = ta_ok;
    field1.data.reg = reg;

    CAN_265_Byte2Struct field2;
    //field2.data.pty_mode_in_progress = pty_not_selected == 0 ? CAN_RADIO_PTY_LISTEN_SAME_AS_SELECTED : CAN_RADIO_PTY_LISTEN_NOT_AVAILABLE;
    //field2.data.pty_mode_request = (pty_not_selected == 0) ? CAN_RADIO_PTY_MODE_IN_LISTENING : CAN_RADIO_PTY_MODE_NOT_REQUESTED;

    _data[0] = field1.asByte;
    _data[1] = field2.asByte;
}

uint8_t MessageHandler_265::SendMessage(unsigned long currentTime, bool forcedSend)
{
    if (_config->BRIDGE_RADIO_DATA)
    {
        return ICanMessageHandler::SendMessage(currentTime, forcedSend);
    }

    return 0;
}