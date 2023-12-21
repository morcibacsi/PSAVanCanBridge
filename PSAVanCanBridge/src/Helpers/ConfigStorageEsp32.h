// ConfigStorageEsp32.h
#pragma once

#ifndef _ConfigStorageEsp32_h
    #define _ConfigStorageEsp32_h

#include "IConfigStorage.h"
#include "../../Config.h"
#include <ArduinoJson.h>

class ConfigStorageEsp32 : public IConfigStorage
{
    #define JSON_MEMORY_SIZE 3072

    Config* _config;
    const char* _fileName = "/config";
    StaticJsonDocument<JSON_MEMORY_SIZE> _configJson;

    public:
    ConfigStorageEsp32(Config* config);

    void Remove() override;

    bool Load() override;
    bool LoadFromDoc(DynamicJsonDocument doc);

    StaticJsonDocument<JSON_MEMORY_SIZE> GetConfig();

    bool Save() override;
};

#endif
