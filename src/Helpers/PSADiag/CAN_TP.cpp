#include "CAN_TP.hpp"
#include <cstring>

CAN_TP::CAN_TP(ITransportLayer *canInterface, uint16_t txId, uint16_t rxId)
{
    _canInterface = canInterface;
    SetIds(txId, rxId);
}

void CAN_TP::SetIds(uint16_t txId, uint16_t rxId)
{
    //Serial.print("Set ids: ");Serial.print(txId, 16);Serial.print(" ");Serial.println(rxId, 16);

    _txState = Idle;
    _rxState = Idle;
    _consecutiveFrameCounter = 0;
    _lastMillis = 0;

    _txId = txId;
    _rxId = rxId;
}

uint8_t CAN_TP::Send(uint8_t* byteArray, uint8_t sizeOfByteArray)
{
    if (_txState != Idle)
    {
        return 1; // Busy
    }

    _consecutiveFrameCounter = 0;
    _rxState = Idle;

    if (sizeOfByteArray <= 7)
    {
        // Single frame
        sendSingleFrame(byteArray, sizeOfByteArray);
        _txState = Idle; // Transmission ends here
    }
    else
    {
        // Start multi-frame transmission
        memcpy(txBuffer, byteArray, sizeOfByteArray);
        _txMessageLength = sizeOfByteArray;
        sendFirstFrame();
        _txState = WaitForFlowControl;
    }

    return 0; // Success
}

CAN_TP::ProcessResult CAN_TP::ProcessIncomingMessage(unsigned long millis, uint16_t canId, uint8_t length, uint8_t incomingBuffer[])
{
    if (canId != _rxId)
    {
        return RxCanIdMismatch;
    }

    PCIDataSingleFrame pci; // Protocol control information (first byte)
    pci.byte = incomingBuffer[0];

    //printf("RxState: %d, Frame type: %d, consecutive frame counter: %d\n", _rxState, pci.data.frameType, _consecutiveFrameCounter);
    //Serial.print("Frame type: ");Serial.println(pci.data.frameType);
    //Serial.print("Consecutive frame counter: ");Serial.println(_consecutiveFrameCounter);

    switch (pci.data.frameType)
    {
        case SINGLE_FRAME:
        {
            if (_rxState == WaitForConsecutiveFrames)
            {
                return RxInvalidFrameType; // Received single frame while waiting for consecutive frames
            }

            rxMessageLength = pci.data.length_or_sequence;
            if (rxMessageLength != length - 1)
            {
                return RxLengthMismatch;
            }

            // Store data in buffer, excluding the first byte
            memcpy(rxBuffer, incomingBuffer + 1, rxMessageLength);

            _consecutiveFrameCounter = 0;
            _rxState = EndReception;
            return RxSuccess; // Successfully received single frame
            break;
        }
        case FIRST_FRAME:
        {
            PCIDataFirstFrame ff_pci;
            ff_pci.raw[0] = incomingBuffer[0];
            ff_pci.raw[1] = incomingBuffer[1];

            // First Frame received, extract total message length
            rxMessageLength = ff_pci.getLength();

            //Serial.print("Incoming msg length: ");Serial.println(rxMessageLength);

            memcpy(rxBuffer, incomingBuffer+2, 6);

            // Prepare for consecutive frames
            _consecutiveFrameCounter = 1;
            _rxState = WaitForConsecutiveFrames;
            sendFlowControl();

            return InProgress;
            break;
        }
        case CONSECUTIVE_FRAME:
        {
            uint8_t currentSequenceNumber = pci.data.length_or_sequence;

            if (currentSequenceNumber != _consecutiveFrameCounter)
            {
                return RxSequenceNumberMismatch;
            }

            // Length of first frame is 6, so the start position is 6 + (currentSequenceNumber-1) * 7
            uint16_t startPos = 6+((_consecutiveFrameCounter-1) * 7);

            memcpy(rxBuffer + startPos, incomingBuffer + 1, length - 1);

            uint8_t bytesRead = startPos + length - 1;
            //Serial.print("bytesRead: ");Serial.println(bytesRead);

            _consecutiveFrameCounter++;
            if (bytesRead >= rxMessageLength)
            {
                _rxState = EndReception; // Full message received
            }
            return InProgress;
            break;
        }
        case FLOW_CONTROL_FRAME:
        {
            _consecutiveFrameCounter++;
            sendConsecutiveFrame();

            return InProgress;
            break;
        }
        default:
            return RxInvalidFrameType;
    }

    _rxState = Idle;
    return RxInvalidFrameType;
}

