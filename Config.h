#pragma once

#ifndef _Config_h
    #define _Config_h

// if defined messages are sent on bluetooth connection, otherwise standard serial is used
#define USE_BLUETOOTH_SERIAL

// if true then the ignition signal is used from the VAN bus otherwise the ignition is considered as always on
constexpr bool USE_IGNITION_SIGNAL_FROM_VAN_BUS = true;

// if true the AC status is queried with a diagnostic message (only works with rev 1.4)
constexpr bool QUERY_AC_STATUS = false;

//if true the distance data from the parking aid is queried with a diagnostic message (only works with rev 1.4)
constexpr bool QUERY_PARKING_AID_DISTANCE = false;

//if true the VAN messages with CRC ERROR are logged
constexpr bool LOG_MSG_WITH_CRC_ERROR = true;

constexpr uint8_t ENABLE_PARKING_AID_SOUND_FROM_SPEAKER = 0;

constexpr uint8_t TASK_WATCHDOG_TIMEOUT = 7;
constexpr uint8_t FUEL_TANK_CAPACITY_IN_LITERS = 60;

// 11: HW revision v1.1
// 14: HW revision v1.4
#define HW_VERSION 14

// 1: original layout
// 2: my layout (displays fuel level, vehicle speed, rpm instead of the basic trip computer data)
constexpr uint8_t DISPLAY_MODE = 3;

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
