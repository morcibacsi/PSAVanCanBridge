#pragma once
#include <array>
#include <stdint.h>

constexpr uint8_t VAN_MAX_DATA_LENGTH = 28;

enum class VanFrameType : uint8_t
{
    Broadcast,      // Normal frame, no ACK requested
    Write,          // Normal frame, ACK requested
    ReplyRequest,   // Reply requested, no immediate responder/data
    ReplyResponse   // Reply requested and requested module replied in-frame
};

enum class VanAckState : uint8_t
{
    NotRequested,
    Acknowledged,
    NotAcknowledged
};

struct VanCom
{
    uint8_t raw = 0;

    bool ext = false;
    bool rak = false;
    bool readWrite = false;
    bool rtr = false;
};

struct VanFrame
{
    // Raw/logical frame fields
    uint8_t sof = 0;
    uint16_t identifier = 0;

    VanCom com;

    std::array<uint8_t, VAN_MAX_DATA_LENGTH> data{};
    uint8_t dataLength = 0;

    // FCS is 15 bits, so uint16_t is sufficient.
    uint16_t fcs = 0;

    // Raw two-bit ACK field as observed on the bus.
    uint8_t ack = 0;

    // Semantic interpretation.
    VanFrameType type = VanFrameType::Broadcast;
    VanAckState ackState = VanAckState::NotRequested;
};
