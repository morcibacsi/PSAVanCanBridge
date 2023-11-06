#pragma once

#ifndef _VanDataParserTask_h
    #define _VanDataParserTask_h

#include "../Can/Handlers/CanVinHandler.h"
#include "../Can/Structs/CanMenuStructs.h"
#include "../Helpers/DoorStatus.h"
#include "../Helpers/VanDataToBridgeToCan.h"
#include "../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../Helpers/VanVinToBridgeToCan.h"
#include "../SerialPort/AbstractSerial.h"

#include "Structs/VanVinStructs.h"
#include "Handlers/AbstractVanMessageHandler.h"

#include "VanHandlerContainer.h"

class VanDataParserTask {
    uint8_t identByte1;
    uint8_t identByte2;
    uint8_t vanMessageLengthWithoutId;

    uint8_t vanMessageWithoutId[32];

    DoorStatus doorStatus;

    AbsSer* _serialPort;
    CanVinHandler* _canVinHandler;
    CanRadioButtonPacketSender* _canRadioButtonSender;
    VanHandlerContainer* _vanHandlerContainer;

public:
    VanDataParserTask(
        AbsSer* serialPort,
        CanVinHandler* canVinHandler,
        VanHandlerContainer* vanHandlerContainer
    )
    {
        _serialPort = serialPort;
        _canVinHandler = canVinHandler;
        _vanHandlerContainer = vanHandlerContainer;

        doorStatus.asByte = 0;
    }

    void ProcessData(uint8_t vanMessage[], uint8_t vanMessageLength, VanDataToBridgeToCan *dataToBridgeToCan, VanIgnitionDataToBridgeToCan *ignitionDataToBridgeToCan, VanVinToBridgeToCan *vanVinToBridgeToCan) {
        if (vanMessageLength > 0 && vanMessage[0] == 0x0E)
        {
            identByte1 = vanMessage[1];
            identByte2 = vanMessage[2];
            vanMessageLengthWithoutId = vanMessageLength - 5;

            //make a copy of the buffer excluding the ids and the crc (otherwise deserializing the packet gives wrong results)
            memcpy(vanMessageWithoutId, vanMessage + 3, vanMessageLengthWithoutId);

            const bool vanMessageHandled = _vanHandlerContainer->ProcessMessage(identByte1, identByte2, vanMessageWithoutId, vanMessageLengthWithoutId, dataToBridgeToCan, ignitionDataToBridgeToCan, doorStatus);

            #pragma region Vin
            if (IsVanIdent(identByte1, identByte2, VAN_ID_VIN))
            {
                if (!_canVinHandler->IsVinSet())
                {
                    memcpy(vanVinToBridgeToCan->Vin, vanMessageWithoutId, vanMessageLengthWithoutId);
                }
            }
            #pragma endregion
        }
        vanMessageLength = 0;
    }
 };

#endif
