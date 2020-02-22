#pragma once

#ifndef _Config_h
    #define _Config_h

#define USE_BLUETOOTH_SERIAL
#define USE_IGNITION_SIGNAL_FROM_VAN_BUS

#define TASK_WATCHDOG_TIMEOUT 7
#define FUEL_TANK_CAPACITY_IN_LITERS 60

// 11: HW revision v1.1
// 14: HW revision v1.4
#define HW_VERSION 14

// 1: original layout
// 2: my layout
#define DISPLAY_MODE 2

//#define USE_VIN_AUTO_MATCHING
#ifdef USE_VIN_AUTO_MATCHING
    uint8_t Vin[17] = { 0 };
#else
    // Fake V2C sends this VIN:
    uint8_t Vin[17] = { 'L', 'D', 'C', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8' };
    // Original V2C sends this VIN:
    //uint8_t Vin[17] = { 'L', 'D', 'C', '9', '4', '3', 'X', '4', '7', 'D', '1', '5', '6', '6', '7', '7', '2' };
#endif

//#define USE_NEW_AIRCON_DISPLAY_SENDER

#endif
