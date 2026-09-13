#pragma once

#ifndef _MessageHandler_4EC_h
    #define _MessageHandler_4EC_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"


class MessageHandler_4EC : public IMessageHandler<MessageHandler_4EC>
{
    uint64_t _tripButtonPressedSince = 0;
    uint8_t _prevTripButtonState = 0;
    uint8_t _prevIgnition = 0;
    uint8_t headerByte = 0x80;
    uint8_t seconds = 0;
    uint64_t _prevGeneratedMessageTimestamp = 0;

    public:
        static constexpr uint32_t MessageId = 0x4EC;

        MessageHandler_4EC()
        {
        }

        BusMessage Generate(CarState* carState)
        {
            BusMessage msg{};
            msg.id = MessageId;
            msg.protocol = ProtocolType::AEE2001;
            msg.type = MessageType::Response;
            msg.ack = true;
            msg.periodicityMs = 1000;
            msg.isActive = false;
            msg.slot = 0;

            uint8_t status = 0xC3; //playing
            uint8_t cartridge = 0x16;
            uint8_t minutes = 0x01;
            uint8_t trackNo = 0x17;
            uint8_t cdNo = 0x02;
            uint8_t trackCount = 0x21;

            uint8_t decToBcdSeconds = seconds/10*16 + seconds%10;

            //increment seconds, and headerbyte every 1000 ms

            if (_prevGeneratedMessageTimestamp == 0 || (carState->CurrenTime - _prevGeneratedMessageTimestamp) >= 1000)
            {
                seconds++;
                if (seconds >= 60)
                {
                    seconds = 0;
                }

                if (headerByte == 0x87){
                    headerByte = 0x80;
                } else {
                    headerByte++;
                }

                _prevGeneratedMessageTimestamp = carState->CurrenTime;
            }

            msg.data[0] = headerByte;
            msg.data[1] = 0x00;
            msg.data[2] = status;
            msg.data[3] = cartridge;
            msg.data[4] = minutes;
            msg.data[5] = decToBcdSeconds;
            msg.data[6] = trackNo;
            msg.data[7] = cdNo;
            msg.data[8] = trackCount;
            msg.data[9] = 0x3f;
            msg.data[10] = 0x01;
            msg.data[11] = headerByte;

            msg.dataLength = 12;

            return msg;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
