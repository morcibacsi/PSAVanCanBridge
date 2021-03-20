#pragma once

#ifndef _VanWriterTask_h
#define _VanWriterTask_h

#include "VanMessageSender.h"
#include "VanWriterContainer.h"

class VanWriterTask {
    VanWriterContainer* vanWriterContainer;
    SPIClass* spi;
    AbstractVanMessageSender* VANInterface;

public:
    VanWriterTask()
    {
        const int SCK_PIN = 25;
        const int MISO_PIN = 5;
        const int MOSI_PIN = 33;
        const int VAN_PIN = 32;

        spi = new SPIClass();
        spi->begin(SCK_PIN, MISO_PIN, MOSI_PIN, VAN_PIN);

        VANInterface = new VanMessageSender(VAN_PIN, spi, VAN_COMFORT);
        VANInterface->begin();

        vanWriterContainer = new VanWriterContainer(VANInterface);

    }

    void Process(VanIgnitionDataToBridgeToCan dataToBridge, unsigned long currentTime)
    {
        vanWriterContainer->Process(dataToBridge, currentTime);
    }
};

#endif