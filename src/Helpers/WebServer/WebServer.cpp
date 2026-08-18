#include "WebServer.hpp"
#include "esp_ota_ops.h"
#include "esp_image_format.h"
#include "mdns.h"
#include <cstring>

static bool webServerCanBeStarted = false;
static bool stationIpAcquired = false;
static bool stationConnectionFailed = false;

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

bool WebServer::HasSavedStaCredentials() const
{
    if (_carState == nullptr)
    {
        return false;
    }

    const auto ssidLen = strnlen(reinterpret_cast<const char*>(_carState->STA_WIFI_SSID), sizeof(_carState->STA_WIFI_SSID));
    const auto passwordLen = strnlen(reinterpret_cast<const char*>(_carState->STA_WIFI_PASSWORD), sizeof(_carState->STA_WIFI_PASSWORD));
    return ssidLen > 0 && passwordLen > 0;
}

bool WebServer::ScanForConfiguredStaNetwork()
{
    wifi_scan_config_t scanConfig = {};

    printf("Scanning nearby Wi-Fi networks...\n");
    esp_err_t scanErr = esp_wifi_scan_start(&scanConfig, true);
    if (scanErr != ESP_OK)
    {
        printf("Wi-Fi scan failed: %s\n", esp_err_to_name(scanErr));
        return false;
    }

    uint16_t accessPointCount = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&accessPointCount));
    if (accessPointCount == 0)
    {
        printf("No nearby Wi-Fi networks found\n");
        return false;
    }

    constexpr uint16_t maxRecords = 20;
    wifi_ap_record_t records[maxRecords] = {};
    uint16_t recordsToRead = MIN(accessPointCount, maxRecords);
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&recordsToRead, records));

    for (uint16_t i = 0; i < recordsToRead; i++)
    {
        if (strcmp(reinterpret_cast<const char*>(records[i].ssid), reinterpret_cast<const char*>(_carState->STA_WIFI_SSID)) == 0)
        {
            printf("Configured SSID found: %s\n", reinterpret_cast<const char*>(_carState->STA_WIFI_SSID));
            return true;
        }
    }

    printf("Configured SSID not found: %s\n", reinterpret_cast<const char*>(_carState->STA_WIFI_SSID));
    return false;
}

bool WebServer::ConnectToStationWithTimeout(int timeoutMs)
{
    stationIpAcquired = false;
    stationConnectionFailed = false;

    printf("Connecting to SSID: %s\n", reinterpret_cast<const char*>(_carState->STA_WIFI_SSID));
    esp_err_t connectErr = esp_wifi_connect();
    if (connectErr != ESP_OK)
    {
        printf("esp_wifi_connect failed: %s\n", esp_err_to_name(connectErr));
        return false;
    }

    int elapsedMs = 0;
    constexpr int stepMs = 200;

    while (elapsedMs < timeoutMs)
    {
        if (stationIpAcquired)
        {
            printf("STA connected and got IP\n");
            return true;
        }

        if (stationConnectionFailed)
        {
            printf("STA connection failed\n");
            return false;
        }

        vTaskDelay(pdMS_TO_TICKS(stepMs));
        elapsedMs += stepMs;
    }

    printf("STA connection timed out after %d ms\n", timeoutMs);
    return false;
}

// Initialize NVS and Wi-Fi
void WebServer::CreateWebServer()
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

    bool shouldUseApMode = false;

    if (!shouldUseApMode)
    {
        if (!HasSavedStaCredentials())
        {
            printf("STA credentials are empty, starting in AP mode\n");
            shouldUseApMode = true;
        }
        else
        {
            StartStationMode();
            ESP_ERROR_CHECK(esp_wifi_start());
            ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(8));

            const bool ssidFound = ScanForConfiguredStaNetwork();
            const bool connected = ssidFound && ConnectToStationWithTimeout(STA_CONNECT_TIMEOUT_MS);

            if (connected)
            {
                StartWebServer();
                webServerCanBeStarted = false;
                return;
            }

            printf("STA startup failed, switching to AP mode\n");
            StopWifi();
            shouldUseApMode = true;
        }
    }

    if (shouldUseApMode)
    {
        StartApMode();
        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(8));
        StartWebServer();
    }
}

