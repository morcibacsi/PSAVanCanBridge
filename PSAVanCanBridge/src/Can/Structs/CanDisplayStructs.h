// CanDisplayStructs.h
#pragma once

#ifndef _CanDisplayStructs_h
    #define _CanDisplayStructs_h

#include "../ICanMessageSender.h"

#pragma region Popup message consts
//0x80
const uint8_t CAN_POPUP_MSG_DIAGNOSIS_OK                                             = 0x00;
const uint8_t CAN_POPUP_MSG_ENGINE_TEMPERATURE_FAULT_STOP_THE_VEHICLE                = 0x01;
const uint8_t CAN_POPUP_MSG_TOP_UP_COOLANT_LEVEL                                     = 0x03;
const uint8_t CAN_POPUP_MSG_TOP_UP_ENGINE_OIL_LEVEL                                  = 0x04;
const uint8_t CAN_POPUP_MSG_ENGINE_OIL_PRESSURE_FAULT_STOP_THE_VEHICLE               = 0x05;
const uint8_t CAN_POPUP_MSG_BRAKING_SYSTEM_FAULTY                                    = 0x08;
const uint8_t CAN_POPUP_MSG_REQUEST_NOT_GRANTED                                      = 0x0A;
const uint8_t CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN         = 0x0B;
const uint8_t CAN_POPUP_MSG_MORE_THAN_ONE_TYRE_PUNCTURED                             = 0x0D;
const uint8_t CAN_POPUP_MSG_RISK_OF_PARTICLE_FILTER_CLOGGING_SEE_HANDBOOK            = 0x0F;
const uint8_t CAN_POPUP_MSG_SUSPENSION_FAULTY_MAX_SPEED_90_KMH                       = 0x11;
const uint8_t CAN_POPUP_MSG_SUSPENSION_FAULTY                                        = 0x12;
const uint8_t CAN_POPUP_MSG_POWER_STEERING_FAULTY                                    = 0x13;
const uint8_t CAN_POPUP_MSG_SUSPENSION_10_KMH                                        = 0x14;
const uint8_t CAN_POPUP_MSG_UP_IN_PROGRESS                                           = 0x16;
const uint8_t CAN_POPUP_MSG_PARKING_BRAKE_APPLIED                                    = 0x61;
const uint8_t CAN_POPUP_MSG_PARKING_BRAKE_RELEASED                                   = 0x62;
const uint8_t CAN_POPUP_MSG_PARKING_BRAKE_CONTROL_FAULTY_AUTO_PARKING_BRAKE_ACTIVATED= 0x64;
const uint8_t CAN_POPUP_MSG_BRAKE_PADS_WORN                                          = 0x67;
const uint8_t CAN_POPUP_MSG_PARKING_BRAKE_FAULTY                                     = 0x68;
const uint8_t CAN_POPUP_MSG_ADJUSTABLE_WING_FAULT_SPEED_RESTRICTED_SEE_HANDBOOK      = 0x69;
const uint8_t CAN_POPUP_MSG_ABS_BRAKING_SYSTEM_FAULTY                                = 0x6A;
const uint8_t CAN_POPUP_MSG_ESP_ASR_SYSTEM_FAULTY                                    = 0x6B;
const uint8_t CAN_POPUP_MSG_SUSPENSION_FAULTY_2                                      = 0x6C;
const uint8_t CAN_POPUP_MSG_POWER_STEERING_FAULTY_2                                  = 0x6D;
const uint8_t CAN_POPUP_MSG_GEARBOX_FAULT_REPAIR_NEEDED                              = 0x6E;
const uint8_t CAN_POPUP_MSG_CRUISE_CONTROL_SYSTEM_FAULTY                             = 0x6F;
const uint8_t CAN_POPUP_MSG_AMBIENT_BRIGHTNESS_SENSOR_FAULTY                         = 0x73;
const uint8_t CAN_POPUP_MSG_SIDELAMP_BULBS_FAULTY                                    = 0x74;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_HEADLAMP_ADJUSTMENT_FAULTY                     = 0x75;
const uint8_t CAN_POPUP_MSG_DIRECTIONAL_HEADLAMPS_FAULTY                             = 0x76;
const uint8_t CAN_POPUP_MSG_AIRBAGS_OR_PRETENSIONER_SEAT_BELTS_FAULTY                = 0x78;//
const uint8_t CAN_POPUP_MSG_ACTIVE_BONNET_FAULTY                                     = 0x79;//
const uint8_t CAN_POPUP_MSG_GEARBOX_FAULT_REPAIR_NEEDED_2                            = 0x7A;
const uint8_t CAN_POPUP_MSG_APPLY_FOOT_ON_BRAKE_AND_LEVER_IN_POSITION_N              = 0x7B;
const uint8_t CAN_POPUP_MSG_PRESENCE_OF_WATER_IN_DIESEL_FILTER_REPAIR_NEEDED         = 0x7D;
const uint8_t CAN_POPUP_MSG_ENGINE_FAULT_REPAIR_NEEDED__DEPOLLUTION_SYSTEM_FAULTY    = 0x7E;
const uint8_t CAN_POPUP_MSG_ENGINE_FAULT_REPAIR_NEEDED__DEPOLLUTION_SYSTEM_FAULTY2   = 0x7F;
const uint8_t CAN_POPUP_MSG_PARTICLE_FILTER_ADDITIVE_LEVEL_TOO_LOW                   = 0x81;//??ENG_fault
const uint8_t CAN_POPUP_MSG_ELECTRONIC_ANTI_THEFT_FAULTY                             = 0x83;
const uint8_t CAN_POPUP_MSG_RIGHT_REAR_SLIDING_DOOR_FAULTY                           = 0x86;
const uint8_t CAN_POPUP_MSG_LEFT_REAR_SLIDING_DOOR_FAULTY                            = 0x87;
const uint8_t CAN_POPUP_MSG_PARKING_ASSISTANCE_SYSTEM_FAULTY                         = 0x88;
const uint8_t CAN_POPUP_MSG_SPACE_MEASURING_SYSTEM_FAULTY                            = 0x89;
const uint8_t CAN_POPUP_MSG_BATTERY_CHARGE_OR_ELECTRICAL_SUPPLY_FAULTY               = 0x8A;
const uint8_t CAN_POPUP_MSG_TYRE_PRESSURES_TOO_LOW                                   = 0x8D;
const uint8_t CAN_POPUP_MSG_OVERTAKING_ASSISTANCE_SYSTEM_FAULTY                      = 0x92;
const uint8_t CAN_POPUP_MSG_LANE_MONITORING_SYSTEM_DEACTIVATED                       = 0x95;
const uint8_t CAN_POPUP_MSG_ANTI_WANDER_SYSTEM_LANE_CROSSING_WARNING_DEVICE_FAULTY   = 0x97;
const uint8_t CAN_POPUP_MSG_DIPPED_BEAM_BULBS_FAULTY                                 = 0x9A;
const uint8_t CAN_POPUP_MSG_MAIN_BEAM_BULBS_FAULTY                                   = 0x9B;
const uint8_t CAN_POPUP_MSG_BRAKE_LIGHT_BULBS_FAULTY                                 = 0x9C;
const uint8_t CAN_POPUP_MSG_FOGLAMP_BULBS_FAULTY                                     = 0x9D;
const uint8_t CAN_POPUP_MSG_DIRECTION_INDICATORS_FAULTY                              = 0x9E;
const uint8_t CAN_POPUP_MSG_REVERSING_LAMP_BULBS_FAULTY                              = 0x9F;
const uint8_t CAN_POPUP_MSG_SIDELAMP_BULBS_FAULTY_2                                  = 0xA0;
const uint8_t CAN_POPUP_MSG_PARKING_LAMPS_ACTIVATED                                  = 0xA1;
const uint8_t CAN_POPUP_MSG_TYRE_PRESSURES_REGISTERED                                = 0xB4;
const uint8_t CAN_POPUP_MSG_UNDER_INFLATION_MONITORING_FAULT                         = 0xB5;
const uint8_t CAN_POPUP_MSG_ADJUST_THE_TYRE_PRESSURES_THEN_REINITIALISE              = 0xB7;
const uint8_t CAN_POPUP_MSG_TOP_UP_EMISSIONS_ADDITIVE_STARTING_PREVENTED_IN_X_KM     = 0xBB;
const uint8_t CAN_POPUP_MSG_TOP_UP_EMISSIONS_ADDITIVE_STARTING_PREVENTED_IN_X_KM_2   = 0xBC;
const uint8_t CAN_POPUP_MSG_TOP_UP_EMISSIONS_ADDITIVE_STARTING_PREVENTED             = 0xBD;
const uint8_t CAN_POPUP_MSG_EMISSIONS_CONTROL_FAULT                                  = 0xBE;
const uint8_t CAN_POPUP_MSG_EMISSIONS_CONTROL_FAULT_STARTING_PREVENTED_IN_X_KM       = 0xBF;
const uint8_t CAN_POPUP_MSG_EMISSIONS_CONTROL_FAULT_STARTING_PREVENTED               = 0xC0;
const uint8_t CAN_POPUP_MSG_REINITIALISATION_IMPOSSIBLE_WHEN_DRIVING                 = 0xC2;
const uint8_t CAN_POPUP_MSG_REMOVE_IGNITION_KEY                                      = 0xCA;//ruforum
const uint8_t CAN_POPUP_MSG_CRUISE_CONTROL_NOT_POSSIBLE_SPEED_TOO_LOW                = 0xCD;
const uint8_t CAN_POPUP_MSG_CONTROL_ACTIVATION_NOT_POSSIBLE_ENTER_THE_SPEED          = 0xCE;
const uint8_t CAN_POPUP_MSG_ACTIVE_BONNET_DEPLOYED                                   = 0xD1;
const uint8_t CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED                            = 0xD2;
const uint8_t CAN_POPUP_MSG_REAR_PASSENGER_SEAT_BELTS_FASTENED                       = 0xD3;
const uint8_t CAN_POPUP_MSG_PLACE_AUTOMATIC_GEARBOX_IN_POSITION_P                    = 0xD7;
const uint8_t CAN_POPUP_MSG_RISK_OF_ICE                                              = 0xD8;
const uint8_t CAN_POPUP_MSG_HANDBRAKE                                                = 0xD9;
const uint8_t CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN_2       = 0xDE;
const uint8_t CAN_POPUP_MSG_SCREEN_WASH_FLUID_LEVEL_TOO_LOW                          = 0xDF;
const uint8_t CAN_POPUP_MSG_FUEL_LEVEL_TOO_LOW                                       = 0xE0;
const uint8_t CAN_POPUP_MSG_FUEL_CIRCUIT_DEACTIVATED                                 = 0xE1;
const uint8_t CAN_POPUP_MSG_REMOTE_CONTROL_BATTERY_FLAT                              = 0xE3;
const uint8_t CAN_POPUP_MSG_CHECK_AND_REINITIALIZE_TYRE_PRESSURE                     = 0xE4;
const uint8_t CAN_POPUP_MSG_TYRE_PRESSURES_NOT_MONITORED                             = 0xE5;
const uint8_t CAN_POPUP_MSG_HIGH_SPEED_CHECK_TYRE_PRESSURES_CORRECT                  = 0xE7;
const uint8_t CAN_POPUP_MSG_TYRE_PRESSURES_TOO_LOW_2                                 = 0xE8;
const uint8_t CAN_POPUP_MSG_HANDS_FREE_STARTING_SYSTEM_FAULTY                        = 0xEA;
const uint8_t CAN_POPUP_MSG_STARTING_PHASE_HAS_FAILED_CONSULT_HANDBOOK               = 0xEB;
const uint8_t CAN_POPUP_MSG_PROLONGED_START_IN_PROGRESS                              = 0xEC;
const uint8_t CAN_POPUP_MSG_STARTING_IMPOSSIBLE_UNLOCK_STEERING                      = 0xED;
const uint8_t CAN_POPUP_MSG_REMOTE_CONTROL_NOT_DETECTED                              = 0xEF;
const uint8_t CAN_POPUP_MSG_DIAGNOSIS_IN_PROGRESS                                    = 0xF0;
const uint8_t CAN_POPUP_MSG_DIAGNOSIS_COMPLETED                                      = 0xF1;
const uint8_t CAN_POPUP_MSG_REAR_LEFT_HAND_PASSENGER_SEAT_BELT_UNFASTENED            = 0xF7;
const uint8_t CAN_POPUP_MSG_REAR_CENTRAL_PASSENGER_SEAT_BELT_UNFASTENED              = 0xF8;
const uint8_t CAN_POPUP_MSG_REAR_RIGHT_HAND_PASSENGER_SEAT_BELT_UNFASTENED           = 0xF9;
//0x81
const uint8_t CAN_POPUP_MSG_AUTOMATIC_SCREEN_WIPE_ACTIVATED                          = 0x2F;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_SCREEN_WIPE_DEACTIVATED                        = 0x30;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_HEADLAMP_LIGHTING_ACTIVATED                    = 0x31;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_HEADLAMP_LIGHTING_DEACTIVATED                  = 0x32;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_DOOR_LOCKING_ACTIVATED                         = 0x33;
const uint8_t CAN_POPUP_MSG_AUTOMATIC_DOOR_LOCKING_DEACTIVATED                       = 0x34;
const uint8_t CAN_POPUP_MSG_CHILD_SAFETY_ACTIVATED                                   = 0x37;
const uint8_t CAN_POPUP_MSG_CHILD_SAFETY_DEACTIVATED                                 = 0x38;
const uint8_t CAN_POPUP_MSG_ESP_ON_ON_CMB                                            = 0x39;
const uint8_t CAN_POPUP_MSG_ESP_OFF_ON_CMB                                           = 0x3A;
const uint8_t CAN_POPUP_MSG_PARKING_NO                                               = 0x3D;
const uint8_t CAN_POPUP_MSG_STOP_START_SYSTEM_FAULT                                  = 0x98;
const uint8_t CAN_POPUP_MSG_PLACE_GEARBOX_IN_N_POSITION                              = 0x9A;
const uint8_t CAN_POPUP_MSG_TO_RELEASE_PARKING_BRAKE_PRESS_THE_BRAKE_PEDAL           = 0x9B;
const uint8_t CAN_POPUP_MSG_TO_PRESS_THE_PARK_BUTTON_AND_THE_BRAKE_PEDAL             = 0x9C;
const uint8_t CAN_POPUP_MSG_IMPOSSIBLE_TO_MOVE_ROOF_OUTSIDE_TEMPERATURE_TOO_LOW      = 0xF6;
const uint8_t CAN_POPUP_MSG_IMPOSSIBLE_TO_MOVE_ROOF_SPEED_TOO_HIGH                   = 0xF7;
const uint8_t CAN_POPUP_MSG_IMPOSSIBLE_TO_MOVE_ROOF_BOOT_OPEN                        = 0xF8;
const uint8_t CAN_POPUP_MSG_IMPOSSIBLE_TO_MOVE_ROOF_SCREEN_NOT_DEPLOYED              = 0xFA;
const uint8_t CAN_POPUP_MSG_ROOF_MANOEUVRE_COMPLETED                                 = 0xFB;
const uint8_t CAN_POPUP_MSG_COMPLETE_ROOF_MANOEUVRE_IMMEDIATELY                      = 0xFC;
const uint8_t CAN_POPUP_MSG_MANOEUVRE_IMPOSSIBLE_ROOF_LOCKED                         = 0xFD;
const uint8_t CAN_POPUP_MSG_FOLDING_ROOF_MECHANISM_FAULTY                            = 0xFE;
const uint8_t CAN_POPUP_MSG_OPERATION_IMPOSSIBLE_REAR_SCREEN_OPEN                    = 0xFF;
//0x82
const uint8_t CAN_POPUP_MSG_ROOF_MOVEMENT_NOT_POSSIBLE_CASETTE_OPEN                  = 0x00;
const uint8_t CAN_POPUP_MSG_OPERATION_IMPOSIBBLE_LOCK_ROOF                           = 0x01;
const uint8_t CAN_POPUP_MSG_ANTI_ROLLBACK_SYSTEM_FAULTY                              = 0x02;
const uint8_t CAN_POPUP_MSG_ENHANCED_TRACTION_NORMAL_MODE                            = 0x03;
const uint8_t CAN_POPUP_MSG_ENHANCED_TRACTION_MUD_MODE                               = 0x04;
const uint8_t CAN_POPUP_MSG_ENHANCED_TRACTION_SNOW_MODE                              = 0x05;
const uint8_t CAN_POPUP_MSG_ENHANCED_TRACTION_SAND_MODE                              = 0x06;
const uint8_t CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED                                   = 0x09;
const uint8_t CAN_POPUP_MSG_INTER_VEHICLES_TIME_MEASURING_IMPOSSIBLE_POOR_VISIBILITY = 0x0A;
const uint8_t CAN_POPUP_MSG_INTER_VEHICLES_TIME_MEASURING_IMPOSSIBLE_INIT_IN_PROGRESS= 0x0B;
const uint8_t CAN_POPUP_MSG_ECO_ACTIVATED                                            = 0x20;
const uint8_t CAN_POPUP_MSG_ECO_DEACTIVATED                                          = 0x21;
const uint8_t CAN_POPUP_MSG_PRESS_THE_CLUTCH_FULLY                                   = 0x22;
const uint8_t CAN_POPUP_MSG_ENGINE_FAULT_STOP_THE_VEHICLE                            = 0x2E;

