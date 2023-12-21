#include "MessageHandler_221.h"
#include "../Structs/CAN_221.h"
#include "../../Helpers/IntUnions.h"
#include <Arduino.h>

void MessageHandler_221::TripButtonPress() {
    CanTripByte1Struct button;
    button.asByte = _data[0];
    button.data.trip_switch_pressed = 1;
    _data[0] = button.asByte;

    uint8_t messageSentCount = 0;

    // we have to send the "trip button pressed" state several times to change the trip computer on the display
    // the display changes the trip computer after we don't send the "trip button pressed" state any more
    while (messageSentCount < 10)
    {
        _canInterface->SendMessage(0x221, 0, _dataLength, _data);
        messageSentCount++;
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }

    button.data.trip_switch_pressed = 0;
    _data[0] = button.asByte;

    messageSentCount = 0;
    while (messageSentCount < 2)
    {
        _canInterface->SendMessage(0x221, 0, _dataLength, _data);
        messageSentCount++;
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void MessageHandler_221::SetData()
{
    CanTripByte1Struct button;
    button.data.trip_switch_pressed = _dataBroker->IsTripPressed;

    _data[1] = _dataBroker->InstantConsumptionByte1;
    _data[2] = _dataBroker->InstantConsumptionByte2;
    _data[3] = _dataBroker->RemainingRangeByte1;
    _data[4] = _dataBroker->RemainingRangeByte2;
    _data[5] = _dataBroker->TotalRangeByte1;
    _data[6] = _dataBroker->TotalRangeByte2;

    if (_config->LEFT_STICK_BUTTON_SHOWS_OIL_TEMP)
    {
        if (_dataBroker->LeftStickButtonPushed)
        {
            UInt16 oilTemp;
            oilTemp.asUint16 = (_dataBroker->OilTemperature - 40) * 10;

            _data[3] = 0;
            _data[4] = _dataBroker->FuelLevel;
            _data[5] = oilTemp.data.leftByte;
            _data[6] = oilTemp.data.rightByte;
        }
    }
    else
    {
        button.data.left_stick_button = _dataBroker->LeftStickButtonPushed;
    }

    _data[0] = button.asByte;
}
