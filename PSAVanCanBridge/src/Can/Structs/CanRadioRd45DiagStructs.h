// CanRadioRd45DiagStructs.h
#pragma once

#ifndef _CanRadioRd45DiagStructs_h
    #define _CanRadioRd45DiagStructs_h

#include "../AbstractCanMessageSender.h"

// CANID: 760
const uint16_t CAN_ID_RADIO_RD45_DIAG = 0x760;
const uint16_t CAN_ID_RADIO_RD45_DIAG_ANSWER = 0x660;

#pragma region Consts

const uint8_t CAN_RD45_GEO_LOCATION_OTHER_COUNTRIES = 0;
const uint8_t CAN_RD45_GEO_LOCATION_JAPAN           = 1;
const uint8_t CAN_RD45_GEO_LOCATION_LATIN_AMERICA   = 2;
const uint8_t CAN_RD45_GEO_LOCATION_ASIA            = 3;
const uint8_t CAN_RD45_GEO_LOCATION_USA             = 4;
const uint8_t CAN_RD45_GEO_LOCATION_SAUDI_ARABIA    = 6;
const uint8_t CAN_RD45_GEO_LOCATION_CHINA           = 7;

const uint8_t CAN_RD45_VOLUME_CORRECTION_PATTERN1 = 0;
const uint8_t CAN_RD45_VOLUME_CORRECTION_PATTERN2 = 1;
const uint8_t CAN_RD45_VOLUME_CORRECTION_PATTERN3 = 2;
const uint8_t CAN_RD45_VOLUME_CORRECTION_PATTERN4 = 3;

const uint8_t CAN_RD45_LO_DX_SENSITIVITY_CURVE_LOI_STANDARD = 0;
const uint8_t CAN_RD45_LO_DX_SENSITIVITY_CURVE_1            = 1;
const uint8_t CAN_RD45_LO_DX_SENSITIVITY_CURVE_2            = 2;
const uint8_t CAN_RD45_LO_DX_SENSITIVITY_CURVE_3            = 3;

const uint8_t CAN_RD45_NUMBER_OF_AERIALS_1 = 0;
const uint8_t CAN_RD45_NUMBER_OF_AERIALS_2 = 1;

const uint8_t CAN_RD45_AUX_MODE_DEACTIVATED            = 0;
const uint8_t CAN_RD45_AUX_MODE_ACTIVATED              = 1;
const uint8_t CAN_RD45_AUX_MODE_EXTERNAL_MUTE          = 2;
const uint8_t CAN_RD45_AUX_MODE_RESERVED_FOR_HANDSFREE = 3;

const uint8_t CAN_RD45_SCREEN_TYPE_A = 0;
const uint8_t CAN_RD45_SCREEN_TYPE_C = 1;

#pragma endregion

#pragma region Options1

// Read right to left in documentation
typedef struct {
    uint8_t geographic_location            : 3; // bit 0-2
    uint8_t am_waveband_activated          : 1; // bit 3
    uint8_t lo_dx_curve                    : 1; // bit 4
    uint8_t lo_dx_curve_type               : 2; // bit 5-6
    uint8_t detection_of_faults_on_antenna : 1; // bit 7
    uint8_t type_of_antenna_1               : 1; // bit 8
} CanRadioRd45DiagOptions1Byte1Struct;

typedef struct {
    uint8_t unknown0                      : 1; // bit 0
    uint8_t digital_tuner_activated       : 1; // bit 1
    uint8_t type_of_antenna_2             : 1; // bit 2
    uint8_t number_of_aerials             : 1; // bit 3
    uint8_t TA_activated                  : 1; // bit 4
    uint8_t display_name_of_radio_station : 1; // bit 5
    uint8_t station_search_rds_activated  : 1; // bit 6
    uint8_t radio_text_activated          : 1; // bit 7
} CanRadioRd45DiagOptions1Byte2Struct;

typedef struct {
    uint8_t radio_signal_filtering   : 2; // bit 0-1
    uint8_t aux_type                 : 2; // bit 2-3
    uint8_t screen_type              : 1; // bit 4
    uint8_t unknown5                 : 1; // bit 5
    uint8_t unknown6                 : 1; // bit 6
    uint8_t unknown7                 : 1; // bit 7
} CanRadioRd45DiagOptions1Byte3Struct;

