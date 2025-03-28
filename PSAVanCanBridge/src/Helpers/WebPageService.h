#include "../../Config.h"

#ifdef WIFI_ENABLED

#ifndef WebPageService_H
    #define WebPageService_H

#include <inttypes.h>

#include "ApiEndpoint.h"
#include "WebPageEndpoint.h"
#include "../Can/CanMessageHandlerContainer.h"
#include "../Can/Handlers/TpMessageHandler_760.h"
#include "../Helpers/ConfigStorageEsp32.h"
#include "../Helpers/IVinFlashStorage.h"

// Includes for the server
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

class WebPageService
{
    private:
    bool _serverStarted = false;
    CanMessageHandlerContainer* _canMessageHandler;
    Config* _config;
    ConfigStorageEsp32* _configStorage;
    IVinFlashStorage* _vinFlashStorage;
    TpMessageHandler_760* _radioDiag;

    DNSServer dnsServer;
    AsyncWebServer* webServer;
    ApiEndpoint* apiEndpoint;
    WebPageEndpoint* webPageEndpoint;

    void StartWifiAP();
    void StartWifiClient();

    public:
    WebPageService(
        CanMessageHandlerContainer* canMessageHandler,
        Config *config,
        ConfigStorageEsp32 *configStorage,
        IVinFlashStorage* vinFlashStorage,
        TpMessageHandler_760* radioDiag
        );
    virtual ~WebPageService(){ }
    void Start();
    AsyncWebServer* GetHTTPServer();
    void Loop();
    bool IsRunning();
};
#endif
#endif