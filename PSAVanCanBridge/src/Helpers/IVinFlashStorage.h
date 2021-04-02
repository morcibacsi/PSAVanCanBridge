// IVinFlashStorage.h
#pragma once

#ifndef _IVinFlashStorage_h
    #define _IVinFlashStorage_h

class IVinFlashStorage
{
    public:

    virtual void Remove() = 0;
    virtual bool Load() = 0;
    virtual bool Save() = 0;
};

#endif
