#pragma once

#ifndef _SerialReader_h
    #define _SerialReader_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "../../Config.h"
#include "../Can/AbstractCanMessageSender.h"
#include "../Can/Handlers/CanTripInfoHandler.h"
#include "../Can/Structs/CanMenuStructs.h"
#include "../Helpers/IVinFlashStorage.h"
#include "../SerialPort/AbstractSerial.h"

class SerialReader {
    AbsSer* _serialPort;
    AbstractCanMessageSender* _CANInterface;
    CanTripInfoHandler* _tripInfoHandler;
    CanRadioButtonPacketSender* _canRadioButtonSender;
    IVinFlashStorage* _vinFlashStorage;

    void SendRadioButton(uint8_t button)
    {
        uint8_t sendCount = 2;
        for (int i = 0; i < sendCount; ++i)
        {
            _canRadioButtonSender->SendButtonCode(button);
        }
        _canRadioButtonSender->SendButtonCode(0);
    }

public:
    SerialReader(
        AbsSer* serialPort, 
        AbstractCanMessageSender* CANInterface,
        CanTripInfoHandler* tripInfoHandler,
        CanRadioButtonPacketSender* canRadioButtonSender,
        IVinFlashStorage* vinFlashStorage
    )
    {
        _serialPort = serialPort;
        _CANInterface = CANInterface;
        _tripInfoHandler = tripInfoHandler;
        _canRadioButtonSender = canRadioButtonSender;
        _vinFlashStorage = vinFlashStorage;
    }

    void Receive(uint8_t* messageLength, uint8_t message[])
    {
        uint8_t vanMessageLength = 0;
        if (_serialPort->available() > 0) {
            uint8_t inChar = (uint8_t)_serialPort->read();
            if (inChar == 'v') { // got a sync byte?
                while (_serialPort->available()) {
                    message[vanMessageLength] = _serialPort->read();
                    vanMessageLength++;
                }
            }

            if (READ_SERIAL_PORT_FOR_COMMANDS)
            {
                if (inChar == 'm') {
                    //PrintVanMessageToSerial = !PrintVanMessageToSerial;
                }
                if (inChar == 'r') {
                    _vinFlashStorage->Remove();
                }
                if (inChar == 'V') {
                    _serialPort->print("VIN: ");
                    for (int i = 0; i < 17; ++i)
                    {
                        _serialPort->write(Vin[i]);
                    }
                    _serialPort->println();
                }
                if (inChar == 'W')
                {
                    SendRadioButton(CONST_UP_ARROW);
                }
                if (inChar == 'A')
                {
                    SendRadioButton(CONST_LEFT_ARROW);
                }
                if (inChar == 'S')
                {
                    SendRadioButton(CONST_DOWN_ARROW);
                }
                if (inChar == 'D')
                {
                    SendRadioButton(CONST_RIGHT_ARROW);
                }
                if (inChar == 'E')
                {
                    SendRadioButton(CONST_OK_BUTTON);
                }
                if (inChar == 'X')
                {
                    // Something is wrong with this method that's why the workaround
                    //SendRadioButton(CONST_ESC_BUTTON);

                    uint8_t data[] = { 0x00, 0x00, 0x10, 0x00, 0x00, 0x00 };
                    _CANInterface->SendMessage(CAN_ID_MENU_BUTTONS, 0, 6, data);
                }
                if (inChar == 'x')
                {
                    SendRadioButton(CONST_ESC_BUTTON);
                }
                if (inChar == 'O')
                {
                    SendRadioButton(CONST_MODE_BUTTON);
                }
                if (inChar == 'M')
                {
                    SendRadioButton(CONST_MENU_BUTTON);
                }
                if (inChar == 'T')
                {
                    //Serial.println("M pressed");

                    for (int i = 0; i < 10; ++i)
                    {
                        _tripInfoHandler->TripButtonPress();
                    }
                }
            }
        }
        *messageLength = vanMessageLength;
    }
};

#endif
