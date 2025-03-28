#include "VanQueryAirCon.h"
void VanQueryAirCon::InternalProcess()
{
    if (_ignition == 1)
    {
        if (_initDiag)
        {
            acDiagSender->GetManufacturerInfo(AC_DIAG_START_CHANNEL);
            _initDiag = 0;
        }
        else
        {
            //
            /*
            if (_diagStatus == 0)
            {
                acDiagSender->GetSensorStatus(AC_DIAG_QUERY_SENSOR_STATUS_CHANNEL);
                _diagStatus = 1;
            }
            else
            //*/
            {
                acDiagSender->GetActuatorStatus(AC_DIAG_QUERY_ACTUATOR_STATUS_CHANNEL);
                _diagStatus = 0;
            }
        }
    }
}

void VanQueryAirCon::SetData(uint8_t ignition)
{
    _ignition = ignition;
    if (_prevIgnition == 0 && _ignition == 1)
    {
        _initDiag = 1;
    }
    _prevIgnition = _ignition;
}

void VanQueryAirCon::QueryAirConData()
{
    acDiagSender->QueryAirConData(AC_DIAG_DATA_CHANNEL);
}