// CanDash2Structs.h
#pragma once

#ifndef _CanDash2Structs_h
    #define _CanDash2Structs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

const uint16_t CAN_ID_DASH2_INTERVAL = 200;

// CANID: 128
const uint16_t CAN_ID_DASH2 = 0x128;

const uint8_t CAN_DASH_GEAR_P       = 0;
const uint8_t CAN_DASH_GEAR_R       = 1;
const uint8_t CAN_DASH_GEAR_N       = 2;
const uint8_t CAN_DASH_GEAR_D       = 3;
const uint8_t CAN_DASH_GEAR_6       = 4;
const uint8_t CAN_DASH_GEAR_5       = 5;
const uint8_t CAN_DASH_GEAR_4       = 6;
const uint8_t CAN_DASH_GEAR_3       = 7;
const uint8_t CAN_DASH_GEAR_2       = 8;
const uint8_t CAN_DASH_GEAR_1       = 9;
const uint8_t CAN_DASH_GEAR_INVALID = 15;

const uint8_t CAN_DASH_GEAR_MODE_AUTO             = 0;
const uint8_t CAN_DASH_GEAR_MODE_UNUSED1          = 1;
const uint8_t CAN_DASH_GEAR_MODE_AUTO_SPORT       = 2;
const uint8_t CAN_DASH_GEAR_MODE_UNUSED3          = 3;
const uint8_t CAN_DASH_GEAR_MODE_SEQUENTIAL       = 4;
const uint8_t CAN_DASH_GEAR_MODE_SEQUENTIAL_SPORT = 5;
const uint8_t CAN_DASH_GEAR_MODE_AUTO_SNOW        = 6;
const uint8_t CAN_DASH_GEAR_MODE_UNUSED7          = 7;

const uint8_t CAN_DASH_GEAR_SELECTION_BVA    = 0;
const uint8_t CAN_DASH_GEAR_SELECTION_BVM    = 1;
const uint8_t CAN_DASH_GEAR_SELECTION_BVMP   = 2;
const uint8_t CAN_DASH_GEAR_SELECTION_UNUSED = 3;

// Read right to left in documentation
typedef struct {
    uint8_t service_indicator_enabled    : 1; // bit 0
    uint8_t passenger_seatbelt_forgotten : 1; // bit 1
    uint8_t pre_heat_light               : 1; // bit 2 //not lit up on petrol cars
    uint8_t fuel_circuit_neutralized     : 1; // bit 3
    uint8_t fuel_low_light               : 1; // bit 4
    uint8_t parking_brake_active         : 1; // bit 5
    uint8_t driver_seatbelt_light        : 1; // bit 6 //doesn't work on all dials
    uint8_t passenger_airbag_deactivated : 1; // bit 7
} CanDash2Byte1Struct;

typedef struct {
    uint8_t rear_seatbelt_not_fastened : 1; // bit 0
    uint8_t abs_work_in_progress       : 1; // bit 1
    uint8_t fuel_flap_open             : 1; // bit 2
    uint8_t any_door_open_above_10kmh  : 1; // bit 3
    uint8_t any_door_open_below_10kmh  : 1; // bit 4
    uint8_t stop_pict_blink            : 1; // bit 5
    uint8_t stop                       : 1; // bit 6
    uint8_t warning                    : 1; // bit 7
} CanDash2Byte2Struct;

typedef struct {
    uint8_t safety_scope_light        : 1; // bit 0
    uint8_t warning_operation_control : 1; // bit 1
    uint8_t auto_suspension_status    : 1; // bit 2
    uint8_t esp_in_progress           : 1; // bit 3
    uint8_t esp_inhibited             : 1; // bit 4
    uint8_t child_security_active     : 1; // bit 5
    uint8_t conviction_state          : 1; // bit 6
    uint8_t conviction_while_driving  : 1; // bit 7
} CanDash2Byte3Struct;

typedef struct {
    uint8_t seatbelt_forgotten_led_flash : 1; // bit 0
    uint8_t foot_on_break_light          : 2; // bit 1-2
    uint8_t mpd_flash_requested          : 1; // bit 3
    uint8_t space_measurement_led_on     : 1; // bit 4
    uint8_t autopark_led_on              : 1; // bit 5
    uint8_t unused6                      : 1; // bit 6
    uint8_t seatbelt_blinks              : 1; // bit 7
} CanDash2Byte4Struct;

