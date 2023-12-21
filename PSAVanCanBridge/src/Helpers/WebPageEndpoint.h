#include "../../Config.h"

#ifdef WIFI_ENABLED

#ifndef WebPageEndpoint_H
    #define WebPageEndpoint_H

#include <inttypes.h>

// Includes for the server
#include <ESPAsyncWebServer.h>

class WebPageEndpoint
{
    private:
    public:
    WebPageEndpoint(AsyncWebServer* webServer);
    virtual ~WebPageEndpoint(){ }
};
#endif
#endif