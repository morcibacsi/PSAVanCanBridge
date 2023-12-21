#pragma once

#ifndef _IntUnions_h
    #define _IntUnions_h

#include <stdint.h>

union UInt16
{
    struct
    {
        uint8_t rightByte;
        uint8_t leftByte;
    } data;
    uint16_t asUint16;
};

union UInt24
{
    struct
    {
        uint8_t rightByte;
        uint8_t middleByte;
        uint8_t leftByte;
    } data;
    uint32_t asUint24;
};

#endif
