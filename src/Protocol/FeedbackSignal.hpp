#pragma once

enum class FeedbackSignal {
    None,
    GetTripComputerData,
    QueryParkingRadarData,
    QueryAirConData,
    IgnitionChanged,
    DisplayMessage,
    ServiceReminderReset,
    EmergencyBrakeActivated,
    ResetTripComputer,
    ClockSetByUser,
    CarSettingsChanged,
};

using FeedbackSignalCallback = void (*)(FeedbackSignal signal);
