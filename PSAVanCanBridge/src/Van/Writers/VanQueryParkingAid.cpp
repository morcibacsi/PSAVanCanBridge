#include "VanQueryParkingAid.h"

 void VanQueryParkingAid::InternalProcess()
{
    if (_ignition == 1 && _isReverseEngaged == 1)
    {
        if (_initDiag)
        {
            parkingAidDiagSender->GetManufacturerInfo(PARKING_AID_DIAG_START_CHANNEL);
            _initDiag = 0;
        }
        else
        {
            parkingAidDiagSender->QueryParkingRadarData(PARKING_AID_DIAG_DATA_CHANNEL);
            parkingAidDiagSender->GetDistance(PARKING_AID_DIAG_START_CHANNEL);
        }
    }
}

void VanQueryParkingAid::SetData(uint8_t ignition, uint8_t isReverseEngaged)
{
    _ignition = ignition;
    _isReverseEngaged = isReverseEngaged;
    if (_prevReverseEngaged == 0 && _isReverseEngaged == 1)
    {
        _initDiag = 1;
    }
    _prevReverseEngaged = _isReverseEngaged;
}
