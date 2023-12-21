// CAN_260_2010.h
#pragma once

#ifndef _CAN_260_2010_h
    #define _CAN_260_2010_h

#include <stdint.h>

const uint16_t CAN_260_2010_INTERVAL = 500;

/*
0b00000 = French
0b00001 = English
0b00010 = German
0b00011 = Spanish
0b00100 = Italian
0b00101 = Portuguese
0b00110 = Dutch
0b00111 = Greek
0b01000 = Brazilian Portuguese
0b01001 = Polish
0b01010 = Traditional Chinese
0b01011 = Simplified Chinese
0b01100 = Turkish
0b01101 = Japanese
0b01110 = Russian
0b01111 = Czech
0b10000 = Croat
0b10001 = Hungarian
0b10010 = Arab
0b10011 = Bulgarian
0b10100 = Korean
0b10101 = Danish
0b10110 = Estonian
0b10111 = Farsi
0b11000 = Finnish
0b11001 = Hebrew
0b11010 = Norwegian
0b11011 = Rumanian
0b11100 = Serbian
0b11101 = Swedish
0b11110 = Ukrainian
0b11111 = Vietnamese
*/
const uint8_t LANGUAGE_FRENCH               = 0b00000;
const uint8_t LANGUAGE_ENGLISH              = 0b00001;
const uint8_t LANGUAGE_GERMAN               = 0b00010;
const uint8_t LANGUAGE_SPANISH              = 0b00011;
const uint8_t LANGUAGE_ITALIAN              = 0b00100;
const uint8_t LANGUAGE_PORTUGUESE           = 0b00101;
const uint8_t LANGUAGE_DUTCH                = 0b00110;
const uint8_t LANGUAGE_GREEK                = 0b00111;
const uint8_t LANGUAGE_BRAZILIAN_PORTUGUESE = 0b01000;
const uint8_t LANGUAGE_POLISH               = 0b01001;
const uint8_t LANGUAGE_TRADITIONAL_CHINESE  = 0b01010;
const uint8_t LANGUAGE_SIMPLIFIED_CHINESE   = 0b01011;
const uint8_t LANGUAGE_TURKISH              = 0b01100;
const uint8_t LANGUAGE_JAPANESE             = 0b01101;
const uint8_t LANGUAGE_RUSSIAN              = 0b01110;
const uint8_t LANGUAGE_CHECZ                = 0b01111;
const uint8_t LANGUAGE_CROATIAN             = 0b10000;
const uint8_t LANGUAGE_HUNGARIAN            = 0b10001;
const uint8_t LANGUAGE_ARABIC               = 0b10010;
const uint8_t LANGUAGE_BULGARIAN            = 0b10011;
const uint8_t LANGUAGE_KOREAN               = 0b10100;
const uint8_t LANGUAGE_DANISH               = 0b10101;
const uint8_t LANGUAGE_ESTONIAN             = 0b10110;
const uint8_t LANGUAGE_FARSI                = 0b10111;
const uint8_t LANGUAGE_FINNISH              = 0b11000;
const uint8_t LANGUAGE_HEBREW               = 0b11001;
const uint8_t LANGUAGE_NORWEGIAN            = 0b11010;
const uint8_t LANGUAGE_ROMANIAN             = 0b11011;
const uint8_t LANGUAGE_SERBIAN              = 0b11100;
const uint8_t LANGUAGE_SWEDISH              = 0b11101;
const uint8_t LANGUAGE_UKRAINIAN            = 0b11110;
const uint8_t LANGUAGE_VIETNAMESE           = 0b11111;

const uint8_t AMBIENCE_LEVEL_0           = 0b000;
const uint8_t AMBIENCE_LEVEL_1           = 0b001;
const uint8_t AMBIENCE_LEVEL_2           = 0b010;
const uint8_t AMBIENCE_LEVEL_3           = 0b011;
const uint8_t AMBIENCE_LEVEL_4           = 0b100;
const uint8_t AMBIENCE_LEVEL_5           = 0b101;
const uint8_t AMBIENCE_LEVEL_6           = 0b110;
const uint8_t AMBIENCE_LEVEL_INVALID     = 0b111;

