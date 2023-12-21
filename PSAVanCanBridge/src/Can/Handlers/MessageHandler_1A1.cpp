#include "MessageHandler_1A1.h"
#include "../Structs/CanDisplayStructs.h"
#include "../../Helpers/DebugPrint.h"
#include "../../Config.h"

void MessageHandler_1A1::SetData(uint8_t category, uint8_t messageType, uint8_t show, uint8_t doorStatus1, uint8_t doorStatus2, uint8_t km1, uint8_t km2)
{
    CanDisplayByte2Struct field2;
    field2.data.show_popup_on_emf = show;

    field2.data.show_popup_on_cmb = show;
    field2.data.show_popup_on_vth = show;
    field2.data.priority = 1;

    _data[0] = category;
    _data[1] = messageType;
    _data[2] = field2.asByte;
    _data[3] = doorStatus1;
    _data[4] = doorStatus2;
    _data[5] = 0xff;
    _data[6] = km1;
    _data[7] = km2;
}

uint8_t MessageHandler_1A1::SendMessage(unsigned long currentTime, bool forcedSend)
{
    return ICanMessageHandler::SendMessage(currentTime, forcedSend);

    if (_data[0] == CAN_POPUP_MSG_SHOW_CATEGORY3 && _data[1] == CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED && !_cmbEspSent)
    {
        _cmbEspSent = true;
        //the CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED does not trigger a message on the CMB, so we need to send it manually
        _data[0] = CAN_POPUP_MSG_SHOW_CATEGORY2;
        _data[1] = CAN_POPUP_MSG_ESP_OFF_ON_CMB;

        _canInterface->SendMessage(_canId, 0, _dataLength, _data); //workaround, because using the SendMessage() from the base class messes up the timing

        _data[0] = CAN_POPUP_MSG_SHOW_CATEGORY3;
        _data[1] = CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED;
    }

    if (_data[0] == CAN_POPUP_MSG_SHOW_CATEGORY2 && _data[1] == CAN_POPUP_MSG_ESP_ON_ON_CMB)
    {
         _cmbEspSent = false;
    }
}
