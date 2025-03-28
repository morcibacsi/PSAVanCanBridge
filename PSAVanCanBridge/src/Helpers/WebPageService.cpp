#include "WebPageService.h"

#ifdef WIFI_ENABLED
#include "../SerialPort/WebSocketSerialAbs.h"
#include "../SerialPort/AbstractSerial.h"
#include "DebugPrint.h"

#include <WiFi.h>
#include <DNSServer.h>

static bool _wifiStarted = false;

IPAddress apIP(192, 168, 100, 1);
const char *hosturl = "http://192.168.100.1";

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    /*
    Serial.print("WiFiEvent: ");
    Serial.println(event);
    */
    if (event == ARDUINO_EVENT_WIFI_STA_START || event == ARDUINO_EVENT_WIFI_AP_START)
    {
        _wifiStarted = true;
    }
    if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP || event == ARDUINO_EVENT_ETH_GOT_IP)
    {
        Serial.print("Obtained IP address: ");
        Serial.println(WiFi.localIP());
    }
    if (event == ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED)
    {
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    }
    if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED)
    {
        Serial.println("Client (AP mode) connected to Wi-Fi");
    }
    if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED)
    {
        Serial.println("Client (AP mode) disconnected from Wi-Fi, restarting ESP32");
        esp_restart();
    }
//
/*
    debug_print("[WiFi-event] event: ");
    debug_println(event);

    switch (event) {
        case ARDUINO_EVENT_WIFI_READY:
            debug_println("WiFi interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            debug_println("Completed scan for access points");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            debug_println("WiFi client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            debug_println("WiFi clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            debug_println("Connected to access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            debug_println("Disconnected from WiFi access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            debug_println("Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            debug_print("Obtained IP address: ");
            debug_println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            debug_println("Lost IP address and IP address is reset to 0");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            debug_println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            debug_println("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            debug_println("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            debug_println("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PBC_OVERLAP:
            debug_println("ESP32 station wps overlap in enrollee mode");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            debug_println("WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            debug_println("WiFi access point  stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            debug_println("Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            debug_println("Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            debug_println("Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            debug_println("Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            debug_println("AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            debug_println("Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            debug_println("Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            debug_println("Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            debug_println("Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            debug_println("Obtained IP address");
            break;
        default: break;
    }
//*/
}

WebPageService::WebPageService(
    CanMessageHandlerContainer* canMessageHandler,
    Config *config,
    ConfigStorageEsp32 *configStorage,
    IVinFlashStorage* vinFlashStorage,
    TpMessageHandler_760* radioDiag
    )
{
    _config = config;
    _configStorage = configStorage;
    _canMessageHandler = canMessageHandler;
    _vinFlashStorage = vinFlashStorage;
    _radioDiag = radioDiag;

    WiFi.onEvent(WiFiEvent);
}

void WebPageService::StartWifiAP()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_config->WIFI_AP_NAME, _config->WIFI_PASSWORD);

    delay(100);

    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    //dnsServer.start(53, "*", apIP);
}

void WebPageService::StartWifiClient()
{
    WiFi.begin(_config->WIFI_SSID, _config->WIFI_PASSWORD);
}

void WebPageService::Start()
{
    if (_config->WIFI_AP_MODE)
    {
        StartWifiAP();
    }
    else
    {
        StartWifiClient();
    }
}

void WebPageService::Loop()
{
    if (_wifiStarted && !_serverStarted)
    {
        //debug_println(WiFi.localIP());
        webServer = new AsyncWebServer(80);

        apiEndpoint = new ApiEndpoint(webServer, _canMessageHandler, _config, _configStorage, _vinFlashStorage, _radioDiag);
        webPageEndpoint = new WebPageEndpoint(webServer);
        webServer->begin();
        _serverStarted = true;

        //debug_println("WebPageService started");
    }

    if (_serverStarted)
    {
        if (_config->WIFI_AP_MODE)
        {
            //dnsServer.processNextRequest();
        }
    }
}

bool WebPageService::IsRunning()
{
    return _wifiStarted || _serverStarted;
}

AsyncWebServer* WebPageService::GetHTTPServer()
{
    return webServer;
}
#endif