#pragma once

#ifndef _VAN_524_h
    #define _VAN_524_h


#include <stdint.h>

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
const uint16_t VAN_ID_DISPLAY_POPUP_V2 = 0x524;

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
} VanDisplayMsgV2Byte0Struct;

typedef struct {
    uint8_t unblock_diesel_filter     : 1; // bit 0
    uint8_t stop_car_icon             : 1; // bit 1
    uint8_t diesel_additive_too_low   : 1; // bit 2
    uint8_t fuel_tank_access_open     : 1; // bit 3
    uint8_t tyre_punctured            : 1; // bit 4
    uint8_t coolant_level_low         : 1; // bit 5
    uint8_t oil_pressure_too_low      : 1; // bit 6
    uint8_t oil_level_too_low         : 1; // bit 7
} VanDisplayMsgV2Byte1Struct;

typedef struct {
    uint8_t mil                          : 1; // bit 0  displays Antipollution fault
    uint8_t brake_pads_worn              : 1; // bit 1
    uint8_t diagnosis_ok                 : 1; // bit 2
    uint8_t automatic_gearbox_faulty     : 1; // bit 3
    uint8_t esp                          : 1; // bit 4  displays ESP faulty
    uint8_t abs                          : 1; // bit 5  displays ABS fault
    uint8_t suspension_or_steering_fault : 1; // bit 6
    uint8_t braking_system_faulty        : 1; // bit 7
} VanDisplayMsgV2Byte2Struct;

typedef struct {
    uint8_t side_airbag_faulty              : 1; // bit 0
    uint8_t airbags_faulty                  : 1; // bit 1
    uint8_t cruise_control_faulty           : 1; // bit 2
    uint8_t engine_temperature_too_high     : 1; // bit 3
    uint8_t fault_load_shedding_in_progress : 1; // bit 4
    uint8_t ambient_brightness_sensor_fault : 1; // bit 5
    uint8_t rain_sensor_fault               : 1; // bit 6
    uint8_t water_in_diesel_fuel_filter     : 1; // bit 7
} VanDisplayMsgV2Byte3Struct;

typedef struct {
    uint8_t left_rear_sliding_door_faulty  : 1; // bit 0
    uint8_t headlight_corrector_fault      : 1; // bit 1
    uint8_t right_rear_sliding_door_faulty : 1; // bit 2
    uint8_t no_broken_lamp                 : 1; // bit 3
    uint8_t battery_low                    : 1; // bit 4
    uint8_t battery_charge_fault           : 1; // bit 5
    uint8_t diesel_particle_filter_faulty  : 1; // bit 6
    uint8_t catalytic_converter_fault      : 1; // bit 7
} VanDisplayMsgV2Byte4Struct;

typedef struct {
    uint8_t handbrake                          : 1; // bit 0
    uint8_t seatbelt_warning                   : 1; // bit 1  lights the instrument cluster also
    uint8_t passenger_airbag_deactivated       : 1; // bit 2
    uint8_t screen_washer_liquid_level_too_low : 1; // bit 3
    uint8_t current_speed_too_high             : 1; // bit 4
    uint8_t ignition_key_left_in               : 1; // bit 5
    uint8_t sidelights_left_on                 : 1; // bit 6
    uint8_t hill_holder_active                 : 1; // bit 7  on RT3 monochrome: Driver's seatbelt not fastened
} VanDisplayMsgV2Byte5Struct;

typedef struct {
    uint8_t shock_sensor_faulty                  : 1; // bit 0
    uint8_t seatbelt_warning_                    : 1; // bit 1 not sure if it is seatbelt warning
    uint8_t check_and_reinitialize_tyre_pressure : 1; // bit 2
    uint8_t remote_control_battery_low           : 1; // bit 3
    uint8_t left_stick_button                    : 1; // bit 4
    uint8_t put_automatic_gearbox_in_P_position  : 1; // bit 5
    uint8_t stop_lights_test_brake_gently        : 1; // bit 6
    uint8_t fuel_level_low                       : 1; // bit 7  lights the instrument cluster also
} VanDisplayMsgV2Byte6Struct;

