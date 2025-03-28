#include "ApiEndpoint.h"

#ifdef WIFI_ENABLED
#include <ArduinoJson.h>
#include "../Can/Handlers/MessageHandler_3E5.h"
#include "DebugPrint.h"
#include <Update.h>

static CanMessageHandlerContainer* _canMessageHandler;
static Config* _config;
static ConfigStorageEsp32 *_configStorage;
static IVinFlashStorage* _vinFlashStorage;
static TpMessageHandler_760* _radioDiag;

void sendResponse(AsyncWebServerRequest* request, bool success, int8_t failedAt, uint8_t trace)
{
    //int responseCapacity = 48;
    int responseCapacity = JSON_MEMORY_SIZE;
    DynamicJsonDocument jsonResponse(responseCapacity);
    if (success)
    {
        jsonResponse[F("success")] = F("OK");
    }
    else
    {
        jsonResponse[F("success")] = F("FAILED");
        jsonResponse[F("failedAt")] = failedAt;
        jsonResponse[F("trace")] = trace;
    }

    // Print to response
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    serializeJson(jsonResponse, *response);
    request->send(response);
}

void handleDisplayControlButtonPostEvent(AsyncWebServerRequest* request, uint8_t *data, size_t len)
{
    int requestCapacity = 48;

    // Create buffer to read request
    char * buffer = new char[requestCapacity];
    memset(buffer, 0, requestCapacity);

    // Try to read request into buffer
    for (size_t i = 0; i < len; i++)
    {
        buffer[i] = data[i];
    }

    DynamicJsonDocument jsonRequest(requestCapacity);
    deserializeJson(jsonRequest, buffer);
    int buttonId = jsonRequest["button"];

    switch (buttonId)
    {
        case 0:
            _config->BRIDGE_RADIO_DATA = false;
        break;
        case 1:
            _config->BRIDGE_RADIO_DATA = true;
        break;

        default:
            MessageHandler_3E5* messageHandler_3E5 = static_cast<MessageHandler_3E5*>(_canMessageHandler->GetHandler(0x3E5));
            if (messageHandler_3E5)
            {
                messageHandler_3E5->SendButton(millis(), static_cast<RadioButton>(buttonId));
            }
        break;
    }

    sendResponse(request, true, 0, 0);
}

void handleConfigPostEvent(AsyncWebServerRequest* request, uint8_t *data, size_t len)
{
    int requestCapacity = JSON_MEMORY_SIZE;

    // Create buffer to read request
    char * buffer = new char[requestCapacity];
    memset(buffer, 0, requestCapacity);

    // Try to read request into buffer
    for (size_t i = 0; i < len; i++)
    {
        buffer[i] = data[i];
    }

    uint8_t saveStatus = 0;
    int8_t failedAt = -1;
    uint8_t trace = 0;
    uint8_t firstByte = 0;
    uint8_t lastByte = 0;

    DynamicJsonDocument jsonRequest(JSON_MEMORY_SIZE);
    if (deserializeJson(jsonRequest, buffer) == DeserializationError::Ok)
    {
        trace = 5;
        _configStorage->LoadFromDoc(jsonRequest);

        trace = 6;
        _configStorage->Save();
        _vinFlashStorage->Save();

        trace = 7;
        saveStatus = 1;
    }
    else
    {
        failedAt = -2;
    }

    if (saveStatus == 1)
    {
        sendResponse(request, true, 0, 0);
    }
    else
    {
        sendResponse(request, false, failedAt, trace);
    }
}

void handleConfigGetEvent(AsyncWebServerRequest* request)
{
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    DynamicJsonDocument jsonResponse(JSON_MEMORY_SIZE);
    jsonResponse = _configStorage->GetConfig();

    // Print to response
    serializeJson(jsonResponse, *response);
    request->send(response);
}

void handleConfigDeleteEvent(AsyncWebServerRequest* request)
{
    _configStorage->Remove();

    sendResponse(request, true, 0, 0);
}

