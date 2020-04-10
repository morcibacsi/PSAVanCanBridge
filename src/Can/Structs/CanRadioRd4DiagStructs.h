// CanRadioRd4DiagStructs.h
#pragma once

#ifndef _CanRadioRd4DiagStructs_h
    #define _CanRadioRd4DiagStructs_h

#include "../AbstractCanMessageSender.h"

// CANID: 760
const uint16_t CAN_ID_RADIO_RD4_DIAG = 0x760;
// CANID: 660
const uint16_t CAN_ID_RADIO_RD4_DIAG_ANSWER = 0x660;

const uint8_t CAN_RD4_GEO_LOCATION_WESTERN_EU      = 0;
const uint8_t CAN_RD4_GEO_LOCATION_JAPAN           = 1;
const uint8_t CAN_RD4_GEO_LOCATION_LATIN_AMERICA   = 2;
const uint8_t CAN_RD4_GEO_LOCATION_ASIA            = 3;
const uint8_t CAN_RD4_GEO_LOCATION_SAUDI_ARABIA    = 6;
const uint8_t CAN_RD4_GEO_LOCATION_OTHER_COUNTRIES = 7;

const uint8_t CAN_RD4_VOLUME_CORRECTION_PATTERN1 = 0x0;
const uint8_t CAN_RD4_VOLUME_CORRECTION_PATTERN2 = 0x1;
const uint8_t CAN_RD4_VOLUME_CORRECTION_PATTERN3 = 0x2;
const uint8_t CAN_RD4_VOLUME_CORRECTION_PATTERN4 = 0x3;
const uint8_t CAN_RD4_VOLUME_CORRECTION_PATTERN5 = 0x4;

const uint8_t CAN_RD4_LO_DC_SENSITIVITY_CURVE_1 = 0;
const uint8_t CAN_RD4_LO_DC_SENSITIVITY_CURVE_2 = 1;
const uint8_t CAN_RD4_LO_DC_SENSITIVITY_CURVE_3 = 2;

const uint8_t CAN_RD4_NUMBER_OF_AERIALS_1 = 0;
const uint8_t CAN_RD4_NUMBER_OF_AERIALS_2 = 1;

const uint8_t CAN_RD4_AUX_MODE_DEACTIVATED = 0;
const uint8_t CAN_RD4_AUX_MODE_ACTIVATED = 1;
const uint8_t CAN_RD4_AUX_MODE_EXTERNAL_MUTE = 2;
const uint8_t CAN_RD4_AUX_MODE_RESERVED_FOR_NOMAD = 3;

// Read right to left in documentation
typedef struct {
    uint8_t volume_auto_control       : 1; // bit 0
    uint8_t am_waveband_activated     : 1; // bit 1
    uint8_t rear_fader_activated      : 1; // bit 2
    uint8_t cd_changer_or_usb_enabled : 1; // bit 3
    uint8_t geographic_location       : 4; // bit 4-7
} CanRadioRd4DiagOptions1Byte3Struct;

typedef struct {
    uint8_t aerial_failure_detection_activated  : 1; // bit 0
    uint8_t lo_dc_sensitivity_curve             : 2; // bit 1-2
    uint8_t volume_level_correction_pattern     : 3;
    uint8_t unknown6                            : 1; // bit 6
    uint8_t amplifier_enabled                   : 1; // bit 7
} CanRadioRd4DiagOptions1Byte4Struct;

typedef struct {
    uint8_t unknown0                     : 1; // bit 0
    uint8_t unknown1                     : 1; // bit 1
    uint8_t parking_aid_enabled          : 1; // bit 2
    uint8_t cd_text_activated            : 1; // bit 3
    uint8_t radio_text_activated         : 1; // bit 4
    uint8_t type_of_aerial_no2_activated : 1; // bit 5
    uint8_t type_of_aerial_no1_activated : 1; // bit 6
    uint8_t number_of_aerials            : 1; // bit 7
} CanRadioRd4DiagOptions1Byte5Struct;

typedef struct {
    uint8_t unknown0                 : 1; // bit 0
    uint8_t unknown1                 : 1; // bit 1
    uint8_t unknown2                 : 1; // bit 2
    uint8_t is_amplifier_multiplexed : 1; // bit 3 (also changes bit in byte4)
    uint8_t aux2_mode                : 2; // bit 4-5
    uint8_t aux1_mode                : 2; // bit 5-6
} CanRadioRd4DiagOptions1Byte6Struct;

typedef struct {
    uint8_t rear_parking_beep_volume  : 3; // bit 0-2 (value: 0-7)
    uint8_t front_parking_beep_volume : 3; // bit 3-5 (value: 0-7)
    uint8_t unknown6                 : 1; // bit 6
    uint8_t unknown7                 : 1; // bit 7
} CanRadioRd4DiagOptions1Byte7Struct;

// Read left to right in documentation
typedef struct CanRadioRd4DiagOptions1Struct {
    uint8_t Field0;
    uint8_t Field1;
    uint8_t Field2;
    CanRadioRd4DiagOptions1Byte3Struct Field3;
    CanRadioRd4DiagOptions1Byte4Struct Field4;
    CanRadioRd4DiagOptions1Byte5Struct Field5;
    CanRadioRd4DiagOptions1Byte6Struct Field6;
    CanRadioRd4DiagOptions1Byte7Struct Field7;
};

