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

typedef esp_err_t (*my_httpd_handler_t)(httpd_req_t *req);
const int WIFI_INITIAL_TIMEOUT = 120;
const int WIFI_AFTER_CONNECT_TIMEOUT = 7;

static bool webServerCanBeStarted = false;

const bool startInApMode = true;
const char* STA_WIFI_SSID = "ssid";
const char* STA_WIFI_PASSWORD = "password";

class WebServer {
public:
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
        // Initialize NVS and Wi-Fi
    void CreateWebServer()
    {
        // Initialize NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        // Initialize the Wi-Fi driver
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        if (startInApMode)
        {
            StartApMode();
            startWebServer();
        }
        else
        {
            StartStationMode();

            ESP_ERROR_CHECK(esp_event_handler_instance_register(
                                WIFI_EVENT,
                                ESP_EVENT_ANY_ID,
                                &wifi_event_handler,
                                NULL,
                                NULL));

            ESP_ERROR_CHECK(esp_event_handler_instance_register(
                                IP_EVENT,
                                IP_EVENT_STA_GOT_IP,
                                &ip_event_handler,
                                NULL,
                                NULL));
        }

        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(8));
    }

    void StartApMode()
    {
        esp_netif_t *netif = esp_netif_create_default_wifi_ap();

        //Set the IP address of the AP
        ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));
        // Configure the static IP settings
        esp_netif_ip_info_t ip_info;
        ip_info.ip.addr = esp_ip4addr_aton("192.168.100.1");
        ip_info.netmask.addr = esp_ip4addr_aton("255.255.255.0");
        ip_info.gw.addr = esp_ip4addr_aton("192.168.100.1");

        // Set the IP information to the network interface
        ESP_ERROR_CHECK(esp_netif_set_ip_info(netif, &ip_info));

        // Restart the DHCP server (if required)
        ESP_ERROR_CHECK(esp_netif_dhcps_start(netif));

        // Initialize Wi-Fi
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        cfg.nvs_enable = true;
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        // Set Wi-Fi to station mode
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

        // Configure the Wi-Fi connection
        wifi_config_t wifi_config = {};
        wifi_config = {
            .ap = {
                .ssid = "PSA VAN-CAN Bridge",
                .password = "123456789",
                .ssid_len = strlen("PSA VAN-CAN Bridge"),
                .channel = 1,
                .authmode = WIFI_AUTH_WPA_PSK,
                .max_connection = 4,
            }
        };

        // Set Wi-Fi configuration and start Wi-Fi
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    }

    void StartStationMode()
    {
        esp_netif_t *netif = esp_netif_create_default_wifi_sta();

        // Initialize Wi-Fi
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        cfg.nvs_enable = true;
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        // Set Wi-Fi to station mode
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

        // Configure the Wi-Fi connection
        wifi_config_t wifi_config = {};
        strncpy((char*)wifi_config.sta.ssid, STA_WIFI_SSID, sizeof(wifi_config.sta.ssid) - 1);
        strncpy((char*)wifi_config.sta.password, STA_WIFI_PASSWORD, sizeof(wifi_config.sta.password) - 1);

        // Set Wi-Fi configuration and start Wi-Fi
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    }

    void RegisterHandler(const char* uri, httpd_method_t method, my_httpd_handler_t handler, bool isWebSocket = false)
    {
        httpd_uri_t uri_t = {
            .uri = uri,
            .method = method,
            .handler = handler,
            .user_ctx = this,
            .is_websocket = isWebSocket,
            .handle_ws_control_frames = false,
            .supported_subprotocol = NULL
        };
        httpd_register_uri_handler(server, &uri_t);
    }

    void RegisterEndpoints()
    {
        RegisterHandler("/ws", HTTP_GET, &WebServer::ws_handler, true);
        RegisterHandler("/", HTTP_GET, &WebServer::get_index_handler);
        RegisterHandler("/index.html", HTTP_GET, &WebServer::get_index_handler);
        RegisterHandler("/api/time", HTTP_GET, &WebServer::get_time_handler);
        RegisterHandler("/api/reboot", HTTP_GET, &WebServer::get_reboot_handler);
        RegisterHandler("/api/getVin", HTTP_GET, &WebServer::get_vin_handler);
        RegisterHandler("/api/config.json", HTTP_GET, &WebServer::get_config_handler);
        RegisterHandler("/api/config", HTTP_POST, &WebServer::post_config_handler);
        RegisterHandler("/api/time", HTTP_POST, &WebServer::post_time_handler);
    }

    void UnRegisterEndpoints()
    {
        // Unregister all endpoints
        httpd_unregister_uri_handler(server, "/", HTTP_GET);
        httpd_unregister_uri_handler(server, "/index.html", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/time", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/reboot", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/getVin", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/config.json", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/config", HTTP_POST);
        httpd_unregister_uri_handler(server, "/api/time", HTTP_POST);
        httpd_unregister_uri_handler(server, "/ws", HTTP_GET);
    }

    // Start web server
    esp_err_t startWebServer()
    {
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.uri_match_fn = httpd_uri_match_wildcard;
        config.stack_size = 8192;
        config.max_uri_handlers = 10;
        config.global_user_ctx = this;
        config.close_fn = WebServer::OnClose;

        if (httpd_start(&server, &config) == ESP_OK)
        {
            RegisterEndpoints();

            printf("Web server started\n");
            _isRunning = true;
            _lastRequestTime = _carState->CurrenTime;
            _inactivityTimeout = WIFI_INITIAL_TIMEOUT;

            _webSocketSerial->OnWebServerStarted(server);
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Failed to start the web server");
        return ESP_FAIL;
    }

    static void OnClose(httpd_handle_t hd, int sockfd)
    {
        auto *instance = static_cast<WebServer *>(httpd_get_global_user_ctx(hd));

        if (instance && instance->_webSocketSerial)
        {
            instance->_webSocketSerial->OnClientDisconnected(sockfd);
        }
    }

    // Stop web server
    void stopWebServer()
    {
        if (server)
        {
            UnRegisterEndpoints();
            httpd_stop(server);
            server = nullptr;
            ESP_LOGI(TAG, "Web server stopped");
        }
    }

    // Stop Wi-Fi
    void stopWifi()
    {
        ESP_ERROR_CHECK(esp_wifi_stop());
        ESP_ERROR_CHECK(esp_wifi_deinit());
        ESP_LOGI(TAG, "Wi-Fi stopped");
    }

    httpd_handle_t GetServerHandle()
    {
        return server;
    }

    void SetPsaDiagLib(PsaDiagLib* psaDiag)
    {
        _psaDiag = psaDiag;
    }

    static esp_err_t get_index_handler(httpd_req_t *req)
    {
        // Set the appropriate headers for gzipped content
        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_lastRequestTime = instance->_carState->CurrenTime;
        instance->_inactivityTimeout = WIFI_AFTER_CONNECT_TIMEOUT;

        httpd_resp_set_type(req, "text/html");
        httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

        // Send the data from PROGMEM
        size_t data_size = sizeof(ESP_REACT_DATA_0);
        httpd_resp_send(req, (const char*)ESP_REACT_DATA_0, data_size);
        return ESP_OK;
    }

    static esp_err_t get_time_handler(httpd_req_t *req)
    {
        // Send a 200 OK response with no content
        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_lastRequestTime = instance->_carState->CurrenTime;

        httpd_resp_set_type(req, "application/json");

        cJSON *json = cJSON_CreateObject();
        cJSON_AddNumberToObject(json, "hour", instance->_carState->Hour);
        cJSON_AddNumberToObject(json, "minute", instance->_carState->Minute);
        cJSON_AddNumberToObject(json, "second", instance->_carState->Second);
        cJSON_AddNumberToObject(json, "day", instance->_carState->MDay);
        cJSON_AddNumberToObject(json, "month", instance->_carState->Month);
        cJSON_AddNumberToObject(json, "year", instance->_carState->Year);
        cJSON_AddStringToObject(json, "firmware_version", instance->_carState->Version);
        const char *jsonResponse = cJSON_Print(json);
        cJSON_Delete(json);
        httpd_resp_sendstr(req, jsonResponse);

        return ESP_OK;
    }

    static esp_err_t get_reboot_handler(httpd_req_t *req)
    {
        // Send a 200 OK response with no content
        httpd_resp_send(req, NULL, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_restart();
        return ESP_OK;
    }

    static esp_err_t get_vin_handler(httpd_req_t *req)
    {
        // Send a 200 OK response with no content
        httpd_resp_send(req, NULL, 0);
        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_carState->RADIO_TYPE = req->uri[strlen(req->uri) - 1] - '0';

        printf("Request VIN read, radio type: %d\n", instance->_carState->RADIO_TYPE);

        if (instance->_immediateSignalCallback)
        {
            instance->_immediateSignalCallback(ImmediateSignal::StartVinRead);
        }
        return ESP_OK;
    }

    static esp_err_t get_config_handler(httpd_req_t *req)
    {
        printf("GET /api/config\n");

        auto *instance = static_cast<WebServer *>(req->user_ctx);
        auto _configFile = instance->_configFile;

        if (!_configFile)
        {
            printf("Config file is null\n");
            return ESP_FAIL;
        }
        else
        {
            printf("Config file is not null\n");
        }

        httpd_resp_set_type(req, "application/json");

        printf("Reading config file\n");
        auto jsonHandle = _configFile->GetAsJson();
        printf("Reading config file ok\n");
        if (jsonHandle)
        {
            cJSON *json = jsonHandle.get();
            const char *jsonString = cJSON_Print(json);
            httpd_resp_sendstr(req, jsonString);
            free((void *)jsonString);
        }
        else
        {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read config file");
        }
        return ESP_OK;
    }

    static esp_err_t post_config_handler(httpd_req_t *req)
    {
        printf("POST /api/config\n");
        char *content = (char *)malloc(req->content_len + 1);
        int ret, remaining = req->content_len;
        printf("Content length: %d\n", remaining);

        if (!content) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory");
            return ESP_FAIL;
        }
        while (remaining > 0)
        {
            ret = httpd_req_recv(req, content, remaining);
            if (ret <= 0)
            {
                if (ret == HTTPD_SOCK_ERR_TIMEOUT)
                {
                    httpd_resp_send_408(req);
                }
                free(content);
                return ESP_FAIL;
            }
            remaining -= ret;
        }
        content[req->content_len] = '\0'; // Null-terminate the received data

        //printf("Received config: %s\n", content);
        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_configFile->SaveJson(content);
        instance->_configFile->Read();
        httpd_resp_set_status(req, "200 OK");
        httpd_resp_sendstr(req, "Config saved");

        free(content);
        return ESP_OK;
    }

    static esp_err_t post_time_handler(httpd_req_t *req)
    {
        char content[100];
        int ret, remaining = req->content_len;
        if (remaining > sizeof(content))
        {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content too large");
            return ESP_FAIL;
        }
        while (remaining > 0)
        {
            ret = httpd_req_recv(req, content, sizeof(content));
            if (ret <= 0)
            {
                if (ret == HTTPD_SOCK_ERR_TIMEOUT)
                {
                    httpd_resp_send_408(req);
                }
                return ESP_FAIL;
            }
            remaining -= ret;
        }
        cJSON *root = cJSON_Parse(content);
        if (!root)
        {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
            return ESP_FAIL;
        }

        cJSON *jsonObj = cJSON_GetObjectItem(root, "year");
        double year = cJSON_GetNumberValue(jsonObj);

        jsonObj = cJSON_GetObjectItem(root, "month");
        double month = cJSON_GetNumberValue(jsonObj);

        jsonObj = cJSON_GetObjectItem(root, "day");
        double day = cJSON_GetNumberValue(jsonObj);

        jsonObj = cJSON_GetObjectItem(root, "hour");
        double hour = cJSON_GetNumberValue(jsonObj);

        jsonObj = cJSON_GetObjectItem(root, "minute");
        double minute = cJSON_GetNumberValue(jsonObj);

        printf("year: %d\n", (int)year);
        printf("month: %d\n", (int)month);
        printf("day: %d\n", (int)day);
        printf("hour: %d\n", (int)hour);
        printf("minute: %d\n", (int)minute);

        cJSON_Delete(root);

        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_timeProvider->SetDateTime((int)year, (int)month, (int)day, (int)hour, (int)minute, 0);

        httpd_resp_set_status(req, "200 OK");
        httpd_resp_sendstr(req, "Time saved");
        httpd_resp_set_type(req, "application/json");

        return ESP_OK;
    }

    static esp_err_t ws_handler(httpd_req_t *req)
    {
        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_lastRequestTime = instance->_carState->CurrenTime;

        if (req->method == HTTP_GET)
        {
            int sock = httpd_req_to_sockfd(req);

            if (instance->_webSocketSerial)
            {
                instance->_webSocketSerial->OnClientConnected(sock);
            }

            return ESP_OK;
        }

        httpd_ws_frame_t frame = {};
        frame.type = HTTPD_WS_TYPE_TEXT;

        esp_err_t ret = httpd_ws_recv_frame(req, &frame, 0);

        if (ret != ESP_OK)
            return ret;

        if (frame.len == 0)
            return ESP_OK;

        frame.payload = (uint8_t*)malloc(frame.len);
        if (!frame.payload)
            return ESP_ERR_NO_MEM;

        ret = httpd_ws_recv_frame(req, &frame, frame.len);

        for (size_t i = 0; i < frame.len; i++)
        {
            //printf("%02X ", frame.payload[i]);
            printf("%c", frame.payload[i]);
        }
        printf("\n");
        //printf("ret: %d, len: %d\n", ret, frame.len);

        if (ret == ESP_OK && instance->_psaDiag)
        {
            instance->_psaDiag->ParseCommand(instance->_carState->CurrenTime, frame.payload, frame.len);
        }
        else
        {
            printf("psaDiag is null\n");
        }

        free(frame.payload);
        return ret;
    }

    static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
    {
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        {
            esp_wifi_connect();
        }
    }

    static void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
    {
        if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

            printf("Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
            webServerCanBeStarted = true;
        }
    }

    void Process()
    {
        if (webServerCanBeStarted)
        {
            startWebServer();
            webServerCanBeStarted = false;
        }

        if (_isRunning && server != nullptr)
        {
            if (
                _carState->DiagConnected == false &&
                (_carState->CurrenTime - _lastRequestTime) > _inactivityTimeout * 1000)
            {
                _isRunning = false;
                printf("Stopping web server due to inactivity\n");
                stopWebServer();
                stopWifi();
            }
        }
    }

private:
    CarState* _carState = nullptr;
    ConfigFile* _configFile = nullptr;
    TimeProvider* _timeProvider = nullptr;
    ImmediateSignalCallback _immediateSignalCallback = nullptr;
    WebSocketSerial* _webSocketSerial = nullptr;
    PsaDiagLib* _psaDiag = nullptr;

    uint64_t _lastRequestTime = 0;
    uint64_t _inactivityTimeout = WIFI_INITIAL_TIMEOUT;
    bool _isRunning = false;

    httpd_handle_t server;
    const char* TAG = "WebServer";
};

