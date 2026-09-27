#pragma once

#include "VanLpShared.h"

class ILpCoreVanDiagnostics
{
public:
    virtual ~ILpCoreVanDiagnostics() = default;

    virtual VanLpResult GetLastTxResult() const = 0;
    virtual uint32_t GetLastTxAbortDetail() const = 0;
    virtual bool GetLastTxArbitrationTrace(VanLpArbitrationTrace& trace) const = 0;
    virtual void GetGpioState(VanLpGpioState& state) const = 0;
    virtual uint32_t GetLastTxEodTs() const = 0;
    virtual bool GetLastTxRxTrace(uint8_t index, VanLpRxTrace& trace) const = 0;
};

class NullLpCoreVanDiagnostics final : public ILpCoreVanDiagnostics
{
public:
    VanLpResult GetLastTxResult() const override { return VAN_LP_NONE; }
    uint32_t GetLastTxAbortDetail() const override { return 0; }
    bool GetLastTxArbitrationTrace(VanLpArbitrationTrace&) const override { return false; }
    void GetGpioState(VanLpGpioState& state) const override { state = {}; }
    uint32_t GetLastTxEodTs() const override { return UINT32_MAX; }
    bool GetLastTxRxTrace(uint8_t, VanLpRxTrace&) const override { return false; }
};
