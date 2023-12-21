#include "../../Config.h"

#ifdef WEBSOCKET_SERIAL

#include "WebSocketSerialAbs.h"
#include "SerialSocketHandler.h"

#define WEBSOCKETSERVABS_MAX_CLIENTS 4

static SerialSocketHandler* activeClients[WEBSOCKETSERVABS_MAX_CLIENTS];
static std::string _lastMessage;

WebsocketHandler* WebSocketSerAbs::CreateSocket()
{
    SerialSocketHandler *handler = new SerialSocketHandler();
    handler->setCallbacks(&WebSocketSerAbs::SocketClosed, &WebSocketSerAbs::SocketMessage);
    for(int i = 0; i < 4; i++)
    {
        if (activeClients[i] == nullptr)
        {
            activeClients[i] = handler;
            break;
        }
    }
    return handler;
}

void WebSocketSerAbs::SocketClosed(WebsocketHandler* socketHandler)
{
    for(int i = 0; i < 4; i++) {
        if (activeClients[i] == socketHandler) {
            activeClients[i] = nullptr;
        }
    }
}

void WebSocketSerAbs::SocketMessage(WebsocketHandler* socketHandler, WebsocketInputStreambuf* input)
{
    for(int i = 0; i < 4; i++) {
        if (activeClients[i] == socketHandler) {
            _lastMessage = activeClients[i]->GetLastMessage();
        }
    }
}

bool WebSocketSerAbs::isConnected()
{
    for(int i = 0; i < WEBSOCKETSERVABS_MAX_CLIENTS; i++)
    {
        if (activeClients[i] != nullptr) {
            return true;
        }
    }
    return false;
}

void WebSocketSerAbs::SendBufferToSocket()
{
    for(int i = 0; i < WEBSOCKETSERVABS_MAX_CLIENTS; i++)
    {
        if (activeClients[i] != nullptr) {
            activeClients[i]->send(_buffer, _bufferIdx, WebsocketHandler::SEND_TYPE_TEXT);
        }
    }
}

void WebSocketSerAbs::begin(unsigned long baud, uint8_t config)
{
    for(uint8_t i = 0; i < WEBSOCKETSERVABS_MAX_CLIENTS; i++)
        activeClients[i] = nullptr;
}

void WebSocketSerAbs::begin(unsigned long baud)
{
    for(uint8_t i = 0; i < WEBSOCKETSERVABS_MAX_CLIENTS; i++)
        activeClients[i] = nullptr;
}

void WebSocketSerAbs::end()
{
    //return 0;//!!
}

int WebSocketSerAbs::available(void)
{
    if (isConnected())
    {
        //return _lastMessage.length();
        return 0;//!!
    }
    return 0;
}

int WebSocketSerAbs::peek(void)
{
    if (isConnected())
    {
        return 0;//!!
    }
    return 0;
}

int WebSocketSerAbs::read(void)
{
    if (isConnected())
    {
        return 0;//!!
    }
    return 0;
}

int WebSocketSerAbs::availableForWrite(void)
{
    if (isConnected())
    {
        return 1;
    }
    return 0;
}

void WebSocketSerAbs::flush(void)
{
}

size_t WebSocketSerAbs::write(uint8_t n)
{
    ///*
    if (isConnected())
    {
        if (_bufferIdx < 256)
        {
            _buffer[_bufferIdx] = n;
            _bufferIdx++;
        }
        else
        {
            _bufferIdx = 0;
        }

        if (n == 0x0A && _bufferIdx >= 2 && _buffer[_bufferIdx-2] == 0x0D)
        {
            SendBufferToSocket();
            _bufferIdx = 0;
        }
        return 1;
    }
    _bufferIdx = 0;
    return 0;
    //*/
}

size_t WebSocketSerAbs::write(unsigned long n) {
    return 0;
    /*
    if (isConnected)
    {
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if (activeClients[i] != nullptr) {
            unsigned long data[] = { n };
            activeClients[i]->send(data, 1, WebsocketHandler::SEND_TYPE_BINARY);
            }
        }

        return 1;
    }
    return 0;
    */
}

size_t WebSocketSerAbs::write(long n) {
    return 0;
    /*
    if (isConnected)
    {
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if (activeClients[i] != nullptr) {
            long data[] = { n };
            activeClients[i]->send(data, 1, WebsocketHandler::SEND_TYPE_BINARY);
            }
        }

        return 1;
    }
    return 0;
    */
}

size_t WebSocketSerAbs::write(unsigned int n) {
    return 0;
    /*
    if (isConnected)
    {
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if (activeClients[i] != nullptr) {
            unsigned int data[] = { n };
            activeClients[i]->send(data, 1, WebsocketHandler::SEND_TYPE_BINARY);
            }
        }

        return 1;
    }
    return 0;
    */
}

size_t WebSocketSerAbs::write(int n) {
    return 0;
    /*
    if (isConnected)
    {
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if (activeClients[i] != nullptr) {
            int data[] = { n };
            activeClients[i]->send(data, 1, WebsocketHandler::SEND_TYPE_BINARY);
            }
        }

        return 1;
    }
    return 0;
    */
}
#endif