// Read left to right in documentation
typedef struct CanRadioRd45DiagOptions1Struct {
    uint8_t Field0;
    CanRadioRd45DiagOptions1Byte1Struct Field1;
    CanRadioRd45DiagOptions1Byte2Struct Field2;
    CanRadioRd45DiagOptions1Byte3Struct Field3;
    uint8_t Field4;
};

typedef union CanRadioRd45DiagOptions1Packet {
    CanRadioRd45DiagOptions1Struct data;
    uint8_t CanRadioRd45DiagOptions1Packet[sizeof(CanRadioRd45DiagOptions1Struct)];
};

#pragma endregion

#pragma region Options 2

typedef struct {
    uint8_t handsfree_activated : 1; // bit 0
    uint8_t unknown2            : 1; // bit 1
    uint8_t unknown3            : 1; // bit 2
    uint8_t usb_activated       : 1; // bit 3
    uint8_t cd_text_activated   : 1; // bit 4
    uint8_t cd_player_activated : 1; // bit 5
    uint8_t unknown6            : 1; // bit 6
    uint8_t unknown7            : 1; // bit 7
} CanRadioRd45DiagOptions2Byte4Struct;

typedef struct {
    uint8_t close_control_config : 2; // bit 0
    uint8_t unknown2             : 1; // bit 2
    uint8_t unknown3             : 1; // bit 3
    uint8_t parkig_aid           : 2; // bit 4-5
    uint8_t unknown6             : 1; // bit 6
    uint8_t streaming_activated  : 1; // bit 7
} CanRadioRd45DiagOptions2Byte5Struct;

typedef struct {
    uint8_t sound_level_pattern                : 2; // bit 0-1
    uint8_t unknown2                           : 1; // bit 2
    uint8_t unknown3                           : 1; // bit 3
    uint8_t sound_leve_linked_to_vehicle_speed : 1; // bit 4
    uint8_t unknown6                           : 1; // bit 5
    uint8_t scrolling_of_remote_dial_2         : 1; // bit 6
    uint8_t scrolling_of_remote_dial_1         : 1; // bit 7
} CanRadioRd45DiagOptions2Byte6Struct;

typedef struct {
    uint8_t sound_distribution                 : 2; // bit 0-1
    uint8_t unknown2                           : 1; // bit 2
    uint8_t amplifier_type                     : 2; // bit 3-4
    uint8_t unknown6                           : 1; // bit 5
    uint8_t fader_activated                    : 1; // bit 6
    uint8_t display_russian_characters         : 1; // bit 7
} CanRadioRd45DiagOptions2Byte7Struct;

// Read left to right in documentation
typedef struct CanRadioRd45DiagOptions2Struct {
    uint8_t Field0;
    uint8_t Field1;
    uint8_t Field2;
    uint8_t Field3;
    CanRadioRd45DiagOptions2Byte4Struct Field4;
    CanRadioRd45DiagOptions2Byte5Struct Field5;
    CanRadioRd45DiagOptions2Byte6Struct Field6;
    CanRadioRd45DiagOptions2Byte7Struct Field7;
};

typedef union CanRadioRd45DiagOptions1Packet {
    CanRadioRd45DiagOptions1Struct data;
    uint8_t CanRadioRd45DiagOptions1Packet[sizeof(CanRadioRd45DiagOptions1Struct)];
};

#pragma endregion

class CanRadioRd45DiagPacketSender
{
    AbstractCanMessageSender * canMessageSender;

    uint8_t Vin[17];

