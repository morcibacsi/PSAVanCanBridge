#pragma once

enum class FeedbackSignal {
    None,
    GetTripComputerData,
    QueryParkingRadarData,
    IgnitionChanged,
    DisplayMessage,
    ServiceReminderReset,
    EmergencyBrakeActivated
};

using FeedbackSignalCallback = void (*)(FeedbackSignal signal);
