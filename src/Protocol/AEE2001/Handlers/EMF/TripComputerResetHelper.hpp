#pragma once

#include <cstdint>
#include <cstring>
#include <cstdio>

#include "../../../../Helpers/CarState.hpp"
#include "../../../FeedbackSignal.hpp"

class TripComputerResetHelper
{
    CarState* _carState;
    FeedbackSignalCallback _feedbackSignalCallback;
    uint64_t _tripButtonPressedSince = 0;
    uint8_t _prevTripButtonState = 0;

    public:

        TripComputerResetHelper(CarState* carState)
        {
            _carState = carState;
        }

        void SetFeedbackSignalCallback(FeedbackSignalCallback feedbackSignalCallback) { _feedbackSignalCallback = feedbackSignalCallback; }

        void Process()
        {
            if (_carState->RightStickButtonPushed)
            {
                if (_prevTripButtonState == 0)
                {
                    _prevTripButtonState = 1;
                    _tripButtonPressedSince = _carState->CurrenTime;
                }

                if (_tripButtonPressedSince != 0 && _carState->CurrenTime - _tripButtonPressedSince > 3000)
                {
                    //printf("Trip button pressed for more than 3 seconds, resetting trip data\n");
                    _tripButtonPressedSince = 0;
                    switch (_carState->CurrentEmfMode)
                    {
                        case 2:
                            _carState->ResetTotals = 1;
                            break;
                        case 4:
                            _carState->ResetCumulative = 1;
                            break;
                        case 7:
                            _carState->ResetTotals = 1;
                            _carState->ResetCumulative = 1;
                            break;

                        default:
                            break;
                    }
                }
            }
            else
            {
                //
                /*
                if (_prevTripButtonState == 1)
                {
                    printf("Trip button released, resetting trip button state\n");
                }
                //*/
                _prevTripButtonState = 0;
                _tripButtonPressedSince = 0;
                _carState->ResetTotals = 0;
                _carState->ResetCumulative = 0;
            }

            if (_carState->ResetCumulative || _carState->ResetTotals)
            {
                if (_feedbackSignalCallback)
                {
                    _feedbackSignalCallback(FeedbackSignal::ResetTripComputer);
                }
            }
        }
};
