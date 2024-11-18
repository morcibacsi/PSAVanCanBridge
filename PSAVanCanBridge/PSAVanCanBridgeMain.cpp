#include <Arduino.h>
#include <SPI.h>
#include "Config.h"
#include "src/Can/CanMessageSenderEsp32Idf.h"
#include "src/Can/CanMessageSenderOnSerial.h"
#include "src/Can/CanDataSenderTask.h"
#include "src/Can/ICanMessageSender.h"

#include "src/Can/Handlers/MessageHandler_21F.h"
#include "src/Can/Handlers/MessageHandler_128.h"
#include "src/Can/Handlers/MessageHandler_161.h"
#include "src/Can/Handlers/MessageHandler_3A7.h"
#include "src/Can/Handlers/MessageHandler_1A8.h"

#include "src/Can/Handlers/TpMessageHandler_760.h"

#include "src/Can/CanMessageHandlerContainer.h"

#include "src/Van/IVanMessageReader.h"
#include "src/Van/VanMessageReaderEsp32Rmt.h"

#include "src/Helpers/IGetDeviceInfo.h"
#include "src/Helpers/GetDeviceInfoEsp32.h"
#include "src/Helpers/SerialReader.h"
#include "src/Helpers/DataBroker.h"
#include "src/Helpers/VanCrcCalculator.h"
#include "src/Helpers/IntUnions.h"
#include "src/Helpers/CrcStore.h"
#include "src/Helpers/CanPinConfigHandler.h"
#include "src/Helpers/VinFlashStorageEsp32.h"
#include "src/Helpers/ConfigStorageEsp32.h"
#include "src/Helpers/DebugPrint.h"

#include "src/Van/Structs/VAN_4FC.h"
#include "src/Can/Structs/CAN_128.h"
#include "src/Can/Structs/CAN_217.h"
#include "src/Can/Structs/CAN_167.h"

#include "src/Van/VanWriterTask.h"
#include "Esp32RmtChineseParkingAidReader.h"

#ifdef BOARD_TYPE_ESP32
    #include "BoardConfig_ESP32.h"
#endif
#ifdef BOARD_TYPE_ESP32_v15
    #include "BoardConfig_ESP32_v15.h"
#endif
#ifdef BOARD_TYPE_tamc_termod_s3
    #include "BoardConfig_ESP32_tamc_termod_s3.h"
#endif

#ifdef USE_BLUETOOTH_SERIAL
    #include <BluetoothSerial.h>
    #include "src/SerialPort/BluetoothSerialAbs.h"
    BluetoothSerial SerialBT;
#else
    #include "src/SerialPort/HardwareSerialAbs.h"
    #include "src/SerialPort/USBSerialAbs.h"
#endif

#ifdef WIFI_ENABLED
    #ifdef WEBSOCKET_SERIAL
        #include "src/SerialPort/WebSocketSerialAbs.h"
    #endif
    #include "./src/Helpers/WebPageService.h"
    WebPageService* webPageService;
    TaskHandle_t RunWebPageTask;
#endif

const IVAN_LINE_LEVEL VAN_DATA_RX_LINE_LEVEL = LINE_LEVEL_HIGH;

AbsSer *serialPort;
IGetDeviceInfo *deviceInfo;
ICanMessageSender *canInterface;
CanDataSenderTask *canDataSenderTask;
VanWriterTask* vanWriterTask;
DataBroker *dataBroker;
Config *config;
VanCrcCalculator *crcCalculator;
CrcStore *crcStore;

TaskHandle_t CANSendDataTask;
TaskHandle_t VANReadDataTask;
TaskHandle_t VANWriteTask;
TaskHandle_t CANReadTask;
TaskHandle_t ParkingAidReadTask;

CanMessageHandlerContainer *canMessageHandlerContainer;
TpMessageHandler_760 *radioDiag;
CanPinConfigHandler *pinConfigHandler;
IVinFlashStorage *vinFlashStorage;
ConfigStorageEsp32 *configStorage;

struct VANMessage
{
    uint8_t Length;
    uint8_t Data[34];
} ReceivedVANMessage;

void PrintVANArrayToSerialForPython(const uint16_t vanId, const uint8_t vanMessage[], uint8_t vanMessageLength)
{
    char tmp[3];

    serialPort->print(vanId, HEX);
    serialPort->print(",1,");
    serialPort->print(vanMessageLength-5, DEC);
    serialPort->print(",");

    // strip the CRC
    if (vanMessageLength - 5 > 0)
    {
        for (size_t i = 3; i < vanMessageLength-2; i++)
        {
            snprintf(tmp, 3, "%02X", vanMessage[i]);
            if (i != vanMessageLength - 3)
            {
                serialPort->print(tmp);
                serialPort->print("");
            }
        }
        serialPort->println(tmp);
    }
    else
    {
        serialPort->println();
    }
}

