#pragma once

#ifndef _SerialReader_h
    #define _SerialReader_h

#include "../SerialPort/AbstractSerial.h"
#include "../../Config.h"
#include "../Helpers/DataBroker.h"

class SerialReader {
    AbsSer* _serialPort;
    Config *_config;
    DataBroker *_dataBroker;

public:
    SerialReader(AbsSer *serialPort, Config *config, DataBroker *dataBroker);

    void Receive(uint8_t *messageLength, uint8_t message[]);
};

#endif
