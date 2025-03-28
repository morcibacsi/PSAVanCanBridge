#include "WebPageEndpoint.h"

#ifdef WIFI_ENABLED
#include <ArduinoJson.h>
#include "WWWData.h"

void sendResponse(AsyncWebServerRequest* request, AsyncWebServerResponse *response)
{
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "-1");
    request->send(response);
}

void handleRootPage(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", ESP_REACT_DATA_0, sizeof(ESP_REACT_DATA_0));
    sendResponse(request, response);
}

void handleData_1(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "application/javascript", ESP_REACT_DATA_1, sizeof(ESP_REACT_DATA_1));
    sendResponse(request, response);
}

void handleData_2(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "application/javascript", ESP_REACT_DATA_2, sizeof(ESP_REACT_DATA_2));
    sendResponse(request, response);
}

void handleData_3(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse(200, "text/css", ESP_REACT_DATA_3, sizeof(ESP_REACT_DATA_3));
    sendResponse(request, response);
}

WebPageEndpoint::WebPageEndpoint(AsyncWebServer* webServer)
{
    webServer->onNotFound([](AsyncWebServerRequest *request) {
        //Serial.println(request->url());
        request->send(404, "application/json", "{\"message\":\"Endpoint not found\"}");
        //request->redirect("/");
    });

    //This is an example of triggering for a known location. This one seems to be common for android devices
    webServer->on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){
        handleRootPage(request);
    });

    webServer->on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleRootPage(request);
    });

    webServer->on("/index.html", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleRootPage(request);
    });
    webServer->on("/scripts/main.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleData_1(request);
    });
    webServer->on("/scripts/vendor.js", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleData_2(request);
    });
    webServer->on("/styles/app.css", HTTP_GET, [](AsyncWebServerRequest* request) {
        handleData_3(request);
    });
}
#endif