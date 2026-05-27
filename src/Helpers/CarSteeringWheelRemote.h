#pragma once

#include <cstdint>

//These are in the same order as in the AEE2004/2010 so don't change the order
union CarSteeringWheelRemoteStruct {
    struct {
        uint8_t NavMenuScroll       : 8;

        uint8_t voice_command       : 1;
        uint8_t left_push           : 1;
        uint8_t mode_button         : 1;
        uint8_t menu_button         : 1;
        uint8_t escape_button       : 1;
        uint8_t validation_button   : 1;
        uint8_t scroll_underflow    : 1;
        uint8_t scroll_overflow     : 1;

        uint8_t telephone_command   : 1;

        uint8_t volume_knob_underflow : 1;
        uint8_t volume_knob_overflow  : 1;
        uint8_t                       : 1;
        uint8_t ac_recycling_command  : 1;
        uint8_t cmb_escape_button     : 1;
        uint8_t cmb_validation        : 1;
        uint8_t cmb_underflow         : 1;
        uint8_t cmb_overflow          : 1;

        uint8_t CmbNavMenuCounter     : 8;
        uint8_t VolumeCounter         : 8;
    } data;
    uint64_t asUint64;
};
