#pragma once

#include <inttypes.h>

//These are in the same order as in the AEE2004/2010 so don't change the order
union DisplayMessageStruct {
    struct {
        uint8_t Field1;
        uint8_t Field2;
        uint8_t Field3;
        uint8_t Field4;
        uint8_t Field5;
        uint8_t Field6;
        uint8_t Field7;
        uint8_t Field8;
    } data;
    uint64_t asUint64;
};
