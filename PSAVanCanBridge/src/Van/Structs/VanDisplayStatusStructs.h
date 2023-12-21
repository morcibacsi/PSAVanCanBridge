// VanDisplayStatusStructs.h
#pragma once

#ifndef _VanDisplayStatusStructs_h
    #define _VanDisplayStatusStructs_h

#include <stdint.h>

// VANID: 5E4
const uint16_t VAN_ID_DISPLAY_STATUS = 0x5E4;
const uint8_t VAN_ID_EMF_BSI_REQUEST_LENGTH = 2;

union VanEmfBsiRequestByte1 {
    struct {
        uint8_t unused                             : 2; // bit 0-1
        uint8_t alert_reminder_request             : 1; // bit 2
        uint8_t overspeed_memorization_request     : 1; // bit 3
        uint8_t overspeed_alert                    : 1; // bit 4
        uint8_t request_for_keep_van_comfort_alive : 1; // bit 5
        uint8_t request_to_reset_cumulative        : 1; // bit 6
        uint8_t request_to_reset_course_totals     : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct VanDisplayStatusStruct {
    VanEmfBsiRequestByte1 Requests;
    uint8_t OverSpeedAlertValue;
};

union VanDisplayStatusPacket {
    VanDisplayStatusStruct data;
    uint8_t VanDisplayStatusPacket[sizeof(VanDisplayStatusStruct)];
};

#endif
