#pragma once

#ifndef _esp32_arduino_rmt_van_rx_h
    #define _esp32_arduino_rmt_van_rx_h

    #include "Esp32RmtReader.h"

typedef enum {
    VAN_LINE_LEVEL_LOW   = 0,
    VAN_LINE_LEVEL_HIGH  = 1,
} VAN_LINE_LEVEL;

typedef enum {
    VAN_NETWORK_TYPE_BODY     = 0,
    VAN_NETWORK_TYPE_COMFORT  = 1,
} VAN_NETWORK_TYPE;

class ESP32_RMT_VAN_RX: public Esp32RmtReader
{
private:
    //static constexpr uint32_t minSignal = 1250;          // the shortest duration for one bit is 8us, 1250ns < 8us, valid signal won't be treated as noise
    static constexpr uint32_t minSignal = 1000;          // the shortest duration for one bit is 8us, 1250ns < 8us, valid signal won't be treated as noise
    //static constexpr uint32_t maxSignal = 8*8000+500;    // the longest duration is the IFS which is 8*8us so 64.5 us > 64 us, the receive won't stop early
    static constexpr uint32_t maxSignal = (80000);    // the longest duration is the IFS which is 8*8us so 64.5 us > 64 us, the receive won't stop early

    uint8_t _bitCounter = 0;
    uint8_t _tempByte = 0;
    uint8_t _mask = 1 << 7;
    uint8_t _timeSliceDivisor = 0;
    VAN_LINE_LEVEL _vanLineLevel = VAN_LINE_LEVEL_LOW;

    uint8_t RoundToNearest(uint8_t numToRound, uint8_t multiple);
    void BeforeProcessSignal() override;
    void AfterProcessSignal() override;
    void ProcessSignal(uint32_t level, uint32_t duration) override;

public:
    ESP32_RMT_VAN_RX(uint8_t rxPin, int8_t ledPin, VAN_LINE_LEVEL vanLineLevel, VAN_NETWORK_TYPE vanNetworkType) : Esp32RmtReader(minSignal, maxSignal, rxPin, ledPin)
    {
        _vanLineLevel = vanLineLevel;
        if(vanNetworkType == VAN_NETWORK_TYPE_COMFORT)
        {
            _timeSliceDivisor = 8;
        }
        else if(vanNetworkType == VAN_NETWORK_TYPE_BODY)
        {
            _timeSliceDivisor = 16;
            _maxSignal = maxSignal * 2; // on the body network transferrate is 62.5 kbps so one bit takes twice as long to transmit
        }
    }

    ~ESP32_RMT_VAN_RX();

    // You can check the CRC of the received message with this function
    bool IsCrcOk(uint8_t vanMessage[], uint8_t vanMessageLength);

    // This function calculates the CRC of a message
    uint16_t Crc15(uint8_t data[], uint8_t lengthOfData);
};
#endif
