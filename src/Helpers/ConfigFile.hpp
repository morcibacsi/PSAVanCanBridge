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

        std::shared_ptr<CarState> _carState;

    public:
        ConfigFile(std::shared_ptr<CarState> carState);
        ~ConfigFile() = default;
        void Write();
        bool Read();
        void Remove();
        void Test();
        void SaveJson(const char *json_str);
        std::unique_ptr<cJSON, cJSONDeleter> GetAsJson();
};