    void SetVinPart2()
    {
        uint8_t vinPart2[] = { 0x21, Vin[4], Vin[5], Vin[6], Vin[7], Vin[8], Vin[9], Vin[10] };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 8, vinPart2);
        uint8_t vinPart3[] = { 0x22,  Vin[11], Vin[12], Vin[13], Vin[14], Vin[15], Vin[16] };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 7, vinPart3);
    }

    void Continue()
    {
        uint8_t continuePacket[] = { 0x30, 0x00, 0x0A };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 3, continuePacket);
    }

    void AnswerToDigitalKey(uint8_t answerByte1, uint8_t answerByte2, uint8_t answerByte3, uint8_t answerByte4)
    {
        uint8_t data[] = { 0x06, 0x27, 0x84, answerByte1, answerByte2, answerByte3, answerByte4 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 7, data);
    }

    public:
    CanRadioRd45DiagPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    // We need to tell the radio to enter into diagnostics mode, and after that we can issue the other commands
    void EnterDiagMode()
    {
        uint8_t diagStart[] = { 0x02, 0x10, 0xC0 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 3, diagStart);
    }

    // Start reading the VIN number
    // Take care that when you issue this command you need to watch for the answers from the radio and forward it to the ProcessReceivedCanMessage() method to successfully get the second part of the VIN
    void GetVinNumber()
    {
        uint8_t getVin[] = { 0x02, 0x21, 0xB0 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 3, getVin);
    }

    // We need to tell the radio periodically to stay in diag mode
    void KeepDiagModeAlive()
    {
        uint8_t keepAlive[] = { 0x01, 0x3E };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 2, keepAlive);
    }

    // Used to clear the stored fault codes
    void ClearFaults()
    {
        uint8_t clearFaults[] = { 0x03, 0x14, 0xFF, 0x00 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 4, clearFaults);
    }

    // Used to set the VIN number 
    // Take care that when you issue this command you need to watch for the answers from the radio and forward it to the ProcessReceivedCanMessage() method to successfully program the VIN
    void SetVin(uint8_t vinAsciiBytes[17])
    {
        memcpy(Vin, vinAsciiBytes, (17 * sizeof(uint8_t)));
        uint8_t vinPart1[] = { 0x10, 0x13, 0x3B, 0xB0, Vin[0], Vin[1], Vin[2], Vin[3] };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 8, vinPart1);
    }

    // Used to exit the radio from diagnostics mode
    void ExitDiagMode()
    {
        uint8_t diagStop[] = { 0x02, 0x10, 0x81 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 3, diagStop);
    }

    // Before we can store any data on the head unit we have to query a 4 byte key (key1) from it
    // After we have the key, we need to answer with another 4 byte key (key2) which - I assume - can be calculated from key1 with an unknown formula
    void GetDigitalKey()
    {
        uint8_t data[] = { 0x02, 0x27, 0x83 };
        canMessageSender->SendMessage(CAN_ID_RADIO_RD45_DIAG, 0, 3, data);
    }

    // While we are reading/writing diagnostics data we need to watch for the answers from the radio and based on the answer we need to reply immediately
    void ProcessReceivedCanMessage(uint16_t canId, uint8_t length, uint8_t canMsg[])
    {
        // We start coding the VIN with the SetVin() method and when we get the following reply from the radio, we continue with setting the second part of the VIN
        if (canId == CAN_ID_RADIO_RD45_DIAG_ANSWER && length == 3 && canMsg[0] == 0x30 && canMsg[1] == 0x00 && canMsg[2] == 0x0A)
        {
            SetVinPart2();
        }

        // There is some data which is longer than 8 byte which a CAN frame can contain. If we query such data, the first byte in the answer from the radio is 0x10 and the second byte is the length (in BCD) of the additional data we are going to receive if we issue a Continue() command
        // The first byte of the data which follows the Continue() message are numbered from 0x21

        // For example:
        // 760 02 21 B0                  Get VIN number
        // 660 10 13 61 B0 4C 44 43 32   VIN part 1 (first byte indicates that we need to call Continue, second byte means that 13 bytes of additional useful data will arrive)
        // 760 30 00 0A                  Issue continue command
        // 660 21 38 38 38 38 38 38 38   VIN part 2 (21 is a kind of an indexer, not part of the VIN)
        // 660 22 38 38 38 38 38 38      VIN part 3 (22 is a kind of an indexer, not part of the VIN)

        if (canId == CAN_ID_RADIO_RD45_DIAG_ANSWER && length == 8 && canMsg[0] == 0x10)
        {
            Continue();
        }

        if (canId == CAN_ID_RADIO_RD45_DIAG_ANSWER && length == 7 && canMsg[0] == 0x06 && canMsg[1] == 0x27 && canMsg[2] == 0x84)
        {
            // unfortunately we don't know how to calculate the answerBytes
            uint8_t answerByte1 = 0x00;
            uint8_t answerByte2 = 0x00;
            uint8_t answerByte3 = 0x00;
            uint8_t answerByte4 = 0x00;

            AnswerToDigitalKey(answerByte1, answerByte2, answerByte3, answerByte4);
        }
    }
};

#endif
