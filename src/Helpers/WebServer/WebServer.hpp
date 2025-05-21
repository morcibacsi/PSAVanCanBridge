#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_netif.h"
#include "../ConfigFile.hpp"
#include "../TimeProvider.h"
#include "../CarState.hpp"
#include "gzipped_webpage_data.h"

typedef esp_err_t (*my_httpd_handler_t)(httpd_req_t *req);
const int WIFI_INITIAL_TIMEOUT = 40;
const int WIFI_AFTER_CONNECT_TIMEOUT = 7;

class WebServer {
public:
WebServer(
    CarState* carState,
    ConfigFile* configFile,
    TimeProvider* timeProvider

) : server(NULL) {
    _carState = carState;
    _configFile = configFile;
    _timeProvider = timeProvider;
}

    // Initialize NVS and Wi-Fi
    void StartWebServer() {
        // Initialize NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        // Initialize the Wi-Fi driver
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());

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
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        // Set Wi-Fi to station mode
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

        // Configure the Wi-Fi connection
        wifi_config_t wifi_config = {
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
        ESP_ERROR_CHECK(esp_wifi_start());
        startWebServer();
    }

    void RegisterHandler(const char* uri, httpd_method_t method, my_httpd_handler_t handler)
    {
        httpd_uri_t uri_t = {
            .uri = uri,
            .method = method,
            .handler = handler,
            .user_ctx = this
        };
        httpd_register_uri_handler(server, &uri_t);
    }

    void RegisterEndpoints()
    {
        RegisterHandler("/", HTTP_GET, &WebServer::get_index_handler);
        RegisterHandler("/index.html", HTTP_GET, &WebServer::get_index_handler);
        RegisterHandler("/api/keepalive", HTTP_GET, &WebServer::get_keep_alive_handler);
        RegisterHandler("/api/reboot", HTTP_GET, &WebServer::get_reboot_handler);
        RegisterHandler("/api/config.json", HTTP_GET, &WebServer::get_config_handler);
        RegisterHandler("/api/config", HTTP_POST, &WebServer::post_config_handler);
        RegisterHandler("/api/time", HTTP_POST, &WebServer::post_time_handler);
    }

    void UnRegisterEndpoints()
    {
        // Unregister all endpoints
        httpd_unregister_uri_handler(server, "/", HTTP_GET);
        httpd_unregister_uri_handler(server, "/index.html", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/keepalive", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/reboot", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/config.json", HTTP_GET);
        httpd_unregister_uri_handler(server, "/api/config", HTTP_POST);
        httpd_unregister_uri_handler(server, "/api/time", HTTP_POST);
    }

    // Start web server
    esp_err_t startWebServer() {
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.uri_match_fn = httpd_uri_match_wildcard;
        if (httpd_start(&server, &config) == ESP_OK)
        {
            RegisterEndpoints();

            ESP_LOGI(TAG, "Web server started");
            printf("Web server started\n");
            _isRunning = true;
            _lastRequestTime = _carState->CurrenTime;
            _inactivityTimeout = WIFI_INITIAL_TIMEOUT;
            return ESP_OK;
        }
        ESP_LOGE(TAG, "Failed to start the web server");
        return ESP_FAIL;
    }

    // Stop web server
    void stopWebServer() {
        if (server) {
            UnRegisterEndpoints();
            httpd_stop(server);
            server = nullptr;
            ESP_LOGI(TAG, "Web server stopped");
        }
    }

    // Stop Wi-Fi
    void stopWifi() {
        ESP_ERROR_CHECK(esp_wifi_stop());
        ESP_ERROR_CHECK(esp_wifi_deinit());
        ESP_LOGI(TAG, "Wi-Fi stopped");
    }

    // Handle GET requests
    static esp_err_t hello_get_handler(httpd_req_t *req) {
        printf("Received GET request\n");
        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_lastRequestTime = instance->_carState->CurrenTime;
        const char* resp_str = "Hello from ESP32!";
        httpd_resp_set_type(req, "text/html");
        httpd_resp_set_status(req, "200 OK");
        httpd_resp_sendstr(req, "Time saved");
        //httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
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

    static esp_err_t get_keep_alive_handler(httpd_req_t *req)
    {
        // Send a 200 OK response with no content
        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_lastRequestTime = instance->_carState->CurrenTime;
        httpd_resp_send(req, NULL, 0);
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
        char content[2000];
        int ret, remaining = req->content_len;
        if (remaining > sizeof(content)) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content too large");
            return ESP_FAIL;
        }
        while (remaining > 0) {
            ret = httpd_req_recv(req, content, sizeof(content));
            if (ret <= 0) {
                if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                    httpd_resp_send_408(req);
                }
                return ESP_FAIL;
            }
            remaining -= ret;
        }

        auto *instance = static_cast<WebServer *>(req->user_ctx);
        instance->_configFile->SaveJson(content);
        instance->_configFile->Read();
        httpd_resp_set_status(req, "200 OK");
        httpd_resp_sendstr(req, "Config saved");

        return ESP_OK;
    }

    static esp_err_t post_time_handler(httpd_req_t *req)
    {
        char content[100];
        int ret, remaining = req->content_len;
        if (remaining > sizeof(content)) {
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content too large");
            return ESP_FAIL;
        }
        while (remaining > 0) {
            ret = httpd_req_recv(req, content, sizeof(content));
            if (ret <= 0) {
                if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                    httpd_resp_send_408(req);
                }
                return ESP_FAIL;
            }
            remaining -= ret;
        }
        cJSON *root = cJSON_Parse(content);
        if (!root) {
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

    void Process()
    {
        if (_isRunning && server != nullptr)
        {
            if ((_carState->CurrenTime - _lastRequestTime) > _inactivityTimeout * 1000)
            {
                _isRunning = false;
                printf("Stopping web server due to inactivity\n");
                stopWebServer();
                stopWifi();
            }
        }
    }

private:
    CarState* _carState;
    ConfigFile* _configFile;
    TimeProvider* _timeProvider;
    uint64_t _lastRequestTime = 0;
    uint64_t _inactivityTimeout = WIFI_INITIAL_TIMEOUT;
    bool _isRunning = false;

    httpd_handle_t server;
    const char* TAG = "WebServer";
};

