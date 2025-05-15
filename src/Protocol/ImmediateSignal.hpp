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

using ImmediateSignalCallback = void (*)(ImmediateSignal signal);
