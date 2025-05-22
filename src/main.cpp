#include <stdio.h>
#include  <vector>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "Protocol/IProtocolHandler.hpp"
#include "Protocol/ITransportLayer.hpp"

#include "Protocol/VANTransportLayer.hpp"
#include "Protocol/VANTransportLayerOnSerial.hpp"
#include "Protocol/AEE2001/AEE2001ComfortBus.hpp"

#include "Protocol/CANTransportLayer.hpp"
#include "Protocol/CANTransportLayerOnSerial.hpp"
#include "Protocol/AEE2004/AEE2004ComfortBus.hpp"

#include "Protocol/CANTransportLayer.hpp"
#include "Protocol/AEE2010/AEE2010ComfortBus.hpp"

#include "RgbLed.hpp"
#include "Helpers/TimeProvider.hpp"

#include "Helpers/CarState.hpp"
#include "Helpers/FileSystem.hpp"
#include "Helpers/ConfigFile.hpp"
#include "Helpers/CrcStore.hpp"
#include "Helpers/CpuConfig.h"
#include "Helpers/WebServer/WebServer.hpp"

ITransportLayer* sourceTransportLayer;
ITransportLayer* destinationTransportLayer;
IProtocolHandler* sourceProtocolHandler;
IProtocolHandler* destinationProtocolHandler;
CarState* carState;
CrcStore* crcStore;

FileSystem* fileSystem;
ConfigFile* configFile;

TaskHandle_t ReadSourceTask;
TaskHandle_t SendToSourceTask;
TaskHandle_t SendToDestinationTask;

RgbLed* led;
TimeProvider* timeProvider;
WebServer* webServer;

std::vector<InitItem> crcStoreItems;
bool automaticallyStoreNewIds = false;

//#include "Helpers/Ble/NimBLE.h"
//NimBLE ble;

#define VAN_RX_PIN GPIO_NUM_3
#define VAN_TX_PIN GPIO_NUM_2
#define VAN_DATA_RX_LED_INDICATOR_PIN 15

#define CAN1_RX_PIN 0
#define CAN1_TX_PIN 1

#define CAN2_RX_PIN 3
#define CAN2_TX_PIN 2

#define SDA_PIN 18
#define SCL_PIN 19

uint64_t IRAM_ATTR millis() {
    return (uint64_t)(esp_timer_get_time() / 1000ULL);
}
/*
void bleEvent(string msg, size_t select)
{
    switch (select)
    {
        case FROM_BLE:
            //cout << "BLE Income : " << msg << endl;
            printf("BLE Income : %s\n", msg.c_str());
            break;
    }
}
*/

void SendImmediateSignalToDestination(ImmediateSignal signal) {
    if (destinationProtocolHandler != nullptr) {
        destinationProtocolHandler->ProcessImmediateSignal(signal);
    }
}

void EmptyImmediateSignalCaller(ImmediateSignal signal) { }

void PrintMessage(const BusMessage& message)
{
    //if (message.id == 0x8C4 || message.id == 0x564)
    //if (message.id == 0x9C4)
    //if (message.id == 0x0F6 || message.id == 0x036)
    {
        //printf("printing message \n");
        printf("<< Id: %03X ", (unsigned int)(message.id));
        for (size_t i = 0; i < message.dataLength; i++)
        {
            if (i != message.dataLength - 1)
            {
                printf("%02X ", message.data[i]);
            }
            else
            {
                printf("%02X ", message.data[i]);
                printf("%04X", (unsigned int)(message.crc));
            }
        }
        printf("\n");
    }
}

void ReadSourceFunction(void * parameter)
{
    BusMessage message{};
    bool processMessage = true;

    do
    {
        if (sourceProtocolHandler->ReceiveMessage(message))
        {
            if (message.id == 0)
            {
                continue;
            }

            bool destinationCanAcceptMessage = destinationProtocolHandler->CanAcceptMessage(message);
            if (destinationCanAcceptMessage)
            {
                destinationProtocolHandler->HandleForwardedMessage(message);
                continue;
            }

            //processMessage = !crcStore->IsCrcSameAsPrevious(message.id, message.command, message.crc, carState->CurrenTime);
            processMessage = sourceProtocolHandler->CanParseMessage(message);
            if (processMessage)
            {
                PrintMessage(message);
                sourceProtocolHandler->ParseMessage(message);
            }
            taskYIELD();
        }
    } while (1);
}

void SendToDestinationFunction(void * parameter)
{
    do
    {
        carState->CurrenTime =  millis();
        timeProvider->Process(carState->CurrenTime);
        webServer->Process();
        //printf("Time: %04d.%02d.%02d %02d:%02d:%02d\n",carState->Year, carState->Month, carState->MDay, carState->Hour, carState->Minute, carState->Second);
        destinationProtocolHandler->GenerateMessages(IProtocolHandler::MessageDirection::Destination);
        destinationProtocolHandler->UpdateMessages(carState->CurrenTime);
        vTaskDelay(pdMS_TO_TICKS(10));
    } while (1);
}

void SendToSourceFunction(void * parameter)
{
    do
    {
        sourceProtocolHandler->GenerateMessages(IProtocolHandler::MessageDirection::Source);
        sourceProtocolHandler->UpdateMessages(carState->CurrenTime);
        vTaskDelay(pdMS_TO_TICKS(10));
    } while (1);
}

