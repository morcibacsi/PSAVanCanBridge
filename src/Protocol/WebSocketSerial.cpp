#include "WebSocketSerial.hpp"
#include <cstring>
#include <cctype>

struct ws_async_msg_t {
    WebSocketSerial* instance;
    int fd;
    size_t len;
    uint8_t data[];
};

WebSocketSerial::WebSocketSerial(CarState* carState)
{
    _carState = carState;
    _rxRing = xRingbufferCreate(4096, RINGBUF_TYPE_BYTEBUF);
    _txMutex = xSemaphoreCreateMutex();
}

WebSocketSerial::~WebSocketSerial()
{
    end();

    if (_rxRing)
        vRingbufferDelete(_rxRing);
    if (_txMutex)
        vSemaphoreDelete(_txMutex);
}

esp_err_t WebSocketSerial::begin(int)
{
    return ESP_OK;
}

void WebSocketSerial::end()
{
    if (_clientFd >= 0)
    {
        httpd_sess_trigger_close(_server, _clientFd);
        _clientFd = -1;
    }
}

void WebSocketSerial::OnClientConnected(int sockfd)
{
    _clientFd = sockfd;
    printf("WebSocket connected: %d\n", sockfd);
    _carState->DiagConnected = true;
}

void WebSocketSerial::OnClientDisconnected(int sockfd)
{
    if (_clientFd == sockfd)
    {
        _clientFd = -1;
        printf("WebSocket disconnected: %d\n", sockfd);
        _carState->DiagConnected = false;
    }
}

void WebSocketSerial::OnFrameReceived(const uint8_t* data, size_t len)
{
    if (!_rxRing) return;

    xRingbufferSend(_rxRing, data, len, 0);
}

void WebSocketSerial::OnWebServerStarted(httpd_handle_t server)
{
    printf("WebSocketSerial: Web server started, server handle: %p\n", server);
    _server = server;
}

int WebSocketSerial::available()
{
    size_t itemsWaiting = 0;
    vRingbufferGetInfo(_rxRing, nullptr, nullptr, nullptr, nullptr, &itemsWaiting);
    return itemsWaiting;
}

int WebSocketSerial::read(uint8_t* buffer, size_t length, TickType_t timeout)
{
    size_t itemSize;
    uint8_t* data = (uint8_t*)xRingbufferReceiveUpTo(
        _rxRing,
        &itemSize,
        timeout,
        length
    );

    if (!data)
        return 0;

    memcpy(buffer, data, itemSize);
    vRingbufferReturnItem(_rxRing, data);

    return itemSize;
}

int WebSocketSerial::readByte(uint8_t* byte, TickType_t timeout)
{
    return read(byte, 1, timeout);
}

int WebSocketSerial::write(const uint8_t* data, size_t length)
{
    if (_server == nullptr)
    {
        printf("Server is null\n");
        return -1;
    }

    if (_clientFd < 0)
    {
        printf("No client connected\n");
        return -1;
    }

    if (data == nullptr || length == 0)
    {
        printf("Data is null or length is 0\n");
        return -1;
    }

    xSemaphoreTake(_txMutex, portMAX_DELAY);

    memcpy(_txBuffer, data, length);

    httpd_ws_frame_t frame = {};
    frame.type = HTTPD_WS_TYPE_TEXT;
    frame.payload = _txBuffer;
    frame.len = length;
    frame.final = true;
    frame.fragmented = false;

    for(int i = 0; i < length; i++)
    {
        printf("%c", data[i]);
    }

    esp_err_t ret = httpd_ws_send_data_async(_server, _clientFd, &frame, NULL, NULL);

    xSemaphoreGive(_txMutex);

    return (ret == ESP_OK) ? length : -1;
}

int WebSocketSerial::writeByte(uint8_t byte)
{
    return write(&byte, 1);
}

void WebSocketSerial::flush()
{
    // Nothing required
}
