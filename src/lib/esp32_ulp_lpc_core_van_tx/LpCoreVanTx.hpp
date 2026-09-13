#pragma once

#ifndef _LpCoreVanTx_hpp
    #define _LpCoreVanTx_hpp

#include <stdint.h>
#include "VanLpShared.h"
#include "VanBusTiming.hpp"
#include "driver/gpio.h"
#include "../IVanMessageSender.h"
#include "../IVanMessageReceiver.h"

#ifdef CONFIG_IDF_TARGET_ESP32C6

#include "esp_sleep.h"
#include "driver/rtc_io.h"

static_assert(VAN_RX_MAX_MESSAGE_BYTES == VAN_LP_RX_MAX_MESSAGE_BYTES,
              "Transport and LP receive buffer sizes must match");

struct VanLpRxDiagnostics
{
    uint32_t receivedCount;
    uint32_t overflowCount;
    uint32_t malformedCount;
    uint32_t maxOccupancy;
};

class LpCoreVanTx : public IVanMessageSender, public IVanMessageReceiver
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
    public:
        LpCoreVanTx(gpio_num_t rxPin, gpio_num_t txPin, LP_VAN_NETWORK_SPEED networkSpeed);
        ~LpCoreVanTx();
        // Nonblocking: false means invalid input, not started, or previous update
        // still in use by LP. Retry from task context once configuration is ready.
        bool IsConfigurationReady() const;
        bool ConfigureAckFrame(uint8_t slot, uint16_t identifier, bool enabled = true);
        bool ConfigureReplyFrame(uint8_t slot, uint16_t identifier, const uint8_t* data, uint8_t length, bool enabled = true);
        bool TrySendFrame(uint16_t identifier, const uint8_t* data, uint8_t length, uint8_t command, bool query);
        VanLpResult GetLastTxResult() const;
        uint32_t GetLastTxAbortDetail() const;
        bool GetLastTxArbitrationTrace(VanLpArbitrationTrace& trace) const;
        void GetGpioState(VanLpGpioState& state) const;
        // Zero-based second EOD slice, or UINT32_MAX if none was completed.
        uint32_t GetLastTxEodTs() const;
        bool GetLastTxRxTrace(uint8_t index, VanLpRxTrace& trace) const;
        VanLpResult GetLastBusResult() const;
        void GetReceiveDiagnostics(VanLpRxDiagnostics& diagnostics) const;
        void ReceiveData(uint8_t* messageLength, uint8_t message[]) override;
        void Start();
        void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck);
        void SendReplyRequestFrame(const uint16_t identifier);
        bool IsTxPossible();
        void SetAckIdentifiers(const uint16_t identifiers[], const uint8_t count) override;
        void SetQueryRequesterAckEnabled(const bool enabled) override;
        void SetRequestedReplyFrame(const uint8_t slot, const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool enabled) override;
};
#else
    #if !defined(CONFIG_IDF_TARGET_ESP32)
    //dummy class to avoid compiler errors when compiling for a target which is not ESP32 (for example ESP32C3)
    class LpCoreVanTx : public IVanMessageSender, public IVanMessageReceiver
    {
        public:
            using LP_VAN_NETWORK_SPEED = VanBusSpeed;
            static constexpr VanBusSpeed LP_VAN_62K5BPS = VanBusSpeed::Kts62_5;
            static constexpr VanBusSpeed LP_VAN_125KBPS = VanBusSpeed::Kts125;

            LpCoreVanTx(gpio_num_t rxPin, gpio_num_t txPin, LP_VAN_NETWORK_SPEED networkSpeed) {}
            ~LpCoreVanTx() {}
            void Start() {}
            void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck) {}
            void SendReplyRequestFrame(const uint16_t identifier) {}
            bool IsTxPossible() { return false; }
            void ReceiveData(uint8_t* messageLength, uint8_t message[]) override { *messageLength = 0; (void)message; }
            void SetAckIdentifiers(const uint16_t identifiers[], const uint8_t count) override {(void)identifiers; (void)count;}
            void SetQueryRequesterAckEnabled(const bool enabled) override {(void)enabled;}
            void SetRequestedReplyFrame(const uint8_t slot, const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool enabled) override {(void)slot; (void)identifier; (void)data; (void)length; (void)enabled;}
    };
    #endif
#endif
#endif
