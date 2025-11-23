#pragma once

#include <cstdint>
#include "../IVanMessageSender.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32

#include "../../../components/esp32_ulp_van_tx-main/src/UlpVanTx.h"

class HulpVanTx : public IVanMessageSender
{
private:
    UlpVanTx* _ulpVanTx = nullptr;
public:
    HulpVanTx(uint8_t vanRxPin, uint8_t vanTxPin);
    virtual void Start() override;
    virtual void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck) override;
    virtual void SendReplyRequestFrame(const uint16_t identifier) override;
    virtual bool IsTxPossible() override;
};
#endif