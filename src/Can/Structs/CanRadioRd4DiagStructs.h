// CanRadioRd4DiagStructs.h
#pragma once

#ifndef _CanRadioRd4DiagStructs_h
    #define _CanRadioRd4DiagStructs_h

#include "../AbstractCanMessageSender.h"

// CANID: 760
const uint16_t CAN_ID_RADIO_DIAG = 0x760;
const uint16_t CAN_ID_RADIO_DIAG_ANSWER = 0x660;

class CanRadioRd4DiagPacketSender
{
    AbstractCanMessageSender * canMessageSender;

    uint8_t Vin[17];

    void SetVinPart2()
    {
        uint8_t vinPart2[] = { 0x21, Vin[4], Vin[5], Vin[6], Vin[7], Vin[8], Vin[9], Vin[10] };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 8, vinPart2);
        uint8_t vinPart3[] = { 0x22,  Vin[11], Vin[12], Vin[13], Vin[14], Vin[15], Vin[16] };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 7, vinPart3);
    }

    void ContinueGetVin()
    {
        uint8_t continuePacket[] = { 0x30, 0x00, 0x0A };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 3, continuePacket);
    }

    public:
    CanRadioRd4DiagPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    // We need to tell the radio to enter into diagnostics mode, and after that we can issue the other commands
    void EnterDiagMode()
    {
        uint8_t diagStart[] = { 0x02, 0x10, 0xC0 };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 3, diagStart);
    }

    // Start reading the VIN number
    // Take care that when you issue this command you need to watch for the answers from the radio and forward it to the ProcessReceivedCanMessage() method to successfully get the second part of the VIN
    void GetVinNumber()
    {
        uint8_t getVin[] = { 0x02, 0x21, 0xCC };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 3, getVin);
    }

    // We need to tell the radio periodically to stay in diag mode
    void KeepDiagModeAlive()
    {
        uint8_t keepAlive[] = { 0x01, 0x3E };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 2, keepAlive);
    }

    // Used to clear the stored fault codes
    void ClearFaults()
    {
        uint8_t clearFaults[] = { 0x03, 0x14, 0xFF, 0x00 };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 4, clearFaults);
    }

    // Used to set the VIN number 
    // Take care that when you issue this command you need to watch for the answers from the radio and forward it to the ProcessReceivedCanMessage() method to successfully program the VIN
    void SetVin(uint8_t vinAsciiBytes[17])
    {
        // on RD45 the 4th byte is 0xB0 instead of 0xCC 
        memcpy(Vin, vinAsciiBytes, (17 * sizeof(uint8_t)));
        uint8_t vinPart1[] = { 0x10, 0x13, 0x3B, 0xCC, Vin[0], Vin[1], Vin[2], Vin[3] };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 8, vinPart1);
    }

    // Used to exit the radio from diagnostics mode
    void ExitDiagMode()
    {
        uint8_t diagStop[] = { 0x02, 0x10, 0x81 };
        canMessageSender->SendMessage(CAN_ID_RADIO_DIAG, 0, 3, diagStop);
    }

    // While we are reading/writing diagnostics data we need to watch for the answers from the radio and based on the answer we need to reply immediately
    void ProcessReceivedCanMessage(uint16_t canId, uint8_t length, uint8_t canMsg[])
    {
        // We start coding the VIN with the SetVin() method and when we get the following reply from the radio, we continue with setting the second part of the VIN
        if (canId == CAN_ID_RADIO_DIAG_ANSWER && length == 3 && canMsg[0] == 0x30 && canMsg[1] == 0x00 && canMsg[2] == 0x0A)
        {
            SetVinPart2();
        }

        // We start reading the VIN with the GetVinNumber() method and when we get the following reply from the radio, we continue with getting the second part of the VIN
        if (canId == CAN_ID_RADIO_DIAG_ANSWER && length == 8 && canMsg[0] == 0x10 && canMsg[1] == 0x13 && canMsg[2] == 0x61 && canMsg[3] == 0xCC)
        {
            ContinueGetVin();
        }
    }
};

#endif
