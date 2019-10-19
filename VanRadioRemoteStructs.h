// VanRadioRemoteStructs.h
#pragma once

#ifndef _VanRadioRemoteStructs_h
    #define _VanRadioRemoteStructs_h

// VANID: 9C4
const uint16_t VAN_ID_RADIO_REMOTE = 0x9C4;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0            : 1; // bit 0
    uint8_t source_pressed      : 1; // bit 1
    uint8_t volume_down_pressed : 1; // bit 2
    uint8_t volume_up_pressed   : 1; // bit 3
    uint8_t unknown4            : 1; // bit 4
    uint8_t unknown5            : 1; // bit 5
    uint8_t seek_down_pressed   : 1; // bit 6
    uint8_t seek_up_pressed     : 1; // bit 7
} VanRadioRemoteButtonStruct;

// Read left to right in documentation
typedef struct VanRadioRemoteStructs {
    VanRadioRemoteButtonStruct RemoteButton;
    uint8_t ScrollPosition;
};

typedef union VanRadioRemotePacket {
    VanRadioRemoteStructs data;
    uint8_t VanRadioRemotePacket[sizeof(VanRadioRemoteStructs)];
};

#endif
