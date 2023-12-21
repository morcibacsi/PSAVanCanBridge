#include "MessageHandler_3E5.h"
#include "../Structs/CAN_3E5_2004.h"
#include "../Structs/CAN_3E5_2010.h"

void MessageHandler_3E5::SendButton(unsigned long currentTime, RadioButton button)
{
    if (_config->CAN_TYPE == 0)
    {
        SetupButton2004(currentTime, button);
        _dataLength = 6;
    }
    else
    {
        SetupButton2010(currentTime, button);
        _dataLength = 3;
    }

    _sendOnce = 1;

    SendMessage(currentTime, true);

    // Button commands only work when a message with 0s follows them
    // _sendOnce variable is used to control if a message should be sent

    _data[0] = 0;
    _data[1] = 0;
    _data[2] = 0;
    _data[5] = 0;

    _sendOnce = 1;
}

void MessageHandler_3E5::SetupButton2004(unsigned long currentTime, RadioButton button)
{
    CAN_3E5_2004_Byte6Struct directions;
    directions.asByte = 0;

    CAN_3E5_2004_Byte3Struct escOk;
    escOk.asByte = 0;

    CAN_3E5_2004_Byte1Struct menu;
    menu.asByte = 0;

    CAN_3E5_2004_Byte2Struct mode;
    mode.asByte = 0;

    switch (button)
    {
        case Up:
            directions.data.up = 1;
            break;
        case Down:
            directions.data.down = 1;
            break;
        case Left:
            directions.data.left = 1;
            break;
        case Right:
            directions.data.right = 1;
            break;
        case Esc:
            escOk.data.esc = 1;
            break;
        case Ok:
            escOk.data.ok = 1;
            break;
        case Dark:
            escOk.data.dark = 1;
            break;
        case Menu:
            menu.data.menu = 1;
            break;
        case Mode:
            mode.data.mode = 1;
            break;
        case Trip:
            mode.data.odb = 1;
            break;
        case AC:
            menu.data.aircon = 1;
            break;

        default:
            break;
    }

    _data[0] = menu.asByte;
    _data[1] = mode.asByte;
    _data[2] = escOk.asByte;
    _data[5] = directions.asByte;
}

void MessageHandler_3E5::SetupButton2010(unsigned long currentTime, RadioButton button)
{
    CAN_3E5_2010_Byte1Struct directions;
    directions.asByte = 0;

    CAN_3E5_2010_Byte2Struct escOk;
    escOk.asByte = 0;

    switch (button)
    {
        case Up:
            directions.data.up = 1;
            break;
        case Down:
            directions.data.down = 1;
            break;
        case Left:
            directions.data.left = 1;
            break;
        case Right:
            directions.data.right = 1;
            break;
        case Esc:
            escOk.data.esc = 1;
            break;
        case Ok:
            escOk.data.ok = 1;
            break;
        case Dark:
            escOk.data.dark = 1;
            break;
        case Menu:
            directions.data.menu = 1;
            break;
        case Mode:
            directions.data.mode = 1;
            break;

        default:
            break;
    }

    _data[0] = directions.asByte;
    _data[1] = escOk.asByte;
}

uint8_t MessageHandler_3E5::SendMessage(unsigned long currentTime, bool forcedSend)
{
    // The radio sends the 3E5 message periodically, however we don't want to send it because we don't know if there is a radio or not (and also it isn't necessary)
    // _sendOnce is used to control if a message should be sent. After it was successfully transmitted, we don't send it anymore

    if (_sendOnce == 1)
    {
        uint8_t sendResult = ICanMessageHandler::SendMessage(currentTime, forcedSend);
        if (sendResult == 1)
        {
            _sendOnce = 0;
        }
        return sendResult;
    }

    return 0;
}