void PrintVANArrayToSerial(const uint8_t vanMessage[], uint8_t vanMessageLength)
{
    uint16_t vanId = (vanMessage[1] << 8 | vanMessage[2]) >> 4;

    //if (!(vanId == 0xAE8 && vanMessageLength > 27))
    //
    /*
    if (!(vanId == 0xA68) && !(vanId == 0xAE8))
    {
        return;
    }
    //*/

    bool sendToPython = false;
    if (sendToPython)
    {
        PrintVANArrayToSerialForPython(vanId, vanMessage, vanMessageLength);
    }
    else
    {
        char tmp[3];
        for (uint8_t i = 0; i < vanMessageLength; i++)
        {
            snprintf(tmp, 3, "%02X", vanMessage[i]);
            if (i != vanMessageLength - 1)
            {
                serialPort->print(tmp);
                serialPort->print(" ");
            }
            else
            {
                serialPort->println(tmp);
            }
        }
    }
}

void PrintCANArrayToSerial(const uint16_t canId, const uint8_t canMessage[], uint8_t canLength)
{
    char tmp[3];

    if (canId > 0)
    {
        //if (!(canId == 0x760 || canId == 0x660))
        //if (!(canId == 0x0E1))
        {
            return;
        }
        serialPort->print(canId, HEX);
        serialPort->print(",1,");
        serialPort->print(canLength, DEC);
        serialPort->print(",");

        for (size_t i = 0; i < canLength; i++)
        {
            snprintf(tmp, 3, "%02X", canMessage[i]);
            if (i != canLength - 1)
            {
                serialPort->print(tmp);
                serialPort->print("");
            }
        }
        serialPort->println(tmp);
    }
}

bool IsMessageToProcess(uint16_t vanId)
{
    switch (vanId)
    {
        case 0x8A4:
        case 0x4FC:
        case 0x744:
        case 0x9C4:
        case 0x824:
        case 0x524:
        case 0x564:
        case 0x464:
        case 0x4DC:
        case 0xA68:
        case 0xAE8:
        case 0x4D4:
        case 0x554:
        case 0x8C4:
        case 0x8D4:
        case 0xE24:
            return true;
        default:
            return false;
    }
}

