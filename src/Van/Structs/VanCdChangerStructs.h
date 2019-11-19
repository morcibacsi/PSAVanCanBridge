// VanCdChangerStructs.h
#pragma once

#ifndef _VanCdChangerStructs_h
    #define _VanCdChangerStructs_h

// VANID: 4EC
const uint16_t VAN_ID_CD_CHANGER = 0x4EC;

const uint8_t VAN_ID_CD_CHANGER_STATUS_OFF          = 0x41;
const uint8_t VAN_ID_CD_CHANGER_STATUS_PAUSED       = 0xC1;
const uint8_t VAN_ID_CD_CHANGER_STATUS_BUSY         = 0xD3;
const uint8_t VAN_ID_CD_CHANGER_STATUS_PLAYING      = 0xC3;
const uint8_t VAN_ID_CD_CHANGER_STATUS_FAST_FORWARD = 0xC4;
const uint8_t VAN_ID_CD_CHANGER_STATUS_FAST_REWIND  = 0xC5;

const uint8_t VAN_ID_CD_CHANGER_CARTRIDGE_NOT_PRESENT = 0x06;
const uint8_t VAN_ID_CD_CHANGER_CARTRIDGE_PRESENT     = 0x16;

typedef struct {
    uint8_t Cd1Present : 1; // bit 0
    uint8_t Cd2Present : 1; // bit 1
    uint8_t Cd3Present : 1; // bit 2
    uint8_t Cd4Present : 1; // bit 3
    uint8_t Cd5Present : 1; // bit 4
    uint8_t Cd6Present : 1; // bit 5
    uint8_t undefined6 : 1; // bit 6
    uint8_t undefined7 : 1; // bit 7
} VanCdPresentStruct;

// Read left to right in documentation
// The numbers are in BCD
typedef struct VanCdChangerStruct {
    uint8_t Header;
    uint8_t RandomTrackPlay;
    uint8_t Status;
    uint8_t CartridgePresent;
    uint8_t TrackMinutes;
    uint8_t TrackSeconds;
    uint8_t TrackNumber;
    uint8_t CdNumber;
    uint8_t TrackCount;
    uint8_t Unknown;
    VanCdPresentStruct CdPresent;
    uint8_t Footer;
};

typedef union VanCdChangerPacket {
    VanCdChangerStruct data;
    uint8_t VanCdChangerPacket[sizeof(VanCdChangerStruct)];
};

#pragma region Sender class
class VanCdChangerPacketSender
{
    AbstractVanMessageSender * vanMessageSender;
    uint8_t headerByte = 0x80;
public:
    VanCdChangerPacketSender(AbstractVanMessageSender * object)
    {
        vanMessageSender = object;
    }

    void Send(uint8_t channelId, uint8_t randomTrackPlay, uint8_t status, uint8_t cartridgePresent, uint8_t trackMin, uint8_t trackSec, uint8_t currentTrackNo, uint8_t currentCdNo, uint8_t trackCount, uint8_t cdPresent)
    {
        headerByte = (headerByte == 0x87) ? 0x80 : headerByte + 1;

        VanCdChangerPacket packet;
        memset(&packet, 0, sizeof(packet));

        packet.data.Header = headerByte;
        packet.data.RandomTrackPlay = randomTrackPlay;
        packet.data.Status = status;
        packet.data.CartridgePresent = cartridgePresent;
        packet.data.TrackMinutes = DecimalToBcd(trackMin);
        packet.data.TrackSeconds = DecimalToBcd(trackSec);
        packet.data.TrackNumber  = DecimalToBcd(currentTrackNo);
        packet.data.CdNumber     = DecimalToBcd(currentCdNo);
        packet.data.TrackCount   = DecimalToBcd(trackCount);
        //packet.data.CdPresent = cdPresent;
        packet.data.Footer = headerByte;

        //printf("sending speed: %d (%#x)\n", x, x);
        unsigned char *serializedPacket = Serialize<VanCdChangerPacket>(packet);
        vanMessageSender->set_channel_for_immediate_reply_message(channelId, VAN_ID_CD_CHANGER, serializedPacket, sizeof(packet));
        memset(&packet, 0, 0);
        delete[] serializedPacket;
    }

    uint8_t DecimalToBcd(uint8_t input)
    {
        return((input / 10 * 16) + (input % 10));
    }

    uint8_t BcdToDecimal(uint8_t input)
    {
        return((input / 16 * 10) + (input % 16));
    }
};
#pragma endregion

#endif
