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
};

using FeedbackSignalCallback = void (*)(FeedbackSignal signal);
