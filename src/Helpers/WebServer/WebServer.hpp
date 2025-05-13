#pragma once

#include <string.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_event.h>
#include <esp_vfs.h>
#include "cJSON.h"
#include "../ConfigFile.hpp"
#include "../TimeProvider.h"

#include "esp_http_server.h"
#include "gzipped_webpage_data.h"

#define SCRATCH_BUFSIZE (10240)
/*
typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;
*/
typedef esp_err_t (*my_httpd_handler_t)(httpd_req_t *req);

static void OnWifiConnected() {
    // Start HTTP server
    //http.StartServer();
    //http.RegisterHandler(&helloWorldHandler);
}

static void NetworkEventHandler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        ESP_LOGI("wifi", "Retrying to connect to Wifi...");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI("wifi", "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        // Call sub-handler
        OnWifiConnected();
    }
    else {
        //ESP_LOGI("wifi", "Unknown event: %d %d", event_base, event_id);
    }
}
/*
esp_err_t hello_get_handler(httpd_req_t *req) {
    const char *response = "Hello, World!";
    httpd_resp_send(req, response, HTTPD_RESP_USE_STRLEN);  // Send response
    ESP_LOGI("HTTP_SERVER", "GET /hello request handled");
    return ESP_OK;
}

esp_err_t index_get_handler(httpd_req_t *req)
{
    // Set the appropriate headers for gzipped content
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

    // Send the data from PROGMEM
    size_t data_size = sizeof(ESP_REACT_DATA_0);
    httpd_resp_send(req, (const char*)ESP_REACT_DATA_0, data_size);
    return ESP_OK;
}

httpd_uri_t hello_uri = {
    .uri       = "/hello",        // URI endpoint
    .method    = HTTP_GET,        // HTTP method
    .handler   = hello_get_handler, // Handler function
    .user_ctx  = NULL             // Optional user data
};
*/

static httpd_handle_t _server = nullptr;

class WebServer {

    private:
        uint64_t _lastRequestTime = 0;
        bool _isRunning = false;
        std::shared_ptr<CarState> _carState;
        std::shared_ptr<ConfigFile> _configFile;
        std::shared_ptr<TimeProvider> _timeProvider;

        //rest_server_context_t* _rest_context = NULL;

        static esp_err_t SaveConfig(httpd_req_t *req)
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

            /*
            cJSON *root = cJSON_Parse(content);
            if (!root) {
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
                return ESP_FAIL;
            }
            cJSON *brightness = cJSON_GetObjectItem(root, "brightness");
            if (!brightness) {
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing brightness");
                cJSON_Delete(root);
                return ESP_FAIL;
            }
            cJSON *json = cJSON_CreateObject();
            cJSON_AddNumberToObject(json, "brightness", brightness->valueint);
            const char *json_str = cJSON_Print(json);

            //save to NVS
            nvs_handle_t nvs_handle;
            esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
            if (err != ESP_OK) {
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "NVS open failed");
                cJSON_Delete(json);
                cJSON_Delete(root);
                return ESP_FAIL;
            }

            httpd_resp_set_type(req, "application/json");
            httpd_resp_sendstr(req, json_str);
            free((void *)json_str);
            cJSON_Delete(json);
            cJSON_Delete(root);
            */
            return ESP_OK;
        }

        static esp_err_t index_get_handler(httpd_req_t *req)
        {
            // Set the appropriate headers for gzipped content
            httpd_resp_set_type(req, "text/html");
            httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

            // Send the data from PROGMEM
            size_t data_size = sizeof(ESP_REACT_DATA_0);
            httpd_resp_send(req, (const char*)ESP_REACT_DATA_0, data_size);
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
            /*
            cJSON *resultRoot = cJSON_CreateObject();
            cJSON_AddBoolToObject(resultRoot, "success", true);
            cJSON_Delete(resultRoot);
            */

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
            //httpd_resp_set_type(req, "text/html");

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
/*
            cJSON *root = cJSON_CreateObject();
            //esp_chip_info_t chip_info;
            //esp_chip_info(&chip_info);
            cJSON_AddStringToObject(root, "version", IDF_VER);
            cJSON_AddNumberToObject(root, "cores", 2);
            const char *sys_info = cJSON_Print(root);
            httpd_resp_sendstr(req, sys_info);
            free((void *)sys_info);
            cJSON_Delete(root);
            return ESP_OK;
*/
        }
