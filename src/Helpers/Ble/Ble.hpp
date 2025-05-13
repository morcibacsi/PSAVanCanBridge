#pragma once

#include <NimBLEDevice.h>
#include "nimblehelp.hpp"



class Ble {
static NimBLEServer* pServer;
    public:
    Ble(){
        NimBLEDevice::init("NimBLE");

        pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(&serverCallbacks);

        NimBLEService*        pDeadService = pServer->createService("DEAD");
        NimBLECharacteristic* pBeefCharacteristic =
            pDeadService->createCharacteristic("BEEF",
                                            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE |
                                                /** Require a secure connection for read and write access */
                                                NIMBLE_PROPERTY::READ_ENC | // only allow reading if paired / encrypted
                                                NIMBLE_PROPERTY::WRITE_ENC  // only allow writing if paired / encrypted
            );

        pBeefCharacteristic->setValue("Burger");
        pBeefCharacteristic->setCallbacks(&chrCallbacks);

        /**
         *  2902 and 2904 descriptors are a special case, when createDescriptor is called with
         *  either of those uuid's it will create the associated class with the correct properties
         *  and sizes. However we must cast the returned reference to the correct type as the method
         *  only returns a pointer to the base NimBLEDescriptor class.
         */
        NimBLE2904* pBeef2904 = pBeefCharacteristic->create2904();
        pBeef2904->setFormat(NimBLE2904::FORMAT_UTF8);
        pBeef2904->setCallbacks(&dscCallbacks);

        NimBLEService*        pBaadService = pServer->createService("BAAD");
        NimBLECharacteristic* pFoodCharacteristic =
            pBaadService->createCharacteristic("F00D", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);

        pFoodCharacteristic->setValue("Fries");
        pFoodCharacteristic->setCallbacks(&chrCallbacks);

        /** Custom descriptor: Arguments are UUID, Properties, max length of the value in bytes */
        NimBLEDescriptor* pC01Ddsc =
            pFoodCharacteristic->createDescriptor("C01D",
                                                NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC,
                                                20);
        pC01Ddsc->setValue("Send it back!");
        pC01Ddsc->setCallbacks(&dscCallbacks);

        /** Start the services when finished creating all Characteristics and Descriptors */
        pDeadService->start();
        pBaadService->start();

        /** Create an advertising instance and add the services to the advertised data */
        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->setName("NimBLE-Server");
        pAdvertising->addServiceUUID(pDeadService->getUUID());
        pAdvertising->addServiceUUID(pBaadService->getUUID());
        /**
         *  If your device is battery powered you may consider setting scan response
         *  to false as it will extend battery life at the expense of less data sent.
         */
        pAdvertising->enableScanResponse(true);
        pAdvertising->start();

        printf("Advertising Started\n");

    }
    void Process()
    {
        /** Loop here and send notifications to connected peers */
        if (pServer->getConnectedCount()) {
            NimBLEService* pSvc = pServer->getServiceByUUID("BAAD");
            if (pSvc) {
                NimBLECharacteristic* pChr = pSvc->getCharacteristic("F00D");
                if (pChr) {
                    pChr->notify();
                }
            }
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
};
