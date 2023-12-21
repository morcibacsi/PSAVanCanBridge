#include "../../Config.h"

#ifdef WIFI_ENABLED

#ifndef ApiEndpoint_H
    #define ApiEndpoint_H

#include <inttypes.h>
#include "../Can/CanMessageHandlerContainer.h"
#include "../Can/Handlers/TpMessageHandler_760.h"
#include "../Helpers/ConfigStorageEsp32.h"
#include "../Helpers/IVinFlashStorage.h"

// uses https://github.com/fhessel/esp32_https_server

// Includes for the server
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>

class ApiEndpoint
{
    private:
    public:
    ApiEndpoint(
        AsyncWebServer* webServer,
        CanMessageHandlerContainer* canMessageHandler,
        Config *config,
        ConfigStorageEsp32 *configStorage,
        IVinFlashStorage* vinFlashStorage,
        TpMessageHandler_760* radioDiag
        );
    virtual ~ApiEndpoint(){ }
};
#endif
#endif