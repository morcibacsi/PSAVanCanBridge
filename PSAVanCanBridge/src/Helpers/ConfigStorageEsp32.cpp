#include "ConfigStorageEsp32.h"
#include "../ESPFlash/ESPFlash.h"
#include "DebugPrint.h"

ConfigStorageEsp32::ConfigStorageEsp32(Config* config)
{
    _config = config;
}

void ConfigStorageEsp32::Remove()
{
    SPIFFS.begin();
    if (SPIFFS.exists(_fileName))
    {
        debug_println(F("config removed"));
        SPIFFS.remove(_fileName);
    }
    SPIFFS.end();
}

bool ConfigStorageEsp32::LoadFromDoc(DynamicJsonDocument doc)
{
    _config->ORIGINAL_DISPLAY_PRESENT = doc["ORIGINAL_DISPLAY_PRESENT"];
    _config->BRIDGE_RADIO_DATA = doc["BRIDGE_RADIO_DATA"];
    _config->BRIDGE_ODOMETER_DATA = doc["BRIDGE_ODOMETER_DATA"];
    _config->DISPLAY_CONTROL = doc["DISPLAY_CONTROL"];
    _config->ENABLE_PARKING_AID_SOUND_FROM_SPEAKER = doc["ENABLE_PARKING_AID_SOUND_FROM_SPEAKER"];
    _config->FUEL_TANK_CAPACITY_IN_LITERS = doc["FUEL_TANK_CAPACITY_IN_LITERS"];
    _config->NO_EMF_C_INSTALLED = doc["NO_EMF_C_INSTALLED"];
    _config->QUERY_AC_STATUS = doc["QUERY_AC_STATUS"];
    _config->PARKING_AID_TYPE = doc["PARKING_AID_TYPE"];
    _config->SEND_AC_CHANGES_TO_DISPLAY = doc["SEND_AC_CHANGES_TO_DISPLAY"];
    _config->SEND_AC_FAN_CHANGES_TO_DISPLAY = doc["SEND_AC_FAN_CHANGES_TO_DISPLAY"];
    _config->USE_BRIGHTNESS_FROM_CAN_ODOMETER = doc["USE_BRIGHTNESS_FROM_CAN_ODOMETER"];
    _config->USE_IGNITION_SIGNAL_FROM_VAN_BUS = doc["USE_IGNITION_SIGNAL_FROM_VAN_BUS"];
    _config->BRIDGE_REMOTE_STALK_DATA = doc["BRIDGE_REMOTE_STALK_DATA"];
    _config->MILEAGE_AT_CMB_TRIP_RESET = doc["MILEAGE_AT_CMB_TRIP_RESET"];
    _config->RADIO_TYPE = doc["RADIO_TYPE"];
    _config->ENABLE_MANUAL_GEARBOX_DISPLAY = doc["ENABLE_MANUAL_GEARBOX_DISPLAY"];
    _config->TIRE_SIZE = doc["TIRE_SIZE"];
    _config->TIRE_WIDTH = doc["TIRE_WIDTH"];
    _config->TIRE_RATIO = doc["TIRE_RATIO"];
    _config->CAN_TYPE = doc["CAN_TYPE"];
    _config->DISPLAY_CONTROL_WITH_COM = doc["DISPLAY_CONTROL_WITH_COM"];
    _config->LEFT_STICK_BUTTON_SHOWS_OIL_TEMP = doc["LEFT_STICK_BUTTON_SHOWS_OIL_TEMP"];
    _config->ENABLE_REVERSE_CAMERA_ON_RTX = doc["ENABLE_REVERSE_CAMERA_ON_RTX"];

    JsonArray VIN_FOR_HEADUNIT = doc["VIN_FOR_HEADUNIT"];
    for (size_t i = 0; i < VIN_LENGTH; i++)
    {
        _config->VIN_FOR_HEADUNIT[i] = VIN_FOR_HEADUNIT[i];
    }

    JsonArray GEAR_RATIOS = doc["GEAR_RATIOS"];
    for (size_t i = 0; i < 7; i++)
    {
        _config->GEAR_RATIOS[i] = GEAR_RATIOS[i];
    }

    JsonArray AAS_REAR_DISTANCES = doc["AAS_REAR_DISTANCES"];

    for (size_t i = 0; i < 4; i++)
    {
        _config->AAS_REAR_DISTANCES[i] = AAS_REAR_DISTANCES[i];
    }

    JsonArray AAS_REAR_CORNER_DISTANCES = doc["AAS_REAR_CORNER_DISTANCES"];

    for (size_t i = 0; i < 4; i++)
    {
        _config->AAS_REAR_CORNER_DISTANCES[i] = AAS_REAR_CORNER_DISTANCES[i];
    }

    //printArray1("Headunit vin in config: ", _config->VIN_FOR_HEADUNIT, VIN_LENGTH);

    return true;
}

bool ConfigStorageEsp32::Load()
{
    // Use the tool here: https://arduinojson.org/v6/assistant/
    debug_println(F("try to load config"));
    SPIFFS.begin(true);
    if (SPIFFS.exists(_fileName))
    {
        File configFile = SPIFFS.open(_fileName, FILE_READ);
        DynamicJsonDocument doc(JSON_MEMORY_SIZE);
        DeserializationError error = deserializeJson(doc, configFile);
        if (error == DeserializationError::Ok)
        {
            debug_println(F("config loaded"));
            LoadFromDoc(doc);

            configFile.close();
        }
        else
        {
            //
            /*
            if (error == DeserializationError::EmptyInput)
            {
                debug_println("Empty");
            }
            if (error == DeserializationError::IncompleteInput)
            {
                debug_println("Incomplete");
            }
            if (error == DeserializationError::NoMemory)
            {
                debug_println("NoMemory");
            }
            if (error == DeserializationError::InvalidInput)
            {
                debug_println("Invalid");
            }
            if (error == DeserializationError::TooDeep)
            {
                debug_println("TooDeep");
            }
            //*/
        }
    }
    else
    {
        debug_println(F("config does not exists"));
    }
    SPIFFS.end();

    return true;
}

