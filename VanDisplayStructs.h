// VanDisplayStructs.h
#pragma once

#ifndef _VanDisplayStructs_h
    #define _VanDisplayStructs_h

#include "AbstractVanMessageSender.h"

#pragma region Message definitions
const int VAN_POPUP_MSG_TYRES_DEFLATED                         = 0X00;
const int VAN_POPUP_MSG_DOOR_OPEN                              = 0X01;
const int VAN_POPUP_MSG_GEARBOX_OIL_TEMP_TOO_HIGH              = 0X02;
const int VAN_POPUP_MSG_BRAKE_FLUID_LEVEL_LOW                  = 0X03;
const int VAN_POPUP_MSG_HYDRAULIC_SUSPENSION_PRESSURE          = 0X04;
const int VAN_POPUP_MSG_SERIOUS_SUSPENSION_FAULT               = 0X05;
const int VAN_POPUP_MSG_ENGINE_OIL_TEMPERATURE_TOO_HIGH        = 0X06;
const int VAN_POPUP_MSG_ENGINE_COOLANT_TEMP_TOO_HIGH           = 0X07;
const int VAN_POPUP_MSG_UNBLOCK_DIESEL_FILTER                  = 0X08;
const int VAN_POPUP_MSG_AUTO_ICON_WITH_STOP                    = 0x09;
const int VAN_POPUP_MSG_DIESEL_ADDITIVE_MINIMUM_LEVEL          = 0X0A;
const int VAN_POPUP_MSG_FUEL_TANK_ACCESS_OPEN                  = 0X0B;
const int VAN_POPUP_MSG_TYRES_PUNCTURED                        = 0X0C;
const int VAN_POPUP_MSG_TOP_UP_ENGINE_COOLANT_LEVEL            = 0X0D;
const int VAN_POPUP_MSG_OIL_PRESSURE_INSUFFICIENT              = 0X0E;
const int VAN_POPUP_MSG_TOP_UP_ENGINE_OIL_LEVEL                = 0X0F;
const int VAN_POPUP_MSG_ANTIPOLLUTION_FAULT                    = 0x10;
const int VAN_POPUP_MSG_BRAKE_PADS_WORN                        = 0x11;
const int VAN_POPUP_MSG_CHECK_CONTROL_OK                       = 0x12;
const int VAN_POPUP_MSG_AUTOMATIC_GEAR_FAULT                   = 0x13;
const int VAN_POPUP_MSG_ESP_ASR_NOT_FUNCTIONING                = 0x14;
const int VAN_POPUP_MSG_ABS_FAULT                              = 0x15;
const int VAN_POPUP_MSG_SUSPENSION_OR_STEERING_FAULT           = 0x16;
const int VAN_POPUP_MSG_BRAKING_FAULT                          = 0x17;
const int VAN_POPUP_MSG_AIRBAG_FAULT                           = 0x18;
const int VAN_POPUP_MSG_AIRBAG_FAULT2                          = 0x19;
const int VAN_POPUP_MSG_CRUISE_CONTROL_FAULT                   = 0x1A;
const int VAN_POPUP_MSG_ENGINE_COOLANT_TEMPERATURE_HIGH        = 0x1B;

const int VAN_POPUP_MSG_AUTO_LIGHTING_FAULT                    = 0x1D;
const int VAN_POPUP_MSG_AUTO_WIPER_FAULT                       = 0x1E;
const int VAN_POPUP_MSG_WATER_IN_DIESEL_FUEL_FILTER            = 0x1F;
const int VAN_POPUP_MSG_LEFT_SLIDING_DOOR_FAULT                = 0x20;
const int VAN_POPUP_MSG_HEADLIGHT_CORRECTOR_FAULT              = 0x21;
const int VAN_POPUP_MSG_RIGHT_SLIDING_DOOR_FAULT               = 0x22;

const int VAN_POPUP_MSG_SECONDARY_BATTERY_CHARGE_FAULT         = 0x24;
const int VAN_POPUP_MSG_BATTERY_CHARGE_FAULT                   = 0x25;
const int VAN_POPUP_MSG_DIESEL_FUEL_ADDITIVE_FAULT             = 0x26;
const int VAN_POPUP_MSG_CATALYTIC_CONVERTER_FAULT              = 0X27;
const int VAN_POPUP_MSG_HANDBRAKE_ON                           = 0X28;
const int VAN_POPUP_MSG_SEAT_BELT_REMINDER                     = 0X29;
const int VAN_POPUP_MSG_PASSENGER_AIRBAG_DEACTIVATED           = 0X2A;
const int VAN_POPUP_MSG_TOP_UP_WASHER_FLUID                    = 0X2B;
const int VAN_POPUP_MSG_SPEED_TOO_HIGH                         = 0X2C;
const int VAN_POPUP_MSG_KEY_REMINDER                           = 0X2D;
const int VAN_POPUP_MSG_SIDE_LIGHTS_REMINDER                   = 0X2E;
const int VAN_POPUP_MSG_HILL_HOLDER_ACTIVE                     = 0X2F;
const int VAN_POPUP_MSG_IMPACT_SENSOR_FAULT                    = 0X30;

