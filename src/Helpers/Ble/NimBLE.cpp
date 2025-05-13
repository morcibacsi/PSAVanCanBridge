/*
#include "NimBLE.h"

void queueTask(void *pvParams)
{
  NimBLE *_this = (NimBLE *)pvParams;
  for (;;)
  {
    char rxBuf[bufSize];
    if (xQueueReceive(_this->queue_handler, &rxBuf, (TickType_t)5))
    {
      cout << "Sending : " << rxBuf << endl;
      _this->pTxCharacteristic->setValue((uint8_t *)rxBuf, strlen(rxBuf));
      _this->pTxCharacteristic->notify();
    }
    vTaskDelay(1);
  }
}

void NimBLE::onConnect(BLEServer *pServer, BLEConnInfo &connInfo)
{
  this->deviceConnected = true;
}

void NimBLE::onDisconnect(BLEServer *pServer, BLEConnInfo &connInfo, int reason)
{
  this->deviceConnected = false;
}

void NimBLE::onWrite(BLECharacteristic *pCharacteristic, BLEConnInfo &connInfo)
{
  string income = pCharacteristic->getValue();
  if (income.length() > 1)
  {
    this->rxValue = income;
    this->is_available = true;
    this->bleEvent(this->rxValue, 1);
  }
}

bool NimBLE::isDataAvailable(void)
{
  return this->is_available;
}

string NimBLE::getData(void)
{
  this->is_available = false;
  return this->rxValue;
}

void NimBLE::setReaderHandler(void (*func)(string msg, size_t select))
{
  this->bleEvent = func;
}

void NimBLE::initBle(string ble_name)
{
  BLEDevice::init(ble_name);
  this->pServer = BLEDevice::createServer();
  this->pServer->setCallbacks(this);
  this->pService = this->pServer->createService(this->SERVICE_UUID);

  this->pTxCharacteristic = this->pService->createCharacteristic(
      this->CHARACTERISTIC_UUID_TX,
      NIMBLE_PROPERTY::NOTIFY);

  this->pRxCharacteristic = this->pService->createCharacteristic(
      this->CHARACTERISTIC_UUID_RX,
      NIMBLE_PROPERTY::WRITE);

  this->pRxCharacteristic->setCallbacks(this);
  this->queue_handler = xQueueCreate(200, sizeof(this->txBuf));
  xTaskCreate(queueTask, "queue_task", 4096, this, 2, NULL);
}

void NimBLE::startAdv()
{
  this->pService->start();
  this->pServer->getAdvertising()->start();
}

bool NimBLE::isConnected()
{
  return this->deviceConnected;
}

void NimBLE::pushQueue(char *buf)
{
  xQueueSend(this->queue_handler, buf, (TickType_t)0);
}
*/