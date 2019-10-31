// PacketGenerator.h
#pragma once

#ifndef _PacketGenerator_H
    #define _PacketGenerator_H

    #include "Serializer.h"

template <class T>
class PacketGenerator
{
    public:
        PacketGenerator();
        ~PacketGenerator();

        T packet;

        unsigned char * GetSerializedPacket();

    private:
        unsigned char * serializedPacket;
};

template <class T>
PacketGenerator<T>::PacketGenerator()
{
    memset(&packet, 0, sizeof(packet));
}

template <class T>
PacketGenerator<T>::~PacketGenerator()
{
    memset(&packet, 0, 0);
    delete[] serializedPacket;
}

template <class T>
unsigned char * PacketGenerator<T>::GetSerializedPacket() {
    serializedPacket = Serialize<T>(packet);
    return serializedPacket;
}

#endif