/*
        rest_server_context_t* create_rest_context(const char *base_path)
        {
            rest_server_context_t *rest_context = static_cast<rest_server_context_t*>(calloc(1, sizeof(rest_server_context_t)));

            if (!rest_context) {
                ESP_LOGE("HTTP_SERVER", "No memory for rest context");
                return NULL;
            }
            strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));
            return rest_context;
        }
*/
        static esp_err_t get_keep_alive(httpd_req_t *req)
        {
            // Send a 200 OK response with no content
            auto *instance = static_cast<WebServer *>(req->user_ctx);
            instance->_lastRequestTime = instance->_carState->CurrenTime;
            httpd_resp_send(req, NULL, 0);
            return ESP_OK;
        }

        static esp_err_t get_reboot(httpd_req_t *req)
        {
            // Send a 200 OK response with no content
            httpd_resp_send(req, NULL, 0);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            esp_restart();
            return ESP_OK;
        }

        void RegisterHandler(const char* uri, httpd_method_t method, my_httpd_handler_t handler)
        {
            httpd_uri_t uri_t = {
                .uri = uri,
                .method = method,
                .handler = handler,
                .user_ctx = this
            };
            httpd_register_uri_handler(_server, &uri_t);
        }

        void RegisterEndpoints()
        {
            RegisterHandler("/", HTTP_GET, &WebServer::index_get_handler);
            /*
            RegisterHandler("/index.html", HTTP_GET, &WebServer::index_get_handler);
            RegisterHandler("/api/config", HTTP_GET, &WebServer::get_config_handler);
            RegisterHandler("/api/config", HTTP_POST, &WebServer::SaveConfig);
            RegisterHandler("/api/reboot", HTTP_GET, &WebServer::get_reboot);
            RegisterHandler("/api/time", HTTP_POST, &WebServer::post_time_handler);
            RegisterHandler("/api/keepalive", HTTP_GET, &WebServer::get_keep_alive);
            */

            //
            /*
            RegisterHandler("/", HTTP_GET,
                [](httpd_req_t *req) -> esp_err_t {
                    WebServer *instance = static_cast<WebServer *>(req->user_ctx);
                    return instance->index_get_handler(req);
                });

            RegisterHandler("/index.html", HTTP_GET,
                [](httpd_req_t *req) -> esp_err_t {
                    WebServer *instance = static_cast<WebServer *>(req->user_ctx);
                    return instance->index_get_handler(req);
                });

            RegisterHandler("/api/config", HTTP_GET,
                [](httpd_req_t *req) -> esp_err_t {
                    WebServer *instance = static_cast<WebServer *>(req->user_ctx);
                    return instance->get_config_handler(req);
                });

            RegisterHandler("/api/config", HTTP_POST,
                [](httpd_req_t *req) -> esp_err_t {
                    WebServer *instance = static_cast<WebServer *>(req->user_ctx);
                    return instance->SaveConfig(req);
                });

            RegisterHandler("/api/reboot", HTTP_GET,
                [](httpd_req_t *req) -> esp_err_t {
                    esp_restart();
                });

            RegisterHandler("/api/time", HTTP_POST,
                [](httpd_req_t *req) -> esp_err_t {
                    WebServer *instance = static_cast<WebServer *>(req->user_ctx);
                    return instance->post_time_handler(req);
                });

            RegisterHandler("/api/keepalive", HTTP_GET,
                [](httpd_req_t *req) -> esp_err_t {
                    WebServer *instance = static_cast<WebServer *>(req->user_ctx);
                    return instance->get_keep_alive(req);
                });
            //*/
        }

        void UnregisterEndpoints()
        {
            // Unregister all endpoints
            httpd_unregister_uri_handler(_server, "/", HTTP_GET);
            return;
            /*
            httpd_unregister_uri_handler(_server, "/index.html", HTTP_GET);
            httpd_unregister_uri_handler(_server, "/api/config", HTTP_GET);
            httpd_unregister_uri_handler(_server, "/api/config", HTTP_POST);
            httpd_unregister_uri_handler(_server, "/api/reboot", HTTP_GET);
            httpd_unregister_uri_handler(_server, "/api/time", HTTP_POST);
            httpd_unregister_uri_handler(_server, "/api/keepalive", HTTP_GET);
            */
        }

        void InitNVS() {
            // Initialize NVS
            esp_err_t ret = nvs_flash_init();
            if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                ESP_ERROR_CHECK(nvs_flash_erase());
                ret = nvs_flash_init();
            }
            ESP_ERROR_CHECK(ret);
        }

        void InitWifi() {

            // Initialize TCP/IP network interface (required for Wi-Fi)
            ESP_ERROR_CHECK(esp_netif_init());

            // Initialize the event loop
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
            // Register event handler
            ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                                ESP_EVENT_ANY_ID,
                                                                &NetworkEventHandler,
                                                                NULL,
                                                                NULL));
            ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                                IP_EVENT_STA_GOT_IP,
                                                                &NetworkEventHandler,
                                                                NULL,
                                                                NULL));

            // Set Wi-Fi configuration and start Wi-Fi
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
            ESP_ERROR_CHECK(esp_wifi_start());
        }

        void DeInitWifi() {
            // Deinitialize Wi-Fi
            ESP_ERROR_CHECK(esp_wifi_stop());
            ESP_ERROR_CHECK(esp_wifi_deinit());
            //ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &NetworkEventHandler));
            //ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &NetworkEventHandler));
        }
    public:
        bool StartWebServer(void) {
            httpd_config_t config = HTTPD_DEFAULT_CONFIG();  // Default server config
            config.uri_match_fn = httpd_uri_match_wildcard;
            config.global_user_ctx = this;

            if (httpd_start(&_server, &config) == ESP_OK) {
                // Register URI handlers
                //httpd_register_uri_handler(_server, &hello_uri);
                //httpd_register_uri_handler(server, &echo_uri);
                RegisterEndpoints();

                ESP_LOGI("HTTP_SERVER", "Server started");
                _isRunning = true;
                _lastRequestTime = _carState->CurrenTime;
                return true;
            } else {
                ESP_LOGE("HTTP_SERVER", "Failed to start server");
                return false;
            }
        }

        bool StopWebServer(void) {
            if (_isRunning && _server != nullptr)
            {
                printf("WebServer stopped\n");
                UnregisterEndpoints();

                esp_err_t result = httpd_stop(_server);
                if (result == ESP_OK) {
                    _server = nullptr;
                    _isRunning = false;
                    ESP_LOGI("HTTP_SERVER", "Server stopped");
                }
                else
                {
                    ESP_LOGE("HTTP_SERVER", "httpd_stop failed with error: %d", result);
                }

                return true;
            }
            return false;
        }

        void Process() {
            //stop the server after 5 seconds of inactivity

            if (_server != nullptr) {
                if (_carState->CurrenTime - _lastRequestTime > 5 * 1000) {
                    StopWebServer();
                }
            }
        }

        WebServer(
            std::shared_ptr<CarState> carState,
            std::shared_ptr<ConfigFile> configFile,
            std::shared_ptr<TimeProvider> timeProvider
        )
        {
            _carState = carState;
            _configFile = configFile;
            _timeProvider = timeProvider;

            if (!_configFile)
            {
                printf("Config file is null\n");
            }
            else
            {
                printf("Config file is not null\n");
            }

            InitNVS();
            InitWifi();
        }
};
