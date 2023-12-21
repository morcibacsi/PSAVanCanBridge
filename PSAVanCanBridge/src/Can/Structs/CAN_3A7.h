#pragma once

#ifndef _Can3A7Struct_h
    #define _Can3A7Struct_h

const uint16_t CAN_ID_3A7_INTERVAL = 500;

// CANID: 0x3A7
const uint16_t CAN_ID_3A7 = 0x3A7;

// Read right to left in documentation
union Can3A7Byte2Struct {
    struct {
        uint8_t unused04         : 5; // bit 0-4
        uint8_t display_blinking : 1; // bit 5
        uint8_t unused6          : 1; // bit 6
        uint8_t maintenance_due  : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can3A7Struct {
    uint8_t MaintenanceType;
    Can3A7Byte2Struct KmMaintenanceInfo;
    uint8_t TemporaryMaintenanceInfo;
    uint8_t MaintenanceKmByte1;
    uint8_t MaintenanceKmByte2;
    uint8_t NumberOfDaysBeforeMaintenanceByte1;
    uint8_t NumberOfDaysBeforeMaintenanceByte2;
    uint8_t DisplayDuration;
};
#endif
