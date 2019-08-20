// VanDisplayStructs.h
#pragma once

#ifndef _VanDisplayStructs_h
    #define _VanDisplayStructs_h

#include "AbstractVanMessageSender.h"

#pragma region Message definitions
const int VAN_POPUP_MSG_TYRES_DEFLATED                         = 0X00;
const int VAN_POPUP_MSG_DOOR_OPEN                              = 0X01;
const int VAN_POPUP_MSG_GEARBOX_OIL_TEMP_TOO_HIGH              = 0X02;
const int VAN_POPUP_MSG_BRAKE_FLUID_LEVEL_LOW                  = 0X03;
const int VAN_POPUP_MSG_HYDRAULIC_SUSPENSION_PRESSURE          = 0X04;
const int VAN_POPUP_MSG_SERIOUS_SUSPENSION_FAULT               = 0X05;
const int VAN_POPUP_MSG_ENGINE_OIL_TEMPERATURE_TOO_HIGH        = 0X06;
const int VAN_POPUP_MSG_ENGINE_COOLANT_TEMP_TOO_HIGH           = 0X07;
const int VAN_POPUP_MSG_UNBLOCK_DIESEL_FILTER                  = 0X08;
const int VAN_POPUP_MSG_AUTO_ICON_WITH_STOP                    = 0x09;
const int VAN_POPUP_MSG_DIESEL_ADDITIVE_MINIMUM_LEVEL          = 0X0A;
const int VAN_POPUP_MSG_FUEL_TANK_ACCESS_OPEN                  = 0X0B;
const int VAN_POPUP_MSG_TYRES_PUNCTURED                        = 0X0C;
const int VAN_POPUP_MSG_TOP_UP_ENGINE_COOLANT_LEVEL            = 0X0D;
const int VAN_POPUP_MSG_OIL_PRESSURE_INSUFFICIENT              = 0X0E;
const int VAN_POPUP_MSG_TOP_UP_ENGINE_OIL_LEVEL                = 0X0F;
const int VAN_POPUP_MSG_ANTIPOLLUTION_FAULT                    = 0x10;
const int VAN_POPUP_MSG_BRAKE_PADS_WORN                        = 0x11;
const int VAN_POPUP_MSG_CHECK_CONTROL_OK                       = 0x12;
const int VAN_POPUP_MSG_AUTOMATIC_GEAR_FAULT                   = 0x13;
const int VAN_POPUP_MSG_ESP_ASR_NOT_FUNCTIONING                = 0x14;
const int VAN_POPUP_MSG_ABS_FAULT                              = 0x15;
const int VAN_POPUP_MSG_SUSPENSION_OR_STEERING_FAULT           = 0x16;
const int VAN_POPUP_MSG_BRAKING_FAULT                          = 0x17;
const int VAN_POPUP_MSG_SIDE_AIRBAG_FAULT                      = 0x18;
const int VAN_POPUP_MSG_AIRBAG_FAULT                           = 0x19;
const int VAN_POPUP_MSG_CRUISE_CONTROL_FAULT                   = 0x1A;
const int VAN_POPUP_MSG_ENGINE_COOLANT_TEMPERATURE_HIGH        = 0x1B;
const int VAN_POPUP_MSG_FAULT_LOAD_SHEDDING_IN_PROGRESS        = 0x1C;
const int VAN_POPUP_MSG_AUTO_LIGHTING_FAULT                    = 0x1D;
const int VAN_POPUP_MSG_AUTO_WIPER_FAULT                       = 0x1E;
const int VAN_POPUP_MSG_WATER_IN_DIESEL_FUEL_FILTER            = 0x1F;
const int VAN_POPUP_MSG_LEFT_SLIDING_DOOR_FAULT                = 0x20;
const int VAN_POPUP_MSG_HEADLIGHT_CORRECTOR_FAULT              = 0x21;
const int VAN_POPUP_MSG_RIGHT_SLIDING_DOOR_FAULT               = 0x22;
const int VAN_POPUP_MSG_NO_BROKEN_LAMP                         = 0x23;
const int VAN_POPUP_MSG_SECONDARY_BATTERY_CHARGE_FAULT         = 0x24;
const int VAN_POPUP_MSG_BATTERY_CHARGE_FAULT                   = 0x25;
const int VAN_POPUP_MSG_DIESEL_FUEL_ADDITIVE_FAULT             = 0x26;
const int VAN_POPUP_MSG_CATALYTIC_CONVERTER_FAULT              = 0X27;
const int VAN_POPUP_MSG_HANDBRAKE_ON                           = 0X28;
const int VAN_POPUP_MSG_SEAT_BELT_REMINDER                     = 0X29;
const int VAN_POPUP_MSG_PASSENGER_AIRBAG_DEACTIVATED           = 0X2A;
const int VAN_POPUP_MSG_TOP_UP_WASHER_FLUID                    = 0X2B;
const int VAN_POPUP_MSG_SPEED_TOO_HIGH                         = 0X2C;
const int VAN_POPUP_MSG_KEY_REMINDER                           = 0X2D;
const int VAN_POPUP_MSG_SIDE_LIGHTS_REMINDER                   = 0X2E;
const int VAN_POPUP_MSG_HILL_HOLDER_ACTIVE                     = 0X2F;
const int VAN_POPUP_MSG_IMPACT_SENSOR_FAULT                    = 0X30;

