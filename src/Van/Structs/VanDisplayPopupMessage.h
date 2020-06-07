// VanDisplayPopupMessage.h

#pragma once

#ifndef _VanDisplayPopupMessage_h
    #define _VanDisplayPopupMessage_h

#pragma region Message definitions

const uint8_t VAN_POPUP_MSG_TYRES_DEFLATED                          = 0x00;
const uint8_t VAN_POPUP_MSG_DOOR_OPEN                               = 0x01;
const uint8_t VAN_POPUP_MSG_GEARBOX_OIL_TEMP_TOO_HIGH               = 0x02;
const uint8_t VAN_POPUP_MSG_BRAKE_FLUID_LEVEL_LOW                   = 0x03;
const uint8_t VAN_POPUP_MSG_HYDRAULIC_SUSPENSION_PRESSURE           = 0x04;
const uint8_t VAN_POPUP_MSG_SERIOUS_SUSPENSION_FAULT                = 0x05;
const uint8_t VAN_POPUP_MSG_ENGINE_OIL_TEMPERATURE_TOO_HIGH         = 0x06;
const uint8_t VAN_POPUP_MSG_ENGINE_COOLANT_TEMP_TOO_HIGH            = 0x07;
const uint8_t VAN_POPUP_MSG_UNBLOCK_DIESEL_FILTER                   = 0x08;
const uint8_t VAN_POPUP_MSG_AUTO_ICON_WITH_STOP                     = 0x09;
const uint8_t VAN_POPUP_MSG_DIESEL_ADDITIVE_MINIMUM_LEVEL           = 0x0A;
const uint8_t VAN_POPUP_MSG_FUEL_TANK_ACCESS_OPEN                   = 0x0B;
const uint8_t VAN_POPUP_MSG_TYRES_PUNCTURED                         = 0x0C;
const uint8_t VAN_POPUP_MSG_TOP_UP_ENGINE_COOLANT_LEVEL             = 0x0D;
const uint8_t VAN_POPUP_MSG_OIL_PRESSURE_INSUFFICIENT               = 0x0E;
const uint8_t VAN_POPUP_MSG_TOP_UP_ENGINE_OIL_LEVEL                 = 0x0F;
const uint8_t VAN_POPUP_MSG_ANTIPOLLUTION_FAULT                     = 0x10;
const uint8_t VAN_POPUP_MSG_BRAKE_PADS_WORN                         = 0x11;
const uint8_t VAN_POPUP_MSG_CHECK_CONTROL_OK                        = 0x12;
const uint8_t VAN_POPUP_MSG_AUTOMATIC_GEAR_FAULT                    = 0x13;
const uint8_t VAN_POPUP_MSG_ESP_ASR_NOT_FUNCTIONING                 = 0x14;
const uint8_t VAN_POPUP_MSG_ABS_FAULT                               = 0x15;
const uint8_t VAN_POPUP_MSG_SUSPENSION_OR_STEERING_FAULT            = 0x16;
const uint8_t VAN_POPUP_MSG_BRAKING_FAULT                           = 0x17;
const uint8_t VAN_POPUP_MSG_SIDE_AIRBAG_FAULT                       = 0x18;
const uint8_t VAN_POPUP_MSG_AIRBAG_FAULT                            = 0x19;
const uint8_t VAN_POPUP_MSG_CRUISE_CONTROL_FAULT                    = 0x1A;
const uint8_t VAN_POPUP_MSG_ENGINE_COOLANT_TEMPERATURE_HIGH         = 0x1B;
const uint8_t VAN_POPUP_MSG_FAULT_LOAD_SHEDDING_IN_PROGRESS         = 0x1C;
const uint8_t VAN_POPUP_MSG_AUTO_LIGHTING_FAULT                     = 0x1D;
const uint8_t VAN_POPUP_MSG_AUTO_WIPER_FAULT                        = 0x1E;
const uint8_t VAN_POPUP_MSG_WATER_IN_DIESEL_FUEL_FILTER             = 0x1F;
const uint8_t VAN_POPUP_MSG_LEFT_SLIDING_DOOR_FAULT                 = 0x20;
const uint8_t VAN_POPUP_MSG_HEADLIGHT_CORRECTOR_FAULT               = 0x21;
const uint8_t VAN_POPUP_MSG_RIGHT_SLIDING_DOOR_FAULT                = 0x22;
const uint8_t VAN_POPUP_MSG_NO_BROKEN_LAMP                          = 0x23;
const uint8_t VAN_POPUP_MSG_SECONDARY_BATTERY_CHARGE_FAULT          = 0x24;
const uint8_t VAN_POPUP_MSG_BATTERY_CHARGE_FAULT                    = 0x25;
const uint8_t VAN_POPUP_MSG_DIESEL_FUEL_ADDITIVE_FAULT              = 0x26;
const uint8_t VAN_POPUP_MSG_CATALYTIC_CONVERTER_FAULT               = 0x27;
const uint8_t VAN_POPUP_MSG_HANDBRAKE_ON                            = 0x28;
const uint8_t VAN_POPUP_MSG_SEAT_BELT_REMINDER                      = 0x29;
const uint8_t VAN_POPUP_MSG_PASSENGER_AIRBAG_DEACTIVATED            = 0x2A;
const uint8_t VAN_POPUP_MSG_TOP_UP_WASHER_FLUID                     = 0x2B;
const uint8_t VAN_POPUP_MSG_SPEED_TOO_HIGH                          = 0x2C;
const uint8_t VAN_POPUP_MSG_KEY_REMINDER                            = 0x2D;
const uint8_t VAN_POPUP_MSG_SIDE_LIGHTS_REMINDER                    = 0x2E;
const uint8_t VAN_POPUP_MSG_HILL_HOLDER_ACTIVE                      = 0x2F; // On RT3 monochrome: Driver seatbelt not fastened
const uint8_t VAN_POPUP_MSG_IMPACT_SENSOR_FAULT                     = 0x30;

