#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_0E1_2004.h"
#include "MessageHandler_0E1_2004.hpp"

BusMessage MessageHandler_0E1::Generate(CarState* carState)
{
    CanParkingAidByte1Struct status{};
    status.data.rear_status  = carState->ParkingAidStatus.data.RearStatus;
    status.data.front_status = carState->ParkingAidStatus.data.FrontStatus;

    CanParkingAidByte2Struct soundOptions{};
    soundOptions.data.sound_enabled = carState->ParkingAidStatus.data.SoundEnabled;
    soundOptions.data.location      = carState->ParkingAidStatus.data.BeepLocation;
    soundOptions.data.channel       = carState->ParkingAidStatus.data.Channel;

    CanParkingAidByte3Struct beep{};
    beep.data.beep_delay    = carState->ParkingAidStatus.data.BeepDelay;
    beep.data.beep_duration = carState->ParkingAidStatus.data.BeepDuration;

    CanParkingAidByte4Struct rearAndRearLeft{};
    rearAndRearLeft.data.rear_distance      = carState->ParkingAidStatus.data.RearDistance;
    rearAndRearLeft.data.rear_left_distance = carState->ParkingAidStatus.data.RearLeftDistance;

    CanParkingAidByte5Struct frontLeftAndRearRight{};
    frontLeftAndRearRight.data.rear_right_distance = carState->ParkingAidStatus.data.RearRightDistance;
    frontLeftAndRearRight.data.front_left_distance = carState->ParkingAidStatus.data.FrontLeftDistance;

    CanParkingAidByte6Struct field6{};
    field6.data.show                 = carState->ParkingAidStatus.data.Show;
    field6.data.front_right_distance = carState->ParkingAidStatus.data.FrontRightDistance;
    field6.data.front_distance       = carState->ParkingAidStatus.data.FrontDistance;

    CanParkingAidByte7Struct measurement{};
    measurement.data.which_side_was_measured        = carState->ParkingAidStatus.data.WhichSideWasMeasured;
    measurement.data.show_measured_space_on_display = carState->ParkingAidStatus.data.ShowMeasuredSpaceOnDisplay;
    measurement.data.measured_free_space            = carState->ParkingAidStatus.data.MeasuredFreeSpace;
    measurement.data.measurement_status             = carState->ParkingAidStatus.data.MeasurementStatus;

    message.data[0] = status.asByte;
    message.data[1] = soundOptions.asByte;
    message.data[2] = beep.asByte;
    message.data[3] = rearAndRearLeft.asByte;
    message.data[4] = frontLeftAndRearRight.asByte;
    message.data[5] = field6.asByte;
    message.data[6] = measurement.asByte;

    return message;
}

void MessageHandler_0E1::Parse(CarState* carState, const BusMessage& message)
{
    CanParkingAidStruct packet;
    std::memcpy(&packet, message.data, sizeof(packet));

    carState->ParkingAidStatus.data.RearStatus         = packet.Status.data.rear_status;
    carState->ParkingAidStatus.data.FrontStatus        = packet.Status.data.front_status;
    carState->ParkingAidStatus.data.SoundEnabled       = packet.SoundOptions.data.sound_enabled;
    carState->ParkingAidStatus.data.BeepLocation       = packet.SoundOptions.data.location;
    carState->ParkingAidStatus.data.Channel            = packet.SoundOptions.data.channel;
    carState->ParkingAidStatus.data.BeepDelay          = packet.BeepPeriod.data.beep_delay;
    carState->ParkingAidStatus.data.BeepDuration       = packet.BeepPeriod.data.beep_duration;

    carState->ParkingAidStatus.data.RearDistance       = packet.RearAndRearLeft.data.rear_distance;
    carState->ParkingAidStatus.data.RearLeftDistance   = packet.RearAndRearLeft.data.rear_left_distance;

    carState->ParkingAidStatus.data.RearRightDistance  = packet.FronLeftAndRearRight.data.rear_right_distance;
    carState->ParkingAidStatus.data.FrontLeftDistance  = packet.FronLeftAndRearRight.data.front_left_distance;

    carState->ParkingAidStatus.data.FrontDistance      = packet.FrontAndFrontRight.data.front_distance;
    carState->ParkingAidStatus.data.FrontRightDistance = packet.FrontAndFrontRight.data.front_right_distance;
    carState->ParkingAidStatus.data.Show               = packet.FrontAndFrontRight.data.show;
    carState->ParkingAidStatus.data.MeasurementSide    = packet.Status.data.measurement_side;
    carState->ParkingAidStatus.data.WhichSideWasMeasured  = packet.Measurement.data.which_side_was_measured;
    carState->ParkingAidStatus.data.ShowMeasuredSpaceOnDisplay  = packet.Measurement.data.show_measured_space_on_display;
    carState->ParkingAidStatus.data.MeasuredFreeSpace  = packet.Measurement.data.measured_free_space;
    carState->ParkingAidStatus.data.MeasurementStatus  = packet.Measurement.data.measurement_status;
}
