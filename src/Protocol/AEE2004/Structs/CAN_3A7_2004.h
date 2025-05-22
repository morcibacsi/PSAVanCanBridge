#pragma once

#ifndef _Can3A7Struct_h
    #define _Can3A7Struct_h

#include <stdint.h>

const uint16_t CAN_ID_3A7_INTERVAL = 500;

// CANID: 0x3A7
const uint16_t CAN_ID_3A7 = 0x3A7;

// Read right to left in documentation
union Can3A7Byte1Struct {
    struct {
        uint8_t                   : 2; // bit 0-1
        uint8_t wrench_without_km : 2; // bit 2-3
        uint8_t wrench_with_km    : 2; // bit 4-5
        uint8_t                   : 1; // bit 6
        uint8_t maintenance_due   : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can3A7Byte2Struct {
    struct {
        uint8_t                     : 5; // bit 0-4
        uint8_t km_blinking         : 1; // bit 5
        uint8_t                     : 1; // bit 6
        uint8_t maintenance_sign_km : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can3A7Byte3Struct {
    struct {
        uint8_t                        : 5; // bit 0-4
        uint8_t time_blinking          : 1; // bit 5
        uint8_t                        : 1; // bit 6
        uint8_t maintenance_sign_time  : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can3A7Struct {
    Can3A7Byte1Struct MaintenanceType;
    Can3A7Byte2Struct KmMaintenanceInfo;
    Can3A7Byte3Struct TemporaryMaintenanceInfo;
    uint8_t MaintenanceKmByte1;
    uint8_t MaintenanceKmByte2;
    uint8_t NumberOfDaysBeforeMaintenanceByte1;
    uint8_t NumberOfDaysBeforeMaintenanceByte2;
    uint8_t DisplayDuration;
};
#endif
