/* ESP32 VAN bus to CAN bus protocol bridge software for Peugeot 307
 * 
 * The software is distributed under Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
*/
#define USE_BLUETOOTH_SERIAL
#define USE_IGNITION_SIGNAL_FROM_VAN_BUS
//#define HW_VERSION 12
#define HW_VERSION 14
//#define USE_NEW_AIRCON_DISPLAY_SENDER

#pragma region Includes
#include <Arduino.h>
#include <esp32_arduino_rmt_van_rx.h>
#include <ArduinoLog.h>
#include <BluetoothSerial.h>
#include "AbstractSerial.h"
#include "HardwareSerialAbs.h"
#include "BluetoothSerialAbs.h"

#include "Serializer.h"

#include "AbstractCanMessageSender.h"
//#include "CanMessageSender.h"
#include "CanMessageSenderEsp32Arduino.h"
#include "CanDisplayStructs.h"
#include "CanDash1Structs.h"
#include "CanIgnitionStructs.h"
#include "CanMenuStructs.h"
#include "CanDisplayPopupItem.h"
#include "CanRadioRemoteMessageHandler.h"
#include "CanVinHandler.h"
#include "CanTripInfoHandler.h"
#ifdef USE_NEW_AIRCON_DISPLAY_SENDER
    #include "CanAirConOnDisplayHandler.h"
#else
    #include "CanAirConOnDisplayHandlerOrig.h"
#endif
#include "CanStatusOfFunctionsHandler.h"
#include "CanWarningLogHandler.h"
#include "CanSpeedAndRpmHandler.h"
#include "CanDash2MessageHandler.h"
#include "CanDash3MessageHandler.h"
#include "CanDash4MessageHandler.h"
#include "CanDisplayStatusStructs.h"
#include "CanDisplayPopupHandler.h"
#include "VanCanDisplayPopupMap.h"

#include "VanMessageSender.h"
#include "VanVinStructs.h"
#include "VanCanAirConditionerSpeedMap.h"
#include "DoorStatus.h"
#include "VanDataToBridgeToCan.h"
#include "VanIgnitionDataToBridgeToCan.h"

#include "AbstractVanMessageHandler.h"
#include "VanAirConditioner1Handler.h"
#include "VanAirConditioner2Handler.h"
#include "VanCarStatusWithTripComputerHandler.h"
#include "VanDashboardHandler.h"
#include "VanDisplayHandler.h"
#include "VanInstrumentClusterV1Handler.h"
#include "VanInstrumentClusterV2Handler.h"
#include "VanRadioRemoteHandler.h"
#include "VanSpeedAndRpmHandler.h"

#pragma endregion

ESP32_RMT_VAN_RX VAN_RX;

const uint8_t VAN_DATA_RX_RMT_CHANNEL = 0;

#if HW_VERSION == 12
const uint8_t VAN_DATA_RX_PIN = 21;
const VAN_LINE_LEVEL VAN_DATA_RX_LINE_LEVEL = VAN_LINE_LEVEL_LOW;

const uint8_t CAN_RX_PIN = 33;
const uint8_t CAN_TX_PIN = 32;
#elif HW_VERSION == 14
const uint8_t VAN_DATA_RX_PIN = 21;
const VAN_LINE_LEVEL VAN_DATA_RX_LINE_LEVEL = VAN_LINE_LEVEL_HIGH;

const uint8_t CAN_RX_PIN = 18;
const uint8_t CAN_TX_PIN = 15;
#endif

const uint8_t VAN_DATA_RX_LED_INDICATOR_PIN = 2;
const uint8_t FUEL_TANK_CAPACITY_IN_LITERS = 60;
const bool SILENT_MODE = false;

struct VanVinToBridgeToCan
{
    uint8_t Vin[17] = { 0 };
};

TaskHandle_t CANSendIgnitionTask;
TaskHandle_t CANSendDataTask;

TaskHandle_t VANReadTask;
TaskHandle_t CANReadTask;

const uint8_t QUEUE_SIZE = 1;
QueueHandle_t dataQueue;
QueueHandle_t ignitionQueue;
QueueHandle_t vinQueue;

