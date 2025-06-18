#pragma once

enum class FeedbackSignal {
    None,
    GetTripComputerData,
    QueryParkingRadarData,
    QueryAirConData,
    IgnitionChanged,
    DisplayMessage,
    ServiceReminderReset,
    EmergencyBrakeActivated
};

using FeedbackSignalCallback = void (*)(FeedbackSignal signal);