void WebServer::StartApMode()
{
    printf("Starting Wi-Fi in AP mode\n");
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
            .ssid_len = static_cast<uint8_t>(strlen("PSA VAN-CAN Bridge")),
            .channel = 1,
            .authmode = WIFI_AUTH_WPA_PSK,
            .max_connection = 4,
        }
    };

    // Set Wi-Fi configuration and start Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
}

void WebServer::StartStationMode()
{
    printf("Starting Wi-Fi in STA mode\n");
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    esp_netif_set_hostname(netif, "psavancanbridge");

    mdns_init();
    mdns_hostname_set("psavancanbridge");
    mdns_instance_name_set("PSA VAN-CAN Bridge");
    mdns_service_add(NULL, "_http", "_tcp", 80, NULL, 0);

    // Initialize Wi-Fi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = true;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Set Wi-Fi to station mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // Configure the Wi-Fi connection
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, reinterpret_cast<const char*>(_carState->STA_WIFI_SSID), sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, reinterpret_cast<const char*>(_carState->STA_WIFI_PASSWORD), sizeof(wifi_config.sta.password) - 1);

    // Set Wi-Fi configuration and start Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
}

void WebServer::RegisterHandler(const char* uri, httpd_method_t method, my_httpd_handler_t handler, bool isWebSocket)
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

void WebServer::RegisterEndpoints()
{
    RegisterHandler("/ws", HTTP_GET, &WebServer::ws_handler, true);
    RegisterHandler("/", HTTP_GET, &WebServer::get_html_page_handler, false);
    RegisterHandler("/*", HTTP_OPTIONS, &WebServer::options_handler, false);
    RegisterHandler("/index.html", HTTP_GET, &WebServer::get_html_page_handler, false);
    RegisterHandler("/monitor.html", HTTP_GET, &WebServer::get_html_page_handler, false);
    RegisterHandler("/telecoding.html", HTTP_GET, &WebServer::get_html_page_handler, false);
    RegisterHandler("/carstate.html", HTTP_GET, &WebServer::get_html_page_handler, false);
    RegisterHandler("/dashboard.html", HTTP_GET, &WebServer::get_html_page_handler, false);
    RegisterHandler("/styles.css", HTTP_GET, &WebServer::get_html_page_handler, false);
    RegisterHandler("/api/time", HTTP_GET, &WebServer::get_time_handler, false);
    RegisterHandler("/api/reboot", HTTP_GET, &WebServer::get_reboot_handler, false);
    RegisterHandler("/api/getVin", HTTP_GET, &WebServer::get_vin_handler, false);
    RegisterHandler("/api/config.json", HTTP_GET, &WebServer::get_config_handler, false);
    RegisterHandler("/api/config", HTTP_POST, &WebServer::post_config_handler, false);
    RegisterHandler("/api/time", HTTP_POST, &WebServer::post_time_handler, false);
    RegisterHandler("/api/update", HTTP_POST, &WebServer::post_ota_update_handler, false);
    RegisterHandler("/api/setmonitor", HTTP_POST, &WebServer::post_network_monitor_handler, false);
    RegisterHandler("/api/carstate.json", HTTP_GET, &WebServer::get_carstate_handler, false);
    RegisterHandler("/api/carstate", HTTP_POST, &WebServer::post_carstate_handler, false);
}

void WebServer::UnRegisterEndpoints()
{
    // Unregister all endpoints
    httpd_unregister_uri_handler(server, "/", HTTP_GET);
    httpd_unregister_uri_handler(server, "/*", HTTP_OPTIONS);
    httpd_unregister_uri_handler(server, "/index.html", HTTP_GET);
    httpd_unregister_uri_handler(server, "/monitor.html", HTTP_GET);
    httpd_unregister_uri_handler(server, "/telecoding.html", HTTP_GET);
    httpd_unregister_uri_handler(server, "/carstate.html", HTTP_GET);
    httpd_unregister_uri_handler(server, "/dashboard.html", HTTP_GET);
    httpd_unregister_uri_handler(server, "/styles.css", HTTP_GET);
    httpd_unregister_uri_handler(server, "/api/time", HTTP_GET);
    httpd_unregister_uri_handler(server, "/api/reboot", HTTP_GET);
    httpd_unregister_uri_handler(server, "/api/getVin", HTTP_GET);
    httpd_unregister_uri_handler(server, "/api/config.json", HTTP_GET);
    httpd_unregister_uri_handler(server, "/api/config", HTTP_POST);
    httpd_unregister_uri_handler(server, "/api/time", HTTP_POST);
    httpd_unregister_uri_handler(server, "/api/update", HTTP_POST);
    httpd_unregister_uri_handler(server, "/api/setmonitor", HTTP_POST);
    httpd_unregister_uri_handler(server, "/ws", HTTP_GET);
    httpd_unregister_uri_handler(server, "/api/carstate.json", HTTP_GET);
    httpd_unregister_uri_handler(server, "/api/carstate", HTTP_POST);
}