AbstractCanMessageSender* CANInterface;
CanDisplayPopupHandler* canPopupHandler;
VanCanDisplayPopupMap* popupMapping;
CanVinHandler* canVinHandler;
CanTripInfoHandler* tripInfoHandler;
CanAirConOnDisplayHandler* canAirConOnDisplayHandler;
CanRadioRemoteMessageHandler* canRadioRemoteMessageHandler;
VanCanAirConditionerSpeedMap* vanCanAirConditionerSpeedMap;
CanStatusOfFunctionsHandler* canStatusOfFunctionsHandler;
CanWarningLogHandler* canWarningLogHandler;
CanSpeedAndRpmHandler* canSpeedAndRpmHandler;
CanDash2MessageHandler* canDash2MessageHandler;
CanDash3MessageHandler* canDash3MessageHandler;
CanDash4MessageHandler* canDash4MessageHandler;
CanIgnitionPacketSender* radioIgnition;
CanDashIgnitionPacketSender* dashIgnition;

const uint8_t VAN_MESSAGE_HANDLER_COUNT = 9;
AbstractVanMessageHandler* vanMessageHandlers[VAN_MESSAGE_HANDLER_COUNT];

AbsSer *serialPort;

#ifdef USE_BLUETOOTH_SERIAL
    BluetoothSerial SerialBT;
#endif

