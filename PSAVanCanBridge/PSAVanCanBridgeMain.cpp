#pragma region Includes

#include <Arduino.h>
#include <esp_task_wdt.h>

#include "Config.h"
#include "src/SerialPort/AbstractSerial.h"

#ifdef USE_BLUETOOTH_SERIAL
    #include <BluetoothSerial.h>
    #include "src/SerialPort/BluetoothSerialAbs.h"
#else
    #include "src/SerialPort/HardwareSerialAbs.h"
#endif

#include "src/Can/CanMessageSenderEsp32Arduino.h"
#include "src/Van/VanMessageReaderEsp32Rmt.h"
#include "src/Helpers/VinFlashStorageEsp32.h"
#include "src/Helpers/GetDeviceInfoEsp32.h"
#include "src/Can/Structs/CanDisplayStructs.h"
#include "src/Can/Structs/CanDash1Structs.h"
#include "src/Can/Structs/CanIgnitionStructs.h"
#include "src/Can/Structs/CanMenuStructs.h"
#include "src/Can/Handlers/CanRadioRemoteMessageHandler.h"
#include "src/Can/Handlers/CanVinHandler.h"
#include "src/Can/Handlers/CanTripInfoHandler.h"
#include "src/Can/Handlers/CanStatusOfFunctionsHandler.h"
#include "src/Can/Handlers/CanWarningLogHandler.h"
#include "src/Can/Handlers/CanSpeedAndRpmHandler.h"
#include "src/Can/Handlers/CanDash2MessageHandler.h"
#include "src/Can/Handlers/CanDash3MessageHandler.h"
#include "src/Can/Handlers/CanDash4MessageHandler.h"
#include "src/Can/Handlers/CanParkingAidHandler.h"
#include "src/Can/CanIgnitionTask.h"
#include "src/Can/CanDataSenderTask.h"
#include "src/Can/CanDataReaderTask.h"
#include "src/Van/VanDataParserTask.h"
#include "src/Van/VanWriterTask.h"

#if POPUP_HANDLER == 1
    #include "src/Can/Handlers/CanDisplayPopupHandler.h"
#endif
#if POPUP_HANDLER == 2
    #include "src/Can/Handlers/CanDisplayPopupHandler2.h"
#endif
#if POPUP_HANDLER == 3
    #include "src/Can/Handlers/CanDisplayPopupHandler3.h"
#endif

#ifdef USE_NEW_AIRCON_DISPLAY_SENDER
    #include "src/Can/Handlers/CanAirConOnDisplayHandler.h"
#else
    //#include "src/Can/Handlers/CanAirConOnDisplayHandlerOrig.h"
#endif

#include "src/Van/AbstractVanMessageSender.h"
#include "src/Van/IVanMessageReader.h"

#if HW_VERSION == 14
    #include "src/Van/VanMessageSender.h"
#endif

#include "src/Helpers/VanDataToBridgeToCan.h"
#include "src/Helpers/VanIgnitionDataToBridgeToCan.h"
#include "src/Helpers/VanVinToBridgeToCan.h"
#include "src/Helpers/IVinFlashStorage.h"
#include "src/Helpers/IGetDeviceInfo.h"
#include "src/Helpers/SerialReader.h"

#include "src/Can/CanMessageHandlerContainer.h"
#include "src/Can/Handlers/CanNaviPositionHandler.h"
#include "src/Van/VanHandlerContainer.h"
#include "src/Can/Handlers/ICanDisplayPopupHandler.h"
#pragma endregion

const uint8_t VAN_DATA_RX_RMT_CHANNEL = 0;

#if HW_VERSION == 11
    const uint8_t VAN_DATA_RX_PIN = 21;
    const IVAN_LINE_LEVEL VAN_DATA_RX_LINE_LEVEL = LINE_LEVEL_HIGH;

    const uint8_t CAN_RX_PIN = 33;
    const uint8_t CAN_TX_PIN = 32;
#elif HW_VERSION == 14
    const uint8_t VAN_DATA_RX_PIN = 21;
    const IVAN_LINE_LEVEL VAN_DATA_RX_LINE_LEVEL = LINE_LEVEL_HIGH;

    const uint8_t CAN_RX_PIN = 18;
    const uint8_t CAN_TX_PIN = 15;

    TaskHandle_t VANWriteTask;
#endif

const uint8_t VAN_DATA_RX_LED_INDICATOR_PIN = 2;

VanDataToBridgeToCan dataToBridge;
VanIgnitionDataToBridgeToCan ignitionDataToBridge;
VanVinToBridgeToCan vinDataToBridge;

TaskHandle_t CANSendIgnitionTask;
TaskHandle_t CANSendDataTask;

TaskHandle_t VANReadTask;
TaskHandle_t CANReadTask;

