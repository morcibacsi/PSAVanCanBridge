#ifndef PsaDiagLib_h
    #define PsaDiagLib_h

#include <inttypes.h>
#include <stdio.h>
#include "../../Protocol/ISerial.hpp"
#include "../../Protocol/ITransportLayer.hpp"
#include "../../Protocol/IsoTpFrame.hpp"

union uint32_converter {
    struct {
        uint8_t byte4;
        uint8_t byte3;
        uint8_t byte2;
        uint8_t byte1;
    } data;
    uint32_t asUInt32_t;
};

class PsaDiagLib : public IsoTpFrame
{
    private:
    constexpr static uint32_t KEEP_ALIVE_INTERVAL = 2000;
    ITransportLayer* _canSender;
    ISerial* _serial;

    uint8_t LIN;
    bool dump = true;
    bool sendKeepAlives = false;

    uint8_t framesDelayInput;

    uint16_t UnlockKey;
    uint8_t UnlockService;
    uint8_t DiagSess;

    char sendKeepAliveType;
    bool waitForUnlock;
    bool swUnlock = false;
    uint8_t UnlockCMD[2];

    bool waitingReplySerialCMD = false;

    unsigned long lastCMDSent = 0;
    unsigned long lastMessageReceived = 0;
    unsigned long lastKeepAliveSent = 0;
    unsigned long lastKeepAliveReceived = 0;

    uint8_t receivedCanTpPacket[4096];
    uint16_t receivedCanTpPacketLength;

    int receiveDiagFrameSize;
    bool customFrameSize = false;

    void PrintOk();
    void PrintError();
    void SendKeepAlive();
    void ChangeId(unsigned long currentTime, uint8_t data[], uint8_t length);
    void ChangeFrameDelay(uint8_t data[]);
    void Unlock(uint8_t data[], uint8_t length);
    void ChangeLIN(uint8_t data[]);
    void ChangeFrameSize(uint8_t data[]);
    void ChangeKeepAlive(uint8_t data[]);
    void ResetCAN();
    void PrintCurrentCANId();
    void SendRawFrames(unsigned long currentTime, uint8_t data[], uint8_t length);
    void SendFrames(unsigned long currentTime, uint8_t data[], uint8_t length);
    void LargeFrameSpliting(uint8_t data[], uint8_t length);
    void PrintArrayToSerial(uint16_t sizeOfByteArray, uint8_t *byteArray, uint8_t startIndex = 0);
    void ProcessUnwrappedMessage(unsigned long currentTime, uint16_t canId, uint8_t length, uint8_t data[]);

    void InternalProcess() override {};
    void ReceiveFinished(unsigned long currentTime) override;
    public:
    PsaDiagLib(ITransportLayer* canSender, ISerial* serial): IsoTpFrame(canSender, 0x760, 0x660, 2000)
    {
        _canSender = canSender;
        _serial = serial;
    };
    void ParseCommand(unsigned long currentTime, uint8_t data[], uint8_t length);
    bool Loop(unsigned long currentTime);
    void ProcessIncomingMessage(unsigned long currentTime, uint16_t canId, uint8_t canMessageLength, uint8_t data[]);
    virtual ~PsaDiagLib(){ }
};
#endif