void CANReadTaskFunction(void * parameter)
{
    uint16_t canId;
    uint8_t canLength;
    uint8_t canMessage[8];
    unsigned long currentTime;

    for (;;)
    {
        canId = 0;
        canInterface->ReadMessage(&canId, &canLength, canMessage);
        currentTime = millis();

        if (canId > 0)
        {
            pinConfigHandler->ProcessMessage(canId, canLength, canMessage);

            radioDiag->Receive(currentTime, canId, canLength, canMessage);

            PrintCANArrayToSerial(canId, canMessage, canLength);
            if (canId == 0x217)
            {
                CAN_217Struct canPacket;
                memcpy(&canPacket, canMessage, sizeof(canPacket));

                if (canPacket.Field2.data.reset_trip)
                {
                    canDataSenderTask->ResetTripOnCMB();
                    configStorage->Save();
                }

                if (config->USE_BRIGHTNESS_FROM_CAN_ODOMETER)
                {
                    canDataSenderTask->SetBrightness(canPacket.Brightness.data.brightness, canPacket.Brightness.data.black_panel);
                }
            }

            if (canId == 0x167)
            {
                CAN2004_167Struct canPacket;
                memcpy(&canPacket, canMessage, sizeof(canPacket));

                dataBroker->PageDisplayedOnTypeC = canPacket.EMFRequest.data.trip_data_on_odometer;
            }

            if (canId == 0x257)
            {
                debug_print("Odo km:");
                debug_print(canMessage[0], HEX);
                debug_print(canMessage[1], HEX);
                debug_print(canMessage[2], HEX);
                debug_println(canMessage[3], HEX);
            }
        }

        radioDiag->Process(currentTime);
        canDataSenderTask->Process(currentTime);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void IRAM_ATTR VANReadDataTaskFunction(void * parameter)
{
    unsigned long currentTime = 0;
    UInt16 crc;

    VANMessage vanMessageItem;

    SerialReader *serialReader = new SerialReader(serialPort, config, dataBroker);
    IVanMessageReader *vanMessageReader;

    if (!config->REPLAY_MODE)
    {
        vanMessageReader = new VanMessageReaderEsp32Rmt(BOARD_VAN_DATA_RX_PIN, BOARD_LED_PIN, VAN_DATA_RX_LINE_LEVEL, NETWORK_TYPE_COMFORT);
        vanMessageReader->Init();
    }

    for (;;)
    {
        currentTime = millis();

        serialReader->Receive(&vanMessageItem.Length, vanMessageItem.Data);
        if (!config->REPLAY_MODE)
        {
            vanMessageReader->Receive(&vanMessageItem.Length, vanMessageItem.Data);
        }

        if (vanMessageItem.Length > 0)
        {
            if (!config->REPLAY_MODE && dataBroker->IsVanLogEnabled)
            {
                PrintVANArrayToSerial(vanMessageItem.Data, vanMessageItem.Length);
            }
            if (crcCalculator->IsCrcOk(vanMessageItem.Data, vanMessageItem.Length))
            {
                uint16_t vanId = (vanMessageItem.Data[1] << 8 | vanMessageItem.Data[2]) >> 4;

                crc.data.leftByte = vanMessageItem.Data[vanMessageItem.Length-2];
                crc.data.rightByte = vanMessageItem.Data[vanMessageItem.Length-1];

                if(IsMessageToProcess(vanId) &&
                   !crcStore->IsCrcSameAsPrevious(vanId, crc.asUint16))
                {
                    canDataSenderTask->ProcessVanMessage(currentTime, vanMessageItem.Data, vanMessageItem.Length);
                    dataBroker->ProcessedPackets++;
                }

                if (vanId == 0x8A4)
                {
                    canDataSenderTask->IgnitionPacketArrived(currentTime);
                }
            }
        }
        if (config->REPLAY_MODE)
        {
            vTaskDelay(15 / portTICK_PERIOD_MS);
        }
    }
}

void CANSendDataTaskFunction(void * parameter)
{
    unsigned long currentTime = 0;

    for (;;)
    {
        currentTime = millis();

        if (true)
        {
            canMessageHandlerContainer->SendDueMessages(currentTime);
            canDataSenderTask->SendCanMessage(currentTime);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void VANWriteTaskFunction(void* parameter)
{
    unsigned long currentTime = 0;
    for (;;)
    {
        currentTime = millis();
        vanWriterTask->Process(currentTime);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

#ifdef WIFI_ENABLED
void RunWebPageTaskFunction(void* parameter)
{
    for (;;)
    {
        webPageService->Loop();

        vTaskDelay(15 / portTICK_PERIOD_MS);
    }
}
#endif

void ParkingAidReadTaskFunction(void* parameter)
{
    uint8_t parkRadarMessageLength;
    uint8_t parkRadarMessage[4];

    Esp32RmtReader *parkingAidReader;
    parkingAidReader = new Esp32RmtChineseParkingAidReader(2, 26, -1);
    parkingAidReader->Start();

    for (;;)
    {
        // sensor order: A D C B

        if (config->PARKING_AID_TYPE == 2)
        {
            parkingAidReader->ReceiveData(&parkRadarMessageLength, parkRadarMessage);
            if (parkRadarMessageLength == 4)
            {
                if (parkRadarMessage[0] < 25)
                {
                    dataBroker->ParkingExteriorRearLeft = parkRadarMessage[0] * 10;
                }
                else
                {
                    dataBroker->ParkingExteriorRearLeft = 0xFF;
                }

                if (parkRadarMessage[3] < 25)
                {
                    dataBroker->ParkingInteriorRearLeft = parkRadarMessage[3] * 10;
                }
                else
                {
                    dataBroker->ParkingInteriorRearLeft = 0xFF;
                }

                if (parkRadarMessage[2] < 25)
                {
                    dataBroker->ParkingInteriorRearRight = parkRadarMessage[2] * 10;
                }
                else
                {
                    dataBroker->ParkingInteriorRearRight = 0xFF;
                }

                if (parkRadarMessage[1] < 25)
                {
                    dataBroker->ParkingExteriorRearRight = parkRadarMessage[1] * 10;
                }
                else
                {
                    dataBroker->ParkingExteriorRearRight = 0xFF;
                }
                //printf("parking aid: %d %d %d %d\n", dataBroker->ParkingExteriorRearLeft, dataBroker->ParkingInteriorRearLeft, dataBroker->ParkingInteriorRearRight, dataBroker->ParkingExteriorRearRight);
                //printf("parking aid: %d %d %d %d\n", parkRadarMessage[0], parkRadarMessage[3], parkRadarMessage[2], parkRadarMessage[1]);
                dataBroker->HasParkingRadarData = 1;
                //dataBroker->IsReverseEngaged = 1;
                canMessageHandlerContainer->SetData(0x0E1);
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}


void InitSerialPort()
{
    uint16_t uniqueIdForBluetooth = 0;
    uniqueIdForBluetooth = deviceInfo->GetId();

    char bluetoothDeviceName[21];
    snprintf(bluetoothDeviceName, 27, "VAN-CAN Bridge %04X", uniqueIdForBluetooth);

#ifdef USE_BLUETOOTH_SERIAL
    serialPort = new BluetoothSerAbs(SerialBT, bluetoothDeviceName);
#else
    #if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
        serialPort = new UsbSerAbs(Serial);
    #else
        serialPort = new HwSerAbs(Serial);
    #endif
#endif

    serialPort->begin(500000);
    serialPort->println(bluetoothDeviceName);
}

void setup()
{
    deviceInfo = new GetDeviceInfoEsp32();
    dataBroker = new DataBroker();
    config = new Config();
    crcCalculator = new VanCrcCalculator();
    crcStore = new CrcStore();

    InitSerialPort();
    canInterface = new CanMessageSenderEsp32Idf(BOARD_CAN_RX_PIN, BOARD_CAN_TX_PIN, serialPort, CanMessageSenderEsp32Idf::CAN_CONTROLLER_0);
    //canInterface = new CanMessageSenderOnSerial(serialPort);
    canInterface->Init();

    configStorage = new ConfigStorageEsp32(config);
    configStorage->Load();

    vinFlashStorage = new VinFlashStorageEsp32(config);
    vinFlashStorage->Load();

    canMessageHandlerContainer = new CanMessageHandlerContainer(canInterface, config, dataBroker);
    radioDiag = new TpMessageHandler_760(canInterface, vinFlashStorage, config, canMessageHandlerContainer);
    pinConfigHandler = new CanPinConfigHandler(config, radioDiag, dataBroker);

    vanWriterTask = new VanWriterTask(config, dataBroker);

    canDataSenderTask = new CanDataSenderTask(
        canInterface,
        config,
        dataBroker,
        serialPort,
        canMessageHandlerContainer,
        vanWriterTask
    );

    #ifdef WIFI_ENABLED
        webPageService = new WebPageService(canMessageHandlerContainer, config, configStorage, vinFlashStorage, radioDiag);

        #ifdef WEBSOCKET_SERIAL
        serialPort = new WebSocketSerAbs(webPageService->GetHTTPServer(), "/log");
        #endif

        webPageService->Start();
    #endif

//core 0
#define CAN_SEND_TASK_PRIORITY 3
#define CAN_READ_TASK_PRIORITY 2
#define PARKING_AID_TASK_PRIORITY 1
#define WEB_PAGE_TASK_PRIORITY 0
//core 1
#define VAN_READ_TASK_PRIORITY 1
#define VAN_WRITE_TASK_PRIORITY 0


    xTaskCreatePinnedToCore(
        CANSendDataTaskFunction,        // Function to implement the task
        "CANSendDataTask",              // Name of the task
        15000,                          // Stack size in words
        NULL,                           // Task input parameter
        CAN_SEND_TASK_PRIORITY,         // Priority of the task (higher the number, higher the priority)
        &CANSendDataTask,               // Task handle.
        0);                             // Core where the task should run
///*
    xTaskCreatePinnedToCore(
        VANReadDataTaskFunction,        // Function to implement the task
        "VANReadDataTask",              // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        VAN_READ_TASK_PRIORITY,         // Priority of the task (higher the number, higher the priority)
        &VANReadDataTask,               // Task handle.
        1);                             // Core where the task should run
//*/
    xTaskCreatePinnedToCore(
        VANWriteTaskFunction,           // Function to implement the task
        "VANWriteTask",                 // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        VAN_WRITE_TASK_PRIORITY,        // Priority of the task (higher the number, higher the priority)
        &VANWriteTask,                  // Task handle.
        1);                             // Core where the task should run
    xTaskCreatePinnedToCore(
        CANReadTaskFunction,            // Function to implement the task
        "CANReadTask",                  // Name of the task
        10000,                          // Stack size in words
        NULL,                           // Task input parameter
        CAN_READ_TASK_PRIORITY,         // Priority of the task (higher the number, higher the priority)
        &CANReadTask,                   // Task handle.
        0);                             // Core where the task should run
    xTaskCreatePinnedToCore(
        ParkingAidReadTaskFunction,     // Function to implement the task
        "ParkingAidReadTask",           // Name of the task
        10000,                          // Stack size in words
        NULL,                           // Task input parameter
        PARKING_AID_TASK_PRIORITY,      // Priority of the task (higher the number, higher the priority)
        &ParkingAidReadTask,            // Task handle.
        0);                             // Core where the task should run

#ifdef WIFI_ENABLED
    xTaskCreatePinnedToCore(
        RunWebPageTaskFunction,         // Function to implement the task
        "RunWebPageTask",               // Name of the task
        10000,                          // Stack size in words
        NULL,                           // Task input parameter
        WEB_PAGE_TASK_PRIORITY,         // Priority of the task (higher the number, higher the priority)
        &RunWebPageTask,                // Task handle.
        0);                             // Core where the task should run
#endif
}

void loop()
{
    vTaskDelay(50 / portTICK_PERIOD_MS);
    //esp_task_wdt_reset();
}