const int VAN_POPUP_MSG_WHEEL_PRESSURE_SENSOR_BATTERY_LOW      = 0X32;
const int VAN_POPUP_MSG_REMOTE_CONTROL_BATTERY_LOW             = 0X33;

const int VAN_POPUP_MSG_PUT_AUTO_TRANSMISS_LEVER_IN_POSITION_P = 0X35;
const int VAN_POPUP_MSG_TEST_STOP_LIGHTS_BRAKE_GENTLY          = 0X36;
const int VAN_POPUP_MSG_FUEL_LEVEL_LOW                         = 0X37;
const int VAN_POPUP_MSG_AUTOMATIC_LIGHTING_INACTIVE            = 0X38;
const int VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_FAULT            = 0X39;
const int VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_DEACTIVATED      = 0X3A;
const int VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_ACTIVATED        = 0X3B;
const int VAN_POPUP_MSG_DRIVING_SCHOOL_PEDALS_INDICATION       = 0X3C;
const int VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING        = 0X3D;
const int VAN_POPUP_MSG_DOORS_LOCKED                           = 0X40;
const int VAN_POPUP_MSG_ESP_ASR_DEACTIVATED                    = 0X41;
const int VAN_POPUP_MSG_CHILD_SAFETY_ACTIVE                    = 0X42;
const int VAN_POPUP_MSG_DEADLOCKING_ACTIVE                     = 0X43;
const int VAN_POPUP_MSG_AUTOMATIC_LIGHTING_ACTIVE              = 0X44;
const int VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE                = 0X45;
const int VAN_POPUP_MSG_ENGINE_IMMOBILISER_FAULT               = 0X46;
const int VAN_POPUP_MSG_SPORTS_SUSPENSION_ACTIVE               = 0X47;

const int VAN_POPUP_MSG_CHANGE_OF_FUEL_USED_IN_PROGRESS        = 0X53;
const int VAN_POPUP_MSG_LPG_FUEL_REFUSED                       = 0X54;
const int VAN_POPUP_MSG_LPG_SYSTEM_FAULTY                      = 0X55;
const int VAN_POPUP_MSG_LPG_IN_USE                             = 0X56;
const int VAN_POPUP_MSG_MIN_LEVEL_LPG                          = 0X57;
const int VAN_POPUP_MSG_ADIN_FAULT                             = 0X58;
const int VAN_POPUP_MSG_USE_STOP_AND_START                     = 0X59;
const int VAN_POPUP_MSG_STOP_AND_START_AVAILABLE               = 0X5A;
const int VAN_POPUP_MSG_STOP_AND_START_SYSTEM_ACTIVE           = 0X5B;
const int VAN_POPUP_MSG_STOP_AND_START_SYSTEM_DEACTIVATED      = 0X5C;
const int VAN_POPUP_MSG_STOP_AND_START_DEFERRED                = 0X5D;
const int VAN_POPUP_MSG_XSARA_DYNALTO                          = 0X5E;
const int VAN_POPUP_MSG_307_DYNALTO                            = 0X5F;

const int VAN_POPUP_MSG_PLACE_LEVER_IN_NEUTRAL                 = 0X67;
const int VAN_POPUP_MSG_DISPLAYS_MESSAGES_AUTOMATICALLY        = 0X121;

#pragma endregion


/*
    This packet is mainly dedicated to the central display and it's messages however some bits might be used by the instrument cluster also.
    The 9th byte (using zero based indexing) contains the ID of the messages which needs to be shown on the multifunction display (MFD).
    The bits and the message ID's in the 9th byte have a connection like this (the message only gets displayed if the connected bit is set to 1)
            Message Id      byte number in packet          bit
                0                   0                       0
                1                   0                       1
                2                   0                       2
                3                   0                       3
                4                   0                       4
                .                   .                       .
                8                   1                       0
                9                   1                       1
*/


//VANID: 524
const unsigned long VAN_ID_DISPLAY_POPUP = 0x524;

