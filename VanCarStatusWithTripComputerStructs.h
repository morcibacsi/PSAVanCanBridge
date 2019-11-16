// VanCarStatusWithTripComputerStructs.h
#pragma once

#ifndef _VanCarStatusWithTripComputerStructs_h
    #define _VanCarStatusWithTripComputerStructs_h

// VANID: 564
const uint16_t VAN_ID_CARSTATUS = 0x564;

// Read right to left in documentation
typedef struct {
    unsigned short data : 16;
} VanTripDistanceStruct;

typedef struct {
    unsigned short data : 16;
} VanTripConsumption;

typedef struct {
    uint8_t Unknown0    : 1; // bit 0 
    uint8_t Unknown1    : 1; // bit 1
    uint8_t Unknown2    : 1; // bit 2
    uint8_t BootLid     : 1; // bit 3
    uint8_t RearLeft    : 1; // bit 4
    uint8_t RearRight   : 1; // bit 5
    uint8_t FrontLeft   : 1; // bit 6
    uint8_t FrontRight  : 1; // bit 7
} VanDoorStatus;

typedef struct {
    uint8_t TripButton  : 1; // bit 0 
    uint8_t Unknown1    : 1; // bit 1
    uint8_t Unknown2    : 1; // bit 2
    uint8_t Unknown3    : 1; // bit 3
    uint8_t Unknown4    : 1; // bit 4
    uint8_t Unknown5    : 1; // bit 5
    uint8_t Unknown6    : 1; // bit 6
    uint8_t Unknown7    : 1; // bit 7
} VanCarStatusWithTripComputerField10ByteStruct;

// Read left to right in documentation
typedef struct VanCarStatusWithTripComputerStruct {
    uint8_t Header;
    uint8_t Field1;
    uint8_t Field2;
    uint8_t Field3;
    uint8_t Field4;
    uint8_t Field5;
    uint8_t Field6;
    VanDoorStatus Doors;
    uint8_t Field8;
    uint8_t Field9;
    VanCarStatusWithTripComputerField10ByteStruct Field10;
    uint8_t Trip1Speed;
    uint8_t Trip2Speed;
    uint8_t Field13;
    VanTripDistanceStruct Trip1Distance;
    //Should be divided by 10
    VanTripConsumption Trip1FuelConsumption;
    VanTripDistanceStruct Trip2Distance;
    //Should be divided by 10
    VanTripConsumption Trip2FuelConsumption;
    VanTripConsumption FuelConsumption;
    VanTripDistanceStruct FuelLeftToPumpInKm;
    uint8_t Footer;
};

typedef union VanCarStatusWithTripComputerPacket {
    VanCarStatusWithTripComputerStruct data;
    uint8_t VanCarStatusWithTripComputerPacket[sizeof(VanCarStatusWithTripComputerStruct)];
};

#pragma region Sender class
class VanCarStatusPacketSender
{
    AbstractVanMessageSender* vanMessageSender;

    public:
    VanCarStatusPacketSender(AbstractVanMessageSender* object)
    {
        vanMessageSender = object;
    }

    void GetCarStatus(uint8_t channelId)
    {
        vanMessageSender->set_channel_for_reply_request_message(channelId, VAN_ID_CARSTATUS, 29, 1);
    }
};
#pragma endregion


#endif
