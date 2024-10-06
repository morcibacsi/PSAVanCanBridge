#pragma once

#ifndef _BoardConfig_ESP32_v15_h
    #define _BoardConfig_ESP32_v15_h

#define BOARD_SPI_INSTANCE HSPI
#define BOARD_SCK_PIN 25
#define BOARD_MISO_PIN 5
#define BOARD_MOSI_PIN 13
#define BOARD_CS_PIN 19

#define BOARD_CAN_RX_PIN 18
#define BOARD_CAN_TX_PIN 15

#define BOARD_VAN_DATA_RX_PIN 33
#define BOARD_VAN_DATA_TX_PIN 32

#define BOARD_SDA_PIN 16
#define BOARD_SCL_PIN 17

#define BOARD_LED_PIN 2
#define BOARD_ULP_VAN_TX_ENABLED true

#endif

