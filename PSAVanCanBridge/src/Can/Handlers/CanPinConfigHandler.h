#pragma once

#ifndef _CanPinConfigHandler_h
    #define _CanPinConfigHandler_h

#include "CanRadioRd4DiagHandler.h"
#include "AbstractCanMessageHandler.h"
#include "../AbstractCanMessageSender.h"
#include "../Structs/CanRadioTunerStructs.h"
#include "../Structs/CanDisplayMenuStructs.h"
#include "../Structs/CanRadioStructs.h"
#include "../Structs/CanMenuStructs.h"

class CanPinConfigHandler : public AbstractCanMessageHandler
{
    CanRadioRd4DiagHandler* _radioDiag;

    bool radioEnabled = false;
    uint8_t source = 0;
    uint8_t band = 0;
    uint8_t freq1 = 0;
    uint8_t freq2 = 0;
    uint8_t menu = 0;
    uint8_t vinKeyCombo[4] = { 0x01, 0x04, 0x01, 0x04 };
    uint8_t keyComboEntered[4] = { 0x00, 0x00, 0x00, 0x00 };
    uint8_t keyComboCount = 0;
    bool readKeyCombo = false;

    public:
    CanPinConfigHandler(AbstractCanMessageSender* object, CanRadioRd4DiagHandler* radioDiag)
    {
        _radioDiag = radioDiag;
    }

    bool ProcessMessage(const uint16_t canId, const uint8_t length, const uint8_t canMsg[]) override
    {
        if (!(canId == CAN_ID_RADIO_TUNER || canId == CAN_ID_DISPLAY_MENU || canId == CAN_ID_RADIO || canId == CAN_ID_MENU_BUTTONS))
        {
            return false;
        }

        if (canId == CAN_ID_RADIO_TUNER)
        {
            const CanRadioTunerPacket packet = DeSerialize<CanRadioTunerPacket>(canMsg);
            band = packet.data.Band.band;
            freq1 = packet.data.Frequency1;
            freq2 = packet.data.Frequency2;
        }
        if (canId == CAN_ID_DISPLAY_MENU)
        {
            const CanDisplayMenuPacket packet = DeSerialize<CanDisplayMenuPacket>(canMsg);
            menu = packet.data.Byte1.menu_open;
        }
        if (canId == CAN_ID_RADIO)
        {
            const CanRadioPacket packet = DeSerialize<CanRadioPacket>(canMsg);
            radioEnabled = packet.data.Field1.radio_enabled == 1;
            source = packet.data.Source.source;
        }

        readKeyCombo = radioEnabled && source == CAN_RADIO_SOURCE_TUNER &&
            band == CAN_RADIO_TUNER_BAND_AM &&
            freq1 == 0x02 &&
            (freq2 == 0x1F || freq2 == 0x21) && //1F = 0x021F: RD43   0x0221: RD45
            menu == 1; //radio turned on, AM, 543 Khz, main menu open

        if (readKeyCombo)
        {
            if (canId == CAN_ID_MENU_BUTTONS)
            {
                if (canMsg[5] != 0)
                {
                    keyComboEntered[keyComboCount] = canMsg[5];

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
                            // if the radio is turned on
                            // tuned to AM 543 kHz (for RD4 and RD43) or 545 kHz (for RD45)
                            // the setting menu is visible
                            // and we enter the following combination with the arrows on the radio:
                            // Left-Right-Left-Right
                            // then we read out the VIN code from the radio
                            // the VIN code will be captured by the CanRadioRd4DiagHandler which saves it to the internal flash
                            if (freq2 == 0x1F)
                            {
                                _radioDiag->SetRadioType(CAN_DIAG_RADIO_RD4_RD43);
                            }
                            else if (freq2 == 0x21)
                            {
                                _radioDiag->SetRadioType(CAN_DIAG_RADIO_RD45);
                            }
                            _radioDiag->GetVin();
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
};

#endif
