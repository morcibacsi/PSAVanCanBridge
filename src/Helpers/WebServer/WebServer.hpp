#pragma once

#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_netif.h"
#include "../../Protocol/ISerial.hpp"
#include "../../Protocol/WebSocketSerial.hpp"
#include "../ConfigFile.hpp"
#include "../TimeProvider.hpp"
#include "../CarState.hpp"
#include "../PSADiag/PsaDiagLib.h"
#include "gzipped_webpage_data.h"
#include "../../Protocol/ImmediateSignal.hpp"

typedef esp_err_t (*my_httpd_handler_t)(httpd_req_t *req);

class WebServer {
  private:

    static constexpr int WIFI_INITIAL_TIMEOUT = 120;
    static constexpr int WIFI_AFTER_CONNECT_TIMEOUT = 30;
    static constexpr bool startInApMode = true;

    TimeProvider* _timeProvider = nullptr;
    ImmediateSignalCallback _immediateSignalCallback = nullptr;
    WebSocketSerial* _webSocketSerial = nullptr;
    PsaDiagLib* _psaDiag = nullptr;

    uint64_t _lastRequestTime = 0;
    uint64_t _inactivityTimeout = WIFI_INITIAL_TIMEOUT;
    bool _isRunning = false;

    httpd_handle_t server;
    const char* TAG = "WebServer";

  public:
    CarState* _carState = nullptr;
    ConfigFile* _configFile = nullptr;

    WebServer(
        CarState* carState,
        ConfigFile* configFile,
        TimeProvider* timeProvider,
        WebSocketSerial* webSocketSerial,
        ImmediateSignalCallback immediateSignalCallback
    ) : server(NULL) {
        _carState = carState;
        _configFile = configFile;
        _timeProvider = timeProvider;
        _webSocketSerial = webSocketSerial;
        _immediateSignalCallback = immediateSignalCallback;
    }

    void CreateWebServer();
    void StartApMode();
    void StartStationMode();
    void RegisterHandler(const char* uri, httpd_method_t method, my_httpd_handler_t handler, bool isWebSocket);
    void RegisterEndpoints();
    void UnRegisterEndpoints();
    esp_err_t StartWebServer();
    void StopWebServer();
    void StopWifi();
    httpd_handle_t GetServerHandle();
    void SetPsaDiagLib(PsaDiagLib* psaDiag);
    void Process();

    static void OnClose(httpd_handle_t hd, int sockfd);
    static esp_err_t get_html_page_handler(httpd_req_t *req);
    static esp_err_t get_time_handler(httpd_req_t *req);
    static esp_err_t get_reboot_handler(httpd_req_t *req);
    static esp_err_t get_vin_handler(httpd_req_t *req);
    static esp_err_t get_config_handler(httpd_req_t *req);
    static esp_err_t get_carstate_handler(httpd_req_t *req);

    static esp_err_t post_config_handler(httpd_req_t *req);
    static esp_err_t post_time_handler(httpd_req_t *req);
    static esp_err_t post_ota_update_handler(httpd_req_t *req);
    static esp_err_t post_network_monitor_handler(httpd_req_t *req);
    static esp_err_t post_carstate_handler(httpd_req_t *req);

    static esp_err_t options_handler(httpd_req_t *req);
    static esp_err_t ws_handler(httpd_req_t *req);
    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
};