typedef union CanRadioRd4DiagOptions1Packet {
    CanRadioRd4DiagOptions1Struct data;
    uint8_t CanRadioRd4DiagOptions1Packet[sizeof(CanRadioRd4DiagOptions1Struct)];
};

class CanRadioRd4DiagPacketSender
{
    AbstractCanMessageSender * canMessageSender;

    uint8_t Vin[17];

    void SetVinPart2()
    {
        uint8_t vinPart2[] = { 0x21, Vin[4], Vin[5], Vin[6], Vin[7], Vin[8], Vin[9], Vin[10] };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 8, vinPart2);
        uint8_t vinPart3[] = { 0x22,  Vin[11], Vin[12], Vin[13], Vin[14], Vin[15], Vin[16] };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 7, vinPart3);
    }

    void Continue()
    {
        //uint8_t continuePacket[] = { 0x30, 0x00, 0x14 }; //RD43
        uint8_t continuePacket[] = { 0x30, 0x00, 0x0A };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 3, continuePacket);
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
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 3, diagStart);
    }

    // Start reading the VIN number
    // Take care that when you issue this command you need to watch for the answers from the radio and forward it to the ProcessReceivedCanMessage() method to successfully get the second part of the VIN
    void GetVinNumber()
    {
        uint8_t getVin[] = { 0x02, 0x21, 0xCC };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 3, getVin);
    }

    // We need to tell the radio periodically to stay in diag mode
    void KeepDiagModeAlive()
    {
        uint8_t keepAlive[] = { 0x01, 0x3E };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 2, keepAlive);
    }

    // Used to clear the stored fault codes
    void ClearFaults()
    {
        uint8_t clearFaults[] = { 0x03, 0x14, 0xFF, 0x00 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 4, clearFaults);
    }

    void GetOptions1()
    {
        uint8_t getOptions[] = { 0x02, 0x21, 0xC0 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 3, getOptions);
    }

    void GetBrightnessLevels()
    {
        uint8_t getBrightnessLevels[] = { 0x02, 0x21, 0xB0 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 3, getBrightnessLevels);
    }

    // Used to set the VIN number 
    // Take care that when you issue this command you need to watch for the answers from the radio and forward it to the ProcessReceivedCanMessage() method to successfully program the VIN
    void SetVin(uint8_t vinAsciiBytes[17])
    {
        // on RD45 the 4th byte is 0xB0 instead of 0xCC 
        memcpy(Vin, vinAsciiBytes, (17 * sizeof(uint8_t)));
        uint8_t vinPart1[] = { 0x10, 0x13, 0x3B, 0xCC, Vin[0], Vin[1], Vin[2], Vin[3] };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 8, vinPart1);
    }

    // Used to exit the radio from diagnostics mode
    void ExitDiagMode()
    {
        uint8_t diagStop[] = { 0x02, 0x10, 0x81 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD4_DIAG, 0, 3, diagStop);
    }

    // While we are reading/writing diagnostics data we need to watch for the answers from the radio and based on the answer we need to reply immediately
    void ProcessReceivedCanMessage(uint16_t canId, uint8_t length, uint8_t canMsg[])
    {
        // We start coding the VIN with the SetVin() method and when we get the following reply from the radio, we continue with setting the second part of the VIN
        if (canId == CAN_ID_RADIO_RD4_DIAG_ANSWER && length == 3 && canMsg[0] == 0x30 && canMsg[1] == 0x00 && canMsg[2] == 0x0A)
        {
            SetVinPart2();
        }

        // There is some data which is longer than 8 byte which a CAN frame can contain. If we query such data, the first byte in the answer from the radio is 0x10 and the second byte is the length (in BCD) of the additional data we are going to receive if we issue a Continue() command
        // The first byte of the data which follows the Continue() message are numbered from 0x21

        // For example:
        // 760 02 21 CC                  Get VIN number
        // 660 10 13 61 B0 4C 44 43 32   VIN part 1 (first byte indicates that we need to call Continue, second byte means that 13 bytes of additional useful data will arrive)
        // 760 30 00 0A                  Issue continue command
        // 660 21 38 38 38 38 38 38 38   VIN part 2 (21 is a kind of an indexer, not part of the VIN)
        // 660 22 38 38 38 38 38 38      VIN part 3 (22 is a kind of an indexer, not part of the VIN)

        if (canId == CAN_ID_RADIO_RD4_DIAG_ANSWER && length == 8 && canMsg[0] == 0x10)
        {
            Continue();
        }

        if (canId == CAN_ID_RADIO_RD4_DIAG_ANSWER && length == 3 && canMsg[0] == 0x02 && canMsg[1] == 0x7B && canMsg[2] == 0xC0)
        {
            GetOptions1();
        }

        if (canId == CAN_ID_RADIO_RD4_DIAG_ANSWER && length == 3 && canMsg[0] == 0x02 && canMsg[1] == 0x50 && canMsg[2] == 0xC0)
        {
            GetVinNumber();
        }
    }
};

#endif
