// IConfigStorage.h
#pragma once

#ifndef _IConfigStorage_h
    #define _IConfigStorage_h

class IConfigStorage
{
    public:

    virtual void Remove() = 0;
    virtual bool Load() = 0;
    virtual bool Save() = 0;
};

#endif
