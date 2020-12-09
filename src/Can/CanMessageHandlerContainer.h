// CanMessageHandlerContainer.h
#pragma once

#ifndef _CanMessageHandlerContainer_h
    #define _CanMessageHandlerContainer_h

#include "Handlers/AbstractCanMessageHandler.h"
#include "Handlers/CanPinConfigHandler.h"
#include "Handlers/CanRadioRd4DiagHandler.h"
#include "../SerialPort/AbstractSerial.h"


class CanMessageHandlerContainer {
    const static uint8_t CAN_MESSAGE_HANDLER_COUNT = 2;
    AbstractCanMessageHandler* canMessageHandlers[CAN_MESSAGE_HANDLER_COUNT];

    public:
    CanMessageHandlerContainer(
        AbstractCanMessageSender* canInterface,
        AbsSer* serialPort,
        VinFlashStorage* vinFlashStorage
    ) {
        canMessageHandlers[0] = new CanRadioRd4DiagHandler(canInterface, serialPort, vinFlashStorage);
        canMessageHandlers[1] = new CanPinConfigHandler(canInterface, static_cast<CanRadioRd4DiagHandler*>(canMessageHandlers[0]));
    }

    bool ProcessMessage(
        const uint16_t canId,
        const uint8_t canMsgLength,
        const uint8_t canMsg[]
        )
    {
        bool canMessageHandled = false;

        for (uint8_t i = 0; i < CAN_MESSAGE_HANDLER_COUNT; i++)
        {
            canMessageHandled = canMessageHandlers[i]->ProcessMessage(canId, canMsgLength, canMsg);
            if (canMessageHandled)
            {
                break;
            }
        }

        return canMessageHandled;
    }
};

#endif