// Start web server
esp_err_t WebServer::StartWebServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.stack_size = 8192;
    config.max_uri_handlers = 19;
    config.global_user_ctx = this;
    // WebServer is owned by the application and must survive httpd_stop() so it
    // can be started again. Without a callback, ESP-IDF calls free() on the
    // global user context while stopping the HTTP server.
    config.global_user_ctx_free_fn = [](void*) {};
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

// Stop web server
void WebServer::StopWebServer()
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
void WebServer::StopWifi()
{
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_LOGI(TAG, "Wi-Fi stopped");
}

httpd_handle_t WebServer::GetServerHandle()
{
    return server;
}

void WebServer::SetPsaDiagLib(PsaDiagLib* psaDiag)
{
    _psaDiag = psaDiag;
}

void WebServer::Process()
{
    if (webServerCanBeStarted)
    {
        StartWebServer();
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
            StopWebServer();
            StopWifi();
        }
    }
}

void WebServer::OnClose(httpd_handle_t hd, int sockfd)
{
    auto *instance = static_cast<WebServer *>(httpd_get_global_user_ctx(hd));

    if (instance && instance->_webSocketSerial)
    {
        instance->_webSocketSerial->OnClientDisconnected(sockfd);
    }
    close(sockfd);
}

esp_err_t WebServer::get_html_page_handler(httpd_req_t *req)
{
    // Set the appropriate headers for gzipped content
    auto *instance = static_cast<WebServer *>(req->user_ctx);
    instance->_lastRequestTime = instance->_carState->CurrenTime;
    instance->_inactivityTimeout = WIFI_AFTER_CONNECT_TIMEOUT;

    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    httpd_resp_set_hdr(req, "Connection", "close");

    const uint8_t* page_data = nullptr;
    size_t data_size = 0;

    if (strcmp(req->uri, "/") == 0 || strcmp(req->uri, "/index.html") == 0)
    {
        page_data = index_html;
        data_size = sizeof(index_html);
    }
    else if (strcmp(req->uri, "/monitor.html") == 0)
    {
        page_data = monitor_html;
        data_size = sizeof(monitor_html);
    }
    else if (strcmp(req->uri, "/telecoding.html") == 0)
    {
        page_data = telecoding_html;
        data_size = sizeof(telecoding_html);
    }
    else if (strcmp(req->uri, "/carstate.html") == 0)
    {
        page_data = carstate_html;
        data_size = sizeof(carstate_html);
    }
    else if (strcmp(req->uri, "/dashboard.html") == 0)
    {
        page_data = dashboard_html;
        data_size = sizeof(dashboard_html);
    }
    else if (strcmp(req->uri, "/styles.css") == 0)
    {
        httpd_resp_set_type(req, "text/css");
        httpd_resp_set_hdr(req, "Connection", "close");
        page_data = styles_css;
        data_size = sizeof(styles_css);
    }
    else
    {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Page not found");
        return ESP_FAIL;
    }

    httpd_resp_send(req, (const char*)page_data, data_size);
    return ESP_OK;
}

esp_err_t WebServer::get_time_handler(httpd_req_t *req)
{
    // Send a 200 OK response with no content
    auto *instance = static_cast<WebServer *>(req->user_ctx);
    instance->_lastRequestTime = instance->_carState->CurrenTime;

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "close");

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "hour", instance->_carState->Hour);
    cJSON_AddNumberToObject(json, "minute", instance->_carState->Minute);
    cJSON_AddNumberToObject(json, "second", instance->_carState->Second);
    cJSON_AddNumberToObject(json, "day", instance->_carState->MDay);
    cJSON_AddNumberToObject(json, "month", instance->_carState->Month);
    cJSON_AddNumberToObject(json, "year", instance->_carState->Year);
    cJSON_AddStringToObject(json, "firmware_version", instance->_carState->Version);

    const char *jsonResponse = cJSON_Print(json);

    httpd_resp_sendstr(req, jsonResponse);

    cJSON_free((void *)jsonResponse);
    cJSON_Delete(json);

    return ESP_OK;
}