const int VAN_POPUP_MSG_WHEEL_PRESSURE_SENSOR_BATTERY_LOW      = 0X32;
const int VAN_POPUP_MSG_REMOTE_CONTROL_BATTERY_LOW             = 0X33;

const int VAN_POPUP_MSG_PUT_AUTO_TRANSMISS_LEVER_IN_POSITION_P = 0X35;
const int VAN_POPUP_MSG_TEST_STOP_LIGHTS_BRAKE_GENTLY          = 0X36;
const int VAN_POPUP_MSG_FUEL_LEVEL_LOW                         = 0X37;
const int VAN_POPUP_MSG_AUTOMATIC_LIGHTING_INACTIVE            = 0X38;
const int VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_FAULT            = 0X39;
const int VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_DEACTIVATED      = 0X3A;
const int VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_ACTIVATED        = 0X3B;

const int VAN_POPUP_MSG_5_TYRE_PRESSURE_SENSORS_MISSING        = 0X3D;
const int VAN_POPUP_MSG_DOORS_LOCKED                           = 0X40;
const int VAN_POPUP_MSG_ESP_ASR_DEACTIVATED                    = 0X41;
const int VAN_POPUP_MSG_CHILD_SAFETY_ACTIVE                    = 0X42;
const int VAN_POPUP_MSG_DEADLOCKING_ACTIVE                     = 0X43;
const int VAN_POPUP_MSG_AUTOMATIC_LIGHTING_ACTIVE              = 0X44;
const int VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE                = 0X45;
const int VAN_POPUP_MSG_ENGINE_IMMOBILISER_FAULT               = 0X46;
const int VAN_POPUP_MSG_SPORTS_SUSPENSION_ACTIVE               = 0X47;

const int VAN_POPUP_MSG_LPG_COMBUSTION_REFUSED                 = 0X54;
const int VAN_POPUP_MSG_LPG_DEFAULT                            = 0X55;

const int VAN_POPUP_MSG_STOP_AND_START_SYSTEM_ACTIVE           = 0X5B;
const int VAN_POPUP_MSG_STOP_AND_START_SYSTEM_DEACTIVATED      = 0X5C;

const int VAN_POPUP_MSG_XSARA_DYNALTO                          = 0X5E;
const int VAN_POPUP_MSG_307_DYNALTO                            = 0X5F;

const int VAN_POPUP_MSG_PLACE_LEVER_IN_NEUTRAL                 = 0X67;
const int VAN_POPUP_MSG_DISPLAYS_MESSAGES_AUTOMATICALLY        = 0X121;

#pragma endregion

//VANID: 524
const unsigned long VAN_ID_DISPLAY_POPUP = 0x524;

//Read right to left in documentation
typedef struct {
    uint8_t             : 1; // bit 0
    uint8_t             : 1; // bit 1
    uint8_t             : 1; // bit 2
    uint8_t             : 1; // bit 3
    uint8_t             : 1; // bit 4
    uint8_t             : 1; // bit 5
    uint8_t             : 1; // bit 6
    uint8_t water_temp_max : 1; // bit 7
} VanDisplayMsg0Struct;

typedef struct {
    uint8_t              : 1; // bit 0
    uint8_t              : 1; // bit 1
    uint8_t              : 1; // bit 2
    uint8_t              : 1; // bit 3
    uint8_t              : 1; // bit 4
    uint8_t              : 1; // bit 5
    uint8_t oil_light    : 1; // bit 6
    uint8_t              : 1; // bit 7
} VanDisplayMsgByte1Struct;

typedef struct {
    uint8_t mil             : 1; // bit 0
    uint8_t                 : 1; // bit 1
    uint8_t                 : 1; // bit 2
    uint8_t                 : 1; // bit 3
    uint8_t esp             : 1; // bit 4
    uint8_t abs             : 1; // bit 5
    uint8_t                 : 1; // bit 6
    uint8_t                 : 1; // bit 7
} VanDisplayMsgByte2Struct;

typedef struct {
    uint8_t airbag            : 1; // bit 0
    uint8_t                   : 1; // bit 1
    uint8_t                   : 1; // bit 2
    uint8_t                   : 1; // bit 3
    uint8_t                   : 1; // bit 4
    uint8_t                   : 1; // bit 5
    uint8_t                   : 1; // bit 6
    uint8_t                   : 1; // bit 7
} VanDisplayMsgByte3Struct;