void handleUniqueIdPostEvent(AsyncWebServerRequest* request, uint8_t *data, size_t len)
{
    int requestCapacity = 1024;

    // Create buffer to read request
    char * buffer = new char[requestCapacity];
    memset(buffer, 0, requestCapacity);

    // Try to read request into buffer
    for (size_t i = 0; i < len; i++)
    {
        buffer[i] = data[i];
    }

    uint8_t uniqueId[10] = { 0 };

    DynamicJsonDocument jsonResponse(JSON_MEMORY_SIZE);

    JsonArray uniqueIdInJson = jsonResponse.createNestedArray("UNIQUE_ID");
    for (size_t i = 0; i < 10; i++)
    {
        uniqueIdInJson.add(uniqueId[i]);
    }

    // Print to response
    AsyncResponseStream* response = request->beginResponseStream("application/json");
    serializeJson(jsonResponse, *response);
    request->send(response);
}

void handleVinGetEvent(AsyncWebServerRequest* request)
{
    if (_config->RADIO_TYPE == 1)
    {
        _radioDiag->SetRadioType(CAN_DIAG_RADIO_RD4_RD43);
    }
    if (_config->RADIO_TYPE == 2 || _config->RADIO_TYPE == 3)
    {
        _radioDiag->SetRadioType(CAN_DIAG_RADIO_RD45);
    }

    _radioDiag->EnterDiagMode();

    sendResponse(request, true, 0, 0);
}

void handleOtaUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index){
        Serial.println("Update");
        // if filename includes spiffs, update the spiffs partition
        int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
            Update.printError(Serial);
        }
    }

    if (Update.write(data, len) != len) {
        Update.printError(Serial);
    }

    if (final) {
        //AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
        //response->addHeader("Refresh", "20");
        //response->addHeader("Location", "/");
        //request->send(response);
        sendResponse(request, true, 0, 0);

        if (!Update.end(true)){
            Update.printError(Serial);
        }
        else
        {
            Serial.println("Update complete");
            Serial.flush();
            ESP.restart();
        }
    }
}

void onRequestBody(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total)
{
    if (request->url() == "/api/config") {
        handleConfigPostEvent(request, data, len);
    }

    if (request->url() == "/api/uniqueId") {
        handleUniqueIdPostEvent(request, data, len);
    }

    if (request->url() == "/api/displaycontrol") {
        handleDisplayControlButtonPostEvent(request, data, len);
    }
}

void handlePost(AsyncWebServerRequest* request)
{
    // This function is not used, but it is required to be defined for the web server to work properly.
    // It is called when a POST request is made to the server.
    // The actual handling of the POST request is done in the onRequestBody function.
    // request->send(200, "text/plain", "OK");
}

ApiEndpoint::ApiEndpoint(
    AsyncWebServer* webServer,
    CanMessageHandlerContainer* canMessageHandler,
    Config *config,
    ConfigStorageEsp32 *configStorage,
    IVinFlashStorage* vinFlashStorage,
    TpMessageHandler_760* radioDiag
    )
{
    _canMessageHandler = canMessageHandler;
    _config = config;
    _configStorage = configStorage;
    _vinFlashStorage = vinFlashStorage;
    _radioDiag = radioDiag;

    webServer->on("/api/config", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleConfigGetEvent(request);
    });

    webServer->on("/api/config",HTTP_POST, handlePost, nullptr, onRequestBody);

    webServer->on("/api/config", HTTP_DELETE, [](AsyncWebServerRequest* request) {
        handleConfigDeleteEvent(request);
    });

    webServer->on("/api/getVin", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleVinGetEvent(request);
    });

    webServer->on("/otaupdate", HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
        size_t len, bool final) {
            handleOtaUpdate(request, filename, index, data, len, final);
        }
    );

    webServer->on("/api/reboot", HTTP_GET, [](AsyncWebServerRequest* request) {
        sendResponse(request, true, 0, 0);
        ESP.restart();
    });

    webServer->onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        onRequestBody(request, data, len, index, total);
    });
}
#endif