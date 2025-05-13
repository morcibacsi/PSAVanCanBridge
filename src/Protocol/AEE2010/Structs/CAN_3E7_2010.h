#pragma once

#ifndef _CAN_3E7_2010_h
    #define _CAN_3E7_2010_h

#include <stdint.h>

const uint16_t CAN_ID_3E7_INTERVAL = 500;

// CANID: 0x3E7
const uint16_t CAN_ID_3E7 = 0x3E7;

// Read right to left in documentation
union CAN_3E7_2010Byte1Struct {
    struct {
        uint8_t                       : 2; // bit 0-1
        uint8_t maintenance_sign_time : 1; // bit 2
        uint8_t maintenance_sign_km   : 1; // bit 3
        uint8_t km_blinking           : 1; // bit 4
        uint8_t wrench_icon           : 1; // bit 5
        uint8_t maintenance_type_time : 1; // bit 6
        uint8_t maintenance_type_km   : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CAN_3E7_2010Struct {
    CAN_3E7_2010Byte1Struct MaintenanceType;
    uint8_t NumberOfDaysBeforeMaintenanceByte1;
    uint8_t NumberOfDaysBeforeMaintenanceByte2;
    uint8_t MaintenanceKmByte1;
    uint8_t MaintenanceKmByte2;
};
#endif
