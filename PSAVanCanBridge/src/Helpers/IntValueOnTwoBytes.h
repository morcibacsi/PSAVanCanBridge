// IntValueOnTwoBytes.h
#pragma once

#ifndef _IntValueOnTwoBytes_h
#define _IntValueOnTwoBytes_h

/*
usage:
IntValueOnTwoBytes.value = 65535;           //Assign a value to the Int var of the Union
HighByte = IntValueOnTwoBytes.Bytes[1];     //Get the High Byte (255)
LowByte  = IntValueOnTwoBytes.Bytes[0];     //Get the Low Byte (255)
*/
typedef union IntValueOnTwoBytes
{
    unsigned int value;
    unsigned char bytes[2];
};

#endif