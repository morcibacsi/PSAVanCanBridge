// CanMenuStructs.h
#pragma once

#ifndef _CanMenuStructs_h
    #define _CanMenuStructs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 3E5
const uint16_t CAN_ID_MENU_BUTTONS = 0x3E5;

const int CONST_UP_ARROW    = 119; //w
const int CONST_DOWN_ARROW  = 115; //s
const int CONST_LEFT_ARROW  = 97;  //a
const int CONST_RIGHT_ARROW = 100; //d
const int CONST_ESC_BUTTON  = 113; //q
const int CONST_OK_BUTTON   = 101; //e
const int CONST_MENU_BUTTON = 109; //m
const int CONST_MODE_BUTTON = 110; //n
const int CONST_TRIP_BUTTON = 108; //n

/*
http://www.avrfreaks.net/comment/748331#comment-748331
0x40 = 0100 0000 - up
0x10 = 0001 0000 - down
0x04 = 0000 0100 - right
0x01 = 0000 0001 - left
*/
typedef struct {
    uint8_t left  : 1; // bit 0
    uint8_t       : 1; // bit 1
    uint8_t right : 1; // bit 2
    uint8_t       : 1; // bit 3
    uint8_t down  : 1; // bit 4
    uint8_t       : 1; // bit 5
    uint8_t up    : 1; // bit 6
    uint8_t       : 1; // bit 7
} CanArrowsOnRadioStruct;

/*
0x40 = 0100 0000 - menu
*/
typedef struct {
    uint8_t aircon : 1; // bit 0
    uint8_t        : 1; // bit 1
    uint8_t        : 1; // bit 2
    uint8_t        : 1; // bit 3
    uint8_t phone  : 1; // bit 4
    uint8_t        : 1; // bit 5
    uint8_t menu   : 1; // bit 6
    uint8_t        : 1; // bit 7
} CanMenuButtonStruct;

/*
0x40 = 0100 0000 - ok
0x10 = 0001 0000 - esc
*/
typedef struct {
    uint8_t	    : 1; // bit 0
    uint8_t     : 1; // bit 1
    uint8_t dark: 1; // bit 2
    uint8_t     : 1; // bit 3
    uint8_t esc : 1; // bit 4
    uint8_t     : 1; // bit 5
    uint8_t ok  : 1; // bit 6
    uint8_t     : 1; // bit 7
} CanEscOkButtonStruct;

typedef struct {
    uint8_t audio : 1; // bit 0
    uint8_t       : 1; // bit 1
    uint8_t       : 1; // bit 2
    uint8_t       : 1; // bit 3
    uint8_t mode  : 1; // bit 4
    uint8_t       : 1; // bit 5
    uint8_t trip  : 1; // bit 6
    uint8_t       : 1; // bit 7
} CanModeButtonStruct;

struct CanMenuStruct {
    CanMenuButtonStruct MenuField;
    CanModeButtonStruct ModeField;
    CanEscOkButtonStruct EscOkField;
    uint8_t byte4;
    uint8_t byte5;
    CanArrowsOnRadioStruct ArrowsField;
};

union CanMenuPacket {
    CanMenuStruct data;
    uint8_t CanMenuPacket[sizeof(CanMenuStruct)];
};

//https://stackoverflow.com/a/9196883/5453350
static int CONST_CAN_RADIO_MENUBUTTONS[] = { CONST_UP_ARROW, CONST_DOWN_ARROW, CONST_LEFT_ARROW, CONST_RIGHT_ARROW, CONST_ESC_BUTTON, CONST_OK_BUTTON, CONST_MENU_BUTTON, CONST_MODE_BUTTON, CONST_TRIP_BUTTON };

#pragma region Sender class
class CanRadioButtonPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanRadioButtonPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendButtonCode(uint8_t buttonId)
    {
        PacketGenerator<CanMenuPacket> generator;

        switch (buttonId) {
            case CONST_MENU_BUTTON: {
                generator.packet.data.MenuField.menu = 1;
                break;
            }
            case CONST_OK_BUTTON: {
                generator.packet.data.EscOkField.ok = 1;
                break;
            }
            case CONST_ESC_BUTTON: {
                generator.packet.data.EscOkField.esc = 1;
                break;
            }
            case CONST_UP_ARROW: {
                generator.packet.data.ArrowsField.up = 1;
                break;
            }
            case CONST_DOWN_ARROW: {
                generator.packet.data.ArrowsField.down = 1;
                break;
            }
            case CONST_LEFT_ARROW: {
                generator.packet.data.ArrowsField.left = 1;
                break;
            }
            case CONST_RIGHT_ARROW: {
                generator.packet.data.ArrowsField.right = 1;
                break;
            }
            case CONST_MODE_BUTTON: {
                generator.packet.data.ModeField.mode = 1;
                break;
            }
            case CONST_TRIP_BUTTON: {
                generator.packet.data.ModeField.trip = 1;
                break;
            }
        }

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_MENU_BUTTONS, 0, sizeof(CanMenuPacket), serializedPacket);
    }

};
#pragma endregion

#endif
