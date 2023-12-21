#include "DisplayRemote.h"
#include "../Can/Structs/CAN_3E5_2004.h"
#include "../Van/Structs/VAN_9C4.h"

DisplayRemote::DisplayRemote(CanMessageHandlerContainer* canMessageHandlerContainer, DataBroker* dataBroker, Config* config)
{
    _canMessageHandlerContainer = canMessageHandlerContainer;
    _messageHandler_3E5 = static_cast<MessageHandler_3E5*>(_canMessageHandlerContainer->GetHandler(0x3E5));

    _config = config;
    _dataBroker = dataBroker;
    _seekButtonsPressTime = 0;
}

void DisplayRemote::Process(unsigned long currentTime)
{
    if (!_config->DISPLAY_CONTROL_WITH_COM)
    {
        return;
    }

    Van9C4Byte1Struct buttons;
    buttons.asByte = _dataBroker->RadioRemoteButton;

    if (buttons.data.seek_up && buttons.data.seek_down)
    {
        if (!_seekButtonsPressed)
        {
            _seekButtonsPressTime = currentTime;
            _seekButtonsPressed = 1;
        }
    }
    else
    {
        _seekButtonsPressTime = 0;
        _seekButtonsPressed = 0;
    }

    if (currentTime - _seekButtonsPressTime > MENU_OPEN_TIME && _seekButtonsPressed && !_dataBroker->IsMenuOpenOnCanEMF)
    {
        SendButton(currentTime, Menu);
    }

    #pragma region Debounce button presses
    if (_prevButtonState == _dataBroker->RadioRemoteButton)
    {
        return;
    }
    _prevButtonState = _dataBroker->RadioRemoteButton;
    #pragma endregion

    if (_dataBroker->IsMenuOpenOnCanEMF)
    {
        if (buttons.data.source)
        {
            SendButton(currentTime, Ok);
        }
        if (buttons.data.volume_plus && !buttons.data.volume_minus)
        {
            SendButton(currentTime, Up);
        }
        if (buttons.data.volume_minus && !buttons.data.volume_plus)
        {
            SendButton(currentTime, Down);
        }
        if (buttons.data.seek_up && !buttons.data.seek_down)
        {
            SendButton(currentTime, Right);
        }
        if (buttons.data.seek_down && !buttons.data.seek_up)
        {
            SendButton(currentTime, Left);
        }
        if (buttons.data.volume_minus && buttons.data.volume_plus)
        {
            SendButton(currentTime, Esc);
        }
    }
    else
    {
        if (buttons.data.volume_plus && buttons.data.volume_minus)
        {
            SendButton(currentTime, Mode);
        }
    }
}

void DisplayRemote::SendButton(unsigned long currentTime, RadioButton button)
{
    if (_messageHandler_3E5)
    {
        _messageHandler_3E5->SendButton(currentTime, button);
    }
}
