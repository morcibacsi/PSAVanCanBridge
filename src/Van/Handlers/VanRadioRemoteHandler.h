// VanRadioRemoteHandler.h
#pragma once

#ifndef _VanRadioRemoteHandler_h
    #define _VanRadioRemoteHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"

#include "../../Can/Handlers/CanTripInfoHandler.h"
#include "../../Can/Handlers/CanRadioRemoteMessageHandler.h"
#include "../Structs/VanRadioRemoteStructs.h"
#include "../../Can/Structs/CanMenuStructs.h"

class VanRadioRemoteHandler : public AbstractVanMessageHandler {
    CanTripInfoHandler* canTripInfoHandler;
    CanRadioRemoteMessageHandler* canRadioRemoteMessageHandler;
    CanRadioButtonPacketSender* canRadioButtonPacketSender;

    ~VanRadioRemoteHandler()
    {

    }

public:
    VanRadioRemoteHandler(CanTripInfoHandler* _canTripInfoHandler, CanRadioRemoteMessageHandler* _canRadioRemoteMessageHandler, CanRadioButtonPacketSender* _canRadioButtonPacketSender)
    {
        canTripInfoHandler = _canTripInfoHandler;
        canRadioRemoteMessageHandler = _canRadioRemoteMessageHandler;
        canRadioButtonPacketSender = _canRadioButtonPacketSender;
    }


    void buttonReset(int delay){
    	vTaskDelay(delay / portTICK_PERIOD_MS);
    	canRadioButtonPacketSender->SendButtonCode(0);
    }

    bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan& dataToBridge,
        VanIgnitionDataToBridgeToCan& ignitionDataToBridge,
        DoorStatus& doorStatus) override
    {
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_RADIO_REMOTE) && messageLength == 2))
        {
            return false;
        }

        const VanRadioRemotePacket packet = DeSerialize<VanRadioRemotePacket>(vanMessageWithoutId);
        dataToBridge.RadioRemoteButton = packet.VanRadioRemotePacket[0];
        dataToBridge.RadioRemoteScroll = packet.VanRadioRemotePacket[1];

        canRadioRemoteMessageHandler->SetData(dataToBridge.RadioRemoteButton, dataToBridge.RadioRemoteScroll);

        if (packet.data.RemoteButton.seek_down_pressed && packet.data.RemoteButton.seek_up_pressed)
                {
                	canRadioButtonPacketSender->SendButtonCode(CONST_MENU_BUTTON);
                	buttonReset(20);

                }

                if (packet.data.RemoteButton.volume_up_pressed && packet.data.RemoteButton.volume_down_pressed)
                     {
                      	canRadioButtonPacketSender->SendButtonCode(CONST_ESC_BUTTON);
                      	buttonReset(20);



                      }

                if (packet.data.RemoteButton.seek_down_pressed){
                	canRadioButtonPacketSender->SendButtonCode(CONST_UP_ARROW);
                	buttonReset(20);
                }

                if (packet.data.RemoteButton.seek_up_pressed){
                       	canRadioButtonPacketSender->SendButtonCode(CONST_DOWN_ARROW);
                       	buttonReset(20);
                       }
                if (packet.data.RemoteButton.volume_up_pressed){
                        canRadioButtonPacketSender->SendButtonCode(CONST_RIGHT_ARROW);
                        buttonReset(20);
                               }
                if (packet.data.RemoteButton.volume_down_pressed){
                        canRadioButtonPacketSender->SendButtonCode(CONST_LEFT_ARROW);
                        buttonReset(20);
                                      }
                if (packet.data.RemoteButton.source_pressed){
                        canRadioButtonPacketSender->SendButtonCode(CONST_OK_BUTTON);
                        buttonReset(20);
                                              }
                buttonReset(50);

        return true;
    }
};

#endif
