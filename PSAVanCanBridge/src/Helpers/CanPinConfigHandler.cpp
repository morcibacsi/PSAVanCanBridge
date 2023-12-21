#include <string.h>

#include "CanPinConfigHandler.h"

#include "../Can/Structs/CAN_225.h"
#include "../Can/Structs/CAN_165.h"
#include "../Can/Structs/CAN_0DF.h"
#include "../Can/Structs/CAN_3E5_2004.h"
#include "../Can/Structs/CAN_328.h"

//#include <Arduino.h>

CanPinConfigHandler::CanPinConfigHandler(Config* config, TpMessageHandler_760* radioDiag, DataBroker* dataBroker)
{
    _radioDiag = radioDiag;
    _config = config;
    _dataBroker = dataBroker;
}

bool CanPinConfigHandler::ProcessMessage(const uint16_t canId, const uint8_t length, const uint8_t canMsg[])
{
    if (!(canId == CAN_ID_225 || canId == CAN_ID_0DF || canId == CAN_ID_165 || canId == CAN_ID_RADIO_BUTTON_2004 || canId == CAN_ID_328))
    {
        return false;
    }

    // RD4/RD43/RD45
    if (_config->RADIO_TYPE == 1 || _config->RADIO_TYPE == 2)
    {
        if (canId == CAN_ID_225)
        {
            Can225Struct packet;
            memcpy(&packet, canMsg, sizeof(packet));

            band = packet.Band.data.band_type;
            freq1 = packet.Frequency1;
            freq2 = packet.Frequency2;
        }
    }

    // RD45 (CAN2010)
    if (_config->RADIO_TYPE == 3)
    {
        if (canId == CAN_ID_328 && canMsg[0] == 0x05)
        {
            Can328Struct packet;
            memcpy(&packet, canMsg, sizeof(packet));

            band = packet.Band;
            freq1 = packet.Frequency1;
            freq2 = packet.Frequency2;
        }
    }

    if (canId == CAN_ID_0DF)
    {
        Can0DFStruct packet;
        memcpy(&packet, canMsg, sizeof(packet));

        menu = packet.Byte1.data.menu_open;
        _dataBroker->IsMenuOpenOnCanEMF = packet.Byte1.data.menu_open;
    }
    if (canId == CAN_ID_165)
    {
        Can165Struct packet;
        memcpy(&packet, canMsg, sizeof(packet));

        radioEnabled = packet.General1.data.power == 1;
        source = packet.General3.data.source;
    }

    readKeyCombo = radioEnabled && source == CAN_RADIO_SRC_TUNER &&
        band == CAN_RADIO_BAND_AM &&
        freq1 == 0x02 &&
        (freq2 == 0x1F || freq2 == 0x21) && //1F = 0x021F: RD43   0x0221: RD45
        menu == 1; //radio turned on, AM, 543 Khz, main menu open

//
/*
    debug_print("Band:");debug_println(band, HEX);
    debug_print("F1:");debug_println(freq1, HEX);
    debug_print("F2:");debug_println(freq2, HEX);
    debug_print("menu:");debug_println(menu, HEX);
    debug_print("radio:");debug_println(radioEnabled, HEX);
    debug_print("source:");debug_println(source, HEX);
    debug_print("readKeyCombo:");debug_println(readKeyCombo, HEX);
//*/
    if (readKeyCombo)
    {
        if (canId == CAN_ID_RADIO_BUTTON_2004)
        {
            uint8_t keyPress = 0;
            if (_config->RADIO_TYPE == 1) // RD4/RD43/RD45
            {
                keyPress = canMsg[5];
                vinKeyCombo = vinKeyCombo_2004;
            }
            else if (_config->RADIO_TYPE == 2) // new RD45
            {
                keyPress = canMsg[0];
                vinKeyCombo = vinKeyCombo_2010;
            }

            if (keyPress != 0)
            {
                keyComboEntered[keyComboCount] = keyPress;

                //debug_print("keypress:");debug_println(keyPress, HEX);

                if (keyComboCount == 3)
                {
                    bool vinCodeMatch = true;

                    for (int i = 0; i < 4; ++i)
                    {
                        vinCodeMatch = vinCodeMatch && vinKeyCombo[i] == keyComboEntered[i];
                        keyComboEntered[i] = 0;
                    }

                    if (vinCodeMatch)
                    {
                        //debug_println("Get VIN");

                        // if the radio is turned on
                        // tuned to AM 543 kHz (for RD4 and RD43) or 545 kHz (for RD45)
                        // the setting menu is visible
                        // and we enter the following combination with the arrows on the radio:
                        // Left-Right-Left-Right
                        // then we read out the VIN code from the radio
                        // the VIN code will be captured by the TpMessageHandler_760 which saves it to the internal flash
                        if (freq2 == 0x1F)
                        {
                            _radioDiag->SetRadioType(CAN_DIAG_RADIO_RD4_RD43);
                        }
                        else if (freq2 == 0x21)
                        {
                            _radioDiag->SetRadioType(CAN_DIAG_RADIO_RD45);
                        }
                        _radioDiag->EnterDiagMode();
                    }

                    keyComboCount = 0;
                }
                else
                {
                    keyComboCount++;
                }
            }
        }
    }
    else
    {
        keyComboCount = 0;
    }
    return false;
}
