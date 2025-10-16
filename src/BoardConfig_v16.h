#pragma once

#define BOARD_SPI_INSTANCE SPI2_HOST
#define BOARD_SCK_PIN 25
#define BOARD_MISO_PIN 5
#define BOARD_MOSI_PIN 13
#define BOARD_CS_PIN 19

#define BOARD_CAN1_RX_PIN 0
#define BOARD_CAN1_TX_PIN 1

#define BOARD_CAN2_RX_PIN 3
#define BOARD_CAN2_TX_PIN 2

#define BOARD_SDA_PIN 18
#define BOARD_SCL_PIN 19

#define BOARD_LED_PIN 8

#define BOARD_SECOND_CAN_TYPE 1 // 1 - built-in, 2 - MCP2515
#define BOARD_PROTOCOL_TYPES 3 // 1 - VAN-CAN, 2 - CAN-CAN, 3 - VAN-CAN + CAN-CAN
#define BOARD_SUPPORTED_SOURCE_PROTOCOLS      { static_cast<uint8_t>(ProtocolType::AEE2001), static_cast<uint8_t>(ProtocolType::AEE2004) }
#define BOARD_SUPPORTED_DESTINATION_PROTOCOLS { static_cast<uint8_t>(ProtocolType::AEE2004), static_cast<uint8_t>(ProtocolType::AEE2010) }
