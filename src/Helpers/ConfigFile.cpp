#include "ConfigFile.hpp"
#include <stdio.h>
#include "esp_littlefs.h"

ConfigFile::ConfigFile(CarState* carState)
{
    _carState = carState;
}

void ConfigFile::SaveJson(const char *json_str)
{
    FILE *file = fopen(_settingsFileName, "w");
    if (file == NULL) {
        printf("Failed to open file %s for writing\n", _settingsFileName);
        return;
    }

    fprintf(file, "%s", json_str);
    fclose(file);
    printf("JSON saved to %s successfully.\n", _settingsFileName);
}

void ConfigFile::Write()
{
    auto jsonHandle = GetAsJson();
    printf("Reading config file ok\n");
    if (jsonHandle)
    {
        cJSON *json = jsonHandle.get();
        const char *jsonString = cJSON_Print(json);
        SaveJson(jsonString);
        free((void *)jsonString);
    }
}

bool ConfigFile::Read()
{
    bool result = false;
    printf("Reading config file 1\n");
    auto jsonHandle = LoadFromFile();
    if (jsonHandle)
    {
        printf("Reading config file 2\n");
        cJSON *vinArray = cJSON_GetObjectItem(jsonHandle.get(),"VIN");
        if (vinArray != NULL)
        {
            for (size_t i = 0; i < VIN_LENGTH; i++)
            {
                cJSON *element = cJSON_GetArrayItem(vinArray, i);
                if (element == NULL)
                {
                    _carState->VIN_FOR_HEADUNIT[i] = 'X';
                }
                else
                {
                    _carState->VIN_FOR_HEADUNIT[i] = element->valueint;
                }
            }
        }

        printf("Reading config file 3\n");

        cJSON *aee2010 = cJSON_GetObjectItem(jsonHandle.get(), "AEE2010");
        if (aee2010 != NULL)
        {
            _carState->CONSUMPTION_UNIT = getJsonInt(aee2010, "CONSUMPTION_UNIT", 0);
            _carState->DISTANCE_UNIT = getJsonInt(aee2010, "DISTANCE_UNIT", 0);
            _carState->LANGUAGE = getJsonInt(aee2010, "LANGUAGE", 1);
            _carState->VOLUME_UNIT = getJsonInt(aee2010, "VOLUME_UNIT", 0);
            _carState->TEMPERATURE_UNIT = getJsonInt(aee2010, "TEMPERATURE_UNIT", 0);
            _carState->AMBIENCE_LEVEL = getJsonInt(aee2010, "AMBIENCE_LEVEL", 6);
            _carState->SOUND_HARMONY = getJsonInt(aee2010, "SOUND_HARMONY", 0);
            _carState->TIME_FORMAT_24H = getJsonInt(aee2010, "TIME_FORMAT_24H", 1);
            _carState->REPLACE_REMOTE_MODE_BTN_WITH_SRC = getJsonBool(aee2010, "REPLACE_REMOTE_MODE_BTN_WITH_SRC", false);
            _carState->SPEED_SIGN_SPEED_TOLERANCE_PERCENT = getJsonInt(aee2010, "SPEED_SIGN_SPEED_TOLERANCE_PERCENT", 0);
            _carState->CONVERT_SPEED_SIGN_FOR_CMB_FROM_NAC = getJsonBool(aee2010, "CONVERT_SPEED_SIGN_FOR_CMB_FROM_NAC", false);
        }

        printf("Reading config file 4\n");

        _carState->FUEL_TANK_CAPACITY_IN_LITERS = getJsonInt(jsonHandle.get(), "FUEL_TANK_CAPACITY_IN_LITERS", 60);
        _carState->SOURCE_PROTOCOL = getJsonInt(jsonHandle.get(), "SOURCE_PROTOCOL", 1);
        _carState->DESTINATION_PROTOCOL = getJsonInt(jsonHandle.get(), "DESTINATION_PROTOCOL", 2);
        _carState->GENERATE_POPUP_FOR_DOOR_STATUS = getJsonBool(jsonHandle.get(), "GENERATE_POPUP_FOR_DOOR_STATUS", false);

        _carState->EMULATE_DISPLAY_ON_DESTINATION = getJsonBool(jsonHandle.get(), "EMULATE_DISPLAY_ON_DESTINATION", false);
        _carState->EMULATE_DISPLAY_ON_SOURCE = getJsonBool(jsonHandle.get(), "EMULATE_DISPLAY_ON_SOURCE", false);
        _carState->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS = getJsonBool(jsonHandle.get(), "USE_IGNITION_SIGNAL_FROM_SOURCE_BUS", true);
        _carState->ENABLE_PARKING_AID_SOUND_FROM_SPEAKER = getJsonBool(jsonHandle.get(), "ENABLE_PARKING_AID_SOUND_FROM_SPEAKER", true);
        _carState->ENABLE_REVERSE_CAMERA_ON_RTX = getJsonBool(jsonHandle.get(), "ENABLE_REVERSE_CAMERA_ON_RTX", false);
        _carState->SEND_AC_FAN_CHANGES_TO_DISPLAY = getJsonBool(jsonHandle.get(), "SEND_AC_FAN_CHANGES_TO_DISPLAY", false);
        _carState->SEND_AC_CHANGES_TO_DISPLAY = getJsonBool(jsonHandle.get(), "SEND_AC_CHANGES_TO_DISPLAY", 1);
        _carState->QUERY_AC_STATUS = getJsonBool(jsonHandle.get(), "QUERY_AC_STATUS", false);

        _carState->PARKING_AID_TYPE = getJsonInt(jsonHandle.get(), "PARKING_AID_TYPE", 0);
        _carState->RADIO_TYPE = getJsonInt(jsonHandle.get(), "RADIO_TYPE", 0);

        _carState->HAS_RTC = getJsonBool(jsonHandle.get(), "HAS_RTC", false);
        _carState->SEND_TIME = getJsonBool(jsonHandle.get(), "SEND_TIME", false);
        _carState->EMULATE_STEERING_WHEEL_CONTROLS_WITH_STALK = getJsonBool(jsonHandle.get(), "EMULATE_STEERING_WHEEL_CONTROLS_WITH_STALK", false);

        _carState->MILEAGE_AT_CMB_TRIP_RESET = getJsonInt(jsonHandle.get(), "MILEAGE_AT_CMB_TRIP_RESET", 0);

        printf("Reading config file 5\n");

        cJSON *aasRearCornerDistances = cJSON_GetObjectItem(jsonHandle.get(), "AAS_REAR_CORNER_DISTANCES");
        if (aasRearCornerDistances != NULL)
        {
            for (size_t i = 0; i < 4; i++)
            {
                cJSON *element = cJSON_GetArrayItem(aasRearCornerDistances, i);
                _carState->AAS_REAR_CORNER_DISTANCES[i] = element->valueint;
            }
        }

        printf("Reading config file 6\n");

        cJSON *aasRearDistances = cJSON_GetObjectItem(jsonHandle.get(), "AAS_REAR_DISTANCES");
        if (aasRearDistances != NULL)
        {
            for (size_t i = 0; i < 4; i++)
            {
                cJSON *element = cJSON_GetArrayItem(aasRearDistances, i);
                _carState->AAS_REAR_DISTANCES[i] = element->valueint;
            }
        }

        result = true;
    }

    return result;
}

