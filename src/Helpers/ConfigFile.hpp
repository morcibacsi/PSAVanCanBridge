#pragma once

#include <string>
#include <memory>
#include "cJSON.h"
#include "CarState.hpp"

struct cJSONDeleter {
    void operator()(cJSON* ptr) const {
        cJSON_Delete(ptr);
    }
};

class ConfigFile {
    static constexpr uint8_t VIN_LENGTH = 17;
    const char *_settingsFileName = "/littlefs/settings.json";

    private:

        CarState* _carState;
        int getJsonInt(cJSON *json, const char *key, int defaultValue);
        bool getJsonBool(cJSON *json, const char *key, bool defaultValue);
        std::unique_ptr<cJSON, cJSONDeleter> LoadFromFile();
    public:
        ConfigFile(CarState* carState);
        ~ConfigFile() = default;
        void Write();
        bool Read();
        void Remove();
        void SaveJson(const char *json_str);
        std::unique_ptr<cJSON, cJSONDeleter> GetAsJson();
};