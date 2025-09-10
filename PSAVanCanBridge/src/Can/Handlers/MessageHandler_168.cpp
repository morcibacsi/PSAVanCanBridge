#include "MessageHandler_168.h"
#include "../../../Config.h"
#include "../Structs/CAN_168.h"
#include "../Structs/CAN_168_2010.h"

void MessageHandler_168::SetData()
{
    {
        if (_config->CAN_TYPE == 0)
        {
            SetData2004();
        }
        else
        {
            SetData2010();
        }
    }
}

void MessageHandler_168::SetData2004()
{
    CanDash3Byte1Struct field1;
    field1.data.auto_gearbox_alert       = _dataBroker->AutoGearboxAlert;
    field1.data.break_fluid_alert        = _dataBroker->BreakFluidAlert;
    field1.data.coolant_level_alert      = _dataBroker->CoolantLevelAlert;
    field1.data.coolant_temp_max         = _dataBroker->CoolantTempMax;
    field1.data.oil_level_alert          = _dataBroker->OilLevelAlert;
    field1.data.oil_pressure_alert       = _dataBroker->OilPressureAlert;
    field1.data.oil_temp_max             = _dataBroker->OilTempMax;

    CanDash3Byte2Struct field2;
    field2.data.diesel_additive_alert    = _dataBroker->DieselAdditiveAlert;
    field2.data.fap_clogged              = _dataBroker->FapClogged;
    field2.data.flat_tyre_alert          = _dataBroker->FlatTyreAlert;
    field2.data.tyre_pressure_alert      = _dataBroker->TyrePressureAlert;

    CanDash3Byte4Struct field4;
    field4.data.abs_fault                = _dataBroker->AbsFault;
    field4.data.brake_pad_fault          = _dataBroker->BrakePadFault;
    field4.data.esp_fault                = _dataBroker->EspFault;
    field4.data.mil                      = _dataBroker->Mil;
    field4.data.gearbox_fault            = _dataBroker->AutoGearboxFault;
    field4.data.water_in_diesel          = _dataBroker->WaterInDiesel;
    field4.data.serious_suspension_fault = _dataBroker->SeriousSuspensionFault;

    CanDash3Byte5Struct field5;
    field5.data.antipollution_fault      = _dataBroker->AntipollutionFault;
    field5.data.battery_charge_fault     = _dataBroker->BatteryChargeFault;
    field5.data.diesel_additive_fault    = _dataBroker->DieselAdditiveFault;
    field5.data.driver_airbag            = _dataBroker->Airbag;

    _data[0] = field1.asByte;
    _data[1] = field2.asByte;
    _data[3] = field4.asByte;
    _data[4] = field5.asByte;
}

void MessageHandler_168::SetData2010()
{
    CanDash3Byte1_2010_Struct field1;
    field1.data.break_fluid_alert        = _dataBroker->BreakFluidAlert;
    field1.data.coolant_level_alert      = _dataBroker->CoolantLevelAlert;
    field1.data.coolant_temp_max         = _dataBroker->CoolantTempMax;
    field1.data.oil_level_alert          = _dataBroker->OilLevelAlert;
    field1.data.oil_pressure_alert       = _dataBroker->OilPressureAlert;

    CanDash3Byte2_2010_Struct field2;
    field2.data.fap_clogged              = _dataBroker->FapClogged;
    field2.data.flat_tyre_alert          = _dataBroker->FlatTyreAlert;
    field2.data.tyre_pressure_alert      = _dataBroker->TyrePressureAlert;
    field2.data.antipollution_fault      = _dataBroker->AntipollutionFault;

    CanDash3Byte3_2010_Struct field3;
    field3.data.battery_charge_fault     = _dataBroker->BatteryChargeFault;
    field3.data.serious_suspension_fault = _dataBroker->SeriousSuspensionFault;;

    CanDash3Byte4_2010_Struct field4;
    field4.data.abs_fault                = _dataBroker->AbsFault;
    field4.data.brake_pad_fault          = _dataBroker->BrakePadFault;
    field4.data.esp_fault                = _dataBroker->EspFault;
    field4.data.mil                      = _dataBroker->Ignition ? _dataBroker->Mil : 0;
    field4.data.gearbox_fault            = _dataBroker->AutoGearboxFault;

    CanDash3Byte5_2010_Struct field5;
    field5.data.water_in_diesel          = _dataBroker->WaterInDiesel;

    CanDash3Byte7_2010_Struct field7;
    field7.data.gearbox_position         = _dataBroker->GearBoxSelection;

    _data[0] = field1.asByte;
    _data[1] = field2.asByte;
    _data[2] = field3.asByte;
    _data[3] = field4.asByte;
    _data[4] = field5.asByte;
    _data[7] = field7.asByte;
}