typedef struct {
    uint8_t                   : 1; // bit 0
    uint8_t                   : 1; // bit 1
    uint8_t                   : 1; // bit 2
    uint8_t                   : 1; // bit 3
    uint8_t                   : 1; // bit 4
    uint8_t battery_fault     : 1; // bit 5
    uint8_t                   : 1; // bit 6
    uint8_t                   : 1; // bit 7
} VanDisplayMsgByte4Struct;

typedef struct {
    uint8_t handbrake                    : 1; // bit 0
    uint8_t seatbelt_warning             : 1; // bit 1
    uint8_t passenger_airbag_deactivated : 1; // bit 2
    uint8_t                              : 1; // bit 3
    uint8_t                              : 1; // bit 4
    uint8_t                              : 1; // bit 5
    uint8_t                              : 1; // bit 6
    uint8_t                              : 1; // bit 7
} VanDisplayMsgByte5Struct;

typedef struct {
    uint8_t                   : 1; // bit 0
    uint8_t seatbelt_warning_ : 1; // bit 1 not sure if it is seatbelt warning
    uint8_t                   : 1; // bit 2
    uint8_t                   : 1; // bit 3
    uint8_t left_stick_button : 1; // bit 4
    uint8_t                   : 1; // bit 5
    uint8_t                   : 1; // bit 6
    uint8_t fuel_low_light    : 1; // bit 7
} VanDisplayMsgByte6Struct;

typedef struct {
    uint8_t car_locked          : 1; // bit 0
    uint8_t esp_asr_deactivated : 1; // bit 1
    uint8_t                     : 1; // bit 2
    uint8_t                     : 1; // bit 3
    uint8_t                     : 1; // bit 4
    uint8_t                     : 1; // bit 5
    uint8_t                     : 1; // bit 6
    uint8_t                     : 1; // bit 7
} VanDisplayMsgByte8Struct;

typedef struct {
    uint8_t                     : 1; // bit 0
    uint8_t                     : 1; // bit 1
    uint8_t                     : 1; // bit 2
    uint8_t                     : 1; // bit 3
    uint8_t                     : 1; // bit 4
    uint8_t                     : 1; // bit 5
    uint8_t                     : 1; // bit 6
    uint8_t                     : 1; // bit 7
} VanDisplayMsgByte9Struct;


//Read left to right in documentation
typedef struct VanDisplayStruct {
    uint8_t Field0;
    VanDisplayMsgByte1Struct Field1;
    VanDisplayMsgByte2Struct Field2;
    VanDisplayMsgByte3Struct Field3;
    uint8_t Field4;
    VanDisplayMsgByte5Struct Field5;
    VanDisplayMsgByte6Struct Field6;
    uint8_t Field7;
    uint8_t Field8;
    uint8_t Message;
    uint8_t Field10;
    uint8_t Field11;
    uint8_t Field12;
    uint8_t Field13;
};

typedef union VanDisplayPacket {
    VanDisplayStruct data;
    uint8_t VanDisplayPacket[sizeof(VanDisplayStruct)];
};

#pragma region Sender class
class VanDisplayPacketSender
{
    AbstractVanMessageSender * vanMessageSender;

public:
    VanDisplayPacketSender(AbstractVanMessageSender * object)
    {
        vanMessageSender = object;
    }

    void Send(uint8_t channelId, uint8_t messageId)
    {
        VanDisplayPacket packet;
        memset(&packet, 0, sizeof(packet));//fill everything with 0 - https://stackoverflow.com/a/6891737/5453350

        //display messages are best to test with everything set to FF, because that way every message type apppears
        //initial testing value was: 0x00, 0x02, 0x01, 0x00, 0xFF, 0x01, 0x00, 0x01, 0xFF, 0x44, 0x00, 0x00, 0x00, 0x00

        packet.data.Field0 = 0xFF;
        //packet.data.Field1 = 0xFF;
        //packet.data.Field2 = 0xFF;
        //packet.data.Field3 = 0xFF;
        packet.data.Field4 = 0xFF;
        //packet.data.Field5 = 0xFF;
        //packet.data.Field6 = 0xFF;
        packet.data.Field7 = 0xFF;
        packet.data.Field8 = 0xFF;
        packet.data.Message = messageId;
        packet.data.Field10 = 0xFF;
        packet.data.Field11 = 0xFF;
        packet.data.Field12 = 0xFF;
        packet.data.Field13 = 0xFF;

        unsigned char *serializedPacket = Serialize<VanDisplayPacket>(packet);
        vanMessageSender->set_channel_for_transmit_message(channelId, 0x52, 4, serializedPacket, sizeof(packet), 0);
        memset(&packet, 0, 0);
        delete[] serializedPacket;
        vanMessageSender->disable_channel(channelId);
    }
};
#pragma endregion

#endif