typedef struct {
    uint8_t automatic_headlamp_lighting_deactivated : 1; // bit 0
    uint8_t rear_lh_passenger_seatbelt_not_fastened : 1; // bit 1
    uint8_t rear_rh_passenger_seatbelt_not_fastened : 1; // bit 2
    uint8_t front_passenger_seatbelt_not_fastened   : 1; // bit 3
    uint8_t driving_school_pedals_indication        : 1; // bit 4
    uint8_t number_of_tpms_missing                  : 3; // bit 5-7  RT3 displays: X tyre pressure sensors missing where x equals this number
} VanDisplayMsgV2Byte7Struct;

typedef struct {
    uint8_t doors_locked                 : 1; // bit 0
    uint8_t esp_asr_deactivated          : 1; // bit 1
    uint8_t child_safety_activated       : 1; // bit 2
    uint8_t deadlocking_active           : 1; // bit 3
    uint8_t automatic_lighting_active    : 1; // bit 4
    uint8_t automatic_wiping_active      : 1; // bit 5
    uint8_t engine_immobilizer_fault     : 1; // bit 6
    uint8_t sport_suspension_mode_active : 1; // bit 7
} VanDisplayMsgV2Byte8Struct;

typedef struct {
    uint8_t                                 : 1; // bit 0
    uint8_t                                 : 1; // bit 1
    uint8_t                                 : 1; // bit 2
    uint8_t change_of_fuel_used_in_progress : 1; // bit 3
    uint8_t lpg_fuel_refused                : 1; // bit 4
    uint8_t lpg_system_faulty               : 1; // bit 5
    uint8_t lpg_in_use                      : 1; // bit 6
    uint8_t min_level_lpg                   : 1; // bit 7
} VanDisplayMsgV2Byte10Struct;

typedef struct {
    uint8_t adin_fault                    : 1; // bit 0
    uint8_t use_stop_start                : 1; // bit 1
    uint8_t stop_start_available          : 1; // bit 2
    uint8_t stop_start_activated          : 1; // bit 3
    uint8_t stop_start_deactivated        : 1; // bit 4
    uint8_t stop_start_deferred           : 1; // bit 5
    uint8_t unknown6                      : 1; // bit 6 displays empty messagebox on RT3 monochrome when the Message byte is 0x5E
    uint8_t unknown7                      : 1; // bit 7 displays empty messagebox on RT3 monochrome when the Message byte is 0x5F
} VanDisplayMsgV2Byte11Struct;

typedef struct {
    uint8_t                               : 1; // bit 0
    uint8_t                               : 1; // bit 1
    uint8_t                               : 1; // bit 2
    uint8_t                               : 1; // bit 3
    uint8_t                               : 1; // bit 4
    uint8_t                               : 1; // bit 5
    uint8_t                               : 1; // bit 6
    uint8_t change_to_neutral             : 1; // bit 7
} VanDisplayMsgV2Byte12Struct;


//Read left to right in documentation
struct VanDisplayV2Struct {
    VanDisplayMsgV2Byte0Struct Field0;
    VanDisplayMsgV2Byte1Struct Field1;
    VanDisplayMsgV2Byte2Struct Field2;
    VanDisplayMsgV2Byte3Struct Field3;
    VanDisplayMsgV2Byte4Struct Field4;
    VanDisplayMsgV2Byte5Struct Field5;
    VanDisplayMsgV2Byte6Struct Field6;
    VanDisplayMsgV2Byte7Struct Field7;
    VanDisplayMsgV2Byte8Struct Field8;
    uint8_t Message;
    VanDisplayMsgV2Byte10Struct Field10;
    VanDisplayMsgV2Byte11Struct Field11;
    VanDisplayMsgV2Byte12Struct Field12;
    uint8_t Field13;
    uint8_t Field14;
    uint8_t Field15;
};

