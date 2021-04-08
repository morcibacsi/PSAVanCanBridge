#include "VanCanGearboxPositionMap.h"
#include "../Can/Structs/CanDash2Structs.h"
#include "../Van/Structs/VanInstrumentClusterV2Structs.h"

void VanCanGearboxPositionMap::SetGearboxMapping()
{
    memset(_gearBoxMap, CAN_DASH_GEAR_INVALID, 15);
    _gearBoxMap[VAN_GEAR_V2_P] = CAN_DASH_GEAR_P;
    _gearBoxMap[VAN_GEAR_V2_R] = CAN_DASH_GEAR_R;
    _gearBoxMap[VAN_GEAR_V2_N] = CAN_DASH_GEAR_N;
    _gearBoxMap[VAN_GEAR_V2_D] = CAN_DASH_GEAR_D;
    _gearBoxMap[VAN_GEAR_V2_4] = CAN_DASH_GEAR_4;
    _gearBoxMap[VAN_GEAR_V2_3] = CAN_DASH_GEAR_3;
    _gearBoxMap[VAN_GEAR_V2_2] = CAN_DASH_GEAR_2;
    _gearBoxMap[VAN_GEAR_V2_1] = CAN_DASH_GEAR_1;
}

void VanCanGearboxPositionMap::SetGearboxModeMapping()
{
    memset(_gearBoxModeMap, CAN_DASH_GEAR_MODE_AUTO, 4);
    _gearBoxModeMap[VAN_GEAR_MODE_V2_NORMAL]              = CAN_DASH_GEAR_MODE_AUTO;
    _gearBoxModeMap[VAN_GEAR_MODE_V2_AUTO_ECO]            = CAN_DASH_GEAR_MODE_AUTO;
    _gearBoxModeMap[VAN_GEAR_MODE_V2_SPORT]               = CAN_DASH_GEAR_MODE_AUTO_SPORT;
    _gearBoxModeMap[VAN_GEAR_MODE_V2_SEQUENTIAL_ECO]      = CAN_DASH_GEAR_MODE_SEQUENTIAL;
    _gearBoxModeMap[VAN_GEAR_MODE_V2_SNOW_AUTO]           = CAN_DASH_GEAR_MODE_AUTO_SNOW;
    _gearBoxModeMap[VAN_GEAR_MODE_V2_SNOW_AUTO_ECO]       = CAN_DASH_GEAR_MODE_AUTO_SNOW;
    _gearBoxModeMap[VAN_GEAR_MODE_V2_SNOW_SEQUENTIAL]     = CAN_DASH_GEAR_MODE_AUTO_SNOW;
    _gearBoxModeMap[VAN_GEAR_MODE_V2_SNOW_SEQUENTIAL_ECO] = CAN_DASH_GEAR_MODE_AUTO_SNOW;
}

void VanCanGearboxPositionMap::SetGearboxSelectionMapping()
{
    _gearBoxSelectionMap[VAN_GEAR_V2_SELECTION_BVA] = CAN_DASH_GEAR_SELECTION_BVA;
    _gearBoxSelectionMap[VAN_GEAR_V2_SELECTION_BVM] = CAN_DASH_GEAR_SELECTION_BVM;
}

VanCanGearboxPositionMap::VanCanGearboxPositionMap()
{
    SetGearboxMapping();
    SetGearboxModeMapping();
}

uint8_t VanCanGearboxPositionMap::GetGearboxPositionFromVanPosition(uint8_t gearboxPosition)
{
    return _gearBoxMap[gearboxPosition];
}

uint8_t VanCanGearboxPositionMap::GetGearboxModeFromVanMode(uint8_t gearboxMode)
{
    return _gearBoxModeMap[gearboxMode];
}

uint8_t VanCanGearboxPositionMap::GetGearboxSelectionFromVanSelection(uint8_t gearboxSelection)
{
    return _gearBoxSelectionMap[gearboxSelection];
}
