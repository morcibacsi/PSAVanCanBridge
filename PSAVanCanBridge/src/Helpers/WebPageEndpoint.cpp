#include "WebPageEndpoint.h"

#ifdef WIFI_ENABLED
#include <ArduinoJson.h>
#include "WWWData.h"

void handleRootPage(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ESP_REACT_DATA_0, sizeof(ESP_REACT_DATA_0));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleData_1(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", ESP_REACT_DATA_1, sizeof(ESP_REACT_DATA_1));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleData_2(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "application/javascript", ESP_REACT_DATA_2, sizeof(ESP_REACT_DATA_2));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void handleData_3(AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/css", ESP_REACT_DATA_3, sizeof(ESP_REACT_DATA_3));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

WebPageEndpoint::WebPageEndpoint(AsyncWebServer* webServer)
{
    webServer->onNotFound([](AsyncWebServerRequest *request) {
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