//Read right to left in documentation
typedef struct {
    uint8_t tyre_pressure_low                      : 1; // bit 0
    uint8_t no_door_open                           : 1; // bit 1
    uint8_t automatic_gearbox_temperature_too_high : 1; // bit 2
    uint8_t brake_system_fault                     : 1; // bit 3
    uint8_t hydraulic_suspension_pressure_faulty   : 1; // bit 4
    uint8_t suspension_faulty                      : 1; // bit 5
    uint8_t engine_oil_temperature_too_high        : 1; // bit 6
    uint8_t engine_overheating                     : 1; // bit 7
} VanDisplayMsgByte0Struct;

typedef struct {
    uint8_t unblock_diesel_filter     : 1; // bit 0
    uint8_t stop_car_icon             : 1; // bit 1
    uint8_t diesel_additive_too_low   : 1; // bit 2
    uint8_t fuel_tank_access_open     : 1; // bit 3
    uint8_t tyres_punctured           : 1; // bit 4
    uint8_t coolant_level_low         : 1; // bit 5
    uint8_t oil_pressure_too_low      : 1; // bit 6
    uint8_t oil_level_too_low         : 1; // bit 7
} VanDisplayMsgByte1Struct;

typedef struct {
    uint8_t mil                          : 1; // bit 0  displays Antipollution fault
    uint8_t brake_pads_worn              : 1; // bit 1
    uint8_t diagnosis_ok                 : 1; // bit 2
    uint8_t automatic_gearbox_faulty     : 1; // bit 3
    uint8_t esp                          : 1; // bit 4  displays ESP faulty
    uint8_t abs                          : 1; // bit 5  displays ABS fault
    uint8_t suspension_or_steering_fault : 1; // bit 6
    uint8_t braking_system_faulty        : 1; // bit 7
} VanDisplayMsgByte2Struct;

typedef struct {
    uint8_t side_airbag_faulty              : 1; // bit 0
    uint8_t airbags_faulty                  : 1; // bit 1
    uint8_t cruise_control_faulty           : 1; // bit 2
    uint8_t engine_temperature_too_high     : 1; // bit 3
    uint8_t fault_load_shedding_in_progress : 1; // bit 4
    uint8_t ambient_brightness_sensor_fault : 1; // bit 5
    uint8_t rain_sensor_fault               : 1; // bit 6
    uint8_t water_in_diesel_fuel_filter     : 1; // bit 7
} VanDisplayMsgByte3Struct;

typedef struct {
    uint8_t left_rear_sliding_door_faulty  : 1; // bit 0
    uint8_t headlight_corrector_fault      : 1; // bit 1
    uint8_t right_rear_sliding_door_faulty : 1; // bit 2
    uint8_t no_broken_lamp                 : 1; // bit 3
    uint8_t battery_low                    : 1; // bit 4
    uint8_t battery_charge_fault           : 1; // bit 5
    uint8_t diesel_particle_filter_faulty  : 1; // bit 6
    uint8_t catalytic_converter_fault      : 1; // bit 7
} VanDisplayMsgByte4Struct;

typedef struct {
    uint8_t handbrake                          : 1; // bit 0
    uint8_t seatbelt_warning                   : 1; // bit 1  lights the instrument cluster also
    uint8_t passenger_airbag_deactivated       : 1; // bit 2
    uint8_t screen_washer_liquid_level_too_low : 1; // bit 3
    uint8_t current_speed_too_high             : 1; // bit 4
    uint8_t ignition_key_left_in               : 1; // bit 5
    uint8_t sidelights_left_on                 : 1; // bit 6
    uint8_t hill_holder_active                 : 1; // bit 7  on RT3 monochrome: Driver's seatbelt not fastened
} VanDisplayMsgByte5Struct;

typedef struct {
    uint8_t shock_sensor_faulty                  : 1; // bit 0
    uint8_t seatbelt_warning_                    : 1; // bit 1 not sure if it is seatbelt warning
    uint8_t check_and_reinitialize_tyre_pressure : 1; // bit 2
    uint8_t remote_control_battery_low           : 1; // bit 3
    uint8_t left_stick_button                    : 1; // bit 4
    uint8_t put_automatic_gearbox_in_P_position  : 1; // bit 5
    uint8_t stop_lights_test_brake_gently        : 1; // bit 6
    uint8_t fuel_level_low                       : 1; // bit 7  lights the instrument cluster also
} VanDisplayMsgByte6Struct;

typedef struct {
    uint8_t automatic_headlamp_lighting_deactivated : 1; // bit 0
    uint8_t rear_lh_passenger_seatbelt_not_fastened : 1; // bit 1
    uint8_t rear_rh_passenger_seatbelt_not_fastened : 1; // bit 2
    uint8_t front_passenger_seatbelt_not_fastened   : 1; // bit 3
    uint8_t driving_school_pedals_indication        : 1; // bit 4
    uint8_t number_of_tpms_missing                  : 3; // bit 5-7  RT3 displays: X tyre pressure sensors missing where x equals this number
} VanDisplayMsgByte7Struct;

