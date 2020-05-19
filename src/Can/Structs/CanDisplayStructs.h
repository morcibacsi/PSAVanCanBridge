// CanDisplayStructs.h
#pragma once

#ifndef _CanDisplayStructs_h
    #define _CanDisplayStructs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

#pragma region Popup message consts
//0x80
const uint8_t CAN_POPUP_MSG_DIAGNOSIS_OK                                             = 0X00;
const uint8_t CAN_POPUP_MSG_ENGINE_TEMPERATURE_FAULT_STOP_THE_VEHICLE                = 0X01;
const uint8_t CAN_POPUP_MSG_TOP_UP_COOLANT_LEVEL                                     = 0X03;
const uint8_t CAN_POPUP_MSG_TOP_UP_ENGINE_OIL_LEVEL                                  = 0X04;
const uint8_t CAN_POPUP_MSG_ENGINE_OIL_PRESSURE_FAULT_STOP_THE_VEHICLE               = 0X05;
const uint8_t CAN_POPUP_MSG_BRAKING_SYSTEM_FAULTY                                    = 0X08;
const uint8_t CAN_POPUP_MSG_REQUEST_NOT_GRANTED                                      = 0X0A;
const uint8_t CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN         = 0X0B;
const uint8_t CAN_POPUP_MSG_MORE_THAN_ONE_TYRE_PUNCTURED                             = 0X0D;
const uint8_t CAN_POPUP_MSG_RISK_OF_PARTICLE_FILTER_CLOGGING_SEE_HANDBOOK            = 0X0F;
const uint8_t CAN_POPUP_MSG_SUSPENSION_FAULTY_MAX_SPEED_90_KMH                       = 0X11;
const uint8_t CAN_POPUP_MSG_SUSPENSION_FAULTY                                        = 0X12;
const uint8_t CAN_POPUP_MSG_POWER_STEERING_FAULTY                                    = 0X13;
const uint8_t CAN_POPUP_MSG_SUSPENSION_10_KMH                                        = 0X14;
const uint8_t CAN_POPUP_MSG_PARKING_BRAKE_APPLIED                                    = 0X61;
const uint8_t CAN_POPUP_MSG_PARKING_BRAKE_RELEASED                                   = 0X62;
const uint8_t CAN_POPUP_MSG_PARKING_BRAKE_CONTROL_FAULTY_AUTO_PARKING_BRAKE_ACTIVATED= 0X64;
const uint8_t CAN_POPUP_MSG_BRAKE_PADS_WORN                                          = 0X67;
const uint8_t CAN_POPUP_MSG_PARKING_BRAKE_FAULTY                                     = 0X68;
const uint8_t CAN_POPUP_MSG_ADJUSTABLE_WING_FAULT_SPEED_RESTRICTED_SEE_HANDBOOK      = 0X69;
const uint8_t CAN_POPUP_MSG_ABS_BRAKING_SYSTEM_FAULTY                                = 0X6A;
const uint8_t CAN_POPUP_MSG_ESP_ASR_SYSTEM_FAULTY                                    = 0X6B;
const uint8_t CAN_POPUP_MSG_SUSPENSION_FAULTY_2                                      = 0X6C;
const uint8_t CAN_POPUP_MSG_POWER_STEERING_FAULTY_2                                  = 0X6D;
const uint8_t CAN_POPUP_MSG_GEARBOX_FAULT_REPAIR_NEEDED                              = 0X6E;
const uint8_t CAN_POPUP_MSG_CRUISE_CONTROL_SYSTEM_FAULTY                             = 0X6F;
const uint8_t CAN_POPUP_MSG_AMBIENT_BRIGHTNESS_SENSOR_FAULTY                         = 0X73;
const uint8_t CAN_POPUP_MSG_SIDELAMP_BULBS_FAULTY                                    = 0X74;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_HEADLAMP_ADJUSTMENT_FAULTY                     = 0X75;
const uint8_t CAN_POPUP_MSG_DIRECTIONAL_HEADLAMPS_FAULTY                             = 0X76;
const uint8_t CAN_POPUP_MSG_AIRBAGS_OR_PRETENSIONER_SEAT_BELTS_FAULTY                = 0X78;//
const uint8_t CAN_POPUP_MSG_GEARBOX_FAULT_REPAIR_NEEDED_2                            = 0X7A;
const uint8_t CAN_POPUP_MSG_APPLY_FOOT_ON_BRAKE_AND_LEVER_IN_POSITION_N              = 0X7B;
const uint8_t CAN_POPUP_MSG_PRESENCE_OF_WATER_IN_DIESEL_FILTER_REPAIR_NEEDED         = 0X7D;
const uint8_t CAN_POPUP_MSG_ENGINE_FAULT_REPAIR_NEEDED__DEPOLLUTION_SYSTEM_FAULTY    = 0X7E;
const uint8_t CAN_POPUP_MSG_ENGINE_FAULT_REPAIR_NEEDED__DEPOLLUTION_SYSTEM_FAULTY2   = 0X7F;
const uint8_t CAN_POPUP_MSG_PARTICLE_FILTER_ADDITIVE_LEVEL_TOO_LOW                   = 0X81;//??ENG_fault
const uint8_t CAN_POPUP_MSG_ELECTRONIC_ANTI_THEFT_FAULTY                             = 0X83;
const uint8_t CAN_POPUP_MSG_PARKING_ASSISTANCE_SYSTEM_FAULTY                         = 0X88;
const uint8_t CAN_POPUP_MSG_SPACE_MEASURING_SYSTEM_FAULTY                            = 0X89;
const uint8_t CAN_POPUP_MSG_BATTERY_CHARGE_OR_ELECTRICAL_SUPPLY_FAULTY               = 0X8A;
const uint8_t CAN_POPUP_MSG_TYRE_PRESSURES_TOO_LOW                                   = 0X8D;
const uint8_t CAN_POPUP_MSG_ANTI_WANDER_SYSTEM_LANE_CROSSING_WARNING_DEVICE_FAULTY   = 0X97;
const uint8_t CAN_POPUP_MSG_DIPPED_BEAM_BULBS_FAULTY                                 = 0X9A;
const uint8_t CAN_POPUP_MSG_MAIN_BEAM_BULBS_FAULTY                                   = 0X9B;
const uint8_t CAN_POPUP_MSG_BRAKE_LIGHT_BULBS_FAULTY                                 = 0X9C;
const uint8_t CAN_POPUP_MSG_FOGLAMP_BULBS_FAULTY                                     = 0X9D;
const uint8_t CAN_POPUP_MSG_DIRECTION_INDICATORS_FAULTY                              = 0X9E;
const uint8_t CAN_POPUP_MSG_REVERSING_LAMP_BULBS_FAULTY                              = 0X9F;
const uint8_t CAN_POPUP_MSG_SIDELAMP_BULBS_FAULTY_2                                  = 0XA0;
const uint8_t CAN_POPUP_MSG_TYRE_PRESSURES_REGISTERED                                = 0XB4;
const uint8_t CAN_POPUP_MSG_UNDER_INFLATION_MONITORING_FAULT                         = 0XB5;
const uint8_t CAN_POPUP_MSG_ADJUST_THE_TYRE_PRESSURES_THEN_REINITIALISE              = 0XB7;
const uint8_t CAN_POPUP_MSG_TOP_UP_EMISSIONS_ADDITIVE_STARTING_PREVENTED_IN_X_KM     = 0XBB;
const uint8_t CAN_POPUP_MSG_TOP_UP_EMISSIONS_ADDITIVE_STARTING_PREVENTED_IN_X_KM_2   = 0XBC;
const uint8_t CAN_POPUP_MSG_TOP_UP_EMISSIONS_ADDITIVE_STARTING_PREVENTED             = 0XBD;
const uint8_t CAN_POPUP_MSG_EMISSIONS_CONTROL_FAULT                                  = 0XBE;
const uint8_t CAN_POPUP_MSG_EMISSIONS_CONTROL_FAULT_STARTING_PREVENTED_IN_X_KM       = 0XBF;
const uint8_t CAN_POPUP_MSG_EMISSIONS_CONTROL_FAULT_STARTING_PREVENTED               = 0XC0;
const uint8_t CAN_POPUP_MSG_REINITIALISATION_IMPOSSIBLE_WHEN_DRIVING                 = 0XC2;
const uint8_t CAN_POPUP_MSG_REMOVE_IGNITION_KEY                                      = 0XCA;//ruforum
const uint8_t CAN_POPUP_MSG_CRUISE_CONTROL_NOT_POSSIBLE_SPEED_TOO_LOW                = 0XCD;
const uint8_t CAN_POPUP_MSG_CONTROL_ACTIVATION_NOT_POSSIBLE_ENTER_THE_SPEED          = 0XCE;
const uint8_t CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED                            = 0XD2;
const uint8_t CAN_POPUP_MSG_REAR_PASSENGER_SEAT_BELTS_FASTENED                       = 0XD3;
const uint8_t CAN_POPUP_MSG_PLACE_AUTOMATIC_GEARBOX_IN_POSITION_P                    = 0XD7;
const uint8_t CAN_POPUP_MSG_RISK_OF_ICE                                              = 0XD8;
const uint8_t CAN_POPUP_MSG_HANDBRAKE                                                = 0XD9;
const uint8_t CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN_2       = 0XDE;
const uint8_t CAN_POPUP_MSG_SCREEN_WASH_FLUID_LEVEL_TOO_LOW                          = 0XDF;
const uint8_t CAN_POPUP_MSG_FUEL_LEVEL_TOO_LOW                                       = 0XE0;
const uint8_t CAN_POPUP_MSG_FUEL_CIRCUIT_DEACTIVATED                                 = 0XE1;
const uint8_t CAN_POPUP_MSG_REMOTE_CONTROL_BATTERY_FLAT                              = 0XE3;
const uint8_t CAN_POPUP_MSG_TYRE_PRESSURES_NOT_MONITORED                             = 0XE5;
const uint8_t CAN_POPUP_MSG_HIGH_SPEED_CHECK_TYRE_PRESSURES_CORRECT                  = 0XE7;
const uint8_t CAN_POPUP_MSG_TYRE_PRESSURES_TOO_LOW_2                                 = 0XE8;
const uint8_t CAN_POPUP_MSG_DIAGNOSIS_IN_PROGRESS                                    = 0XF0;
const uint8_t CAN_POPUP_MSG_DIAGNOSIS_COMPLETED                                      = 0XF1;
const uint8_t CAN_POPUP_MSG_REAR_LEFT_HAND_PASSENGER_SEAT_BELT_UNFASTENED            = 0XF7;
const uint8_t CAN_POPUP_MSG_REAR_CENTRAL_PASSENGER_SEAT_BELT_UNFASTENED              = 0XF8;
const uint8_t CAN_POPUP_MSG_REAR_RIGHT_HAND_PASSENGER_SEAT_BELT_UNFASTENED           = 0XF9;
//0x81
const uint8_t CAN_POPUP_MSG_AUTOMATIC_SCREEN_WIPE_ACTIVATED                          = 0X2F;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_SCREEN_WIPE_DEACTIVATED                        = 0X30;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_HEADLAMP_LIGHTING_ACTIVATED                    = 0X31;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_HEADLAMP_LIGHTING_DEACTIVATED                  = 0X32;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_DOOR_LOCKING_ACTIVATED                         = 0X33;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_DOOR_LOCKING_DEACTIVATED                       = 0X34;
const uint8_t CAN_POPUP_MSG_CHILD_SAFETY_ACTIVATED                                   = 0X37;
const uint8_t CAN_POPUP_MSG_CHILD_SAFETY_DEACTIVATED                                 = 0X38;
const uint8_t CAN_POPUP_MSG_PARKING_NO                                               = 0X3D;
const uint8_t CAN_POPUP_MSG_STOP_START_SYSTEM_FAULT                                  = 0X98;
const uint8_t CAN_POPUP_MSG_PLACE_GEARBOX_IN_N_POSITION                              = 0X9A;
const uint8_t CAN_POPUP_MSG_TO_RELEASE_PARKING_BRAKE_PRESS_THE_BRAKE_PEDAL           = 0X9B;
const uint8_t CAN_POPUP_MSG_TO_PRESS_THE_PARK_BUTTON_AND_THE_BRAKE_PEDAL             = 0X9C;
const uint8_t CAN_POPUP_MSG_IMPOSSIBLE_TO_MOVE_ROOF_OUTSIDE_TEMPERATURE_TOO_LOW      = 0XF6;
const uint8_t CAN_POPUP_MSG_IMPOSSIBLE_TO_MOVE_ROOF_SPEED_TOO_HIGH                   = 0XF7;
const uint8_t CAN_POPUP_MSG_IMPOSSIBLE_TO_MOVE_ROOF_BOOT_OPEN                        = 0XF8;
const uint8_t CAN_POPUP_MSG_IMPOSSIBLE_TO_MOVE_ROOF_SCREEN_NOT_DEPLOYED              = 0XFA;
const uint8_t CAN_POPUP_MSG_ROOF_MANOEUVRE_COMPLETED                                 = 0XFB;
const uint8_t CAN_POPUP_MSG_COMPLETE_ROOF_MANOEUVRE_IMMEDIATELY                      = 0XFC;
const uint8_t CAN_POPUP_MSG_MANOEUVRE_IMPOSSIBLE_ROOF_LOCKED                         = 0XFD;
const uint8_t CAN_POPUP_MSG_FOLDING_ROOF_MECHANISM_FAULTY                            = 0XFE;
const uint8_t CAN_POPUP_MSG_OPERATION_IMPOSSIBLE_REAR_SCREEN_OPEN                    = 0XFF;
//0x82
const uint8_t CAN_POPUP_MSG_ROOF_MOVEMENT_NOT_POSSIBLE_CASETTE_OPEN                  = 0X00;
const uint8_t CAN_POPUP_MSG_ANTI_ROLLBACK_SYSTEM_FAULTY                              = 0X02;
const uint8_t CAN_POPUP_MSG_ENHANCED_TRACTION_NORMAL_MODE                            = 0X03;
const uint8_t CAN_POPUP_MSG_ENHANCED_TRACTION_MUD_MODE                               = 0X04;
const uint8_t CAN_POPUP_MSG_ENHANCED_TRACTION_SNOW_MODE                              = 0X05;
const uint8_t CAN_POPUP_MSG_ENHANCED_TRACTION_SAND_MODE                              = 0X06;
const uint8_t CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED                                   = 0X09;
const uint8_t CAN_POPUP_MSG_INTER_VEHICLES_TIME_MEASURING_IMPOSSIBLE_POOR_VISIBILITY = 0X0A;
const uint8_t CAN_POPUP_MSG_INTER_VEHICLES_TIME_MEASURING_IMPOSSIBLE_INIT_IN_PROGRESS= 0X0B;
const uint8_t CAN_POPUP_MSG_ECO_ACTIVATED                                            = 0X20;
const uint8_t CAN_POPUP_MSG_ECO_DEACTIVATED                                          = 0X21;
const uint8_t CAN_POPUP_MSG_PRESS_THE_CLUTCH_FULLY                                   = 0X22;
const uint8_t CAN_POPUP_MSG_ENGINE_FAULT_STOP_THE_VEHICLE                            = 0X2E;
#pragma endregion

