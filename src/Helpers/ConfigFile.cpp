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
        cJSON_Delete(json);
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
            _carState->CONSUMPTION_UNIT = cJSON_GetObjectItem(aee2010, "CONSUMPTION_UNIT")->valueint;
            _carState->DISTANCE_UNIT = cJSON_GetObjectItem(aee2010, "DISTANCE_UNIT")->valueint;
            _carState->LANGUAGE = cJSON_GetObjectItem(aee2010, "LANGUAGE")->valueint;
            _carState->VOLUME_UNIT = cJSON_GetObjectItem(aee2010, "VOLUME_UNIT")->valueint;
            _carState->TEMPERATURE_UNIT = cJSON_GetObjectItem(aee2010, "TEMPERATURE_UNIT")->valueint;
            _carState->AMBIENCE_LEVEL = cJSON_GetObjectItem(aee2010, "AMBIENCE_LEVEL")->valueint;
            _carState->SOUND_HARMONY = cJSON_GetObjectItem(aee2010, "SOUND_HARMONY")->valueint;
            _carState->REPLACE_REMOTE_MODE_BTN_WITH_SRC = cJSON_GetObjectItem(aee2010, "REPLACE_REMOTE_MODE_BTN_WITH_SRC")->valueint;
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
    cJSON_AddNumberToObject(aee2010, "CONSUMPTION_UNIT", 0);
    cJSON_AddNumberToObject(aee2010, "DISTANCE_UNIT", 0);
    cJSON_AddNumberToObject(aee2010, "LANGUAGE", 0b00001);
    cJSON_AddNumberToObject(aee2010, "VOLUME_UNIT", 0);
    cJSON_AddNumberToObject(aee2010, "TEMPERATURE_UNIT", 0);
    cJSON_AddNumberToObject(aee2010, "AMBIENCE_LEVEL", 0b110);
    cJSON_AddNumberToObject(aee2010, "SOUND_HARMONY", 0b00);
    cJSON_AddBoolToObject(aee2010, "REPLACE_REMOTE_MODE_BTN_WITH_SRC", _carState->REPLACE_REMOTE_MODE_BTN_WITH_SRC);

    return std::unique_ptr<cJSON, cJSONDeleter>(root);
}

int ConfigFile::getJsonInt(cJSON *json, const char *key, int defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(json, key);
    if (item != NULL && cJSON_IsNumber(item))
    {
        //printf("Key: %s, Value: %d\n", key, item->valueint);
        return item->valueint;
    }
    //printf("Key: %s not found, returning default value: %d\n", key, defaultValue);
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