#pragma once

#include "ISerial.hpp"
#include "../Helpers/CarState.hpp"
#include "esp_http_server.h"
#include "freertos/ringbuf.h"
#include "freertos/semphr.h"

class WebSocketSerial : public ISerial
{
private:
    httpd_handle_t _server = nullptr;
    int _clientFd = -1;
    uint8_t _txBuffer[1024];

    RingbufHandle_t _rxRing;
    SemaphoreHandle_t _txMutex;
    CarState* _carState;

public:
    explicit WebSocketSerial(CarState* carState);
    ~WebSocketSerial();

    esp_err_t begin(int unused) override;
    void end() override;

    int available() override;
    int read(uint8_t* buffer, size_t length, TickType_t timeout) override;
    int readByte(uint8_t* byte, TickType_t timeout) override;

    int write(const uint8_t* data, size_t length) override;
    int writeByte(uint8_t byte) override;

    void flush() override;

    // Called by WebServer
    void OnClientConnected(int sockfd);
    void OnClientDisconnected(int sockfd);
    void OnFrameReceived(const uint8_t* data, size_t len);
    void OnWebServerStarted(httpd_handle_t server);
};