AbstractCanMessageSender* CANInterface;
ICanDisplayPopupHandler* canPopupHandler;
CanVinHandler* canVinHandler;
CanTripInfoHandler* tripInfoHandler;
//CanAirConOnDisplayHandler* canAirConOnDisplayHandler;
CanRadioRemoteMessageHandler* canRadioRemoteMessageHandler;
CanStatusOfFunctionsHandler* canStatusOfFunctionsHandler;
CanWarningLogHandler* canWarningLogHandler;
CanSpeedAndRpmHandler* canSpeedAndRpmHandler;
CanDash2MessageHandler* canDash2MessageHandler;
CanDash3MessageHandler* canDash3MessageHandler;
CanDash4MessageHandler* canDash4MessageHandler;
CanIgnitionPacketSender* radioIgnition;
CanDashIgnitionPacketSender* dashIgnition;
CanParkingAidHandler* canParkingAid;
CanRadioButtonPacketSender* canRadioButtonSender;
CanNaviPositionHandler* canNaviPositionHandler;

CanMessageHandlerContainer* canMessageHandlerContainer;
VanHandlerContainer* vanHandlerContainer;

IVanMessageReader* vanReader;
IVinFlashStorage* vinFlashStorage;
IGetDeviceInfo* deviceInfo;
CanIgnitionTask* canIgnitionTask;
CanDataSenderTask* canDataSenderTask;
CanDataReaderTask* canDataReaderTask;
VanDataParserTask* vanDataParserTask;
VanWriterTask* vanWriterTask;

SerialReader* serialReader;

AbsSer *serialPort;

unsigned long currentTime = 0;

#ifdef USE_BLUETOOTH_SERIAL
    BluetoothSerial SerialBT;
#endif

void PrintArrayToSerial(const uint8_t vanMessage[], uint8_t vanMessageLength)
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

