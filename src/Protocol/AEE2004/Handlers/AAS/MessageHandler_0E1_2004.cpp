#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_0E1_2004.h"
#include "MessageHandler_0E1_2004.hpp"

BusMessage MessageHandler_0E1::Generate(CarState* carState)
{
    CanParkingAidByte1Struct status{};
    status.data.rear_status = carState->ParkingAidStatus.data.RearStatus;

    CanParkingAidByte2Struct soundOptions{};
    soundOptions.data.sound_enabled = carState->ParkingAidStatus.data.SoundEnabled;
    soundOptions.data.location      = carState->ParkingAidStatus.data.Location;
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
    field6.data.show                 = carState->ParkingAidStatus.data.RearStatus == (uint8_t)ParkingAidStatus::Active;
    field6.data.front_right_distance = carState->ParkingAidStatus.data.RearRightDistance;
    field6.data.front_distance       = carState->ParkingAidStatus.data.FrontDistance;

    message.data[0] = status.asByte;
    message.data[1] = soundOptions.asByte;
    message.data[2] = beep.asByte;
    message.data[3] = rearAndRearLeft.asByte;
    message.data[4] = frontLeftAndRearRight.asByte;
    message.data[5] = field6.asByte;
    message.data[6] = 0x00;

    return message;
}

void MessageHandler_0E1::Parse(CarState* carState, const BusMessage& message)
{

}