esp_err_t WebServer::get_reboot_handler(httpd_req_t *req)
{
    // Send a 200 OK response with no content
    httpd_resp_send(req, NULL, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();
    return ESP_OK;
}

esp_err_t WebServer::get_vin_handler(httpd_req_t *req)
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

esp_err_t WebServer::get_config_handler(httpd_req_t *req)
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
    httpd_resp_set_hdr(req, "Connection", "close");

    printf("Reading config file\n");
    auto jsonHandle = _configFile->GetAsJson();
    printf("Reading config file ok\n");
    if (jsonHandle)
    {
        cJSON *json = jsonHandle.get();
        const char *jsonResponse = cJSON_Print(json);
        httpd_resp_sendstr(req, jsonResponse);
        free((void *)jsonResponse);
    }
    else
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read config file");
    }
    return ESP_OK;
}

esp_err_t WebServer::post_config_handler(httpd_req_t *req)
{
    printf("POST /api/config\n");
    char *content = (char *)malloc(req->content_len + 1);
    int ret, remaining = req->content_len;
    size_t received = 0;
    printf("Content length: %d\n", remaining);

    if (!content) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory");
        return ESP_FAIL;
    }
    while (remaining > 0)
    {
        ret = httpd_req_recv(req, content + received, remaining);
        if (ret <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                httpd_resp_send_408(req);
            }
            free(content);
            return ESP_FAIL;
        }
        received += ret;
        remaining -= ret;
    }
    content[received] = '\0';

    //printf("Received config: %s\n", content);
    auto *instance = static_cast<WebServer *>(req->user_ctx);
    instance->_configFile->SaveJson(content);
    instance->_configFile->Read();

    httpd_resp_set_hdr(req, "Connection", "close");
    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, "Config saved");

    free(content);
    return ESP_OK;
}

esp_err_t WebServer::post_time_handler(httpd_req_t *req)
{
    char content[101] = {};
    int ret, remaining = req->content_len;
    size_t received = 0;
    if (remaining >= sizeof(content))
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content too large");
        return ESP_FAIL;
    }
    while (remaining > 0)
    {
        ret = httpd_req_recv(req, content + received, remaining);
        if (ret <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                httpd_resp_send_408(req);
            }
            return ESP_FAIL;
        }
        received += ret;
        remaining -= ret;
    }
    content[received] = '\0';
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
    httpd_resp_set_hdr(req, "Connection", "close");

    return ESP_OK;
}

esp_err_t WebServer::post_ota_update_handler(httpd_req_t *req)
{
    auto *instance = static_cast<WebServer *>(req->user_ctx);
    instance->_lastRequestTime = instance->_carState->CurrenTime;

    esp_ota_handle_t update_handle = 0;
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

    if (update_partition == NULL) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "No OTA partition found");
        return ESP_FAIL;
    }

    int remaining = req->content_len;
    char *buf = (char*)malloc(1024);
    if (!buf) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory");
        return ESP_FAIL;
    }

    bool is_first_chunk = true;
    bool ota_begun = false;

    while (remaining > 0)
    {
        int recv_len = httpd_req_recv(req, buf, MIN(remaining, 1024));
        if (recv_len <= 0) {
            if (recv_len == HTTPD_SOCK_ERR_TIMEOUT)
                continue;
            if (ota_begun)
                esp_ota_abort(update_handle);
            free(buf);
            return ESP_FAIL;
        }

        if (is_first_chunk)
        {
            uint16_t expected_chip_id;
            #if CONFIG_IDF_TARGET_ESP32C6
                expected_chip_id = ESP_CHIP_ID_ESP32C6;
            #elif CONFIG_IDF_TARGET_ESP32
                expected_chip_id = ESP_CHIP_ID_ESP32;
            #elif CONFIG_IDF_TARGET_ESP32C3
                expected_chip_id = ESP_CHIP_ID_ESP32C3;
            #endif

            esp_image_header_t *header = (esp_image_header_t *)buf;

            if (header->magic != ESP_IMAGE_HEADER_MAGIC || header->chip_id != expected_chip_id)
            {
                printf("Validation failed! Magic: %02X, Chip ID: %d\n", header->magic, header->chip_id);
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid firmware architecture");
                free(buf);
                // We return ESP_OK here to tell the server the handler is done,
                // but the 400 error was already sent.
                return ESP_OK;
            }

            // ONLY start OTA after we are sure the header is correct
            esp_err_t err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
            if (err != ESP_OK)
            {
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA begin failed");
                free(buf);
                return ESP_FAIL;
            }
            ota_begun = true;
            is_first_chunk = false;
        }

        esp_ota_write(update_handle, (const void *)buf, recv_len);
        remaining -= recv_len;
    }

    free(buf);

    if (esp_ota_end(update_handle) != ESP_OK || esp_ota_set_boot_partition(update_partition) != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "OTA Finalization failed");
        return ESP_FAIL;
    }

    httpd_resp_sendstr(req, "Update successful, rebooting...");
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
    return ESP_OK;
}

