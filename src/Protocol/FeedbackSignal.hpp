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
};

using FeedbackSignalCallback = void (*)(FeedbackSignal signal);