void CANReadTaskFunction(void * parameter)
{
    uint8_t canReadMessage[20] = { 0 };
    uint8_t canReadMessageLength = 0;
    uint16_t canId = 0;

    for (;;)
    {
        canId = 0;
        canReadMessageLength = 0;
        CANInterface->ReadMessage(&canId, &canReadMessageLength, canReadMessage);

        if (canId > 0)
        {
            if (canId == CAN_ID_MENU_BUTTONS)
            {
                CanMenuPacket packet = DeSerialize<CanMenuPacket>(canReadMessage);
                if (packet.data.EscOkField.esc == 1 && canPopupHandler->IsPopupVisible())
                {
                    canPopupHandler->HideCurrentPopupMessage();
                }
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void CANSendDataTaskFunction(void * parameter)
{
    unsigned long currentTime = millis();
    uint8_t ignition = 0;
    uint8_t trip0Icon1Data = 0;

    VanDataToBridgeToCan dataToBridgeReceived;
    VanDataToBridgeToCan dataToBridge;

    for (;;)
    {
        currentTime = millis();

        #ifdef USE_IGNITION_SIGNAL_FROM_VAN_BUS
            ignition = dataToBridge.Ignition;
        #else
            ignition = 1;

            canSpeedAndRpmHandler->SetData(dataToBridge.Speed, dataToBridge.Rpm);
            canSpeedAndRpmHandler->Process(currentTime);
        #endif // USE_IGNITION_SIGNAL_FROM_VAN_BUS

        if (xQueueReceive(dataQueue, &dataToBridgeReceived, portMAX_DELAY) == pdTRUE)
        {
            dataToBridge = dataToBridgeReceived;

            #pragma  region SpeedAndRpm

            canSpeedAndRpmHandler->SetData(dataToBridge.Speed, dataToBridge.Rpm);
            canSpeedAndRpmHandler->Process(currentTime);

            #pragma endregion

            #pragma region TripInfo

            trip0Icon1Data = round(FUEL_TANK_CAPACITY_IN_LITERS * dataToBridge.FuelLevel / 100);

            if (dataToBridge.LeftStickButtonPressed)
            {
                trip0Icon1Data = dataToBridge.FuelLevel;
            }
            //trip0Icon1Data = dataToBridge.FuelLeftToPump;

            tripInfoHandler->SetTripData(
                dataToBridge.Rpm,
                dataToBridge.Speed,
                dataToBridge.Trip1Distance,
                dataToBridge.Trip1Speed,
                dataToBridge.Trip1Consumption,
                dataToBridge.Trip2Distance,
                dataToBridge.Trip2Speed,
                dataToBridge.Trip2Consumption,
                dataToBridge.FuelConsumption,
                trip0Icon1Data
            );
            tripInfoHandler->Process(currentTime);

            #pragma endregion

            #pragma region PopupMessage

            canPopupHandler->Process(currentTime);

            #pragma endregion

            #pragma region Radio remote

            canRadioRemoteMessageHandler->Process(currentTime);

            #pragma endregion

            #pragma region AirCon

            if (!canPopupHandler->IsPopupVisible())
            {
#ifdef USE_NEW_AIRCON_DISPLAY_SENDER
                canAirConOnDisplayHandler->SetData(
                    dataToBridge.InternalTemperature, 
                    dataToBridge.InternalTemperature, 
                    0, 
                    0, // auto mode
                    dataToBridge.IsHeatingPanelPoweredOn == 1 && dataToBridge.IsAirConRunning == 0, //displays: a/c off
                    dataToBridge.IsHeatingPanelPoweredOn == 0, // displays: off
                    dataToBridge.IsWindowHeatingOn == 1, // displays: windshield icon
                    dataToBridge.AirConFanSpeed,
                    dataToBridge.IsAirRecyclingOn);
                canAirConOnDisplayHandler->Process(currentTime);
#else
                canAirConOnDisplayHandler->SendCanAirConToDisplay(
                    currentTime,
                    dataToBridge.InternalTemperature,
                    dataToBridge.InternalTemperature,
                    0,
                    0, // auto mode
                    dataToBridge.IsHeatingPanelPoweredOn == 1 && dataToBridge.IsAirConRunning == 0, //displays: a/c off
                    dataToBridge.IsHeatingPanelPoweredOn == 0, // displays: off
                    dataToBridge.IsWindowHeatingOn == 1, // displays: windshield icon
                    dataToBridge.AirConFanSpeed,
                    dataToBridge.IsAirRecyclingOn);
#endif
            }

            #pragma endregion

            #pragma region Lights

            canDash2MessageHandler->SetData(
                dataToBridge.LightStatuses,
                dataToBridge.DashIcons1Field,
                ignition
            );
            canDash2MessageHandler->Process(currentTime);

            #pragma endregion

            #pragma region Dash icons

            canDash3MessageHandler->SetData(
                dataToBridge.DashIcons1Field
            );
            canDash3MessageHandler->Process(currentTime);

            canDash4MessageHandler->SetData(dataToBridge.FuelLevel);
            canDash4MessageHandler->Process(currentTime);

            #pragma endregion
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void CANSendIgnitionTaskFunction(void * parameter)
{
    VanIgnitionDataToBridgeToCan dataToBridge;
    VanVinToBridgeToCan vinDataToBridge;

    unsigned long currentTime = millis();
    uint8_t economyMode = 0;
    uint8_t ignition = 0;
    uint8_t brightness = 15;
    int8_t externalTemperature = 0;


    for (;;)
    {
        xQueueReceive(ignitionQueue, &dataToBridge, 0);

        currentTime = millis();

        ignition = 0;
        #ifdef USE_IGNITION_SIGNAL_FROM_VAN_BUS
            if (dataToBridge.Ignition == 1)
            {
                ignition = 1;
            }
            else
            {
                canPopupHandler->Reset();
                canStatusOfFunctionsHandler->Reset();
                canWarningLogHandler->Reset();
            }
            economyMode = 0;
            if (dataToBridge.EconomyModeActive == 1)
            {
                economyMode = 1;
            }
        #else
            ignition = 1;
            economyMode = 0;
        #endif // USE_IGNITION_SIGNAL_FROM_VAN_BUS

        if (dataToBridge.NightMode)
        {
            brightness = 7;
        }
        else
        {
            brightness = 15;
        }

        #pragma region Ignition signal for radio

        radioIgnition->SendIgnition(economyMode, brightness, dataToBridge.DashboardLightingEnabled);

        #pragma endregion

        vTaskDelay(3 / portTICK_PERIOD_MS);

        #pragma region Ignition signal for display

        externalTemperature = dataToBridge.OutsideTemperature;

        if (dataToBridge.LeftStickButtonPressed)
        {
            externalTemperature = (dataToBridge.InternalTemperature + 0.5);
        }

        dashIgnition->SendIgnition(
            ignition, 
            dataToBridge.WaterTemperature, 
            externalTemperature, 
            dataToBridge.MileageByte1, 
            dataToBridge.MileageByte2, 
            dataToBridge.MileageByte3);

        if (ignition == 1 &&
            dataToBridge.OutsideTemperature <= 3 && 
            dataToBridge.OutsideTemperature >= -3 && 
            currentTime > 10000)
        {
            if (!canPopupHandler->IsPopupVisible())
            {
                CanDisplayPopupItem item;
                item.DisplayTimeInMilliSeconds = CAN_POPUP_MESSAGE_TIME;
                item.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
                item.MessageType = CAN_POPUP_MSG_RISK_OF_ICE;
                item.DoorStatus1 = 0;
                item.DoorStatus2 = 0;
                canPopupHandler->QueueNewMessage(item);
            }
        }

        #pragma endregion

        #pragma region VIN sending

        if (!canVinHandler->IsVinSet())
        {
            if (xQueueReceive(vinQueue, &vinDataToBridge, portMAX_DELAY) == pdTRUE)
            {
                canVinHandler->SetVin(vinDataToBridge.Vin);
            }
        }

        canVinHandler->Process(currentTime);

        #pragma endregion

        canStatusOfFunctionsHandler->Init();
        canWarningLogHandler->Init();

        vTaskDelay(65 / portTICK_PERIOD_MS);
    }
}

void VANReadTaskFunction(void * parameter)
{
    unsigned long currentTime;
    unsigned long lastMillis = 0;
    uint8_t identByte1;
    uint8_t identByte2;
    uint8_t vanMessageLengthWithoutId;

    uint8_t vanMessageLength;
    uint8_t vanMessage[34];
    uint8_t vanMessageWithoutId[32];

    DoorStatus doorStatus;
    doorStatus.asByte = 0;

    VanDataToBridgeToCan dataToBridge;
    VanIgnitionDataToBridgeToCan ignitionDataToBridge;
    VanVinToBridgeToCan vinDataToBridge;
    char tmp[3];
    bool vanMessageHandled;

    for (;;)
    {
        currentTime = millis();
        if (currentTime - lastMillis > 10)
        {
            lastMillis = currentTime;
            VAN_RX.Receive(&vanMessageLength, vanMessage);
            ///*
            if (serialPort->available() > 0) {
                ///*
                vanMessageLength = 0;
                uint8_t inChar = (uint8_t)serialPort->read();
                if (inChar == 'v') { // got a sync byte?
                    while (serialPort->available()) {
                        vanMessage[vanMessageLength] = serialPort->read();
                        vanMessageLength++;
                    }
                }
            }
            //*/
            if (vanMessageLength > 0 && vanMessage[0] == 0x0E)
            {
                identByte1 = vanMessage[1];
                identByte2 = vanMessage[2];
                vanMessageLengthWithoutId = vanMessageLength - 5;

                //make a copy of the buffer excluding the ids and the crc (otherwise deserializing the packet gives wrong results)
                memcpy(vanMessageWithoutId, vanMessage + 3, vanMessageLengthWithoutId);

                if (!VAN_RX.IsCrcOk(vanMessage, vanMessageLength))
                {
                    Log.error("CRC ERROR\n");
                    /*
                    uint8_t crcByte1;
                    uint8_t crcByte2;
                    uint16_t crcValue;
                    for (size_t i = 0; i < vanMessageLength - 3; i++)
                    {
                        snprintf(tmp, 3, "%02X", vanMessageWithoutId[i]);
                        serialPort->print(tmp);
                        serialPort->print(" ");
                    }
                    serialPort->println();
                    Log.error("CRC byte 1: %X\n", crcByte1);
                    Log.error("CRC byte 2: %X\n", crcByte2);
                    Log.error("CRC : %X\n", crcValue);
                    */
                    continue;
                }

                vanMessageHandled = false;
                for(uint8_t i = 0; i < VAN_MESSAGE_HANDLER_COUNT; i++)
                {
                    vanMessageHandled = vanMessageHandlers[i]->ProcessMessage(identByte1, identByte2, vanMessageWithoutId, vanMessageLengthWithoutId, dataToBridge, ignitionDataToBridge, doorStatus);
                    if (vanMessageHandled)
                    {
                        xQueueOverwrite(ignitionQueue, (void*)&ignitionDataToBridge);
                        xQueueOverwrite(dataQueue, (void*)&dataToBridge);
                        break;
                    }
                }

                #pragma region Vin
                if (IsVanIdent(identByte1, identByte2, VAN_ID_VIN))
                {
                    if (!canVinHandler->IsVinSet())
                    {
                        memcpy(vinDataToBridge.Vin, vanMessageWithoutId, vanMessageLengthWithoutId);
                        xQueueOverwrite(vinQueue, (void*)&vinDataToBridge);
                    }
                }
                #pragma endregion

                if (!SILENT_MODE 
                    //&& (IsVanIdent(identByte1, identByte2, VAN_ID_AIR_CONDITIONER_1))
                    //&& (IsVanIdent(identByte1, identByte2, VAN_ID_DISPLAY_POPUP))
                    //&& identByte1 == 0x8a
                    //|| (IsVanIdent(identByte1, identByte2, VAN_ID_AIR_CONDITIONER_2))
                    )
                {
                    ///*
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
                    //*/
                }
            }
            vanMessageLength = 0;
        }
    }
}

void setup()
{
    uint64_t macAddress;
    macAddress = ESP.getEfuseMac();
    uint16_t uniqueIdForBluetooth = (uint16_t)(macAddress >> 32);
    char bluetoothDeviceName[27];
    snprintf(bluetoothDeviceName, 27, "ESP32 VAN bus monitor %04X", uniqueIdForBluetooth);

#ifdef USE_BLUETOOTH_SERIAL
    serialPort = new BluetoothSerAbs(SerialBT, bluetoothDeviceName);
#else
    serialPort = new HwSerAbs(Serial);
#endif

    //serialPort->begin(115200);
    //serialPort->begin(230400);
    serialPort->begin(500000);
    serialPort->println(bluetoothDeviceName);

    // Pass log level, whether to show log level, and print interface.
    /* Available levels are:
        * 0 - LOG_LEVEL_SILENT     no output
        * 1 - LOG_LEVEL_FATAL      fatal errors
        * 2 - LOG_LEVEL_ERROR      all errors
        * 3 - LOG_LEVEL_WARNING    errors, and warnings
        * 4 - LOG_LEVEL_NOTICE     errors, warnings and notices
        * 5 - LOG_LEVEL_TRACE      errors, warnings, notices & traces
        * 6 - LOG_LEVEL_VERBOSE    all
    */
    if (SILENT_MODE)
    {
#ifdef USE_BLUETOOTH_SERIAL
        Log.begin(LOG_LEVEL_SILENT, &SerialBT);
#else
        Log.begin(LOG_LEVEL_SILENT, &Serial);
#endif
    }
    else
    {
#ifdef USE_BLUETOOTH_SERIAL
        Log.begin(LOG_LEVEL_VERBOSE, &SerialBT);
#else
        //Log.begin(LOG_LEVEL_WARNING, &Serial);
        //Log.begin(LOG_LEVEL_VERBOSE, &Serial);
        Log.begin(LOG_LEVEL_SILENT, &Serial);
#endif
    }

    Log.trace("ESP32 Arduino VAN bus monitor\n");

    VAN_RX.Init(VAN_DATA_RX_RMT_CHANNEL, VAN_DATA_RX_PIN, VAN_DATA_RX_LED_INDICATOR_PIN, VAN_DATA_RX_LINE_LEVEL, VAN_NETWORK_TYPE_COMFORT);

    //CANInterface = new CanMessageSender(CAN_RX_PIN, CAN_TX_PIN);
    CANInterface = new CanMessageSenderEsp32Arduino(CAN_RX_PIN, CAN_TX_PIN);
    CANInterface->Init();

    canPopupHandler = new CanDisplayPopupHandler(CANInterface);
    popupMapping = new VanCanDisplayPopupMap();
    canVinHandler = new CanVinHandler(CANInterface);
    tripInfoHandler = new CanTripInfoHandler(CANInterface);
    canAirConOnDisplayHandler = new CanAirConOnDisplayHandler(CANInterface);
    canRadioRemoteMessageHandler = new CanRadioRemoteMessageHandler(CANInterface);
    vanCanAirConditionerSpeedMap = new VanCanAirConditionerSpeedMap();
    canStatusOfFunctionsHandler = new CanStatusOfFunctionsHandler(CANInterface);
    canWarningLogHandler = new CanWarningLogHandler(CANInterface);
    canSpeedAndRpmHandler = new CanSpeedAndRpmHandler(CANInterface);
    canDash2MessageHandler = new CanDash2MessageHandler(CANInterface);
    canDash3MessageHandler = new CanDash3MessageHandler(CANInterface);
    canDash4MessageHandler = new CanDash4MessageHandler(CANInterface);
    radioIgnition = new CanIgnitionPacketSender(CANInterface);
    dashIgnition = new CanDashIgnitionPacketSender(CANInterface);

    vanMessageHandlers[0] = new VanAirConditioner1Handler(vanCanAirConditionerSpeedMap);
    vanMessageHandlers[1] = new VanAirConditioner2Handler();
    vanMessageHandlers[2] = new VanCarStatusWithTripComputerHandler(canPopupHandler, tripInfoHandler);
    vanMessageHandlers[3] = new VanDashboardHandler();
    vanMessageHandlers[4] = new VanDisplayHandler(canPopupHandler, tripInfoHandler, popupMapping, canStatusOfFunctionsHandler, canWarningLogHandler);
    vanMessageHandlers[5] = new VanInstrumentClusterV1Handler();
    vanMessageHandlers[6] = new VanRadioRemoteHandler(tripInfoHandler, canRadioRemoteMessageHandler);
    vanMessageHandlers[7] = new VanSpeedAndRpmHandler();
    vanMessageHandlers[8] = new VanInstrumentClusterV2Handler();

    dataQueue = xQueueCreate(QUEUE_SIZE, sizeof(VanDataToBridgeToCan));
    ignitionQueue = xQueueCreate(QUEUE_SIZE, sizeof(VanIgnitionDataToBridgeToCan));
    vinQueue = xQueueCreate(QUEUE_SIZE, sizeof(VanVinToBridgeToCan));

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
}

void loop()
{
    delay(100);
}