CAN_TP::ProcessResult CAN_TP::Process(unsigned long millis, uint16_t* receivedMessageLength, uint8_t receivedMessage[])
{
     // **Sending State Machine**
    switch (_txState)
    {
        case Idle:
            break;

        case StartTransmission:
            // Send the first frame and wait for flow control
            _consecutiveFrameCounter = 0;
            sendFirstFrame();

            _txState = WaitForFlowControl;
            _lastMillis = millis;
            break;

        case WaitForFlowControl:
            if (millis - _lastMillis > 1000)
            {
                _txState = Idle;
                return TxFlowControlTimeout;
            }

            // Flow control received, move to consecutive frame transmission
            _txState = SendConsecutiveFrames;
            _consecutiveFrameCounter = 0;
            _lastMillis = millis;
            break;

        case SendConsecutiveFrames:
            // Send consecutive frames until all data is sent
            if (_consecutiveFrameCounter * 7 < _txMessageLength)
            {
                sendConsecutiveFrame();
                _consecutiveFrameCounter++;
            }
            else
            {
                // All frames sent, move to the final state
                _txState = EndTransmission;
            }
            _lastMillis = millis;
            break;

        case EndTransmission:
            _txState = Idle;
            return TxSuccess;

        default:
            break;
    }

    // **Receiving State Machine**
    switch (_rxState)
    {
        case Idle:
            break;

        case WaitForFirstFrame:
            // Wait for the first frame of a multi-frame message
            if (millis - _lastMillis > 1000)
            {
                // Timeout for first frame
                _rxState = Idle;
                return RxFirstFrameTimeout;
            }

            // First frame received, move to consecutive frame reception
            _rxState = WaitForConsecutiveFrames;
            _lastMillis = millis;
            break;

        case WaitForConsecutiveFrames:
            // Wait for consecutive frames to complete the reception
            if (millis - _lastMillis > 1000)
            {
                // Timeout for consecutive frames
                _rxState = Idle;
                return RxConsecutiveFrameTimeout;
            }

            // Check if all frames have been received
            if (6 + ((_consecutiveFrameCounter-1) * 7) >= rxMessageLength)
            {
                _rxState = EndReception;
            }
            _lastMillis = millis;
            break;

        case EndReception:
            _rxState = Idle;
            *receivedMessageLength = rxMessageLength;
            memcpy(receivedMessage, rxBuffer, rxMessageLength);

            return RxSuccess;

        default:
            break;
    }

    return InProgress;
}

void CAN_TP::sendSingleFrame(uint8_t* data, uint8_t size)
{
    uint8_t frame[8];

    PCIDataSingleFrame pci;
    pci.data.frameType = SINGLE_FRAME;
    pci.data.length_or_sequence = size;
    frame[0] = pci.byte;

    memcpy(frame + 1, data, size);

    BusMessage msg = {};
    msg.id = _txId;
    msg.dataLength = size + 1;
    memcpy(msg.data, frame, size + 1);
    msg.protocol = ProtocolType::AEE2004; // Not used, but set it
    _canInterface->SendMessage(msg);

    //_canInterface->SendMessage(_txId, 0, size + 1, frame);
}

void CAN_TP::sendFirstFrame()
 {
    uint8_t frame[8];

    PCIDataFirstFrame pci;
    pci.data.frameType = FIRST_FRAME;
    pci.setLength(_txMessageLength);

    frame[0] = pci.raw[0];
    frame[1] = pci.raw[1];

    // First frame can carry up to 6 data bytes
    memcpy(frame + 2, txBuffer, 6);

    BusMessage msg = {};
    msg.id = _txId;
    msg.dataLength = 8;
    memcpy(msg.data, frame, 8);
    msg.protocol = ProtocolType::AEE2004; // Not used, but set it
    _canInterface->SendMessage(msg);

    //_canInterface->SendMessage(_txId, 0, 8, frame);
}

void CAN_TP::sendConsecutiveFrame()
{


    uint8_t frame[8];

    PCIDataSingleFrame pci;
    pci.data.frameType = CONSECUTIVE_FRAME;
    pci.data.length_or_sequence = _consecutiveFrameCounter;

    frame[0] = pci.byte;

    int remaining =
        (int)_txMessageLength - (_consecutiveFrameCounter * 6);

    if (remaining < 0)
        remaining = 0;

    uint8_t size = (remaining < 7) ? remaining : 7;

    //uint8_t size = min(7, _txMessageLength - (_consecutiveFrameCounter * 6));
    uint8_t start = _consecutiveFrameCounter * 6;

    memcpy(frame + 1, txBuffer + start, size);

    BusMessage msg = {};
    msg.id = _txId;
    msg.dataLength = size + 1;
    memcpy(msg.data, frame, size + 1);
    msg.protocol = ProtocolType::AEE2004; // Not used, but set it
    _canInterface->SendMessage(msg);

    //_canInterface->SendMessage(_txId, 0, size + 1, frame);
    _consecutiveFrameCounter++;
}

void CAN_TP::sendFlowControl()
{
    // Flow control frame is always 3 bytes long
    uint8_t frame[3];

    PCIDataFlowControlFrame pci;
    pci.data.frameType = FLOW_CONTROL_FRAME;
    pci.data.blockSize = 0;
    pci.data.separationTime = 5;
    pci.data.flowStatus = 0;

    frame[0] = pci.raw[0];
    frame[1] = pci.raw[1];
    frame[2] = pci.raw[2];

    BusMessage msg = {};
    msg.id = _txId;
    msg.dataLength = 3;
    memcpy(msg.data, frame, 3);
    msg.protocol = ProtocolType::AEE2004; // Not used, but set it
    _canInterface->SendMessage(msg);

    //_canInterface->SendMessage(_txId, 0, 3, frame);
}
