// VanQueryAirCon.h
#pragma once

#ifndef _VanQueryAirCon_h
    #define _VanQueryAirCon_h

#include "../AbstractVanMessageSender.h"
#include "../Structs/VanAirConditionerDiagStructs.h"
#include "VanMessageWriterBase.h"

class VanQueryAirCon : public VanMessageWriterBase
{
    const static uint16_t AIRCON_QUERY_INTERVAL = 120;

    const static uint8_t AC_DIAG_START_CHANNEL = 1;
    const static uint8_t AC_DIAG_QUERY_SENSOR_STATUS_CHANNEL = 2;
    const static uint8_t AC_DIAG_QUERY_ACTUATOR_STATUS_CHANNEL = 3;
    const static uint8_t AC_DIAG_DATA_CHANNEL = 4;

    uint8_t _ignition = 0;
    uint8_t _diagStatus = 0;

    VanACDiagPacketSender* acDiagSender;

    virtual void InternalProcess() override
    {
        if (_ignition)
        {
            acDiagSender->GetManufacturerInfo(AC_DIAG_START_CHANNEL);
            if (_diagStatus == 0)
            {
                acDiagSender->GetSensorStatus(AC_DIAG_QUERY_SENSOR_STATUS_CHANNEL);
                acDiagSender->QueryAirConData(AC_DIAG_DATA_CHANNEL);
                _diagStatus = 1;
            }
            else
            {
                acDiagSender->GetActuatorStatus(AC_DIAG_QUERY_ACTUATOR_STATUS_CHANNEL);
                acDiagSender->QueryAirConData(AC_DIAG_DATA_CHANNEL);
                _diagStatus = 0;
            }
        }
    }

    public:
        VanQueryAirCon(AbstractVanMessageSender* vanMessageSender) : VanMessageWriterBase(vanMessageSender, AIRCON_QUERY_INTERVAL)
    {
            acDiagSender = new VanACDiagPacketSender(vanMessageSender);
            acDiagSender->GetManufacturerInfo(AC_DIAG_START_CHANNEL);
    }

    void SetData(uint8_t ignition)
    {
        _ignition = ignition;
    }

};

#endif