typedef struct {
    uint8_t unused1            : 1; // bit 0
    uint8_t left_indicator_on  : 1; // bit 1
    uint8_t right_indicator_on : 1; // bit 2
    uint8_t rear_fog_light_on  : 1; // bit 3
    uint8_t front_fog_light_on : 1; // bit 4
    uint8_t high_beam_on       : 1; // bit 5
    uint8_t low_beam_on        : 1; // bit 6
    uint8_t side_lights_on     : 1; // bit 7
} CanDash2Byte5Struct;

typedef struct {
    uint8_t rear_window_heating_active : 1; // bit 0
    uint8_t unused1                    : 1; // bit 1
    uint8_t unused2                    : 1; // bit 2
    uint8_t unused3                    : 1; // bit 3
    uint8_t adin_in_progress           : 1; // bit 4
    uint8_t adin_should_be_used        : 1; // bit 5
    uint8_t adin_available             : 1; // bit 6
    uint8_t dash_enabled               : 1; // bit 7
} CanDash2Byte6Struct;

typedef struct {
    uint8_t blink          : 1; // bit 0
    uint8_t unused1        : 1; // bit 1
    uint8_t unused2        : 1; // bit 2
    uint8_t unused3        : 1; // bit 3
    uint8_t gear           : 4; // bit 4-7
} CanDash2Byte7Struct;

typedef struct {
    uint8_t bva_bvmp_selection : 2; // bit 0-1
    uint8_t unused2            : 1; // bit 2
    uint8_t unused3            : 1; // bit 3
    uint8_t bva_bvmp_mode      : 3; // bit 4-6
    uint8_t unused7            : 1; // bit 7
} CanDash2Byte8Struct;

// Read left to right in documentation
struct CanDash2Struct {
    CanDash2Byte1Struct Field1;
    CanDash2Byte2Struct Field2;
    CanDash2Byte3Struct Field3;
    uint8_t Field4;
    CanDash2Byte5Struct Lights;
    CanDash2Byte6Struct Field6;
    CanDash2Byte7Struct Gearbox;
    CanDash2Byte8Struct GearboxMode;
};

union CanDash2Packet {
    CanDash2Struct data;
    uint8_t CanDash2Packet[sizeof(CanDash2Struct)];
};

class CanDash2PacketSender
{
    AbstractCanMessageSender * canMessageSender;

    public:
    CanDash2PacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendData(
        uint8_t driversSeatbeltLight,
        uint8_t sideLights,
        uint8_t lowBeam,
        uint8_t highBeam,
        uint8_t frontFog,
        uint8_t rearFog,
        uint8_t leftIndicator,
        uint8_t rightIndicator,
        uint8_t dashEnabled,
        uint8_t fuelLowLight,
        uint8_t passengerAirbagDeactivated,
        uint8_t preHeat,
        uint8_t stop,
        uint8_t warning,
        uint8_t gearboxMode,
        uint8_t gearboxSelection,
        uint8_t gearboxPosition)
    {
        PacketGenerator<CanDash2Packet> generator;
        generator.packet.data.Field1.driver_seatbelt_light = driversSeatbeltLight;
        generator.packet.data.Field1.fuel_low_light = fuelLowLight;
        generator.packet.data.Field1.passenger_airbag_deactivated = passengerAirbagDeactivated;
        generator.packet.data.Field1.pre_heat_light = preHeat;

        generator.packet.data.Field2.stop = stop;
        generator.packet.data.Field2.warning = warning;

        generator.packet.data.Lights.side_lights_on = sideLights;
        generator.packet.data.Lights.low_beam_on = lowBeam;
        generator.packet.data.Lights.high_beam_on = highBeam;
        generator.packet.data.Lights.front_fog_light_on = frontFog;
        generator.packet.data.Lights.rear_fog_light_on = rearFog;
        generator.packet.data.Lights.left_indicator_on = leftIndicator;
        generator.packet.data.Lights.right_indicator_on = rightIndicator;

        generator.packet.data.Field6.dash_enabled = dashEnabled;

        generator.packet.data.Gearbox.gear = gearboxPosition;

        generator.packet.data.GearboxMode.bva_bvmp_mode = gearboxMode;
        generator.packet.data.GearboxMode.bva_bvmp_selection = gearboxSelection;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_DASH2, 0, sizeof(CanDash2Packet), serializedPacket);
    }
};
#endif