union VanDisplayPacketV2 {
    VanDisplayV2Struct data;
    uint8_t VanDisplayPacketV2[sizeof(VanDisplayV2Struct)];
};

#pragma region Message definitions

const uint8_t VAN_POPUP_MSG_TYRES_DEFLATED                          = 0x00;
const uint8_t VAN_POPUP_MSG_DOOR_OPEN                               = 0x01;
const uint8_t VAN_POPUP_MSG_GEARBOX_OIL_TEMP_TOO_HIGH               = 0x02;
const uint8_t VAN_POPUP_MSG_BRAKE_FLUID_LEVEL_LOW                   = 0x03;
const uint8_t VAN_POPUP_MSG_HYDRAULIC_SUSPENSION_PRESSURE           = 0x04;
const uint8_t VAN_POPUP_MSG_SERIOUS_SUSPENSION_FAULT                = 0x05;
const uint8_t VAN_POPUP_MSG_ENGINE_OIL_TEMPERATURE_TOO_HIGH         = 0x06;
const uint8_t VAN_POPUP_MSG_ENGINE_COOLANT_TEMP_TOO_HIGH            = 0x07;
const uint8_t VAN_POPUP_MSG_UNBLOCK_DIESEL_FILTER                   = 0x08;
const uint8_t VAN_POPUP_MSG_AUTO_ICON_WITH_STOP                     = 0x09;
const uint8_t VAN_POPUP_MSG_DIESEL_ADDITIVE_MINIMUM_LEVEL           = 0x0A;
const uint8_t VAN_POPUP_MSG_FUEL_TANK_ACCESS_OPEN                   = 0x0B;
const uint8_t VAN_POPUP_MSG_TYRES_PUNCTURED                         = 0x0C;
const uint8_t VAN_POPUP_MSG_TOP_UP_ENGINE_COOLANT_LEVEL             = 0x0D;
const uint8_t VAN_POPUP_MSG_OIL_PRESSURE_INSUFFICIENT               = 0x0E;
const uint8_t VAN_POPUP_MSG_TOP_UP_ENGINE_OIL_LEVEL                 = 0x0F;
const uint8_t VAN_POPUP_MSG_ANTIPOLLUTION_FAULT                     = 0x10;
const uint8_t VAN_POPUP_MSG_BRAKE_PADS_WORN                         = 0x11;
const uint8_t VAN_POPUP_MSG_CHECK_CONTROL_OK                        = 0x12;
const uint8_t VAN_POPUP_MSG_AUTOMATIC_GEAR_FAULT                    = 0x13;
const uint8_t VAN_POPUP_MSG_ESP_ASR_NOT_FUNCTIONING                 = 0x14;
const uint8_t VAN_POPUP_MSG_ABS_FAULT                               = 0x15;
const uint8_t VAN_POPUP_MSG_SUSPENSION_OR_STEERING_FAULT            = 0x16;
const uint8_t VAN_POPUP_MSG_BRAKING_FAULT                           = 0x17;
const uint8_t VAN_POPUP_MSG_SIDE_AIRBAG_FAULT                       = 0x18;
const uint8_t VAN_POPUP_MSG_AIRBAG_FAULT                            = 0x19;
const uint8_t VAN_POPUP_MSG_CRUISE_CONTROL_FAULT                    = 0x1A;
const uint8_t VAN_POPUP_MSG_ENGINE_COOLANT_TEMPERATURE_HIGH         = 0x1B;
const uint8_t VAN_POPUP_MSG_FAULT_LOAD_SHEDDING_IN_PROGRESS         = 0x1C;
const uint8_t VAN_POPUP_MSG_AUTO_LIGHTING_FAULT                     = 0x1D;
const uint8_t VAN_POPUP_MSG_AUTO_WIPER_FAULT                        = 0x1E;
const uint8_t VAN_POPUP_MSG_WATER_IN_DIESEL_FUEL_FILTER             = 0x1F;
const uint8_t VAN_POPUP_MSG_LEFT_SLIDING_DOOR_FAULT                 = 0x20;
const uint8_t VAN_POPUP_MSG_HEADLIGHT_CORRECTOR_FAULT               = 0x21;
const uint8_t VAN_POPUP_MSG_RIGHT_SLIDING_DOOR_FAULT                = 0x22;
const uint8_t VAN_POPUP_MSG_NO_BROKEN_LAMP                          = 0x23;
const uint8_t VAN_POPUP_MSG_SECONDARY_BATTERY_CHARGE_FAULT          = 0x24;
const uint8_t VAN_POPUP_MSG_BATTERY_CHARGE_FAULT                    = 0x25;
const uint8_t VAN_POPUP_MSG_DIESEL_FUEL_ADDITIVE_FAULT              = 0x26;
const uint8_t VAN_POPUP_MSG_CATALYTIC_CONVERTER_FAULT               = 0x27;
const uint8_t VAN_POPUP_MSG_HANDBRAKE_ON                            = 0x28;
const uint8_t VAN_POPUP_MSG_SEAT_BELT_REMINDER                      = 0x29;
const uint8_t VAN_POPUP_MSG_PASSENGER_AIRBAG_DEACTIVATED            = 0x2A;
const uint8_t VAN_POPUP_MSG_TOP_UP_WASHER_FLUID                     = 0x2B;
const uint8_t VAN_POPUP_MSG_SPEED_TOO_HIGH                          = 0x2C;
const uint8_t VAN_POPUP_MSG_KEY_REMINDER                            = 0x2D;
const uint8_t VAN_POPUP_MSG_SIDE_LIGHTS_REMINDER                    = 0x2E;
const uint8_t VAN_POPUP_MSG_HILL_HOLDER_ACTIVE                      = 0x2F; // On RT3 monochrome: Driver seatbelt not fastened
const uint8_t VAN_POPUP_MSG_IMPACT_SENSOR_FAULT                     = 0x30;

