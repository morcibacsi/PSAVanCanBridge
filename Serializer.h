// Serializer.h
#pragma once

#ifndef _Serializer_h
    #define _Serializer_h

//http://forum.arduino.cc/index.php?topic=311337.0
//https://forum.arduino.cc/index.php?topic=406509.0

template <class T> T DeSerialize(const uint8_t b[])
{
    //https://stackoverflow.com/a/13775983/5453350
    //Re-make the struct
    T tmp;
    memcpy(&tmp, b, sizeof(tmp));

    return tmp;
}

template <class T> unsigned char * Serialize(const T& packet)
{
    //https://stackoverflow.com/a/14760796/5453350
    unsigned char * b = new unsigned char[sizeof(packet)];
    memcpy(b, &packet, sizeof(packet));//convert to byte array

    return b;
}

#endif
