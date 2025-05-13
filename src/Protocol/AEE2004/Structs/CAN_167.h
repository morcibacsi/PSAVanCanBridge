#pragma once

#ifndef _2004_167_h
    #define _2004_167_h

#include <stdint.h>

// CANID: 167
const uint16_t CAN2004_ID_167 = 0x167;
const uint8_t CAN2004_ID_167_LEN = 8;

const uint8_t ODOMETER_TRIP_INFO_NONE = 0;
const uint8_t ODOMETER_TRIP_INFO_GENERAL = 1;
const uint8_t ODOMETER_TRIP_INFO_TRIP1 = 2;
const uint8_t ODOMETER_TRIP_INFO_TRIP2 = 4;
const uint8_t ODOMETER_TRIP_INFO_NOT_MANAGED_BY_EMF = 7;

union Can2004_167Byte1 {
    struct {
        uint8_t trip_data_on_odometer      : 3; // bit 0-2
        uint8_t reset_maintenance          : 1; // bit 3
        uint8_t emergency_call_in_progress : 1; // bit 4
        uint8_t alert_reminder_request     : 1; // bit 5
        uint8_t reset_trip2                : 1; // bit 6
        uint8_t reset_trip1                : 1; // bit 7
    } data;
    uint8_t asByte;
};

struct CAN2004_167Struct {
    Can2004_167Byte1 EMFRequest;
    uint8_t GeneralRequest;
    uint16_t TotalDistanceTraveled;
    uint8_t InteractiveMessage;
    uint8_t InteractiveMessageAction;
    uint8_t Unused;
    uint8_t ValueEnteredByUser;
};

union CAN2004_167Packet {
    CAN2004_167Struct data;
    uint8_t CAN2004_167Packet[sizeof(CAN2004_167Struct)];
};
#endif
