#include "../../Config.h"

#ifdef WEBSOCKET_SERIAL

#ifndef SerialSocketHandler_H
#define SerialSocketHandler_H

#include <WebsocketHandler.hpp>
using namespace httpsserver;

typedef void (WebsocketHandlerClosed)(WebsocketHandler* socketHandler);
typedef void (WebsocketHandlerOnMessage)(WebsocketHandler* socketHandler, WebsocketInputStreambuf* input);

class SerialSocketHandler : public WebsocketHandler {
    //static SerialSocketHandler* _activeClients[4];
    std::string _lastMessage;
public:
    static void setCallbacks(const WebsocketHandlerClosed closedFunction, const WebsocketHandlerOnMessage onMessageFunction);

    std::string GetLastMessage();

    // This method is called by the webserver to instantiate a new handler for each
    // client that connects to the websocket endpoint
    static WebsocketHandler* create();

    // This method is called when a message arrives
    void onMessage(WebsocketInputStreambuf * input);

    // Handler function on connection close
    void onClose();
};

#endif
#endif