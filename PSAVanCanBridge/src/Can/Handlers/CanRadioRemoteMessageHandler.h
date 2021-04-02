// CanRadioRemoteMessageHandler.h
#pragma once

#ifndef _CanRadioRemoteMessageHandler_h
    #define _CanRadioRemoteMessageHandler_h

#include "CanMessageHandlerBase.h"
#include "../AbstractCanMessageSender.h"
#include "../Structs/CanRadioRemoteStructs.h"

class CanRadioRemoteMessageHandler : public CanMessageHandlerBase
{
    static const uint16_t CAN_RADIO_REMOTE_MESSAGE_INTERVAL = 500;

    CanRadioRemoteButtonPacketSender* RadioRemoteSender;

    uint8_t ButtonByte;
    uint8_t ScrollByte;

    uint8_t PrevScrollByte;
    bool SendEmpty = false;

    /*
     * Android head units are working in a different way that the RD4/43/45 units. We need to send new packets if something changed otherwise we get repeated keypresses
     * However if we have an RD4/43/45 unit we need to send the button states regularly otherwise we miss out some keypresses
    */

    bool isAndroidHeadUnitInstalled = true;

    virtual void InternalProcess()
    {
        if (isAndroidHeadUnitInstalled == true)
        {
            if (SendEmpty)
            {
                RadioRemoteSender->SendAsByte(0x00, 0x00);
                SendEmpty = false;
            }
        }
        else
        {
            RadioRemoteSender->SendAsByte(ButtonByte, ScrollByte);
        }
    }

    public:
    CanRadioRemoteMessageHandler(AbstractCanMessageSender* object) : CanMessageHandlerBase(object, CAN_RADIO_REMOTE_MESSAGE_INTERVAL)
    {
        RadioRemoteSender = new CanRadioRemoteButtonPacketSender(object);
    }

    void SetData(
        uint8_t buttonByte,
        uint8_t scrollByte
    )
    {
        if (isAndroidHeadUnitInstalled == true)
        {
            if (PrevScrollByte == scrollByte)
            {
                scrollByte = 0x00;
            }
            else
            {
                PrevScrollByte = scrollByte;
            }

            const bool sendData = buttonByte > 0 || scrollByte != 0;

            if (sendData)
            {
                RadioRemoteSender->SendAsByte(buttonByte, scrollByte);
                SendEmpty = true;
            }
        }
        else
        {
            ButtonByte = buttonByte;
            ScrollByte = scrollByte;

            RadioRemoteSender->SendAsByte(ButtonByte, ScrollByte);
        }
    }

    void IsAndroidInstalled(bool isInstalled)
    {
        isAndroidHeadUnitInstalled = isInstalled;
    }
};

#endif
