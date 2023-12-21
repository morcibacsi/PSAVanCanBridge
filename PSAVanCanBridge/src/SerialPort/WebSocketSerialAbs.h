#include "../../Config.h"

#ifdef WEBSOCKET_SERIAL

#ifndef WebSocketSerialAbs_H
    #define WebSocketSerialAbs_H

#define LIBRARY_VERSION_WEBSOCKETSERABS_H   "0.1.0-alpha"

#include <inttypes.h>
#include "AbstractSerial.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

// We will use wifi
#include <WiFi.h>

// uses https://github.com/fhessel/esp32_https_server

// Includes for the server
#include <HTTPServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <WebsocketHandler.hpp>

#include "SerialSocketHandler.h"

using namespace httpsserver;

class WebSocketSerAbs : public AbsSer {
public:
    inline WebSocketSerAbs(HTTPServer *httpServer, const std::string &path) {
        // The websocket handler can be linked to the server by using a WebsocketNode:
        // (Note that the standard defines GET as the only allowed method here,
        // so you do not need to pass it explicitly)
        WebsocketNode *serialSocketNode = new WebsocketNode(path, &CreateSocket);

        // Adding the node to the server works in the same way as for all other nodes
        httpServer->registerNode(serialSocketNode);
    }

    virtual ~WebSocketSerAbs(){ }
    static WebsocketHandler* CreateSocket();
    static void SocketClosed(WebsocketHandler* socketHandler);
    static void SocketMessage(WebsocketHandler* socketHandler, WebsocketInputStreambuf* input);

    bool begin(String localName=String());
    void begin(unsigned long baud, uint8_t config);
    void begin(unsigned long baud);
    void end();
    int available(void);
    int peek(void);
    int read(void);
    int availableForWrite(void);
    void flush(void);
    using Print::write;
    size_t write(uint8_t n);
    size_t write(unsigned long n);
    size_t write(long n);
    size_t write(unsigned int n);
    size_t write(int n);
    inline operator bool() {return true;}//!!

    void SetConnected(bool connected);
private:
    static const uint8_t BUFFER_LEN = 255;
    bool isConnected();
    void SendBufferToSocket();
    uint8_t _buffer[BUFFER_LEN];
    uint8_t _bufferIdx = 0;
};
#endif
#endif