void ConfigFile::Remove()
{
    FILE *file = fopen(_settingsFileName, "r");

    if (file) {
        fclose(file); // Close the file if it exists
        if (remove(_settingsFileName) == 0) {
            printf("File '%s' deleted successfully.\n", _settingsFileName);
        } else {
            perror("Error deleting the file");
        }
    } else {
        printf("File '%s' does not exist.\n", _settingsFileName);
    }
}

std::unique_ptr<cJSON, cJSONDeleter> ConfigFile::LoadFromFile()
{
    // Open the file for reading
    printf("Opening file for reading\n");

    FILE *file = fopen(_settingsFileName, "r");
    if (file == NULL) {
        printf("Failed to open file %s for reading\n", _settingsFileName);
        return NULL;
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Allocate memory to read the file
    char *buffer = static_cast<char*>(malloc(file_size + 1));
    if (buffer == NULL) {
        printf("Failed to allocate memory for file content\n");
        fclose(file);
        return NULL;
    }

    // Read file content into the buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0'; // Null-terminate the string

    // Close the file
    fclose(file);

    // Parse the JSON string
    cJSON *json = cJSON_Parse(buffer);
    if (json == NULL) {
        printf("Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
    } else {
        printf("JSON loaded and parsed successfully\n");
    }

    // Clean up the buffer
    free(buffer);

    return std::unique_ptr<cJSON, cJSONDeleter>(json);
}

std::unique_ptr<cJSON, cJSONDeleter> ConfigFile::GetAsJson()
{
    cJSON *root = cJSON_CreateObject();

    cJSON *supportedSourceProtocolsArray = cJSON_CreateArray();
    for (const auto &protocol : CarState::SOURCE_PROTOCOLS)
    {
        cJSON *element = cJSON_CreateNumber(static_cast<uint8_t>(protocol));
        cJSON_AddItemToArray(supportedSourceProtocolsArray, element);
    }
    cJSON_AddItemToObject(root, "SUPPORTED_SOURCE_PROTOCOLS", supportedSourceProtocolsArray);

    cJSON *supportedDestinationProtocolsArray = cJSON_CreateArray();
    for (const auto &protocol : CarState::DESTINATION_PROTOCOLS)
    {
        cJSON *element = cJSON_CreateNumber(static_cast<uint8_t>(protocol));
        cJSON_AddItemToArray(supportedDestinationProtocolsArray, element);
    }
    cJSON_AddItemToObject(root, "SUPPORTED_DESTINATION_PROTOCOLS", supportedDestinationProtocolsArray);

    cJSON *vinArray = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "VIN", vinArray);

    for (size_t i = 0; i < VIN_LENGTH; i++)
    {
        cJSON *element = cJSON_CreateNumber(_carState->VIN_FOR_HEADUNIT[i]);
        cJSON_AddItemToArray(vinArray, element);
    }

    cJSON_AddNumberToObject(root, "FUEL_TANK_CAPACITY_IN_LITERS", _carState->FUEL_TANK_CAPACITY_IN_LITERS);
    cJSON_AddNumberToObject(root, "SOURCE_PROTOCOL", _carState->SOURCE_PROTOCOL);
    cJSON_AddNumberToObject(root, "DESTINATION_PROTOCOL", _carState->DESTINATION_PROTOCOL);
    cJSON_AddBoolToObject(root, "GENERATE_POPUP_FOR_DOOR_STATUS", _carState->GENERATE_POPUP_FOR_DOOR_STATUS);

    cJSON_AddBoolToObject(root, "EMULATE_DISPLAY_ON_DESTINATION", _carState->EMULATE_DISPLAY_ON_DESTINATION);
    cJSON_AddBoolToObject(root, "EMULATE_DISPLAY_ON_SOURCE", _carState->EMULATE_DISPLAY_ON_SOURCE);
    cJSON_AddBoolToObject(root, "USE_IGNITION_SIGNAL_FROM_SOURCE_BUS", _carState->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS);
    cJSON_AddBoolToObject(root, "ENABLE_PARKING_AID_SOUND_FROM_SPEAKER", _carState->ENABLE_PARKING_AID_SOUND_FROM_SPEAKER);
    cJSON_AddBoolToObject(root, "ENABLE_REVERSE_CAMERA_ON_RTX", _carState->ENABLE_REVERSE_CAMERA_ON_RTX);
    cJSON_AddBoolToObject(root, "SEND_AC_FAN_CHANGES_TO_DISPLAY", _carState->SEND_AC_FAN_CHANGES_TO_DISPLAY);
    cJSON_AddBoolToObject(root, "SEND_AC_CHANGES_TO_DISPLAY", _carState->SEND_AC_CHANGES_TO_DISPLAY);
    cJSON_AddBoolToObject(root, "QUERY_AC_STATUS", _carState->QUERY_AC_STATUS);
    cJSON_AddBoolToObject(root, "HAS_RTC", _carState->HAS_RTC);
    cJSON_AddBoolToObject(root, "SEND_TIME", _carState->SEND_TIME);
    cJSON_AddBoolToObject(root, "EMULATE_STEERING_WHEEL_CONTROLS_WITH_STALK", _carState->EMULATE_STEERING_WHEEL_CONTROLS_WITH_STALK);
    cJSON_AddStringToObject(root, "DATETIME", "");

    cJSON_AddNumberToObject(root, "PARKING_AID_TYPE", _carState->PARKING_AID_TYPE);
    cJSON_AddNumberToObject(root, "RADIO_TYPE", _carState->RADIO_TYPE);

    cJSON_AddNumberToObject(root, "MILEAGE_AT_CMB_TRIP_RESET", _carState->MILEAGE_AT_CMB_TRIP_RESET);

    cJSON *aasRearCornerDistances = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "AAS_REAR_CORNER_DISTANCES", aasRearCornerDistances);
    for (size_t i = 0; i < 4; i++)
    {
        cJSON *element = cJSON_CreateNumber(_carState->AAS_REAR_CORNER_DISTANCES[i]);
        cJSON_AddItemToArray(aasRearCornerDistances, element);
    }

    cJSON *aasRearDistances = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "AAS_REAR_DISTANCES", aasRearDistances);
    for (size_t i = 0; i < 4; i++)
    {
        cJSON *element = cJSON_CreateNumber(_carState->AAS_REAR_DISTANCES[i]);
        cJSON_AddItemToArray(aasRearDistances, element);
    }

    cJSON *aee2010;
    cJSON_AddItemToObject(root, "AEE2010", aee2010=cJSON_CreateObject());
    cJSON_AddNumberToObject(aee2010, "CONSUMPTION_UNIT", _carState->CONSUMPTION_UNIT);
    cJSON_AddNumberToObject(aee2010, "DISTANCE_UNIT", _carState->DISTANCE_UNIT);
    cJSON_AddNumberToObject(aee2010, "LANGUAGE", _carState->LANGUAGE);
    cJSON_AddNumberToObject(aee2010, "VOLUME_UNIT", _carState->VOLUME_UNIT);
    cJSON_AddNumberToObject(aee2010, "TEMPERATURE_UNIT", _carState->TEMPERATURE_UNIT);
    cJSON_AddNumberToObject(aee2010, "AMBIENCE_LEVEL", _carState->AMBIENCE_LEVEL);
    cJSON_AddNumberToObject(aee2010, "SOUND_HARMONY", _carState->SOUND_HARMONY);
    cJSON_AddNumberToObject(aee2010, "TIME_FORMAT_24H", _carState->TIME_FORMAT_24H);
    cJSON_AddNumberToObject(aee2010, "SPEED_SIGN_SPEED_TOLERANCE_PERCENT", _carState->SPEED_SIGN_SPEED_TOLERANCE_PERCENT);
    cJSON_AddBoolToObject(aee2010, "REPLACE_REMOTE_MODE_BTN_WITH_SRC", _carState->REPLACE_REMOTE_MODE_BTN_WITH_SRC);
    cJSON_AddBoolToObject(aee2010, "CONVERT_SPEED_SIGN_FOR_CMB_FROM_NAC", _carState->CONVERT_SPEED_SIGN_FOR_CMB_FROM_NAC);

    return std::unique_ptr<cJSON, cJSONDeleter>(root);
}

