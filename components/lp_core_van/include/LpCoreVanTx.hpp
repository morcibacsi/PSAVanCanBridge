#pragma once

#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32C6

#ifndef _LpCoreVanTx_hpp
    #define _LpCoreVanTx_hpp

#include <stdint.h>
#include "VanLpShared.h"
#include "VanBusTiming.hpp"
#include "VanFrame.h"

#include "driver/gpio.h"

#include "esp_sleep.h"
#include "driver/rtc_io.h"

struct VanLpRxDiagnostics
{
    uint32_t receivedCount;
    uint32_t overflowCount;
    uint32_t malformedCount;
    uint32_t maxOccupancy;
    uint32_t sofCount;
    uint32_t eodCount;
    uint32_t writeIndex;
    uint32_t readIndex;
};

class LpCoreVanTx
{
    public:

    static constexpr uint8_t MAX_ACK_IDENTIFIER_COUNT = VAN_LP_ENTRY_COUNT;

    using LP_VAN_NETWORK_SPEED = VanBusSpeed;
    static constexpr VanBusSpeed LP_VAN_62K5BPS = VanBusSpeed::Kts62_5;
    static constexpr VanBusSpeed LP_VAN_125KBPS = VanBusSpeed::Kts125;

private:
    void PrintToSerial(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, uint8_t *byteArray);

    private:
        gpio_num_t _rxPin;
        gpio_num_t _txPin;
        LP_VAN_NETWORK_SPEED _networkSpeed;

        VanLpConfig _configuration = {};
        bool _started = false;
        void PublishConfiguration();
        bool WaitForConfiguration() const;
    public:
        LpCoreVanTx(gpio_num_t rxPin, gpio_num_t txPin, LP_VAN_NETWORK_SPEED networkSpeed);
        ~LpCoreVanTx();
        // False until Start() and while the previous configuration update is
        // still being adopted by the LP core.
        bool IsConfigurationReady() const;
        bool TrySendFrame(uint16_t identifier, const uint8_t* data, uint8_t length, uint8_t command, bool query);
        VanLpResult GetLastTxResult() const;
        uint32_t GetLastTxAbortDetail() const;
        bool GetLastTxArbitrationTrace(VanLpArbitrationTrace& trace) const;
        void GetGpioState(VanLpGpioState& state) const;
        // Zero-based second EOD slice, or UINT32_MAX if none was completed.
        uint32_t GetLastTxEodTs() const;
        bool GetLastTxRxTrace(uint8_t index, VanLpRxTrace& trace) const;
        VanLpResult GetLastBusResult() const;
        // Blocks like ReceiveData(); false means reception stopped or the
        // single consumed queue entry was not a complete, FCS-valid frame.
        bool ReceiveFrame(VanFrame& frame) const;
        void GetReceiveDiagnostics(VanLpRxDiagnostics& diagnostics) const;
        void ReceiveData(uint8_t* messageLength, uint8_t message[]);
        void Start();
        void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck);
        void SendReplyRequestFrame(const uint16_t identifier);
        bool IsTxPossible();
        // Configuration setters wait up to 50 ms for the previous LP update.
        // False means invalid input, not started, or configuration remained busy.
        bool SetAckIdentifiers(const uint16_t identifiers[], uint8_t count);
        bool SetAckIdentifier(uint8_t slot, uint16_t identifier, bool enabled = true);
        bool SetQueryRequesterAckEnabled(bool enabled);
        bool SetRequestedReplyFrame(uint8_t slot, uint16_t identifier,
                                    const uint8_t data[], uint8_t length,
                                    bool enabled = true);
};
#endif

#endif // CONFIG_IDF_TARGET_ESP32C6
