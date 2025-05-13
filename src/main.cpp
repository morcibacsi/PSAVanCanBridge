#include <stdio.h>
#include  <vector>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <queue>

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
#include "Helpers/TimeProvider.h"

#include "Helpers/CarState.hpp"
#include "Helpers/FileSystem.hpp"
#include "Helpers/ConfigFile.hpp"
#include "Helpers/CrcStore.hpp"
#include "Helpers/CpuConfig.h"

ITransportLayer* sourceTransportLayer;
ITransportLayer* destinationTransportLayer;
std::shared_ptr<IProtocolHandler> sourceProtocolHandler;
std::shared_ptr<IProtocolHandler> destinationProtocolHandler;
std::shared_ptr<CarState> carState;
std::shared_ptr<CrcStore> crcStore;

std::shared_ptr<FileSystem> fileSystem;
std::shared_ptr<ConfigFile> configFile;

TaskHandle_t ReadSourceTask;
TaskHandle_t ParseSourceTask;
TaskHandle_t SendToSourceTask;
TaskHandle_t SendToDestinationTask;
TaskHandle_t MessageForwarderTask;
QueueHandle_t sourceMessageQueueLowPrio;
QueueHandle_t sourceMessageQueueHighPrio;
QueueHandle_t messageToForwardQueue;

RgbLed* led;
std::shared_ptr<TimeProvider> timeProvider;
std::vector<InitItem> crcStoreItems;
bool automaticallyStoreNewIds = false;

//#include "Helpers/Ble/NimBLE.h"
//NimBLE ble;

//ESP_IDF_VERSION_MAJOR

#include "Helpers/WebServer/WebServer2.hpp"
std::shared_ptr<WebServer> webServer;

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
        if (sourceTransportLayer->ReceiveMessage(message))
        {
            //printf("Received message from source\n");
            ///*
            if (message.id == 0)
            {
                continue;
            }
            //*/
///*
            bool destinationCanAcceptMessage = destinationProtocolHandler->CanAcceptMessage(message);
            if (destinationCanAcceptMessage)
            //if (true)
            {
                // Forward the message directly to the destination
                xQueueSendToBack(messageToForwardQueue, &message, 0);
                //destinationProtocolHandler->HandleForwardedMessage(message);
                continue;
            }
//            */
            processMessage = !crcStore->IsCrcSameAsPrevious(message.id, message.command, message.crc, carState->CurrenTime);
            if (processMessage)
            {
                //printf("Received message from source\n");
                //led->blink_led();
                //PrintMessage(message);
                if (message.priority)
                {
                    //printf("Add message to front of the queue, id: %04X \n", (unsigned int)(message.id));
                    xQueueSendToBack(sourceMessageQueueHighPrio, &message, 0);
                }
                else
                {
                    //printf("Add message to back of the queue, id: %04X \n", (unsigned int)(message.id));
                    xQueueSendToBack(sourceMessageQueueLowPrio, &message, 0);
                }
            }
            //printf("Ignition: %d IgnitionMode: %d\r\n", carState->Ignition, carState->IgnitionMode);
            taskYIELD();
        }

        //vTaskDelay(pdMS_TO_TICKS(10));
    } while (1);
}

void IRAM_ATTR ParseSourceFunction(void * parameter)
{
    bool processMessage = true;
    uint64_t currentMillis = 0;
    BusMessage message{};

    do
    {
        //printf("ParseSourceFunction\n");
        webServer->Process();
///*
        if (xQueueReceive(sourceMessageQueueHighPrio, &message, pdMS_TO_TICKS(10)) == pdPASS ||
            xQueueReceive(sourceMessageQueueLowPrio, &message, pdMS_TO_TICKS(10)) == pdPASS)
        {
            //printf("Received message from queue, id: %03X \n", (unsigned int)(message.id));
            sourceProtocolHandler->ParseMessage(message);
        }
//*/
//
/*
        if ((currentMillis - lastSend) > 100)
        {
            led->blink_led();

            lastSend = currentMillis;
            sourceTransportLayer->SendMessage({0x8A4, {0x0F, 0x07, 0x81, 0x1D, 0xA4 ,0x93, -7 * 2 + 0x50}, {}, false, AEE2001, MessageType::Normal, 0, 0});
        }
//*/
//
/*
        carState->CurrenTime = currentMillis;

        if (sourceProtocolHandler->ReceiveMessage(message))
        {
            bool destinationCanAcceptMessage = destinationProtocolHandler->CanAcceptMessage(message);
            if (destinationCanAcceptMessage)
            //if (true)
            {
                // Forward the message directly to the destination
                destinationProtocolHandler->HandleForwardedMessage(message);
            }
            else
            {
                if (sourceProtocolHandler->CanParseMessage(message) == false)
                {
                    continue;
                }

                processMessage = !crcStore->IsCrcSameAsPrevious(message.id, message.command, message.crc);
                if (processMessage)
                {
                    PrintMessage(message);
                    sourceProtocolHandler->ParseMessage(message);
                }
            }
        }
//*/
        //ble->Process();
        vTaskDelay(pdMS_TO_TICKS(10));
    } while (1);
}

