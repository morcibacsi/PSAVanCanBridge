#pragma once

#ifndef _Config_h
    #define _Config_h

#include <stdint.h>
#include <WString.h>

#include "Config_PSAVanCanBridge_WifiAp.h"
//#include "Config_PSAVanCanBridge_Bluetooth.h"
//#include "Config_WifiAp_Replay.h"
//#include "Config_WifiClient.h"

struct Config
{
    #ifdef PIO_INI_BOARD_tamc_termod_s3
        #define BOARD_TYPE_tamc_termod_s3
    #endif
    #ifdef PIO_INI_BOARD_ESP32
        #define BOARD_TYPE_ESP32
    #endif
    #ifdef PIO_INI_BOARD_ESP32_v15
        #define BOARD_TYPE_ESP32_v15
    #endif

    String  VERSION = "2.0.0";
    //#define PRINT_DEBUG

    #define VIN_LENGTH 17

    #ifdef WIFI_ENABLED
        //#define WEBSOCKET_SERIAL
    #endif

    #ifndef WEBSOCKET_SERIAL
        // if defined messages are sent on bluetooth connection, otherwise standard serial is used
        #ifndef WIFI_ENABLED
            #define USE_BLUETOOTH_SERIAL
        #endif
    #endif

    const bool REPLAY_MODE = REPLAY_MODE_VALUE;

    // if true then we can control the display from the built-in webpage
    bool DISPLAY_CONTROL = false;

    bool ORIGINAL_DISPLAY_PRESENT = false;

    // if true then data of RD2/RD3 is bridged for the new display
    bool BRIDGE_RADIO_DATA = false;

    bool BRIDGE_ODOMETER_DATA = false;

    // if true then remote stalk data is bridged for the head unit
    bool BRIDGE_REMOTE_STALK_DATA = false;

    // if true then the ignition signal is used from the VAN bus otherwise the ignition is considered as always on
    bool USE_IGNITION_SIGNAL_FROM_VAN_BUS = false;

    //0: none / PSA CAN
    //1: PSA VAN - the distance data from the parking aid is queried with a diagnostic message (only works with rev 1.4)
    //2: Chinese
    uint8_t PARKING_AID_TYPE = 0;

    bool READ_SERIAL_PORT_FOR_COMMANDS = false;

    bool ENABLE_PARKING_AID_SOUND_FROM_SPEAKER = 0;

    uint8_t FUEL_TANK_CAPACITY_IN_LITERS = 60;

    uint32_t MILEAGE_AT_CMB_TRIP_RESET = 0;

    bool ENABLE_MANUAL_GEARBOX_DISPLAY = true;

    //0: final, 1: 1st, 2: 2nd ...
    float GEAR_RATIOS[7] = { 4.53, 3.454, 1.869, 1.28, 0.95, 0.8, 0 };

    //the values are in centimeters and are coming from observing the values of a 5008 from 2012
    uint8_t AAS_REAR_CORNER_DISTANCES[4] = { 60, 50, 40, 30 };
    uint8_t AAS_REAR_DISTANCES[4] = { 145, 100, 50, 30 };

    uint8_t TIRE_SIZE = 15;
    uint8_t TIRE_WIDTH = 195;
    uint8_t TIRE_RATIO = 65;

    // 0: CAN2004
    // 1: CAN2010
    uint8_t CAN_TYPE = 0;

    // 0: Aftermarket
    // 1: RD4/RD43
    // 2: RD45
    // 3: RD45 (CAN2010)
    // 4: RD3
    // 5: RTx
    uint8_t RADIO_TYPE = 1;

    bool DISPLAY_CONTROL_WITH_COM = false;

    bool LEFT_STICK_BUTTON_SHOWS_OIL_TEMP = false;

    bool USE_BRIGHTNESS_FROM_CAN_ODOMETER = false;
    uint8_t VIN_FOR_HEADUNIT[VIN_LENGTH] = { 0 };

    // if true, then SEND 0x167 on CAN bus
    bool NO_EMF_C_INSTALLED = false;

    // 1: original layout
    // 2: my layout (displays fuel level, vehicle speed, rpm instead of the basic trip computer data)
    uint8_t DISPLAY_MODE = 1;

    // if true then AC changes are sent to the display
    bool SEND_AC_CHANGES_TO_DISPLAY = false;

    // if true then AC changes are sent to the display
    bool SEND_AC_FAN_CHANGES_TO_DISPLAY = false;

    bool ENABLE_REVERSE_CAMERA_ON_RTX = false;

    // if true the AC status is queried with a diagnostic message (only works with rev 1.4)
    bool QUERY_AC_STATUS = false;

    const bool WIFI_AP_MODE = WIFI_AP_MODE_VALUE;
    const char* WIFI_AP_NAME = WIFI_AP_NAME_VALUE;
    const char* WIFI_SSID = WIFI_SSID_VALUE;
    const char* WIFI_PASSWORD = WIFI_PASSWORD_VALUE;

    Config();
};

#endif