const uint8_t VAN_POPUP_MSG_WHEEL_PRESSURE_SENSOR_BATTERY_LOW       = 0x32;
const uint8_t VAN_POPUP_MSG_REMOTE_CONTROL_BATTERY_LOW              = 0x33;

const uint8_t VAN_POPUP_MSG_PUT_AUTO_TRANSMISS_LEVER_IN_POSITION_P  = 0x35;
const uint8_t VAN_POPUP_MSG_TEST_STOP_LIGHTS_BRAKE_GENTLY           = 0x36;
const uint8_t VAN_POPUP_MSG_FUEL_LEVEL_LOW                          = 0x37;
const uint8_t VAN_POPUP_MSG_AUTOMATIC_LIGHTING_INACTIVE             = 0x38;
const uint8_t VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_FAULT             = 0x39; // On RT3 monochrome: Rear LH passenger seatbelt unfastened
const uint8_t VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_DEACTIVATED       = 0x3A; // On RT3 monochrome: Rear RH passenger seatbelt unfastened
const uint8_t VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_ACTIVATED         = 0x3B; // On RT3 monochrome: Front passenger seatbelt unfastened
const uint8_t VAN_POPUP_MSG_DRIVING_SCHOOL_PEDALS_INDICATION        = 0x3C;
const uint8_t VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING0        = 0x3D;
const uint8_t VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING1        = 0x3E;
const uint8_t VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING2        = 0x3F;
const uint8_t VAN_POPUP_MSG_DOORS_LOCKED                            = 0x40;
const uint8_t VAN_POPUP_MSG_ESP_ASR_DEACTIVATED                     = 0x41;
const uint8_t VAN_POPUP_MSG_CHILD_SAFETY_ACTIVE                     = 0x42;
const uint8_t VAN_POPUP_MSG_DEADLOCKING_ACTIVE                      = 0x43;
const uint8_t VAN_POPUP_MSG_AUTOMATIC_LIGHTING_ACTIVE               = 0x44;
const uint8_t VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE                 = 0x45;
const uint8_t VAN_POPUP_MSG_ENGINE_IMMOBILISER_FAULT                = 0x46;
const uint8_t VAN_POPUP_MSG_SPORTS_SUSPENSION_ACTIVE                = 0x47;
const uint8_t VAN_POPUP_MSG_FLH_DIPPED_HEADLIGHT_BULB_BLOWN         = 0x4E;

const uint8_t VAN_POPUP_MSG_CHANGE_OF_FUEL_USED_IN_PROGRESS         = 0x53;
const uint8_t VAN_POPUP_MSG_LPG_FUEL_REFUSED                        = 0x54;
const uint8_t VAN_POPUP_MSG_LPG_SYSTEM_FAULTY                       = 0x55;
const uint8_t VAN_POPUP_MSG_LPG_IN_USE                              = 0x56;
const uint8_t VAN_POPUP_MSG_MIN_LEVEL_LPG                           = 0x57;
const uint8_t VAN_POPUP_MSG_ADIN_FAULT                              = 0x58;
const uint8_t VAN_POPUP_MSG_USE_STOP_AND_START                      = 0x59;
const uint8_t VAN_POPUP_MSG_STOP_AND_START_AVAILABLE                = 0x5A;
const uint8_t VAN_POPUP_MSG_STOP_AND_START_SYSTEM_ACTIVE            = 0x5B;
const uint8_t VAN_POPUP_MSG_STOP_AND_START_SYSTEM_DEACTIVATED       = 0x5C;
const uint8_t VAN_POPUP_MSG_STOP_AND_START_DEFERRED                 = 0x5D;
const uint8_t VAN_POPUP_MSG_XSARA_DYNALTO                           = 0x5E;
const uint8_t VAN_POPUP_MSG_307_DYNALTO                             = 0x5F;

const uint8_t VAN_POPUP_MSG_PLACE_LEVER_IN_NEUTRAL                  = 0x67; // On RT3 monochrome: Change to neutral

const uint8_t VAN_POPUP_MSG_ROOF_OPERATION_COMPLETE                 = 0x70;
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_SCREEN_NOT_IN_PLACE= 0x71;
const uint8_t VAN_POPUP_MSG_ROOF_MECHANISM_NOT_LOCKED               = 0x72;
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_BOOT_OPEN          = 0x73;
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_SPEED_TOO_HIGH     = 0x74;
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_EXT_TEMP_TOO_LOW   = 0x75;
const uint8_t VAN_POPUP_MSG_ROOF_MECHANISM_FAULTY                   = 0x76; // On RT3 monochrome: Folding roof mechanism faulty
const uint8_t VAN_POPUP_MSG_BOOT_MECHANICS_NOT_LOCKED               = 0x77;

const uint8_t VAN_POPUP_MSG_BOW_FAULT                               = 0x7C; // On RT3 monochrome: Roof frame fault
const uint8_t VAN_POPUP_MSG_OPERATION_IMPOSSIBLE_ROOF_NOT_UNLOCKED  = 0x7D;
const uint8_t VAN_POPUP_MSG_ROOF_OPERATION_INCOMPLETE               = 0x7F;

#pragma endregion

#endif