const uint8_t SOUND_HARMONY_1     = 0b00;
const uint8_t SOUND_HARMONY_2     = 0b01;
const uint8_t SOUND_HARMONY_3     = 0b10;
const uint8_t SOUND_HARMONY_4     = 0b11;

// CANID: 260
const uint16_t CAN_ID_260_2010 = 0x260;

// Read right to left in documentation
union CAN_260_2010_Byte1Struct{
    struct {
        uint8_t consumption_unit             : 1; // bit 0
        uint8_t distance_unit                : 1; // bit 1
        uint8_t language                     : 5; // bit 2-6
        uint8_t unit_and_language_data_valid : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_260_2010_Byte2Struct{
    struct {
        uint8_t sound_harmony         : 2; // bit 0-1
        uint8_t vehicle_function_data : 1; // bit 2
        uint8_t ambience_level        : 3; // bit 3-5
        uint8_t temperature_unit      : 1; // bit 6
        uint8_t volume_unit           : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_260_2010_Byte3Struct{
    struct {
        uint8_t ambience_lighting        : 1; // bit 0
        uint8_t drl                      : 1; // bit 1
        uint8_t adaptive_lighting        : 1; // bit 2
        uint8_t welcome                  : 1; // bit 3
        uint8_t selective_openings_rear  : 1; // bit 4
        uint8_t selective_openings       : 1; // bit 5
        uint8_t selective_openings_key   : 1; // bit 6
        uint8_t automatic_electric_brake : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_260_2010_Byte4Struct{
    struct {
        uint8_t automatic_lights         : 1; // bit 1
        uint8_t unlock_lighting_duration : 2; // bit 2
        uint8_t unlock_lighting          : 1; // bit 3
        uint8_t highway_lighting         : 1; // bit 4
        uint8_t follow_me_home_duration  : 2; // bit 5-6
        uint8_t follow_me_home           : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_260_2010_Byte5Struct{
    struct {
        uint8_t configurable_button           : 4; // bit 0-3
        uint8_t rear_flap_motorization        : 1; // bit 4
        uint8_t auto_wiper_in_reverse         : 1; // bit 5
        uint8_t blindspot_monitoring_feedback : 1; // bit 6
        uint8_t aas_user_return_type          : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_260_2010_Byte6Struct{
    struct {
        uint8_t user_profile              : 2; // bit 0-1
        uint8_t indexing_of_rear_driving  : 1; // bit 2
        uint8_t dsg_initialization_status : 1; // bit 3
        uint8_t braking_on_alarm_risk     : 1; // bit 4
        uint8_t arc_sensitivity           : 2; // bit 5-6
        uint8_t arc_function              : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_260_2010_Byte7Struct{
    struct {
        uint8_t access_arm_status         : 1; // bit 0
        uint8_t ilv_status                : 1; // bit 1
        uint8_t grille_lighting_status    : 1; // bit 2
        uint8_t cafr_status               : 1; // bit 3
        uint8_t driver_alert_assist_state : 1; // bit 4
        uint8_t access_arm_lock           : 1; // bit 5
        uint8_t etsr_status               : 1; // bit 6
        uint8_t see_status                : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_260_2010_Byte8Struct{
    struct {
        uint8_t amla_status : 1; // bit 0
        uint8_t afs_status  : 1; // bit 1
        uint8_t ars_status  : 1; // bit 2
        uint8_t irv_status  : 1; // bit 3
        uint8_t dae_status2 : 2; // bit 4-5
        uint8_t dae_status1 : 2; // bit 6-7
    } data;
    uint8_t asByte;
};


// Read left to right in documentation
struct CAN_260_2010_Struct {
    uint8_t SpeedByte1;
    uint8_t SpeedByte2;
    CAN_260_2010_Byte3Struct Status1;
    CAN_260_2010_Byte4Struct Status2;
    CAN_260_2010_Byte5Struct Status3;
    uint8_t Unknown6;
    uint8_t Unknown7;
    uint8_t Unknown8;
};
#endif
