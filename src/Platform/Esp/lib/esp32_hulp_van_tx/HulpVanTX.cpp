#include "lib/esp32_hulp_van_tx/HulpVanTx.hpp"

#if CONFIG_IDF_TARGET_ESP32

HulpVanTx::HulpVanTx(uint8_t vanRxPin, uint8_t vanTxPin)
{
    _ulpVanTx = new UlpVanTx((gpio_num_t)vanRxPin, (gpio_num_t)vanTxPin, ULP_VAN_125KBPS);
}

void HulpVanTx::Start()
{
    _ulpVanTx->Start();
}

void HulpVanTx::SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck)
{
    _ulpVanTx->SendNormalFrame(identifier, data, length, requireAck);
}

void HulpVanTx::SendReplyRequestFrame(const uint16_t identifier)
{
    _ulpVanTx->SendReplyRequestFrame(identifier);
}

bool HulpVanTx::IsTxPossible()
{
    return true;
}

void HulpVanTx::SetAckIdentifiers(const uint16_t identifiers[], const uint8_t count)
{
    if (count > 0)
    {
        //only one acknowledgment identifier is supported, so we take the first one
        _ulpVanTx->SetAckIdentifier(identifiers[0], true);
    }
}
#endif
