#pragma once

class IConfigStore
{
public:
    virtual ~IConfigStore() = default;
    virtual void Write() = 0;
};

