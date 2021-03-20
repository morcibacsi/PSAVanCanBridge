// VanAirConditionerDiagSensorStructs.h
#pragma once

#ifndef _VanAirConditionerDiagSensorStructs_h
    #define _VanAirConditionerDiagSensorStructs_h

// VANID: ADC
const uint16_t VAN_ID_AIR_CONDITIONER_DIAG = 0xADC;
// VANID: A5C
const uint16_t VAN_ID_AIR_CONDITIONER_DIAG_COMMAND = 0xA5C;
// C0
const uint8_t VAN_ID_AIR_CONDITIONER_DIAG_SENSOR_STATUS = 0xC0;
// C1
const uint8_t VAN_ID_AIR_CONDITIONER_DIAG_ACTUATOR_STATUS = 0xC1;

typedef union TwoBytes {
    uint16_t value;
    uint8_t bytes[2];
};

#pragma region Sensor status
// Read left to right in documentation
typedef struct VanAirConditionerDiagSensorStatusStruct {
    uint8_t Header;
    uint8_t Byte1;
    uint8_t DiagFunctionId;
    uint8_t InternalTemperature1;
    uint8_t InternalTemperature2;
    uint8_t CoolantTemperature1;
    uint8_t CoolantTemperature2;
    uint8_t Byte7;
    uint8_t Byte8;
    uint8_t Byte9;
    uint8_t Byte10;
    uint8_t VanSupplyVoltage;
    uint8_t Byte12;
    uint8_t Byte13;
    uint8_t Byte14;
    uint8_t Byte15;
    uint8_t Byte16;
    uint8_t Byte17;
    uint8_t BlowerSupplyVoltage;
    uint8_t Byte19;
    uint8_t Byte20;
    uint8_t Footer;
};

typedef union VanAirConditionerDiagSensorStatusPacket {
    VanAirConditionerDiagSensorStatusStruct data;
    uint8_t VanAirConditionerDiagSensorStatusPacket[sizeof(VanAirConditionerDiagSensorStatusStruct)];
};
#pragma endregion

#pragma region Actuator status
// Read left to right in documentation
typedef struct VanAirConditionerDiagActuatorStatusStruct {
    uint8_t Header;
    uint8_t Byte1;
    uint8_t DiagFunctionId;
    uint8_t RecirculationStatus;
    uint8_t DistributionStatus;
    uint8_t MixingStatus;
    uint8_t Byte6;
    uint8_t Byte7;
    uint8_t Byte8;
    uint8_t Byte9;
    uint8_t Byte10;
    uint8_t Footer;
};

typedef union VanAirConditionerDiagActuatorStatusPacket {
    VanAirConditionerDiagActuatorStatusStruct data;
    uint8_t VanAirConditionerDiagActuatorStatusPacket[sizeof(VanAirConditionerDiagActuatorStatusStruct)];
};
#pragma endregion

float static GetACDiagTemperatureFromVanValue(uint8_t byte1, uint8_t byte2)
{
    TwoBytes temperature;
    temperature.bytes[0] = byte1;
    temperature.bytes[1] = byte2;
    return (SwapHiByteAndLoByte(temperature.value) / 10.0) - 40;
}

float static GetACDiagTemperatureFromVanValue(uint16_t vanValue)
{
    return (SwapHiByteAndLoByte(vanValue) / 10.0) - 40;
}

float static GetACDiagVoltageFromVanValue(uint16_t vanValue)
{
    return vanValue / 10.0;
}

#pragma region Sender class
#include "../AbstractVanMessageSender.h"

class VanACDiagPacketSender
{
    AbstractVanMessageSender* vanMessageSender;

public:
    VanACDiagPacketSender(AbstractVanMessageSender* object)
    {
        vanMessageSender = object;
    }

    void GetManufacturerInfo(uint8_t channelId)
    {
        uint8_t packet[2] = { 0x21, 0x80 };
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
    }

    void GetSettings(uint8_t channelId)
    {
        uint8_t packet[2] = { 0x21, 0xA0 };
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
    }

    void GetSensorStatus(uint8_t channelId)
    {
        uint8_t packet[2] = { 0x21, 0xC0 };
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
    }

    void GetActuatorStatus(uint8_t channelId)
    {
        uint8_t packet[2] = { 0x21, 0xC1 };
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
    }

    void GetButtonStatus(uint8_t channelId)
    {
        uint8_t packet[2] = { 0x21, 0xC2 };
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
    }

    void GetFaultCodes(uint8_t channelId)
    {
        uint8_t packet[2] = { 0x21, 0xC4 };
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
    }

    void QueryAirConData(uint8_t channelId)
    {
        vanMessageSender->set_channel_for_reply_request_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG, 26 + 2, 1);
    }

};
#pragma endregion

#endif
