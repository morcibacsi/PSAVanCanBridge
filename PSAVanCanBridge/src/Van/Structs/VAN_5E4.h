#pragma once

#ifndef _VAN_5E4_h
#define _VAN_5E4_h

#include <stdint.h>

// VANID: 5E4
const uint16_t VAN_ID_EMF_STATUS = 0x5E4;

// Read right to left in documentation
union VanEmfStatusByte0Struct {
    struct {
        uint8_t                        : 2; // bit 0-1
        uint8_t alert_reminder_request : 1; // bit 2
        uint8_t overspeed_memo_request : 1; // bit 3
        uint8_t overspeed_alert        : 1; // bit 4
        uint8_t keep_alive_van_comfort : 1; // bit 5
        uint8_t cumulative_trip_reset  : 1; // bit 6
        uint8_t reset_course_totals    : 1; // bit 7
    } data;
    uint8_t asByte;
};

//Read left to right in documentation
struct VanEmfStatusStructs {
    VanEmfStatusByte0Struct Status;
    uint8_t OverspeedValue;
};
#endif