esp_err_t WebServer::options_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    httpd_resp_set_hdr(req, "Connection", "close");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

esp_err_t WebServer::post_network_monitor_handler(httpd_req_t *req)
{
    char content[101] = {};
    int ret, remaining = req->content_len;
    size_t received = 0;
    if (remaining >= sizeof(content))
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Content too large");
        return ESP_FAIL;
    }
    while (remaining > 0)
    {
        ret = httpd_req_recv(req, content + received, remaining);
        if (ret <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                httpd_resp_send_408(req);
            }
            return ESP_FAIL;
        }
        received += ret;
        remaining -= ret;
    }
    content[received] = '\0';
    cJSON *root = cJSON_Parse(content);
    if (!root)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    cJSON *jsonObj = cJSON_GetObjectItem(root, "network");
    double network = cJSON_GetNumberValue(jsonObj);

    jsonObj = cJSON_GetObjectItem(root, "direction");
    double direction = cJSON_GetNumberValue(jsonObj);

    printf("network: %d\n", (int)network);
    printf("direction: %d\n", (int)direction);

    cJSON_Delete(root);

    auto *instance = static_cast<WebServer *>(req->user_ctx);
    instance->_carState->LogNetwork = (int)network;
    instance->_carState->LogDirection = (int)direction;
    instance->_lastRequestTime = instance->_carState->CurrenTime;

    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, "Monitor set");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Connection", "close");

    return ESP_OK;
}