uint64_t ConfigFile::getJsonInt(cJSON *json, const char *key, uint64_t defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(json, key);
    if (item == NULL)
    {
        return defaultValue;
    }
    if (item->valuestring != NULL) {
        return strtoull(item->valuestring, NULL, 10);
    }
    if (cJSON_IsNumber(item)) {
        return (uint64_t)item->valuedouble;
    }

    return defaultValue;
}

bool ConfigFile::getJsonBool(cJSON *json, const char *key, bool defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(json, key);
    if (item != NULL && cJSON_IsBool(item))
    {
        //printf("Key: %s, Value: %d\n", key, item->valueint);
        return item->valueint == 1;
    }
    //printf("Key: %s not found, returning default value: %d\n", key, defaultValue);
    return defaultValue;
}

// Helper: add any integer to cJSON correctly for Web UI
// - small values (<= 2^53) -> JSON number
// - large values (> 2^53) -> JSON string (decimal)
void ConfigFile::cJSON_AddUInt64Smart(cJSON *json, const char *key, uint64_t value)
{
    const uint64_t MAX_SAFE_JS_INT = 9007199254740992ULL; // 2^53

    if (value <= MAX_SAFE_JS_INT)
    {
        // safe to store as normal JSON number
        cJSON_AddNumberToObject(json, key, (double)value);
    }
    else
    {
        // too large, store as string so Web UI can parse as BigInt
        char buf[32];
        snprintf(buf, sizeof(buf), "%llu", (unsigned long long)value);
        cJSON_AddStringToObject(json, key, buf);
    }
}