// CANID: 0x1A1
const uint16_t CAN_ID_DISPLAY_POPUP = 0x1A1;

const uint8_t CAN_POPUP_MSG_SHOW_CATEGORY1  = 0x80;
const uint8_t CAN_POPUP_MSG_SHOW_CATEGORY2  = 0x81;
const uint8_t CAN_POPUP_MSG_SHOW_CATEGORY3  = 0x82;
const uint8_t CAN_POPUP_MSG_HIDE  = 0x7F;
const uint8_t CAN_POPUP_MSG_CLEAR = 0xFF;

const uint8_t CAN_POPUP_SEAT_BELTS_OF_DRIVER = 0x08;

// Read right to left in documentation
typedef struct {
    uint8_t             : 1; // bit 0
    uint8_t             : 1; // bit 1
    uint8_t             : 1; // bit 2
    uint8_t             : 1; // bit 3
    uint8_t             : 1; // bit 4
    uint8_t             : 1; // bit 5
    uint8_t             : 1; // bit 6
    uint8_t show_popup  : 1; // bit 7
} CanDisplayByte2Struct;

typedef struct {
    uint8_t                     : 1; // bit 0
    uint8_t                     : 1; // bit 1
    uint8_t is_bonnet_open      : 1; // bit 2
    uint8_t is_boot_open        : 1; // bit 3
    uint8_t is_rear_left_open   : 1; // bit 4
    uint8_t is_rear_right_open  : 1; // bit 5
    uint8_t is_front_left_open  : 1; // bit 6
    uint8_t is_front_right_open : 1; // bit 7
} CanDisplayDoorStatus1Struct;

