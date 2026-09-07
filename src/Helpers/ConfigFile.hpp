#pragma once

#include <string>
#include <memory>
#include "cJSON.h"
#include "CarState.hpp"
#include "IConfigStore.hpp"

struct cJSONDeleter {
    void operator()(cJSON* ptr) const {
        cJSON_Delete(ptr);
    }
};

class ConfigFile : public IConfigStore {
    static constexpr uint8_t VIN_LENGTH = 17;
    std::string _settingsFileName;

    private:

        CarState* _carState;
        bool getJsonBool(cJSON *json, const char *key, bool defaultValue);
        std::unique_ptr<cJSON, cJSONDeleter> LoadFromFile();
    public:
        explicit ConfigFile(CarState* carState, const char* settingsFileName = "/littlefs/settings.json");
        ~ConfigFile() = default;
        void Write() override;
        bool Read();
        void Remove();
        void SaveJson(const char *json_str);
        std::unique_ptr<cJSON, cJSONDeleter> GetAsJson();
        uint64_t getJsonInt(cJSON *json, const char *key, uint64_t defaultValue);
        void cJSON_AddUInt64Smart(cJSON *json, const char *key, uint64_t value);
};
