#include "MessageHandler_120.h"
#include "../Structs/CAN_120.h"

void MessageHandler_120::SetData()
{
    CAN_120_2004_Byte4Struct field4;
    field4.data.GearboxFaultRepairNeeded = _dataBroker->AutoGearboxFault;

    CAN_120_2004_Byte5Struct field5;
    field5.data.EngineFaultRepairNeeded = _dataBroker->Mil || _dataBroker->AntipollutionFault;

    CAN_120_2004_Byte7Struct field7;
    field7.data.SuspensionFaulty = _dataBroker->SeriousSuspensionFault;

    _data[3] = field4.asByte;
    _data[4] = field5.asByte;
    _data[6] = field7.asByte;
}
