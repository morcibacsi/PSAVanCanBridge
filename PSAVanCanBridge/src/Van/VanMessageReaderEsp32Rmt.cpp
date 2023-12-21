#include "VanMessageReaderEsp32Rmt.h"

VanMessageReaderEsp32Rmt::VanMessageReaderEsp32Rmt(uint8_t rxPin, uint8_t ledPin, IVAN_LINE_LEVEL vanLineLevel, IVAN_NETWORK_TYPE vanNetworkType)
{
    _van_rx = new ESP32_RMT_VAN_RX(VAN_DATA_RX_RMT_CHANNEL, rxPin, ledPin, static_cast<VAN_LINE_LEVEL>(vanLineLevel), static_cast<VAN_NETWORK_TYPE>(vanNetworkType));
}

void VanMessageReaderEsp32Rmt::Receive(uint8_t* messageLength, uint8_t message[])
{
    _van_rx->ReceiveData(messageLength, message);
}

void VanMessageReaderEsp32Rmt::Init()
{
    _van_rx->Start();
}

void VanMessageReaderEsp32Rmt::Stop()
{
    _van_rx->Stop();
}

bool VanMessageReaderEsp32Rmt::IsCrcOk(uint8_t vanMessage[], uint8_t vanMessageLength)
{
    return _van_rx->IsCrcOk(vanMessage, vanMessageLength);
}