const uint8_t VAN_POPUP_MSG_WHEEL_PRESSURE_SENSOR_BATTERY_LOW       = 0x32;
const uint8_t VAN_POPUP_MSG_REMOTE_CONTROL_BATTERY_LOW              = 0x33;

const uint8_t VAN_POPUP_MSG_PUT_AUTO_TRANSMISS_LEVER_IN_POSITION_P  = 0x35;
const uint8_t VAN_POPUP_MSG_TEST_STOP_LIGHTS_BRAKE_GENTLY           = 0x36;
const uint8_t VAN_POPUP_MSG_FUEL_LEVEL_LOW                          = 0x37;
const uint8_t VAN_POPUP_MSG_AUTOMATIC_LIGHTING_INACTIVE             = 0x38;
const uint8_t VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_FAULT             = 0x39; // On RT3 monochrome: Rear LH passenger seatbelt unfastened
const uint8_t VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_DEACTIVATED       = 0x3A; // On RT3 monochrome: Rear RH passenger seatbelt unfastened
const uint8_t VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_ACTIVATED         = 0x3B; // On RT3 monochrome: Front passenger seatbelt unfastened
const uint8_t VAN_POPUP_MSG_DRIVING_SCHOOL_PEDALS_INDICATION        = 0x3C;
const uint8_t VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING0        = 0x3D;
const uint8_t VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING1        = 0x3E;
const uint8_t VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING2        = 0x3F;
const uint8_t VAN_POPUP_MSG_DOORS_LOCKED                            = 0x40;
const uint8_t VAN_POPUP_MSG_ESP_ASR_DEACTIVATED                     = 0x41;
const uint8_t VAN_POPUP_MSG_CHILD_SAFETY_ACTIVE                     = 0x42;
const uint8_t VAN_POPUP_MSG_DEADLOCKING_ACTIVE                      = 0x43;
const uint8_t VAN_POPUP_MSG_AUTOMATIC_LIGHTING_ACTIVE               = 0x44;
const uint8_t VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE                 = 0x45;
const uint8_t VAN_POPUP_MSG_ENGINE_IMMOBILISER_FAULT                = 0x46;
const uint8_t VAN_POPUP_MSG_SPORTS_SUSPENSION_ACTIVE                = 0x47;

