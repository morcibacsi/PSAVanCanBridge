#pragma once

enum class ImmediateSignal {
    None,
    GetTripComputerData,
    TripButtonPressed,
    RadioRemote,
    ReverseChanged,
    CruiseControl,
    SpeedAndRpm,
    PopupMessage,
    CmbSignalLightsChanged,
    CmbIndicatorLightsChanged,
    CmbStatusChanged,
};