typedef union {
    CanDisplayDoorStatus1Struct status;
    uint8_t asByte;
} CanDisplayDoorStatus1;

typedef struct {
    uint8_t                     : 1; // bit 0
    uint8_t                     : 1; // bit 1
    uint8_t                     : 1; // bit 2
    uint8_t                     : 1; // bit 3
    uint8_t                     : 1; // bit 4
    uint8_t                     : 1; // bit 5
    uint8_t is_fuel_flap_open   : 1; // bit 6
    uint8_t is_rear_screen_open : 1; // bit 7
} CanDisplayDoorStatus2Struct;

typedef union {
    CanDisplayDoorStatus2Struct status;
    uint8_t asByte;
} CanDisplayDoorStatus2;

// Read left to right in documentation
typedef struct CanDisplayStruct {
    uint8_t ShowPopup;
    uint8_t PopupMessageType;
    CanDisplayByte2Struct Field2;
    CanDisplayDoorStatus1 DoorStatus1;
    CanDisplayDoorStatus2 DoorStatus2;
    uint8_t Field5;
    uint8_t KmDividedBy256;
    uint8_t KmRemainderUpTo255;
};

typedef union CanDisplayPacket {
    CanDisplayStruct data;
    uint8_t CanDisplayPacket[sizeof(CanDisplayStruct)];
};

