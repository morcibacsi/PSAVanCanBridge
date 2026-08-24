#include <cstdint>
#include <cstring>

#include "../../Structs/CAN_0E1_2010.h"
#include "MessageHandler_0E1_2010.hpp"

BusMessage MessageHandler_0E1_2010::Generate(CarState* carState)
{
    CanParkingAidByte1_2010Struct status{};
    status.data.rear_status  = carState->ParkingAidStatus.data.RearStatus;
    status.data.front_status = carState->ParkingAidStatus.data.FrontStatus;
    status.data.measurement_side = carState->ParkingAidStatus.data.MeasurementSide;

    CanParkingAidByte2_2010Struct soundOptions{};
    soundOptions.data.sound_enabled = carState->ParkingAidStatus.data.SoundEnabled;
    soundOptions.data.location      = carState->ParkingAidStatus.data.BeepLocation;
    soundOptions.data.channel       = carState->ParkingAidStatus.data.Channel;

    CanParkingAidByte3_2010Struct beep{};
    beep.data.beep_delay    = carState->ParkingAidStatus.data.BeepDelay;
    beep.data.beep_duration = carState->ParkingAidStatus.data.BeepDuration;

    CanParkingAidByte4_2010Struct rearAndRearLeft{};
    rearAndRearLeft.data.rear_distance      = carState->ParkingAidStatus.data.RearDistance;
    rearAndRearLeft.data.rear_left_distance = carState->ParkingAidStatus.data.RearLeftDistance;

    CanParkingAidByte5_2010Struct frontLeftAndRearRight{};
    frontLeftAndRearRight.data.rear_right_distance = carState->ParkingAidStatus.data.RearRightDistance;
    frontLeftAndRearRight.data.front_left_distance = carState->ParkingAidStatus.data.FrontLeftDistance;

    CanParkingAidByte6_2010Struct field6{};
    field6.data.show                 = carState->ParkingAidStatus.data.Show;
    field6.data.front_right_distance = carState->ParkingAidStatus.data.FrontRightDistance;
    field6.data.front_distance       = carState->ParkingAidStatus.data.FrontDistance;

    CanParkingAidByte7_2010Struct field7{};
    field7.data.which_side_was_measured        = carState->ParkingAidStatus.data.WhichSideWasMeasured;
    field7.data.show_measured_space_on_display = carState->ParkingAidStatus.data.ShowMeasuredSpaceOnDisplay;
    field7.data.measured_free_space            = carState->ParkingAidStatus.data.MeasuredFreeSpace;
    field7.data.measurement_status             = carState->ParkingAidStatus.data.MeasurementStatus;

    message.data[0] = status.asByte;
    message.data[1] = soundOptions.asByte;
    message.data[2] = beep.asByte;
    message.data[3] = rearAndRearLeft.asByte;
    message.data[4] = frontLeftAndRearRight.asByte;
    message.data[5] = field6.asByte;
    message.data[6] = field7.asByte;

    message.isActive = true;

    return message;
}

void MessageHandler_0E1_2010::Parse(CarState* carState, const BusMessage& message)
{

}
