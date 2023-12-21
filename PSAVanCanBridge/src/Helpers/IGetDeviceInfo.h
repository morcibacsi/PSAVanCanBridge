// IGetDeviceInfo.h
#pragma once

#ifndef _IGetDeviceInfo_h
    #define _IGetDeviceInfo_h

#include <stdint.h>

class IGetDeviceInfo
{
    public:
        virtual uint16_t GetId() = 0;
};

#endif
