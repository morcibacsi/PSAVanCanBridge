#include "../../Config.h"

#ifdef WEBSOCKET_SERIAL

#include "SerialSocketHandler.h"

static const WebsocketHandlerClosed * _closedFunction;
static const WebsocketHandlerOnMessage * _onMessageFunction;

void SerialSocketHandler::setCallbacks(const WebsocketHandlerClosed closedFunction, const WebsocketHandlerOnMessage onMessageFunction)
{
    _closedFunction = closedFunction;
    _onMessageFunction = onMessageFunction;
}

WebsocketHandler *SerialSocketHandler::create() {
    SerialSocketHandler *handler = new SerialSocketHandler();

    return handler;
}

// When the websocket is closing, we remove the client from the array
void SerialSocketHandler::onClose() {
    if (_closedFunction != nullptr)
    {
        _closedFunction(this);
    }
}

// Finally, passing messages around. If we receive something, we send it to all
// other clients
void SerialSocketHandler::onMessage(WebsocketInputStreambuf * inbuf) {
    if (_onMessageFunction != nullptr)
    {
        /*
        // Get the input message
        std::ostringstream ss;
        std::string msg;
        ss << inbuf;
        msg = ss.str();
        */
        _onMessageFunction(this, inbuf);
    }
    /*

  // Send it back to every client
  for(int i = 0; i < MAX_CLIENTS; i++) {
    if (activeClients[i] != nullptr) {
      activeClients[i]->send(msg, SEND_TYPE_TEXT);
    }
   }
   */
}

std::string SerialSocketHandler::GetLastMessage()
{
    return _lastMessage;
}
#endif