StaticJsonDocument<JSON_MEMORY_SIZE> ConfigStorageEsp32::GetConfig()
{
    _configJson.clear();

    _configJson["ORIGINAL_DISPLAY_PRESENT"] = _config->ORIGINAL_DISPLAY_PRESENT;
    _configJson["BRIDGE_RADIO_DATA"] = _config->BRIDGE_RADIO_DATA;
    _configJson["BRIDGE_ODOMETER_DATA"] = _config->BRIDGE_ODOMETER_DATA;
    _configJson["DISPLAY_CONTROL"] = _config->DISPLAY_CONTROL;
    _configJson["ENABLE_PARKING_AID_SOUND_FROM_SPEAKER"] = _config->ENABLE_PARKING_AID_SOUND_FROM_SPEAKER;
    _configJson["FUEL_TANK_CAPACITY_IN_LITERS"] = _config->FUEL_TANK_CAPACITY_IN_LITERS;
    _configJson["NO_EMF_C_INSTALLED"] = _config->NO_EMF_C_INSTALLED;
    _configJson["QUERY_AC_STATUS"] = _config->QUERY_AC_STATUS;
    _configJson["PARKING_AID_TYPE"] = _config->PARKING_AID_TYPE;
    _configJson["SEND_AC_CHANGES_TO_DISPLAY"] = _config->SEND_AC_CHANGES_TO_DISPLAY;
    _configJson["SEND_AC_FAN_CHANGES_TO_DISPLAY"] = _config->SEND_AC_FAN_CHANGES_TO_DISPLAY;
    _configJson["USE_BRIGHTNESS_FROM_CAN_ODOMETER"] = _config->USE_BRIGHTNESS_FROM_CAN_ODOMETER;
    _configJson["USE_IGNITION_SIGNAL_FROM_VAN_BUS"] = _config->USE_IGNITION_SIGNAL_FROM_VAN_BUS;
    _configJson["BRIDGE_REMOTE_STALK_DATA"] = _config->BRIDGE_REMOTE_STALK_DATA;
    _configJson["MILEAGE_AT_CMB_TRIP_RESET"] = _config->MILEAGE_AT_CMB_TRIP_RESET;
    _configJson["RADIO_TYPE"] = _config->RADIO_TYPE;
    _configJson["ENABLE_MANUAL_GEARBOX_DISPLAY"] = _config->ENABLE_MANUAL_GEARBOX_DISPLAY;
    _configJson["TIRE_SIZE"] = _config->TIRE_SIZE;
    _configJson["TIRE_WIDTH"] = _config->TIRE_WIDTH;
    _configJson["TIRE_RATIO"] = _config->TIRE_RATIO;
    _configJson["CAN_TYPE"] = _config->CAN_TYPE;
    _configJson["DISPLAY_CONTROL_WITH_COM"] = _config->DISPLAY_CONTROL_WITH_COM;
    _configJson["LEFT_STICK_BUTTON_SHOWS_OIL_TEMP"] = _config->LEFT_STICK_BUTTON_SHOWS_OIL_TEMP;
    _configJson["ENABLE_REVERSE_CAMERA_ON_RTX"] = _config->ENABLE_REVERSE_CAMERA_ON_RTX;
    _configJson["VERSION"] = _config->VERSION;

    JsonArray VIN_FOR_HEADUNIT = _configJson.createNestedArray("VIN_FOR_HEADUNIT");

    for (size_t i = 0; i < VIN_LENGTH; i++)
    {
        VIN_FOR_HEADUNIT.add(_config->VIN_FOR_HEADUNIT[i]);
    }

    JsonArray GEAR_RATIOS = _configJson.createNestedArray("GEAR_RATIOS");

    for (size_t i = 0; i < 7; i++)
    {
        GEAR_RATIOS.add(_config->GEAR_RATIOS[i]);
    }

    JsonArray AAS_REAR_DISTANCES = _configJson.createNestedArray("AAS_REAR_DISTANCES");

    for (size_t i = 0; i < 4; i++)
    {
        AAS_REAR_DISTANCES.add(_config->AAS_REAR_DISTANCES[i]);
    }

    JsonArray AAS_REAR_CORNER_DISTANCES = _configJson.createNestedArray("AAS_REAR_CORNER_DISTANCES");

    for (size_t i = 0; i < 4; i++)
    {
        AAS_REAR_CORNER_DISTANCES.add(_config->AAS_REAR_CORNER_DISTANCES[i]);
    }

    return _configJson;
}

bool ConfigStorageEsp32::Save()
{
    Remove();

    SPIFFS.begin();
    File configFile = SPIFFS.open(_fileName, FILE_WRITE);

    GetConfig();

    if (serializeJson(_configJson, configFile))
    {
        debug_println(F("config saved"));
    }
    else
    {
        debug_println(F("config not saved"));
    }

    configFile.close();
    SPIFFS.end();

    return true;
}
