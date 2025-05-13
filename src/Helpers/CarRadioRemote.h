#pragma once

#include <inttypes.h>

//These are in the same order as in the AEE2004/2010 so don't change the order
union CarRadioRemoteStruct {
    struct {
        uint8_t list                   : 1; // bit 0
        uint8_t mode_phone             : 1; // bit 1
        uint8_t volume_minus           : 1; // bit 2
        uint8_t volume_plus            : 1; // bit 3
        uint8_t owerflow_scan_negative : 1; // bit 4
        uint8_t owerflow_scan_positive : 1; // bit 5
        uint8_t seek_down              : 1; // bit 6
        uint8_t seek_up                : 1; // bit 7

        uint8_t scroll_position        : 8; // byte 0-7

        uint8_t reserved               : 2; // bit 0-1
        uint8_t unused2                : 1; // bit 2
        uint8_t list_minus             : 1; // bit 3
        uint8_t list_plus              : 1; // bit 4
        uint8_t unused5                : 1; // bit 5
        uint8_t source                 : 1; // bit 6
        uint8_t command_valid          : 1; // bit 7
    } data;
    uint64_t asUint64;
};