esp_err_t WebServer::get_carstate_handler(httpd_req_t *req)
{
    printf("GET /api/carstate\n");

    auto *instance = static_cast<WebServer *>(req->user_ctx);
    auto _carState = instance->_carState;
    auto _configFile = instance->_configFile;
    instance->_lastRequestTime = instance->_carState->CurrenTime;

    if (!_carState)
    {
        printf("Car state is null\n");
        return ESP_FAIL;
    }
    else
    {
        printf("Car state is not null\n");
    }

    httpd_resp_set_type(req, "application/json");

    cJSON* json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "Ignition", _carState->Ignition);
    cJSON_AddNumberToObject(json, "IgnitionMode", _carState->IgnitionMode);
    cJSON_AddNumberToObject(json, "KeyPosition", _carState->KeyPosition);
    cJSON_AddNumberToObject(json, "IsReverseEngaged", _carState->IsReverseEngaged);
    cJSON_AddNumberToObject(json, "ExternalTemperature", _carState->ExternalTemperature);
    cJSON_AddNumberToObject(json, "NightMode", _carState->NightMode);
    cJSON_AddNumberToObject(json, "DashboardBrightness", _carState->DashboardBrightness);
    cJSON_AddNumberToObject(json, "EngineOilTemperature", _carState->EngineOilTemperature);
    cJSON_AddNumberToObject(json, "FuelLevel", _carState->FuelLevel);
    cJSON_AddNumberToObject(json, "EngineOilLevel", _carState->EngineOilLevel);
    cJSON_AddNumberToObject(json, "CruiseControlActivateFunction", _carState->CruiseControlActivateFunction);
    cJSON_AddNumberToObject(json, "CruiseControlStatusOfSelectedFunction", _carState->CruiseControlStatusOfSelectedFunction);
    cJSON_AddNumberToObject(json, "CruiseControlSelectedFunction", _carState->CruiseControlSelectedFunction);
    cJSON_AddNumberToObject(json, "CruiseControlSpeed", _carState->CruiseControlSpeed.asUint16);
    cJSON_AddNumberToObject(json, "Speed", _carState->Speed.asUint16);
    cJSON_AddNumberToObject(json, "Rpm", _carState->Rpm.asUint16);
    cJSON_AddNumberToObject(json, "Odometer", _carState->Odometer.asUint24);
    cJSON_AddNumberToObject(json, "DoorStatus", _carState->DoorStatus.asByte);
    _configFile->cJSON_AddUInt64Smart(json, "RadioRemote", _carState->RadioRemote.asUint64);
    cJSON_AddNumberToObject(json, "RightStickButtonPushed", _carState->RightStickButtonPushed);
    cJSON_AddNumberToObject(json, "InstantConsumption", _carState->InstantConsumption.asUint16);
    cJSON_AddNumberToObject(json, "RemainingRange", _carState->RemainingRange.asUint16);
    cJSON_AddNumberToObject(json, "TotalRange", _carState->TotalRange.asUint16);
    cJSON_AddNumberToObject(json, "Trip1Speed", _carState->Trip1Speed);
    cJSON_AddNumberToObject(json, "Trip1Distance", _carState->Trip1Distance.asUint16);
    cJSON_AddNumberToObject(json, "Trip1Consumption", _carState->Trip1Consumption.asUint16);
    cJSON_AddNumberToObject(json, "Trip2Speed", _carState->Trip2Speed);
    cJSON_AddNumberToObject(json, "Trip2Distance", _carState->Trip2Distance.asUint16);
    cJSON_AddNumberToObject(json, "Trip2Consumption", _carState->Trip2Consumption.asUint16);
    _configFile->cJSON_AddUInt64Smart(json, "ParkingAidStatus", _carState->ParkingAidStatus.asNumeric);
    cJSON_AddNumberToObject(json, "CoolantTemperature", _carState->CoolantTemperature);
    _configFile->cJSON_AddUInt64Smart(json, "CarSignalLights", _carState->CarSignalLights.asUint64);
    _configFile->cJSON_AddUInt64Smart(json, "CarIndicatorLights", _carState->CarIndicatorLights.asUint64);
    cJSON_AddNumberToObject(json, "KeepWebServerAlive", _carState->DiagConnected ? 1 : 0);
    cJSON_AddNumberToObject(json, "EmulateTripButtonPress", _carState->EmulateTripButtonPress);
    _configFile->cJSON_AddUInt64Smart(json, "AlertHistory1", _carState->AlertHistory1.asUint64);
    _configFile->cJSON_AddUInt64Smart(json, "AlertHistory2", _carState->AlertHistory2.asUint64);
    _configFile->cJSON_AddUInt64Smart(json, "AlertHistory3", _carState->AlertHistory3.asUint64);

    const char *jsonResponse = cJSON_Print(json);

    httpd_resp_sendstr(req, jsonResponse);

    cJSON_free((void *)jsonResponse);
    cJSON_Delete(json);

    return ESP_OK;
}