extern "C" void app_main(void)
{
    /* If user is using USB-serial-jtag then idf monitor needs some time to
    *  re-connect to the USB port. We wait 1 sec here to allow for it to make the reconnection
    *  before we print anything. Otherwise the chip will go back to sleep again before the user
    *  has time to monitor any output.
    */
    vTaskDelay(pdMS_TO_TICKS(2000));

    carState = new CarState();

    led = new RgbLed(8);

    fileSystem = new FileSystem();
    fileSystem->Init();

    configFile = new ConfigFile(carState);

    if (configFile->Read() == false)
    {
        configFile->Write();
    };

    printf("Source protocol: %d\n", carState->SOURCE_PROTOCOL);
    printf("Destination protocol: %d\n", carState->DESTINATION_PROTOCOL);

    timeProvider = new TimeProvider(SDA_PIN, SCL_PIN, carState);
    timeProvider->Start();

    printf("Create webserver\n");
    webServer = new WebServer(carState, configFile, timeProvider);
    webServer->StartWebServer();
    printf("Webserver created\n");

    //if (carState->SOURCE_PROTOCOL == carState->DESTINATION_PROTOCOL)
    //{
        //led->setColor(255, 0, 0);
    //}

    if (carState->SOURCE_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2001))
    {
        printf("Source AEE2001\n");
        crcStoreItems = {
            {0x9C4},
            {0x824},
            {0x8FC},
            {0xE24},
            {0x4FC},
            {0x4DC, 0x8, false}, {0x4DC, 0xC, false},
            {0x464},
            {0x8A4},
            {0x524},
            {0x564},
            {0x744},
            {0x5E4},
            {0x4D4},
            {0xADC}
        };

        sourceTransportLayer = new VANTransportLayer(VAN_RX_PIN, VAN_TX_PIN, VAN_DATA_RX_LED_INDICATOR_PIN);
        //sourceTransportLayer = new VANTransportLayerOnSerial();
        sourceProtocolHandler = new AEE2001ComfortBus(
            carState,
            sourceTransportLayer,
            new MessageScheduler());
    }

    if (carState->SOURCE_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2004))
    {
        printf("Source AEE2004\n");

        crcStoreItems = {
            {0x036},
        };
        automaticallyStoreNewIds = true;

        sourceTransportLayer = new CANTransportLayer(CAN2_RX_PIN, CAN2_TX_PIN, 1);
        //sourceTransportLayer = new CANTransportLayerOnSerial();
        sourceProtocolHandler = new AEE2004ComfortBus(
            carState,
            sourceTransportLayer,
            new MessageScheduler());
    }

    if (carState->DESTINATION_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2004))
    {
        printf("Destination AEE2004\n");
        destinationTransportLayer = new CANTransportLayer(CAN1_RX_PIN, CAN1_TX_PIN, 0);

        destinationProtocolHandler = new AEE2004ComfortBus(
            carState,
            destinationTransportLayer,
            new MessageScheduler());
    }

    if (carState->DESTINATION_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2010))
    {
        printf("Destination AEE2010\n");
        destinationTransportLayer = new CANTransportLayer(CAN1_RX_PIN, CAN1_TX_PIN, 0);

        destinationProtocolHandler = new AEE2010ComfortBus(
            carState,
            destinationTransportLayer,
            new MessageScheduler());
    }

    if (sourceProtocolHandler == nullptr || destinationProtocolHandler == nullptr)
    {
        printf("Error: sourceProtocolHandler or destinationProtocolHandler is null\n");
        return;
    }

    crcStore = new CrcStore(crcStoreItems, automaticallyStoreNewIds);

    printf("Register message handler on destination\n");

    //call RegisterMessageHandlers on the destination protocol handler with a dummy function
    destinationProtocolHandler->RegisterMessageHandlers(EmptyImmediateSignalCaller);

    printf("Register message handler on source\n");
    sourceProtocolHandler->RegisterMessageHandlers(SendImmediateSignalToDestination);

    printf("Starting tasks\n");

    //ble.initBle("BLE_NAME");
    //ble.startAdv();
    //ble.setReaderHandler(&bleEvent);

    cpu_config_t ReadSourceTaskConfig        = { .cpu_core = 0, .priority = 5 };
    cpu_config_t SendToSourceTaskConfig      = { .cpu_core = 0, .priority = 2 };
    cpu_config_t SendToDestinationTaskConfig = { .cpu_core = 0, .priority = 4 };

    xTaskCreatePinnedToCore(
        ReadSourceFunction,             // Function to implement the task
        "ReadSource",                   // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        ReadSourceTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &ReadSourceTask,                // Task handle.
        ReadSourceTaskConfig.cpu_core); // Core where the task should run

    xTaskCreatePinnedToCore(
        SendToSourceFunction,             // Function to implement the task
        "SendToSource",                   // Name of the task
        20000,                            // Stack size in words
        NULL,                             // Task input parameter
        SendToSourceTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &SendToSourceTask,                // Task handle.
        SendToSourceTaskConfig.cpu_core); // Core where the task should run

      xTaskCreatePinnedToCore(
        SendToDestinationFunction,             // Function to implement the task
        "SendToDestination",                   // Name of the task
        20000,                                 // Stack size in words
        NULL,                                  // Task input parameter
        SendToDestinationTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &SendToDestinationTask,                // Task handle.
        SendToDestinationTaskConfig.cpu_core); // Core where the task should run
}