void SendToDestinationFunction(void * parameter)
{
    do
    {
        //printf("SendToDestinationFunction\n");
        carState->CurrenTime =  millis();
        timeProvider->Process(carState->CurrenTime);
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
        //printf("SendToSourceFunction\n");
        sourceProtocolHandler->GenerateMessages(IProtocolHandler::MessageDirection::Source);
        sourceProtocolHandler->UpdateMessages(carState->CurrenTime);
        vTaskDelay(pdMS_TO_TICKS(10));
    } while (1);
}

void MessageForwarderFunction(void * parameter)
{
    BusMessage message{};

    do
    {
        //printf("MessageForwarderFunction\n");
        if (xQueueReceive(messageToForwardQueue, &message, pdMS_TO_TICKS(10)) == pdPASS)
        //if (xQueueReceive(messageToForwardQueue, &message, portMAX_DELAY) == pdPASS)
        {
            //printf("Received message from queue, id: %03X \n", (unsigned int)(message.id));
            destinationProtocolHandler->HandleForwardedMessage(message);
        }
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

    sourceMessageQueueLowPrio = xQueueCreate(32, sizeof(BusMessage));
    sourceMessageQueueHighPrio = xQueueCreate(32, sizeof(BusMessage));
    messageToForwardQueue = xQueueCreate(32, sizeof(BusMessage));

    carState = std::make_shared<CarState>();

    led = new RgbLed(8);

    fileSystem = std::make_shared<FileSystem>();
    fileSystem->Init();

    configFile = std::make_shared<ConfigFile>(carState);

    if (configFile->Read() == false)
    {
        configFile->Write();
    };

    printf("Source protocol: %d\n", carState->SOURCE_PROTOCOL);
    printf("Destination protocol: %d\n", carState->DESTINATION_PROTOCOL);

    timeProvider = std::make_shared<TimeProvider>(SDA_PIN, SCL_PIN, carState);
    timeProvider->Start();

    printf("Create webserver\n");
    webServer = std::make_shared<WebServer>(carState, configFile, timeProvider);
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
        sourceProtocolHandler = std::make_shared<AEE2001ComfortBus>(
            std::shared_ptr<CarState>(carState),
            std::shared_ptr<ITransportLayer>(sourceTransportLayer),
            std::make_shared<MessageScheduler>());
    }

    if (carState->SOURCE_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2004))
    {
        printf("Source AEE2004\n");

        crcStoreItems = {
            {0x036},
//            {0x0B6},
        };
        automaticallyStoreNewIds = true;

//
/*
        crcStoreItems = {
            {0x036},
            {0x0B6},
            {0x0E6},
            {0x0F6},
            {0x10B},
            {0x10B},
            {0x120},
            {0x122},
            {0x127},
            {0x128},
            {0x161},
            {0x12D},
            {0x13E},
            {0x14C},
            {0x168},
            {0x17E},
            {0x18C},
            {0x1A1},
            {0x1A8},
            {0x1BE},
            {0x1CC},
            {0x217},
            {0x21F},
            {0x228},
            {0x236},
            {0x24C},
            {0x260},
            {0x220},
            {0x221},
            {0x227},
            {0x261},
            {0x276},
            {0x28C},
            {0x2A0},
            {0x2A1},
            {0x2B6},
            {0x2E1},
            {0x336},
            {0x361},
            {0x3A7},
            {0x3B6},
        };
//*/
        sourceTransportLayer = new CANTransportLayer(CAN2_RX_PIN, CAN2_TX_PIN, 1);
        //sourceTransportLayer = new CANTransportLayerOnSerial();
        sourceProtocolHandler = std::make_shared<AEE2004ComfortBus>(
            std::shared_ptr<CarState>(carState),
            std::shared_ptr<ITransportLayer>(sourceTransportLayer),
            std::make_shared<MessageScheduler>());
    }

    if (carState->DESTINATION_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2004))
    {
        printf("Destination AEE2004\n");
        destinationTransportLayer = new CANTransportLayer(CAN1_RX_PIN, CAN1_TX_PIN, 0);

        destinationProtocolHandler = std::make_shared<AEE2004ComfortBus>(
            std::shared_ptr<CarState>(carState),
            std::shared_ptr<ITransportLayer>(destinationTransportLayer),
            std::make_shared<MessageScheduler>());
    }

    if (carState->DESTINATION_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2010))
    {
        printf("Destination AEE2010\n");
        destinationTransportLayer = new CANTransportLayer(CAN1_RX_PIN, CAN1_TX_PIN, 0);

        destinationProtocolHandler = std::make_shared<AEE2010ComfortBus>(
            std::shared_ptr<CarState>(carState),
            std::shared_ptr<ITransportLayer>(destinationTransportLayer),
            std::make_shared<MessageScheduler>());
    }

    if (sourceProtocolHandler == nullptr || destinationProtocolHandler == nullptr)
    {
        printf("Error: sourceProtocolHandler or destinationProtocolHandler is null\n");
        return;
    }

    crcStore = std::make_shared<CrcStore>(crcStoreItems, automaticallyStoreNewIds);

    printf("Register message handler on destination\n");
    //call RegisterMessageHandlers on the destination protocol handler with a dummy function
    destinationProtocolHandler->RegisterMessageHandlers([](ImmediateSignal signal) {});

    printf("Register message handler on source\n");
    std::weak_ptr<IProtocolHandler> weakDestinationProtocolHandler = destinationProtocolHandler;
    sourceProtocolHandler->RegisterMessageHandlers([weakDestinationProtocolHandler](ImmediateSignal signal) {
        if (auto destination = weakDestinationProtocolHandler.lock()) {
            destination->ProcessImmediateSignal(signal);
        }
    });
    printf("Starting tasks\n");

    // register the destination messagehandlers passing down the immediate signal callback of the source
    //destinationProtocolHandler->RegisterMessageHandlers(std::bind(&IProtocolHandler::ProcessImmediateSignal, sourceProtocolHandler.get(), std::placeholders::_1));
    /*
    destinationProtocolHandler->RegisterMessageHandlers(
        [&sourceProtocolHandler](ImmediateSignal signal) {
            sourceProtocolHandler->ProcessImmediateSignal(signal); // Replace with actual method
        }

    );
    */

    //ble.initBle("BLE_NAME");
    //ble.startAdv();
    //ble.setReaderHandler(&bleEvent);