const uint8_t VAN_POPUP_MSG_LPG_DEFAULT                             = 0x4B; // Seen on a display from Peugeot 1007
const uint8_t VAN_POPUP_MSG_FLH_DIPPED_HEADLIGHT_BULB_BLOWN         = 0x4E; // Stop and start system active (Peugeot 1007)

const uint8_t VAN_POPUP_MSG_CHANGE_OF_FUEL_USED_IN_PROGRESS         = 0x53;
const uint8_t VAN_POPUP_MSG_LPG_FUEL_REFUSED                        = 0x54; // LPG combustion refused (Peugeot 1007)
const uint8_t VAN_POPUP_MSG_LPG_SYSTEM_FAULTY                       = 0x55; // LPG default (Peugeot 1007)
const uint8_t VAN_POPUP_MSG_LPG_IN_USE                              = 0x56;
const uint8_t VAN_POPUP_MSG_MIN_LEVEL_LPG                           = 0x57;
const uint8_t VAN_POPUP_MSG_ADIN_FAULT                              = 0x58; // Dynalto system fault (Peugeot 1007)
const uint8_t VAN_POPUP_MSG_USE_STOP_AND_START                      = 0x59;
const uint8_t VAN_POPUP_MSG_STOP_AND_START_AVAILABLE                = 0x5A;
const uint8_t VAN_POPUP_MSG_STOP_AND_START_SYSTEM_ACTIVE            = 0x5B;
const uint8_t VAN_POPUP_MSG_STOP_AND_START_SYSTEM_DEACTIVATED       = 0x5C;
const uint8_t VAN_POPUP_MSG_STOP_AND_START_DEFERRED                 = 0x5D;
const uint8_t VAN_POPUP_MSG_XSARA_DYNALTO                           = 0x5E;
const uint8_t VAN_POPUP_MSG_307_DYNALTO                             = 0x5F;

const uint8_t VAN_POPUP_MSG_PLACE_LEVER_IN_NEUTRAL                  = 0x67; // On RT3 monochrome: Change to neutral

const uint8_t VAN_POPUP_MSG_ROOF_OPERATION_COMPLETE                 = 0x70; // Roof folded in cabriolet position (Peugeot 1007)
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_SCREEN_NOT_IN_PLACE= 0x71; // Operation imposibble pull the screen (Peugeot 1007)
const uint8_t VAN_POPUP_MSG_ROOF_MECHANISM_NOT_LOCKED               = 0x72; // Folding roof locking fault(Peugeot 1007)
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_BOOT_OPEN          = 0x73; // Action imposibble boot open
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_SPEED_TOO_HIGH     = 0x74;
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_EXT_TEMP_TOO_LOW   = 0x75;
const uint8_t VAN_POPUP_MSG_ROOF_MECHANISM_FAULTY                   = 0x76; // On RT3 monochrome: Folding roof mechanism faulty
const uint8_t VAN_POPUP_MSG_BOOT_MECHANISM_NOT_LOCKED               = 0x77; // Folding roo compartment fault (Peugeot 1007)

const uint8_t VAN_POPUP_MSG_BOW_FAULT                               = 0x7C; // On RT3 monochrome: Roof frame fault
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_ROOF_NOT_UNLOCKED  = 0x7D;
const uint8_t VAN_POPUP_MSG_ROOF_OPERATION_INCOMPLETE               = 0x7F; // Folding roof in coupe position (Peugeot 1007)

const uint8_t VAN_POPUP_MSG_NONE                                    = 0xFF;

#pragma endregion

#endif
