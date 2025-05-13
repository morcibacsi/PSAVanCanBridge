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

    // Default constructor
    constexpr UInt16() : asUint16(0) {}

    // Constructor from uint16_t
    constexpr UInt16(uint16_t value) : asUint16(value) {}

    // Optional assignment operators
    UInt16& operator=(uint16_t value)
    {
        asUint16 = value;
        return *this;
    }

    UInt16& operator=(const UInt16& other)
    {
        asUint16 = other.asUint16;
        return *this;
    }
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

    // Default constructor
    constexpr UInt24() : asUint24(0) {}

    // Constructor from uint16_t
    constexpr UInt24(uint16_t value) : asUint24(value) {}

    // Optional assignment operators
    UInt24& operator=(uint16_t value)
    {
        asUint24 = value;
        return *this;
    }

    UInt24& operator=(const UInt24& other)
    {
        asUint24 = other.asUint24;
        return *this;
    }
};

#endif
