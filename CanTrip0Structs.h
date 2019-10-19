// CanTrip0Structs.h
#pragma once

#ifndef _CanTrip0Structs_h
    #define _CanTrip0Structs_h

// CANID: 221
const uint16_t CAN_ID_TRIP0 = 0x221;

// Read right to left in documentation
typedef struct {
    uint8_t unused_button             : 1; // bit 0
    uint8_t bit1                      : 1; // bit 1
    uint8_t bit2                      : 1; // bit 2
    uint8_t trip_switch_pressed       : 1; // bit 3
    uint8_t bit4                      : 1; // bit 4
    uint8_t bit5                      : 1; // bit 5
    uint8_t rest_of_run_is_null       : 1; // bit 6
    uint8_t liters_per_100_km_is_null : 1; // bit 7
} CanTripByte1Struct;

typedef struct {
    unsigned int data : 16;
} CanTripLitersPer100kmStruct;

typedef struct {
    unsigned int data : 16;
} CanRestOfRunOnCurrentFuelLevel;

typedef struct {
    unsigned int data : 16;
} CanRestOfRunToFinish;


/*
usage:
IntValueOnTwoBytes.value = 65535;           //Assign a value to the Int var of the Union
HighByte = IntValueOnTwoBytes.Bytes[1];     //Get the High Byte (255)
LowByte  = IntValueOnTwoBytes.Bytes[0];     //Get the Low Byte (255)
*/
union u_type
{
    unsigned int value;
    unsigned char bytes[2];
}
IntValueOnTwoBytes;

//typedef struct {
//    uint8_t bit0 : 1; // bit 0
//    uint8_t bit1 : 1; // bit 1
//    uint8_t bit2 : 1; // bit 2
//    uint8_t bit3 : 1; // bit 3
//    uint8_t bit4 : 1; // bit 4
//    uint8_t bit5 : 1; // bit 5
//    uint8_t bit6 : 1; // bit 6
//    uint8_t economy_mode : 1; // bit 7
//} CanDash1Byte2Struct;


// Read left to right in documentation
typedef struct CanTrip0Struct {
    CanTripByte1Struct Information;
    uint8_t LitersPer100KmByte1;
    uint8_t LitersPer100KmByte2;
    uint8_t KmToGasStationByte1;
    uint8_t KmToGasStationByte2;
    uint8_t KmToFinishByte1;
    uint8_t KmToFinishByte2;
};

typedef union CanTrip0Packet {
    CanTrip0Struct data;
    uint8_t CanTrip0Packet[sizeof(CanTrip0Struct)];
};

#pragma region Sender class
class CanTrip0PacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanTrip0PacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendTripInfo(int kmToGasStation, int lper100km, int kmtoFinish, uint8_t button)
    {
        PacketGenerator<CanTrip0Packet> generator;

        generator.packet.data.Information.trip_switch_pressed = button;

        IntValueOnTwoBytes.value = kmToGasStation;
        generator.packet.data.KmToGasStationByte1 = IntValueOnTwoBytes.bytes[1];
        generator.packet.data.KmToGasStationByte2 = IntValueOnTwoBytes.bytes[0];

        IntValueOnTwoBytes.value = lper100km;
        generator.packet.data.LitersPer100KmByte1 = IntValueOnTwoBytes.bytes[1];
        generator.packet.data.LitersPer100KmByte2 = IntValueOnTwoBytes.bytes[0];

        IntValueOnTwoBytes.value = kmtoFinish * 10;
        generator.packet.data.KmToFinishByte1 = IntValueOnTwoBytes.bytes[1];
        generator.packet.data.KmToFinishByte2 = IntValueOnTwoBytes.bytes[0];

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_TRIP0, 0, sizeof(CanTrip0Packet), serializedPacket);
    }

};
#pragma endregion

#endif
