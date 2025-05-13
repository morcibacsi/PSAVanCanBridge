#pragma once

#include <inttypes.h>

//Same order in AEE2004 0x220 do not change the order
union DoorStatusStruct {
    struct {
        uint8_t fuel_flap_open        : 1; // bit 0
        uint8_t rear_window_open      : 1; // bit 1
        uint8_t hood_open             : 1; // bit 2
        uint8_t trunk_open            : 1; // bit 3
        uint8_t rear_right_door_open  : 1; // bit 4
        uint8_t rear_left_door_open   : 1; // bit 5
        uint8_t front_right_door_open : 1; // bit 6
        uint8_t front_left_door_open  : 1; // bit 7
    } data;
    uint8_t asByte;
};
