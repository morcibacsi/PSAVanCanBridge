#pragma once

#ifndef _CanDataReaderTask_h
    #define _CanDataReaderTask_h

#include "../../Config.h"
#include "AbstractCanMessageSender.h"
#include "CanMessageHandlerContainer.h"
#include "CanDataSenderTask.h"
#include "Handlers/CanRadioRemoteMessageHandler.h"
#include "Handlers/ICanDisplayPopupHandler.h"

class CanDataReaderTask {
    uint8_t canReadMessage[20] = { 0 };
    uint8_t canReadMessageLength = 0;
    uint16_t canId = 0;

    AbstractCanMessageSender* _CANInterface;
    ICanDisplayPopupHandler* _canPopupHandler;
    CanRadioRemoteMessageHandler* _canRadioRemoteMessageHandler;
    CanMessageHandlerContainer* _canMessageHandlerContainer;
    CanDataSenderTask* _canDataSenderTask;
public:
    CanDataReaderTask(
        AbstractCanMessageSender* CANInterface,
        ICanDisplayPopupHandler* canPopupHandler,
        CanRadioRemoteMessageHandler* canRadioRemoteMessageHandler,
        CanMessageHandlerContainer* canMessageHandlerContainer,
        CanDataSenderTask* canDataSenderTask
    )
    {
        _CANInterface = CANInterface;
        _canPopupHandler = canPopupHandler;
        _canRadioRemoteMessageHandler = canRadioRemoteMessageHandler;
        _canMessageHandlerContainer = canMessageHandlerContainer;
        _canDataSenderTask = canDataSenderTask;
    }

    void ReadData() {
        canId = 0;
        canReadMessageLength = 0;
        _CANInterface->ReadMessage(&canId, &canReadMessageLength, canReadMessage);

        if (canId > 0)
        {
            if (canId == CAN_ID_MENU_BUTTONS)
            {
                // the RD4/43/45 units are sending this regularly so if we get this message we can be sure that we have one of those installed
                _canDataSenderTask->SendNoRadioButtonMessage = false;
                _canRadioRemoteMessageHandler->IsAndroidInstalled(false);

                CanMenuPacket packet = DeSerialize<CanMenuPacket>(canReadMessage);
                if (packet.data.EscOkField.esc == 1 && _canPopupHandler->IsPopupVisible())
                {
                    _canPopupHandler->HideCurrentPopupMessage();
                }
            }
            _canMessageHandlerContainer->ProcessMessage(canId, canReadMessageLength, canReadMessage);
        }
    }
 };
#endif
