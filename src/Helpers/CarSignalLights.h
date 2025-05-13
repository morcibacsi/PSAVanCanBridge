#pragma once

#include <inttypes.h>

//These are in the same order as in the AEE2004/2010 so don't change the order
union CarSignalLightsStruct {
    struct {
        //1
        uint8_t drl                     : 1; // bit 0
        uint8_t left_turn_indicator     : 1; // bit 1
        uint8_t right_turn_indicator    : 1; // bit 2
        uint8_t rear_foglight           : 1; // bit 3
        uint8_t front_foglight          : 1; // bit 4
        uint8_t high_beam_on            : 1; // bit 5
        uint8_t low_beam_on             : 1; // bit 6
        uint8_t parking_light_indicator : 1; // bit 7
        //4
        uint8_t fse_inhibited                 : 1; // bit 0
        uint8_t handbrake_signal              : 1; // bit 1
        uint8_t foot_on_break_indicator       : 2; // bit 2-3
        uint8_t passenger_airbag_activated    : 1; // bit 4
        uint8_t child_security_active         : 1; // bit 5
        uint8_t stop_light                    : 1; // bit 6
        uint8_t service_indicator_exclamation : 1; // bit 7
        //5
        uint8_t suspension_status                  : 1; // bit 0
        uint8_t esp_in_progress                    : 1; // bit 1
        uint8_t esp_inactivated                    : 1; // bit 2
        uint8_t cafr_indicator                     : 1; // bit 3
        uint8_t operation_indicator_light_blinking : 1; // bit 4
        uint8_t operation_indicator_light_on       : 1; // bit 5
        uint8_t door_open                          : 1; // bit 6
        uint8_t diesel_pre_heating                 : 1; // bit 7
        //6
        uint8_t row1_rl_seatbelt_forgotten          : 1; // bit 0
        uint8_t scr_indicator                       : 2; // bit 1-2
        uint8_t passenger_seatbelt_warning_blinking : 1; // bit 3
        uint8_t passenger_seatbelt_warning          : 1; // bit 4
        uint8_t driver_seatbelt_warning_blinking    : 1; // bit 5
        uint8_t driver_seatbelt_warning             : 1; // bit 6
        uint8_t fuel_level_low                      : 1; // bit 7
        //7
        uint8_t activate_front_passenger_protection : 1; // bit 0
        uint8_t warning_light_active                : 1; // bit 1
        uint8_t cmb_active                          : 1; // bit 2
        uint8_t row1_rr_seatbelt_forgotten_blinking : 1; // bit 3
        uint8_t row1_rr_seatbelt_forgotten          : 1; // bit 4
        uint8_t row1_rc_seatbelt_forgotten_blinking : 1; // bit 5
        uint8_t row1_rc_seatbelt_forgotten          : 1; // bit 6
        uint8_t row1_rl_seatbelt_forgotten_blinking : 1; // bit 7
        //8
        uint8_t change_personalization : 1; // bit 0
        uint8_t change_color           : 1; // bit 1
        uint8_t ready_lamp             : 1; // bit 2
        uint8_t minc_blinking          : 1; // bit 3
        uint8_t foot_on_clutch         : 2; // bit 5-4
        uint8_t engine_indicator       : 2; // bit 7-6
    } data;
    uint64_t asUint64;
};
