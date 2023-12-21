#include "SerialReader.h"

SerialReader::SerialReader(AbsSer* serialPort, Config *config, DataBroker *dataBroker)
{
    _serialPort = serialPort;
    _config = config;
    _dataBroker = dataBroker;
}

void SerialReader::Receive(uint8_t* messageLength, uint8_t message[])
{
    if (_config->REPLAY_MODE)
    {
        //_serialPort->println("getVanMessage");
    }

    uint8_t vanMessageLength = 0;
    if (_serialPort->available() > 0) {
        uint8_t inChar = (uint8_t)_serialPort->read();
        if (inChar == 'v') { // got a sync byte?
            while (_serialPort->available()) {
                message[vanMessageLength] = _serialPort->read();
                vanMessageLength++;
            }
        }
        if (inChar == 'm')
        {
            _dataBroker->IsVanLogEnabled = !_dataBroker->IsVanLogEnabled;
        }

    }
    *messageLength = vanMessageLength;
}