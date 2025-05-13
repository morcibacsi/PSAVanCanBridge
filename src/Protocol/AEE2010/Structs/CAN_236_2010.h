#pragma once

#ifndef _CAN_236_2010_h
    #define _CAN_236_2010_h

#include <stdint.h>

// CANID: 236
const uint16_t CAN_ID_236_2010 = 0x236;

const int CAN_2010_ELECTRIC_NETWORK_STATUS_NORMAL      = 0b0000;
const int CAN_2010_ELECTRIC_NETWORK_STATUS_ECO         = 0b0001;
const int CAN_2010_ELECTRIC_NETWORK_STATUS_START_AVAIL = 0b0010;
const int CAN_2010_ELECTRIC_NETWORK_STATUS_STARTING    = 0b0011;
const int CAN_2010_ELECTRIC_NETWORK_STATUS_RESTARTING  = 0b0100;
const int CAN_2010_ELECTRIC_NETWORK_STATUS_NORMAL2     = 0b0101;
const int CAN_2010_ELECTRIC_NETWORK_STATUS_ECO2        = 0b0110;
const int CAN_2010_ELECTRIC_NETWORK_STATUS_SECU        = 0b0111;

const int CAN_2010_VEHICLE_CONFIG_FITTING   = 0b0000;
const int CAN_2010_VEHICLE_CONFIG_PLANT     = 0b0001;
const int CAN_2010_VEHICLE_CONFIG_CHECK     = 0b0010;
const int CAN_2010_VEHICLE_CONFIG_TRANSPORT = 0b0011;
const int CAN_2010_VEHICLE_CONFIG_CLIENT    = 0b0100;
const int CAN_2010_VEHICLE_CONFIG_SHOWROOM  = 0b0101;
const int CAN_2010_VEHICLE_CONFIG_APV       = 0b0110;



// Read right to left in documentation
union CAN_236_2010_Byte1Struct{
    struct {
        uint8_t vehicle_config          : 4; // bit 0-3
        uint8_t electric_network_status : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_236_2010_Byte6Struct{
    struct {
        uint8_t jdd_context        : 5; // bit 0-4
        uint8_t brake_pedal_status : 1; // bit 5
        uint8_t driver_door_status : 1; // bit 6
        uint8_t trunk_status       : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_236_2010_Byte8Struct {
    struct {
        uint8_t rear_window_status           : 1; // bit 0
        uint8_t passenger_door_status        : 1; // bit 1
        uint8_t stt_status                   : 1; // bit 2
        uint8_t driver_door_status_estimated : 1; // bit 3
        uint8_t rear_left_door_status        : 1; // bit 4
        uint8_t rear_right_door_status       : 1; // bit 5
        uint8_t mux_panel_present            : 1; // bit 6
        uint8_t start_disabled               : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CAN_236_2010_Struct {
    CAN_236_2010_Byte1Struct VehicleStatus1;
    uint8_t CounterByte1;
    uint8_t CounterByte2;
    uint8_t CounterByte3;
    uint8_t CounterByte4;
    CAN_236_2010_Byte6Struct VehicleStatus2;
    uint8_t ResetCounter;
    CAN_236_2010_Byte8Struct DoorStatus;
};

union CAN_236_2010_Packet {
    CAN_236_2010_Struct data;
    uint8_t CAN_236_2010_Packet[sizeof(CAN_236_2010_Struct)];
};
#pragma endregion

#endif
