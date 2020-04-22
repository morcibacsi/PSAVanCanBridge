#pragma once

#ifndef _Config_h
    #define _Config_h

// if defined messages are sent on bluetooth connection, otherwise standard serial is used
#define USE_BLUETOOTH_SERIAL

// if defined then the ignition signal is used from the VAN bus otherwise the ignition is considered as always on
#define USE_IGNITION_SIGNAL_FROM_VAN_BUS

// if defined the AC status is queried with a diagnostic message (only works with rev 1.4)
#define QUERY_AC_STATUS

//if defined the distance data from the parking aid is queried with a diagnostic message (only works with rev 1.4)
//#define QUERY_PARKING_AID_DISTANCE

#define ENABLE_PARKING_AID_SOUND_FROM_SPEAKER 0

#define TASK_WATCHDOG_TIMEOUT 7
#define FUEL_TANK_CAPACITY_IN_LITERS 60

// 11: HW revision v1.1
// 14: HW revision v1.4
#define HW_VERSION 14

// 1: original layout
// 2: my layout (displays fuel level, vehicle speed, rpm instead of the basic trip computer data)
#define DISPLAY_MODE 2

// 1: original popup handler (displays all messages)
// 2: Karol's popup handler  (applies some filtering, better user experience)
#define POPUP_HANDLER 2

//#define USE_VIN_FROM_BSI
#ifdef USE_VIN_FROM_BSI
    uint8_t Vin[17] = { 0 };
#else
    // Fake V2C sends this VIN:
    uint8_t Vin[17] = { 'L', 'D', 'C', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8', '8' };
    // Original V2C sends this VIN:
    //uint8_t Vin[17] = { 'L', 'D', 'C', '9', '4', '3', 'X', '4', '7', 'D', '1', '5', '6', '6', '7', '7', '2' };
#endif

//#define USE_NEW_AIRCON_DISPLAY_SENDER

#endif
