#include "VanQueryAirCon.h"
void VanQueryAirCon::InternalProcess()
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

void VanQueryAirCon::SetData(uint8_t ignition)
{
    _ignition = ignition;
}
