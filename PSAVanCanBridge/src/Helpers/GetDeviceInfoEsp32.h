// GetDeviceInfoEsp32.h
#pragma once

#ifndef _IGetDeviceInfoEsp32_h
    #define _IGetDeviceInfoEsp32_h

#include "IGetDeviceInfo.h"

class GetDeviceInfoEsp32 : public IGetDeviceInfo
{
    public:
        uint16_t GetId() override;
};

#endif