const uint8_t CAN_POPUP_MSG_NONE                                                     = 0xFF;

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
union CanDisplayByte2Struct {
    struct {
        uint8_t priority          : 4; // bit 0-3 //0-14
        uint8_t check_in_progress : 1; // bit 4
        uint8_t show_popup_on_vth : 1; // bit 5
        uint8_t show_popup_on_cmb : 1; // bit 6
        uint8_t show_popup_on_emf : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDisplayDoorStatus1Struct {
    struct {
        uint8_t                     : 1; // bit 0
        uint8_t                     : 1; // bit 1
        uint8_t is_bonnet_open      : 1; // bit 2
        uint8_t is_boot_open        : 1; // bit 3
        uint8_t is_rear_left_open   : 1; // bit 4
        uint8_t is_rear_right_open  : 1; // bit 5
        uint8_t is_front_left_open  : 1; // bit 6
        uint8_t is_front_right_open : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDisplayDoorStatus2Struct {
    struct {
        uint8_t                     : 1; // bit 0
        uint8_t                     : 1; // bit 1
        uint8_t                     : 1; // bit 2
        uint8_t                     : 1; // bit 3
        uint8_t                     : 1; // bit 4
        uint8_t                     : 1; // bit 5
        uint8_t is_fuel_flap_open   : 1; // bit 6
        uint8_t is_rear_screen_open : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CanDisplayStruct {
    uint8_t ShowPopup;
    uint8_t PopupMessageType;
    CanDisplayByte2Struct Field2;
    CanDisplayDoorStatus1Struct DoorStatus1;
    CanDisplayDoorStatus2Struct DoorStatus2;
    uint8_t Field5;
    uint8_t KmDividedBy256;
    uint8_t KmRemainderUpTo255;
};

union CanDisplayPacket {
    CanDisplayStruct data;
    uint8_t CanDisplayPacket[sizeof(CanDisplayStruct)];
};

uint8_t static CanGetHighKmToDisplay(int kmToDisplay) {
    return kmToDisplay / 256;
}

uint8_t static CanGetLowKmToDisplay(int kmToDisplay){
    return kmToDisplay - (CanGetHighKmToDisplay(kmToDisplay) * 256);
}

#endif