void CANReadTaskFunction(void * parameter)
{
    for (;;)
    {
        canDataReaderTask->ReadData();

        vTaskDelay(10 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
    }
}

void CANSendDataTaskFunction(void * parameter)
{
    for (;;)
    {
        canDataSenderTask->SendData(dataToBridge);

        vTaskDelay(10 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
    }
}

void CANSendIgnitionTaskFunction(void * parameter)
{
    for (;;)
    {
        currentTime = millis();

        canIgnitionTask->SendIgnition(ignitionDataToBridge, vinDataToBridge, currentTime);

        vTaskDelay(40 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
    }
}

void VANReadTaskFunction(void * parameter)
{
    uint8_t vanMessage[32];
    uint8_t msgLength;

    for (;;)
    {
        serialReader->Receive(&msgLength, vanMessage);

        if (msgLength == 0)
        {
            vanReader->Receive(&msgLength, vanMessage);
        }

        if (msgLength > 0 && vanReader->IsCrcOk(vanMessage, msgLength))
        {
            if (true)
            {
                PrintArrayToSerial(vanMessage, msgLength);
            }

            vanDataParserTask->ProcessData(vanMessage, msgLength, &dataToBridge, &ignitionDataToBridge, &vinDataToBridge);
        }

        if (!USE_IGNITION_SIGNAL_FROM_VAN_BUS)
        {
            dataToBridge.Ignition = 1;
            ignitionDataToBridge.Ignition = 1;
            ignitionDataToBridge.EconomyModeActive = 0;
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
    }
}

#if HW_VERSION == 14
void VANWriteTaskFunction(void* parameter)
{
    for (;;)
    {
        currentTime = millis();
        vanWriterTask->Process(ignitionDataToBridge, currentTime);

        vTaskDelay(10 / portTICK_PERIOD_MS);
        esp_task_wdt_reset();
    }
}
#endif

void InitSerialPort()
{
    uint16_t uniqueIdForBluetooth = 0;
    uniqueIdForBluetooth = deviceInfo->GetId();

    char bluetoothDeviceName[21];
    snprintf(bluetoothDeviceName, 27, "VAN-CAN Bridge %04X", uniqueIdForBluetooth);

#ifdef USE_BLUETOOTH_SERIAL
    serialPort = new BluetoothSerAbs(SerialBT, bluetoothDeviceName);
#else
    serialPort = new HwSerAbs(Serial);
#endif

    serialPort->begin(500000);
    serialPort->println(bluetoothDeviceName);
}

void setup()
{
    vinFlashStorage = new VinFlashStorageEsp32();
    deviceInfo = new GetDeviceInfoEsp32();

    vanReader = new VanMessageReaderEsp32Rmt();
    vanReader->Init(VAN_DATA_RX_PIN, VAN_DATA_RX_LED_INDICATOR_PIN, VAN_DATA_RX_LINE_LEVEL, NETWORK_TYPE_COMFORT);

    InitSerialPort();

    const bool success = vinFlashStorage->Load();
    if (success)
    {
        serialPort->println("Using VIN from flash");
    }

    //CANInterface = new CanMessageSender(CAN_RX_PIN, CAN_TX_PIN);
    CANInterface = new CanMessageSenderEsp32Arduino(CAN_RX_PIN, CAN_TX_PIN, true);
    CANInterface->Init();

#if POPUP_HANDLER == 1
    canPopupHandler = new CanDisplayPopupHandler(CANInterface);
#endif
#if POPUP_HANDLER == 2
    canPopupHandler = new CanDisplayPopupHandler2(CANInterface);
#endif
#if POPUP_HANDLER == 3
    canPopupHandler = new CanDisplayPopupHandler3(CANInterface);
#endif

    canVinHandler = new CanVinHandler(CANInterface);
    tripInfoHandler = new CanTripInfoHandler(CANInterface);
    //canAirConOnDisplayHandler = new CanAirConOnDisplayHandler(CANInterface);
    canRadioRemoteMessageHandler = new CanRadioRemoteMessageHandler(CANInterface);
    canStatusOfFunctionsHandler = new CanStatusOfFunctionsHandler(CANInterface);
    canWarningLogHandler = new CanWarningLogHandler(CANInterface);
    canSpeedAndRpmHandler = new CanSpeedAndRpmHandler(CANInterface);
    canDash2MessageHandler = new CanDash2MessageHandler(CANInterface);
    canDash3MessageHandler = new CanDash3MessageHandler(CANInterface);
    canDash4MessageHandler = new CanDash4MessageHandler(CANInterface);
    radioIgnition = new CanIgnitionPacketSender(CANInterface);
    dashIgnition = new CanDashIgnitionPacketSender(CANInterface);
    canParkingAid = new CanParkingAidHandler(CANInterface);
    canRadioButtonSender = new CanRadioButtonPacketSender(CANInterface);
    canNaviPositionHandler = new CanNaviPositionHandler(CANInterface);

    canMessageHandlerContainer = new CanMessageHandlerContainer(CANInterface, serialPort, vinFlashStorage);

    vanHandlerContainer = new VanHandlerContainer(
        canPopupHandler,
        tripInfoHandler,
        canStatusOfFunctionsHandler,
        canWarningLogHandler,
        canRadioRemoteMessageHandler);

    serialReader = new SerialReader(serialPort, CANInterface, tripInfoHandler, canRadioButtonSender, vinFlashStorage);
    canIgnitionTask = new CanIgnitionTask(radioIgnition, dashIgnition, canParkingAid, canRadioRemoteMessageHandler, canStatusOfFunctionsHandler, canPopupHandler, canWarningLogHandler, canVinHandler);
    canDataSenderTask = new CanDataSenderTask(canSpeedAndRpmHandler, tripInfoHandler, canPopupHandler, canRadioRemoteMessageHandler, canDash2MessageHandler, canDash3MessageHandler, canDash4MessageHandler, canRadioButtonSender, canNaviPositionHandler);
    canDataReaderTask = new CanDataReaderTask(CANInterface, canPopupHandler, canRadioRemoteMessageHandler, canMessageHandlerContainer, canDataSenderTask);
    vanDataParserTask = new VanDataParserTask(serialPort, canVinHandler, vanHandlerContainer);
    vanWriterTask = new VanWriterTask();

    xTaskCreatePinnedToCore(
        CANSendIgnitionTaskFunction,    // Function to implement the task
        "CANSendIgnitionTask",          // Name of the task
        15000,                          // Stack size in words
        NULL,                           // Task input parameter
        2,                              // Priority of the task
        &CANSendIgnitionTask,           // Task handle.
        0);                             // Core where the task should run

    xTaskCreatePinnedToCore(
        CANSendDataTaskFunction,        // Function to implement the task
        "CANSendDataTask",              // Name of the task
        15000,                          // Stack size in words
        NULL,                           // Task input parameter
        0,                              // Priority of the task
        &CANSendDataTask,               // Task handle.
        0);                             // Core where the task should run

    xTaskCreatePinnedToCore(
        VANReadTaskFunction,            // Function to implement the task
        "VANReadTask",                  // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        1,                              // Priority of the task
        &VANReadTask,                   // Task handle.
        1);                             // Core where the task should run

    xTaskCreatePinnedToCore(
        CANReadTaskFunction,            // Function to implement the task
        "CANReadTask",                  // Name of the task
        10000,                          // Stack size in words
        NULL,                           // Task input parameter
        0,                              // Priority of the task
        &CANReadTask,                   // Task handle.
        1);                             // Core where the task should run

#if HW_VERSION == 14
    xTaskCreatePinnedToCore(
        VANWriteTaskFunction,                        // Function to implement the task
        "VANWriteTask",                  // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        1,                              // Priority of the task
        &VANWriteTask,                   // Task handle.
        1);                             // Core where the task should run
#endif

    esp_task_wdt_init(TASK_WATCHDOG_TIMEOUT, true);
    esp_task_wdt_add(VANReadTask);
}

void loop()
{
    vTaskDelay(50 / portTICK_PERIOD_MS);
    esp_task_wdt_reset();
}
