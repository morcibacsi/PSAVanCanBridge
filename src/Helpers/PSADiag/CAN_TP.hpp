#ifndef CAN_TP_H
#define CAN_TP_H

#include "../../Protocol/ITransportLayer.hpp"
#include "../IntUnions.h"

const uint8_t SINGLE_FRAME = 0x0;
const uint8_t FIRST_FRAME  = 0x1;
const uint8_t CONSECUTIVE_FRAME = 0x2;
const uint8_t FLOW_CONTROL_FRAME = 0x3;

union PCIDataSingleFrame {
    struct {
        uint8_t length_or_sequence : 4;
        uint8_t frameType          : 4;
    } data;
    uint8_t byte;
};

union PCIDataFirstFrame {
    struct
    {
        uint8_t lengthHigh : 4;  // upper 4 bits of length
        uint8_t frameType  : 4;  // PCI type
        uint8_t lengthLow;       // lower 8 bits of length
    } data;

    uint8_t raw[2];

    void setLength(uint16_t length)
    {
        raw[0] = (0x1 << 4) | ((length >> 8) & 0xF);
        raw[1] = length & 0xFF;
    }

    uint16_t getLength() const
    {
        return ((uint16_t)(raw[0] & 0xF) << 8) | raw[1];
    }
};

union PCIDataFlowControlFrame
{
    struct
    {
        uint8_t flowStatus : 4;
        uint8_t frameType  : 4;
        uint8_t blockSize;
        uint8_t separationTime;
    } data;

    uint8_t raw[3];
};

class CAN_TP {
    private:
    enum State {
        Idle,
        StartTransmission,
        WaitForFlowControl,
        SendConsecutiveFrames,
        WaitForFirstFrame,
        WaitForConsecutiveFrames,
        EndTransmission,
        EndReception
    };

    ITransportLayer *_canInterface;
    uint16_t _txId, _rxId;

    State _txState;
    State _rxState;
    uint8_t _consecutiveFrameCounter;
    unsigned long _lastMillis;

    uint16_t _txMessageLength;
    uint8_t txBuffer[4096];

    uint16_t rxMessageLength;
    uint8_t rxBuffer[4096];

    void sendSingleFrame(uint8_t* data, uint8_t size);
    void sendFirstFrame();
    void sendConsecutiveFrame();
    void sendFlowControl();

    public:
    enum ProcessResult {
        InProgress,
        TxSuccess,
        TxFlowControlTimeout,
        RxSuccess,
        RxFirstFrameTimeout,
        RxConsecutiveFrameTimeout,
        RxSequenceNumberMismatch,
        RxCanIdMismatch,
        RxLengthMismatch,
        RxInvalidFrameType
    };

    CAN_TP(ITransportLayer *canInterface, uint16_t txId, uint16_t rxId);
    void SetIds(uint16_t txId, uint16_t rxId);
    uint8_t Send(uint8_t* byteArray, uint8_t sizeOfByteArray);

    // Incoming CAN message should be passed to this function, it will build the CAN-TP message based on the rxId
    CAN_TP::ProcessResult ProcessIncomingMessage(unsigned long millis, uint16_t canId, uint8_t length, uint8_t incomingBuffer[]);

    // When Process() returns RxSuccess, the received message is stored in receivedMessage and its length in receivedMessageLength
    CAN_TP::ProcessResult Process(unsigned long millis, uint16_t* receivedMessageLength, uint8_t receivedMessage[]);
};

#endif
