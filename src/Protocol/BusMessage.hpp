#pragma once

#include <cstdint>
#include <vector>
#include "ProtocolType.hpp"

typedef enum {
    Normal = 1,
    Query = 2
} MessageType;

struct BusMessage {
    uint32_t id;              // Message identifier.
    uint8_t command;
    uint8_t data[32]; //Message payload.
    uint8_t dataLength;    // Actual length of data.
    uint16_t crc; //Message CRC.
    bool ack; // Message ACK.
    ProtocolType protocol;
    MessageType type;
    uint32_t periodicityMs;
    uint16_t offsetMs;
    bool isActive;
    uint8_t retryCount = 0;
    uint8_t priority = 0;
};