typedef struct {
    uint8_t doors_locked                 : 1; // bit 0
    uint8_t esp_asr_deactivated          : 1; // bit 1
    uint8_t child_safety_activated       : 1; // bit 2
    uint8_t deadlocking_active           : 1; // bit 3
    uint8_t automatic_lighting_active    : 1; // bit 4
    uint8_t automatic_wiping_active      : 1; // bit 5
    uint8_t engine_immobilizer_fault     : 1; // bit 6
    uint8_t sport_suspension_mode_active : 1; // bit 7
} VanDisplayMsgByte8Struct;

typedef struct {
    uint8_t                                 : 1; // bit 0
    uint8_t                                 : 1; // bit 1
    uint8_t                                 : 1; // bit 2
    uint8_t change_of_fuel_used_in_progress : 1; // bit 3
    uint8_t lpg_fuel_refused                : 1; // bit 4
    uint8_t lpg_system_faulty               : 1; // bit 5
    uint8_t lpg_in_use                      : 1; // bit 6
    uint8_t min_level_lpg                   : 1; // bit 7
} VanDisplayMsgByte10Struct;

typedef struct {
    uint8_t adin_fault                    : 1; // bit 0
    uint8_t use_stop_start                : 1; // bit 1
    uint8_t stop_start_available          : 1; // bit 2
    uint8_t stop_start_activated          : 1; // bit 3
    uint8_t stop_start_deactivated        : 1; // bit 4
    uint8_t stop_start_deferred           : 1; // bit 5
    uint8_t unknown6                      : 1; // bit 6 displays empty messagebox on RT3 monochrome when the Message byte is 0x5E
    uint8_t unknown7                      : 1; // bit 7 displays empty messagebox on RT3 monochrome when the Message byte is 0x5F
} VanDisplayMsgByte11Struct;

typedef struct {
    uint8_t                               : 1; // bit 0
    uint8_t                               : 1; // bit 1
    uint8_t                               : 1; // bit 2
    uint8_t                               : 1; // bit 3
    uint8_t                               : 1; // bit 4
    uint8_t                               : 1; // bit 5
    uint8_t                               : 1; // bit 6
    uint8_t change_to_neutral             : 1; // bit 7
} VanDisplayMsgByte12Struct;


//Read left to right in documentation
typedef struct VanDisplayStruct {
    VanDisplayMsgByte0Struct Field0;
    VanDisplayMsgByte1Struct Field1;
    VanDisplayMsgByte2Struct Field2;
    VanDisplayMsgByte3Struct Field3;
    VanDisplayMsgByte4Struct Field4;
    VanDisplayMsgByte5Struct Field5;
    VanDisplayMsgByte6Struct Field6;
    VanDisplayMsgByte7Struct Field7;
    VanDisplayMsgByte8Struct Field8;
    uint8_t Message;
    VanDisplayMsgByte10Struct Field10;
    VanDisplayMsgByte11Struct Field11;
    VanDisplayMsgByte12Struct Field12;
    uint8_t Field13;
};

typedef union VanDisplayPacket {
    VanDisplayStruct data;
    uint8_t VanDisplayPacket[sizeof(VanDisplayStruct)];
};

#pragma region Sender class
class VanDisplayPacketSender
{
    AbstractVanMessageSender * vanMessageSender;

public:
    VanDisplayPacketSender(AbstractVanMessageSender * object)
    {
        vanMessageSender = object;
    }

    void Send(uint8_t channelId, uint8_t messageId)
    {
        VanDisplayPacket packet;
        memset(&packet, 0, sizeof(packet));//fill everything with 0 - https://stackoverflow.com/a/6891737/5453350

        unsigned char *serializedPacket = Serialize<VanDisplayPacket>(packet);

        // display messages are best to test with everything set to FF, because that way every message type apppears

        serializedPacket[0] = 0xFF;
        serializedPacket[1] = 0xFF;
        serializedPacket[2] = 0xFF;
        serializedPacket[3] = 0xFF;
        serializedPacket[4] = 0xFF;
        serializedPacket[5] = 0xFF;
        serializedPacket[6] = 0xFF;
        serializedPacket[7] = 0xFF;
        serializedPacket[8] = 0xFF;
        serializedPacket[9] = messageId;
        serializedPacket[10] = 0xFF;
        serializedPacket[11] = 0xFF;
        serializedPacket[12] = 0xFF;
        serializedPacket[13] = 0xFF;

        vanMessageSender->set_channel_for_transmit_message(channelId, 0x52, 4, serializedPacket, sizeof(packet), 0);
        memset(&packet, 0, 0);
        delete[] serializedPacket;
        vanMessageSender->disable_channel(channelId);
    }
};
#pragma endregion

#endif
