// CanRadioRd4DiagHandler.h
#pragma once

#ifndef _CanRadioRd4DiagHandler_h
    #define _CanRadioRd4DiagHandler_h


#include "AbstractCanMessageHandler.h"
#include "../AbstractCanMessageSender.h"
#include "../Structs/CanRadioRd4DiagStructs.h"
#include "../../SerialPort/AbstractSerial.h"
#include "../../Helpers/IVinFlashStorage.h"

class CanRadioRd4DiagHandler : public AbstractCanMessageHandler
{
    CanRadioRd4DiagPacketSender* _packetSender;
    AbsSer* _serialPort;
    IVinFlashStorage* _vinFlashStorage;

    public:
    CanRadioRd4DiagHandler(AbstractCanMessageSender* object, AbsSer* serialPort, IVinFlashStorage* vinFlashStorage)
    {
        _packetSender = new CanRadioRd4DiagPacketSender(object);
        _serialPort = serialPort;
        _vinFlashStorage = vinFlashStorage;
    }

    void SetRadioType(uint8_t radioType)
    {
        _packetSender->SetRadioType(radioType);
    }

    void GetVin()
    {
        _packetSender->EnterDiagMode();
    }

    bool ProcessMessage(const uint16_t canId, const uint8_t length, const uint8_t canMsg[]) override
    {
        if (canId != CAN_ID_RADIO_RD4_DIAG_ANSWER)
        {
            return false;
        }
        _packetSender->ProcessReceivedCanMessage(canId, length, canMsg);
        //delayMicroseconds(40);
        if (_packetSender->IsVinRead)
        {
            for (int i = 0; i < 17; ++i)
            {
                Vin[i] = _packetSender->Vin[i];
            }

            const bool success = _vinFlashStorage->Save();

            if (success)
            {
                _serialPort->println("VIN save success");
            }
            else
            {
                _serialPort->println("VIN save failed");
            }

            _packetSender->IsVinRead = false;
        }

        return false;
    }
};

#endif
