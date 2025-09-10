#include "MessageHandler_128.h"
#include "../Structs/CAN_128.h"
#include "../Structs/CAN_128_2010.h"
#include "../../../Config.h"

void MessageHandler_128::SetData()
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

void MessageHandler_128::SetData2004()
{
    Can2004CombineLightsByte1 field0;
    field0.data.fuel_level_low             = _dataBroker->FuelLow;
    field0.data.driver_seatbelt_warning    = _dataBroker->SeatbeltWarning;
    field0.data.passenger_seatbelt_warning = _dataBroker->PassengerAirbag;
    field0.data.handbrake_signal           = _dataBroker->Handbrake;
    field0.data.air_bag_fault              = _dataBroker->Airbag;
    field0.data.diesel_pre_heating         = _dataBroker->PreHeatingStatus;

    Can2004CombineLightsByte2 field1;
    field1.data.abs_active = _dataBroker->AbsFault;
    field1.data.stop_light = _dataBroker->StopOnCMB;
    //field1.data.service_indicator_exclamation = 0;

    Can2004CombineLightsByte3 field2;
    field2.data.esp_inactivated = _dataBroker->EspDeactivated;
    field2.data.esp_in_progress = _dataBroker->EspInRegulation;

    Can2004CombineLightsByte5 field4;
    field4.data.right_turn_indicator    = _dataBroker->RightTurnIndicator;
    field4.data.left_turn_indicator     = _dataBroker->LeftTurnIndicator;
    field4.data.front_foglight          = _dataBroker->FrontFog;
    field4.data.rear_foglight           = _dataBroker->RearFog;
    field4.data.low_beam_on             = _dataBroker->LowBeam;
    field4.data.high_beam_on            = _dataBroker->HighBeam;
    field4.data.parking_light_indicator = _dataBroker->SideLights;

    Can2004CombineLightsByte6 field5;
    field5.data.cmb_active = _dataBroker->OdometerEnabled;

    Can2004CombineLightsByte7 field6;
    field6.data.gear_position = _dataBroker->GearPosition;

    Can2004CombineLightsByte8 field7;
    field7.data.auto_gearbox_mode      = _dataBroker->GearBoxMode;
    field7.data.auto_gearbox_selection = _dataBroker->GearBoxSelection;

    _data[0] = field0.asByte;
    _data[1] = field1.asByte;
    _data[2] = field2.asByte;
    _data[4] = field4.asByte;
    _data[5] = field5.asByte;
    _data[6] = field6.asByte;
    _data[7] = field7.asByte;
}

void MessageHandler_128::SetData2010()
{
    Can2010CombineLightsByte1 field0;
    field0.data.right_turn_indicator    = _dataBroker->RightTurnIndicator;
    field0.data.left_turn_indicator     = _dataBroker->LeftTurnIndicator;
    field0.data.front_foglight          = _dataBroker->FrontFog;
    field0.data.rear_foglight           = _dataBroker->RearFog;
    field0.data.low_beam_on             = _dataBroker->LowBeam;
    field0.data.high_beam_on            = _dataBroker->HighBeam;
    field0.data.parking_light_indicator = _dataBroker->SideLights;

    Can2010CombineLightsByte2 field1;
    field1.data.gear_position            = _dataBroker->GearPosition;
    field1.data.gear_position_in_driving = _dataBroker->GearPosition;

    Can2010CombineLightsByte3 field2;
    field2.data.auto_gearbox_mode      = _dataBroker->GearBoxMode;
    field2.data.auto_gearbox_selection = _dataBroker->GearBoxSelection;

    Can2010CombineLightsByte4 field3;
    field3.data.handbrake_signal              = _dataBroker->Handbrake;
    field3.data.passenger_airbag_activated    = _dataBroker->PassengerAirbag;
    field3.data.child_security_active         = !_dataBroker->PassengerAirbag;
    field3.data.stop_light                    = _dataBroker->StopOnCMB;
    //field3.data.service_indicator_exclamation = 0;

    Can2010CombineLightsByte5 field4;
    field4.data.esp_in_progress    = _dataBroker->EspInRegulation;
    field4.data.esp_inactivated    = _dataBroker->EspDeactivated;
    field4.data.diesel_pre_heating = _dataBroker->PreHeatingStatus;

    Can2010CombineLightsByte6 field5;
    field5.data.driver_seatbelt_warning = _dataBroker->SeatbeltWarning;
    field5.data.fuel_level_low          = _dataBroker->FuelLow;

    Can2010CombineLightsByte7 field6;
    field6.data.cmb_active = _dataBroker->OdometerEnabled;

    Can2010CombineLightsByte8 field7;
    field7.data.engine_indicator = _dataBroker->Mil;

    _data[0] = field0.asByte;
    _data[1] = field1.asByte;
    _data[2] = field2.asByte;
    _data[3] = field3.asByte;
    _data[4] = field4.asByte;
    _data[5] = field5.asByte;
    _data[6] = field6.asByte;
    _data[7] = field7.asByte;
}