esp_err_t WebServer::post_carstate_handler(httpd_req_t *req)
{
    printf("POST /api/carstate\n");
    char *content = (char *)malloc(req->content_len + 1);
    int ret, remaining = req->content_len;
    size_t received = 0;
    printf("Content length: %d\n", remaining);

    if (!content) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Out of memory");
        return ESP_FAIL;
    }
    while (remaining > 0) {
        ret = httpd_req_recv(req, content + received, remaining);
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                httpd_resp_send_408(req);
            }
            free(content);
            return ESP_FAIL;
        }
        received += ret;
        remaining -= ret;
    }
    content[received] = '\0';

    printf("Received carstate: %s\n", content);
    // Here you can parse the JSON and update the CarState accordingly

    auto *instance = static_cast<WebServer *>(req->user_ctx);
    auto _carState = instance->_carState;
    auto _configFile = instance->_configFile;
    instance->_lastRequestTime = instance->_carState->CurrenTime;

    cJSON *root = cJSON_Parse(content);
    if (!root) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        free(content);
        return ESP_FAIL;
    }

    _carState->Ignition = _configFile->getJsonInt(root, "Ignition", 0);
    _carState->IgnitionMode = _configFile->getJsonInt(root, "IgnitionMode", 0);
    _carState->KeyPosition = _configFile->getJsonInt(root, "KeyPosition", 0);
    _carState->IsReverseEngaged = _configFile->getJsonInt(root, "IsReverseEngaged", 0);
    _carState->ExternalTemperature = _configFile->getJsonInt(root, "ExternalTemperature", 0);
    _carState->NightMode = _configFile->getJsonInt(root, "NightMode", 0);
    _carState->DashboardBrightness = _configFile->getJsonInt(root, "DashboardBrightness", 0);
    _carState->EngineOilTemperature = _configFile->getJsonInt(root, "EngineOilTemperature", 0);
    _carState->FuelLevel = _configFile->getJsonInt(root, "FuelLevel", 0);
    _carState->EngineOilLevel = _configFile->getJsonInt(root, "EngineOilLevel", 0);
    _carState->CruiseControlActivateFunction = _configFile->getJsonInt(root, "CruiseControlActivateFunction", 0);
    _carState->CruiseControlStatusOfSelectedFunction = _configFile->getJsonInt(root, "CruiseControlStatusOfSelectedFunction", 0);
    _carState->CruiseControlSelectedFunction = _configFile->getJsonInt(root, "CruiseControlSelectedFunction", 0);
    _carState->CruiseControlSpeed.asUint16 = _configFile->getJsonInt(root, "CruiseControlSpeed", 0);
    _carState->Speed.asUint16 = _configFile->getJsonInt(root, "Speed", 0);
    _carState->Rpm.asUint16 = _configFile->getJsonInt(root, "Rpm", 0);
    _carState->Odometer.asUint24 = _configFile->getJsonInt(root, "Odometer", 0);
    _carState->DoorStatus.asByte = _configFile->getJsonInt(root, "DoorStatus", 0);
    _carState->RadioRemote.asUint64 = _configFile->getJsonInt(root, "RadioRemote", 0);
    _carState->RightStickButtonPushed = _configFile->getJsonInt(root, "RightStickButtonPushed", 0);
    _carState->InstantConsumption.asUint16 = _configFile->getJsonInt(root, "InstantConsumption", 0);
    _carState->RemainingRange.asUint16 = _configFile->getJsonInt(root, "RemainingRange", 0);
    _carState->TotalRange.asUint16 = _configFile->getJsonInt(root, "TotalRange", 0);
    _carState->Trip1Speed = _configFile->getJsonInt(root, "Trip1Speed", 0);
    _carState->Trip1Distance.asUint16 = _configFile->getJsonInt(root, "Trip1Distance", 0);
    _carState->Trip1Consumption.asUint16 = _configFile->getJsonInt(root, "Trip1Consumption", 0);
    _carState->Trip2Speed = _configFile->getJsonInt(root, "Trip2Speed", 0);
    _carState->Trip2Distance.asUint16 = _configFile->getJsonInt(root, "Trip2Distance", 0);
    _carState->Trip2Consumption.asUint16 = _configFile->getJsonInt(root, "Trip2Consumption", 0);
    _carState->ParkingAidStatus.asNumeric = _configFile->getJsonInt(root, "ParkingAidStatus", 0);
    _carState->CoolantTemperature = _configFile->getJsonInt(root, "CoolantTemperature", 0);
    _carState->CarSignalLights.asUint64 = _configFile->getJsonInt(root, "CarSignalLights", 0);
    _carState->CarIndicatorLights.asUint64 = _configFile->getJsonInt(root, "CarIndicatorLights", 0);
    _carState->DiagConnected = _configFile->getJsonInt(root, "KeepWebServerAlive", 0) == 1;
    _carState->EmulateTripButtonPress = _configFile->getJsonInt(root, "EmulateTripButtonPress", 0);
    _carState->AlertHistory1.asUint64 = _configFile->getJsonInt(root, "AlertHistory1", 0);
    _carState->AlertHistory2.asUint64 = _configFile->getJsonInt(root, "AlertHistory2", 0);
    _carState->AlertHistory3.asUint64 = _configFile->getJsonInt(root, "AlertHistory3", 0);
    cJSON_Delete(root);

    httpd_resp_set_status(req, "200 OK");
    httpd_resp_sendstr(req, "CarState received");
    httpd_resp_set_hdr(req, "Connection", "close");

    free(content);
    return ESP_OK;
}

esp_err_t WebServer::ws_handler(httpd_req_t *req)
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

void WebServer::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        stationConnectionFailed = true;

        auto *event = static_cast<wifi_event_sta_disconnected_t*>(event_data);
        if (event)
        {
            printf("STA disconnected, reason=%d\n", event->reason);
        }
    }
}

void WebServer::ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

        printf("Got IP: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        stationIpAcquired = true;
        webServerCanBeStarted = true;
    }
}
