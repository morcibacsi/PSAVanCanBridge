// CanDash1Structs.h
#pragma once

#ifndef _CanDash1Structs_h
    #define _CanDash1Structs_h

const uint16_t CAN_ID_DASH1_INTERVAL = 500;

// CANID: 0F6
const uint16_t CAN_ID_0F6_DASH = 0x0F6;

const uint8_t CAN_INDICATOR_LEFT  = 121; //y
const uint8_t CAN_INDICATOR_RIGHT = 120; //x

const uint8_t CAN_KEY_POSITION_STOP       = 0x00;
const uint8_t CAN_KEY_POSITION_CONTACT    = 0x01;
const uint8_t CAN_KEY_POSITION_STARTER    = 0x10;

// Read right to left in documentation
union CanDash1Byte1Struct {
    struct {
        uint8_t engine_status    : 2; // bit 0-1
        uint8_t generator_status : 1; // bit 1
        uint8_t ignition         : 1; // bit 2
        uint8_t starting         : 1; // bit 3
        uint8_t factory_mode     : 1; // bit 5
        uint8_t config_mode      : 2; // bit 7
    }data;
  uint8_t asByte;
};

union CanDash1Byte2Struct {
    struct {
        uint8_t bit0 : 1; // bit 0
        uint8_t bit1 : 1; // bit 1
        uint8_t bit2 : 1; // bit 2
        uint8_t bit3 : 1; // bit 3
        uint8_t bit4 : 1; // bit 4
        uint8_t bit5 : 1; // bit 5
        uint8_t bit6 : 1; // bit 6
        uint8_t economy_mode : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CanDash1Byte8Struct {
    struct {
        uint8_t turn_left_light    : 1; // bit 0
        uint8_t turn_right_light   : 1; // bit 1
        uint8_t                    : 1; // bit 2
        uint8_t                    : 1; // bit 3
        uint8_t                    : 1; // bit 4
        uint8_t                    : 1; // bit 5
        uint8_t wiper_status       : 1; // bit 6
        uint8_t reverse_gear_light : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can0F6Dash1Struct {
    CanDash1Byte1Struct IgnitionField;
    uint8_t CoolantTemperature;
    uint8_t MileageByte1;
    uint8_t MileageByte2;
    uint8_t MileageByte3;
    uint8_t Field6;
    uint8_t ExternalTemperature;
    CanDash1Byte8Struct LightsStatus;
};
#endif
