// VanBsiEventsStructs.h
#pragma once

#ifndef _VanBsiEventsStructs_h
    #define _VanBsiEventsStructs_h

// VANID: 8C4
const uint16_t VAN_ID_BSI_EVENTS       = 0x8C4;
const uint8_t VAN_ID_BSI_EVENTS_LENGTH = 2;

const uint8_t VAN_BSI_EVENT_SOURCE_CLIM           = 0x0E;
const uint8_t VAN_BSI_EVENT_SOURCE_BSI            = 0x12;
const uint8_t VAN_BSI_EVENT_SOURCE_RADIO          = 0x0A;
const uint8_t VAN_BSI_EVENT_SOURCE_NAVIGATION     = 0x07;
const uint8_t VAN_BSI_EVENT_SOURCE_PHONE          = 0x1A;
const uint8_t VAN_BSI_EVENT_SOURCE_CDC            = 0x16;
const uint8_t VAN_BSI_EVENT_SOURCE_PARKING_ASSIST = 0x14;

typedef struct {
    uint8_t event_source    : 5; // bit 0-4
    uint8_t unused          : 1; // bit 5
    uint8_t secondary_event : 1; // bit 6
    uint8_t primary_event   : 1; // bit 7
} VanBsiEventByte1;

typedef struct {
    uint8_t unused02              : 3; // bit 0-2
    uint8_t trip_button_pressed   : 1; // bit 3
    uint8_t unused4               : 1; // bit 4
    uint8_t door_status_changed   : 1; // bit 5
    uint8_t signal_changed        : 1; // bit 6
    uint8_t display_alert_changed : 1; // bit 7
} VanBsiEventByte2;

// Read left to right in documentation
typedef struct VanBsiEventsStruct {
    VanBsiEventByte1 Ident;
    VanBsiEventByte2 Cause;
};

typedef union VanBsiEventsPacket {
    VanBsiEventsStruct data;
    uint8_t VanBsiEventsPacket[sizeof(VanBsiEventsStruct)];
};

#endif