/*
    destinationTransportLayer = new CANTransportLayer(CAN1_RX_PIN, CAN1_TX_PIN);
    destinationProtocolHandler = new AEE2010ComfortBus(
        std::shared_ptr<CarState>(carState),
        std::shared_ptr<ITransportLayer>(destinationTransportLayer),
        std::make_shared<MessageScheduler>());
*/

    cpu_config_t ReadSourceTaskConfig        = { .cpu_core = 0, .priority = 5 };
    cpu_config_t ParseSourceTaskConfig       = { .cpu_core = 0, .priority = 3 };
    cpu_config_t SendToSourceTaskConfig      = { .cpu_core = 0, .priority = 2 };
    cpu_config_t SendToDestinationTaskConfig = { .cpu_core = 0, .priority = 4 };
    cpu_config_t MessageForwarderTaskConfig  = { .cpu_core = 0, .priority = 1 };

    xTaskCreatePinnedToCore(
        ReadSourceFunction,             // Function to implement the task
        "ReadSource",                   // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        ReadSourceTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &ReadSourceTask,                // Task handle.
        ReadSourceTaskConfig.cpu_core); // Core where the task should run

    xTaskCreatePinnedToCore(
        ParseSourceFunction,             // Function to implement the task
        "ParseSource",                   // Name of the task
        20000,                           // Stack size in words
        NULL,                            // Task input parameter
        ParseSourceTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &ParseSourceTask,                // Task handle.
        ParseSourceTaskConfig.cpu_core); // Core where the task should run

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

      xTaskCreatePinnedToCore(
        MessageForwarderFunction,             // Function to implement the task
        "MessageForwarder",                   // Name of the task
        20000,                                // Stack size in words
        NULL,                                 // Task input parameter
        MessageForwarderTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &MessageForwarderTask,                // Task handle.
        MessageForwarderTaskConfig.cpu_core); // Core where the task should run
}