int static CanGetHighKmToDisplay(int kmToDisplay) {
    return kmToDisplay / 256;
}

int static CanGetLowKmToDisplay(int kmToDisplay){
    return kmToDisplay - (CanGetHighKmToDisplay(kmToDisplay) * 256);
}

#pragma region Sender class
class CanDisplayPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanDisplayPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void ShowPopup(uint8_t category, uint8_t messageType, int kmToDisplay, uint8_t doorStatus1, uint8_t doorStatus2)
    {
        int bytes7 = CanGetHighKmToDisplay(kmToDisplay);
        int bytes8 = CanGetLowKmToDisplay(kmToDisplay);

        PacketGenerator<CanDisplayPacket> generator;
        generator.packet.data.ShowPopup = category;
        generator.packet.data.PopupMessageType = messageType;
        generator.packet.data.Field2.show_popup = 1;

        generator.packet.data.DoorStatus1.asByte = doorStatus1;
        generator.packet.data.DoorStatus2.asByte = doorStatus2;
        //generator.packet.data.DoorStatus1.is_bonnet_open = doorStatus1.is_bonnet_open;
        //generator.packet.data.DoorStatus1.is_front_right_open = doorStatus1.is_front_right_open;
        //generator.packet.data.DoorStatus1.is_front_left_open = doorStatus1.is_front_left_open;
        //generator.packet.data.DoorStatus1.is_rear_left_open = doorStatus1.is_rear_left_open;
        //generator.packet.data.DoorStatus1.is_rear_right_open = doorStatus1.is_rear_right_open;
        //generator.packet.data.DoorStatus1.is_boot_open = doorStatus1.is_boot_open;

        //generator.packet.data.DoorStatus2.is_fuel_flap_open = doorStatus2.is_fuel_flap_open;

        generator.packet.data.KmDividedBy256 = bytes7;
        generator.packet.data.KmRemainderUpTo255 = bytes8;
        unsigned char *serializedPacket = generator.GetSerializedPacket();
        if (messageType == CAN_POPUP_MSG_AIRBAGS_OR_PRETENSIONER_SEAT_BELTS_FAULTY)
        {
            serializedPacket[3] = 0x80;
        }
        //serializedPacket[3] = 0xff;
        //serializedPacket[4] = 0xff;
        //serializedPacket[5] = 0xff;
        //serializedPacket[6] = 0xff;
        //serializedPacket[7] = 0xff;

        canMessageSender->SendMessage(CAN_ID_DISPLAY_POPUP, 0, sizeof(CanDisplayPacket), serializedPacket);
    }

    void HidePopup(uint8_t messageType)
    {
        PacketGenerator<CanDisplayPacket> generator;
        generator.packet.data.ShowPopup = CAN_POPUP_MSG_HIDE;
        generator.packet.data.PopupMessageType = messageType;
        generator.packet.data.Field2.show_popup = 0;
        generator.packet.data.KmDividedBy256 = 0;
        generator.packet.data.KmRemainderUpTo255 = 0;
        unsigned char *serializedPacket = generator.GetSerializedPacket();
        serializedPacket[3] = 0xff;
        serializedPacket[4] = 0xff;
        serializedPacket[5] = 0xff;
        serializedPacket[6] = 0xff;
        serializedPacket[7] = 0xff;
        canMessageSender->SendMessage(CAN_ID_DISPLAY_POPUP, 0, sizeof(CanDisplayPacket), serializedPacket);
    }

};
#pragma endregion

#endif
