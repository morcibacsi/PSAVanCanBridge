#pragma once

#include "sdkconfig.h"

#ifdef CONFIG_IDF_TARGET_ESP32C6

#include "LpCoreVanTx.hpp"
#include "Platform/Esp/Protocol/ILpCoreVanDiagnostics.hpp"
#include "lib/VanMessageSenderReceiverAdapter.hpp"

class LpCoreVanAdapter final
    : public VanMessageSenderReceiverAdapter<LpCoreVanTx>,
      public ILpCoreVanDiagnostics
{
private:
    using Base = VanMessageSenderReceiverAdapter<LpCoreVanTx>;

public:
    LpCoreVanAdapter(gpio_num_t rxPin, gpio_num_t txPin, VanBusSpeed networkSpeed)
        : Base(rxPin, txPin, networkSpeed)
    {
    }

    VanLpResult GetLastTxResult() const override
    {
        return BackendInstance().GetLastTxResult();
    }

    uint32_t GetLastTxAbortDetail() const override
    {
        return BackendInstance().GetLastTxAbortDetail();
    }

    bool GetLastTxArbitrationTrace(VanLpArbitrationTrace& trace) const override
    {
        return BackendInstance().GetLastTxArbitrationTrace(trace);
    }

    void GetGpioState(VanLpGpioState& state) const override
    {
        BackendInstance().GetGpioState(state);
    }

    uint32_t GetLastTxEodTs() const override
    {
        return BackendInstance().GetLastTxEodTs();
    }

    bool GetLastTxRxTrace(uint8_t index, VanLpRxTrace& trace) const override
    {
        return BackendInstance().GetLastTxRxTrace(index, trace);
    }
};

#endif
