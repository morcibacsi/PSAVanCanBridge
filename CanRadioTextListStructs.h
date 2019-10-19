// CanRadioTextListStructs.h
#pragma once

#ifndef _CanRadioTextListStructs_h
    #define _CanRadioTextListStructs_h

// CANID: 125
const uint16_t CAN_ID_RADIO_TEXT_LIST = 0x125;

const uint8_t CAN_RADIO_TEXT_LIST_CLOSE  = 0;
const uint8_t CAN_RADIO_TEXT_LIST_RADIO1 = 1;
const uint8_t CAN_RADIO_TEXT_LIST_RADIO2 = 2;
const uint8_t CAN_RADIO_TEXT_LIST_RADIO3 = 3;
const uint8_t CAN_RADIO_TEXT_LIST_RADIO4 = 4;
const uint8_t CAN_RADIO_TEXT_LIST_RADIO5 = 5;
const uint8_t CAN_RADIO_TEXT_LIST_CD     = 9;

// Read right to left in documentation
typedef struct {
    uint8_t             : 4; // bit 0-3
    uint8_t page_number : 4; // bit 4-7
} CanRadioTextListByte1Struct;

typedef struct {
    uint8_t current_track_offset : 4; // bit 0-3
    uint8_t list_moved           : 1; // bit 4
    uint8_t unknown5             : 1; // bit 5
    uint8_t show_track_list      : 1; // bit 6
    uint8_t unknown7             : 1; // bit 7
} CanRadioTextListByte4Struct;


// Read left to right in documentation
typedef struct CanRadioTextListStruct {
    CanRadioTextListByte1Struct Page;
    uint8_t TracksCount;
    uint8_t CurrentViewOffset;
    CanRadioTextListByte4Struct Offset;
    uint8_t TextDataDescriptor;
    uint8_t Field6;
    uint8_t AuthorName[20];
    uint8_t TrackName[20];
};

typedef union CanRadioTextListPacket {
    CanRadioTextListStruct data;
    uint8_t CanRadioTextListPacket[sizeof(CanRadioTextListStruct)];
};

#pragma region Sender class
class CanRadioTextMessageSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanRadioTextMessageSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(uint8_t show)
    {
        uint8_t page = CAN_RADIO_TEXT_LIST_CLOSE;

        if (show)
        {
            page = CAN_RADIO_TEXT_LIST_RADIO1;
        }

        PacketGenerator<CanRadioTextListPacket> generator;

        generator.packet.data.Page.page_number = page;
        generator.packet.data.Offset.show_track_list = show;
        generator.packet.data.TracksCount = 6;
        generator.packet.data.TrackName[0] = 'X';

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_RADIO_TEXT_LIST, 0, sizeof(CanRadioTextListPacket), serializedPacket);
    }
};
#pragma endregion

#endif
