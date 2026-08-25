#include "BoardConfig.h"

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

#include "lib/IVanMessageSender.h"
#include "lib/esp32_ulp_lpc_core_van_tx/LpCoreVanTx.hpp"
#include "lib/esp32_hulp_van_tx/HulpVanTx.hpp"

#include "lib/CanMessageSender/CanMessageSenderEsp32Idf.h"
#include "lib/CanMessageSender/CanMessageSenderMcp2515Idf.h"

#include "Protocol/CANTransportLayer.hpp"
#include "Protocol/CANTransportLayerOnSerial.hpp"
#include "Protocol/AEE2004/AEE2004ComfortBus.hpp"

#include "Protocol/AEE2010/AEE2010ComfortBus.hpp"

#include "Protocol/Diagnostics/DiagnosticsContainer.hpp"
#include "Protocol/WebSocketSerial.hpp"

#include "RgbLed.hpp"
#include "Helpers/TimeProvider.hpp"

#include "Helpers/CarState.hpp"
#include "Helpers/FileSystem.hpp"
#include "Helpers/ConfigFile.hpp"
#include "Helpers/CrcStore.hpp"
#include "Helpers/CpuConfig.h"
#include "Helpers/WebServer/WebServer.hpp"
#include "Helpers/PSADiag/PsaDiagLib.h"
#include "Helpers/FuelRefillTracker.hpp"

IVanMessageSender* sourceVanMessageSender = nullptr;
ICanMessageSender* sourceCanMessageSender = nullptr;
ICanMessageSender* destinationCanMessageSender = nullptr;

ITransportLayer* sourceTransportLayer = nullptr;
ITransportLayer* destinationTransportLayer = nullptr;

IProtocolHandler* sourceProtocolHandler = nullptr;
IProtocolHandler* destinationProtocolHandler = nullptr;
IProtocolHandler* diagnosticsContainer = nullptr;

PsaDiagLib* psaDiagLib = nullptr;

CarState* carState = nullptr;
CrcStore* crcStore = nullptr;

FileSystem* fileSystem = nullptr;
ConfigFile* configFile = nullptr;

TaskHandle_t ReadSourceTask = nullptr;
TaskHandle_t ReadDestinationTask = nullptr;
TaskHandle_t SendToSourceTask = nullptr;
TaskHandle_t SendToDestinationTask = nullptr;

RgbLed* led = nullptr;
TimeProvider* timeProvider = nullptr;
WebServer* webServer = nullptr;
WebSocketSerial* webSocketSerial = nullptr;
FuelRefillTracker* fuelRefillTracker = nullptr;

std::vector<InitItem> crcStoreItems;
bool automaticallyStoreNewIds = false;

//#include "Helpers/Ble/NimBLE.h"
//NimBLE ble;

#define VAN_RX_PIN BOARD_CAN2_RX_PIN
#define VAN_TX_PIN BOARD_CAN2_TX_PIN
#define VAN_DATA_RX_LED_INDICATOR_PIN BOARD_LED_PIN

#define CAN1_RX_PIN BOARD_CAN1_RX_PIN
#define CAN1_TX_PIN BOARD_CAN1_TX_PIN

#define CAN2_RX_PIN BOARD_CAN2_RX_PIN
#define CAN2_TX_PIN BOARD_CAN2_TX_PIN

#define SDA_PIN BOARD_SDA_PIN
#define SCL_PIN BOARD_SCL_PIN

uint64_t IRAM_ATTR millis() {
    return (uint64_t)(esp_timer_get_time() / 1000ULL);
}

void SendImmediateSignalToDestination(ImmediateSignal signal) {
    if (destinationProtocolHandler != nullptr)
    {
        destinationProtocolHandler->ProcessImmediateSignal(signal);
    }
    if (diagnosticsContainer != nullptr)
    {
        diagnosticsContainer->ProcessImmediateSignal(signal);
    }
}

void EmptyImmediateSignalCaller(ImmediateSignal signal) { }

void PrintMessage(const BusMessage& message)
{
    const size_t safeDataLength = (message.dataLength > sizeof(message.data)) ? sizeof(message.data) : message.dataLength;
    //if (message.id == 0x8C4 || message.id == 0x564)
    //if (message.id == 0x9C4)
    //if (message.id == 0x0F6 || message.id == 0x036)
    {
        //printf("printing message \n");
        printf("<< Id: %03X ", (unsigned int)(message.id));
        for (size_t i = 0; i < safeDataLength; i++)
        {
            if (i != safeDataLength - 1)
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

void PrintMessageToWebSocket(const uint8_t network, const uint8_t direction, const BusMessage& message)
{
    if (network == 0 || direction == 0 || network != carState->LogNetwork || direction != carState->LogDirection)
    {
        return;
    }

    const size_t safeDataLength = (message.dataLength > sizeof(message.data)) ? sizeof(message.data) : message.dataLength;
    char buffer[256];
    //int offset = sprintf(buffer, "<< Id: %03X ", (unsigned int)(message.id));
    int offset = sprintf(buffer, "%03X ", (unsigned int)(message.id));
    for (size_t i = 0; i < safeDataLength; i++)
    {
        offset += sprintf(buffer + offset, "%02X ", message.data[i]);
    }
    //offset += sprintf(buffer + offset, "%04X", (unsigned int)(message.crc));
    offset += sprintf(buffer + offset, "\n");
    webSocketSerial->write((const uint8_t*)buffer, offset);
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

            PrintMessageToWebSocket(1, 1, message);

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
        else
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    } while (1);
}

void ReadDestinationFunction(void * parameter)
{
    BusMessage message{};
    bool processMessage = true;
    uint64_t currentTime = 0;

    do
    {
        currentTime = millis();
        if (destinationProtocolHandler->ReceiveMessage(message))
        {
            if (message.id == 0)
            {
                continue;
            }

            PrintMessageToWebSocket(2, 1, message);
            bool sourceCanAcceptMessage = sourceProtocolHandler->CanAcceptMessage(message);
            if (sourceCanAcceptMessage)
            {
                sourceProtocolHandler->HandleForwardedMessage(message);
                continue;
            }

            processMessage = destinationProtocolHandler->CanParseMessage(message);
            if (processMessage)
            {
                //PrintMessage(message);
                destinationProtocolHandler->ParseMessage(message);
            }

            if (carState->DiagConnected == false)
            {
                processMessage = diagnosticsContainer->CanParseMessage(message);
                if (processMessage)
                {
                    diagnosticsContainer->ParseMessage(message);
                }
            }
            else
            {
                psaDiagLib->ProcessIncomingMessage(currentTime, message.id, message.dataLength, message.data);
                psaDiagLib->Loop(currentTime);
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
        fuelRefillTracker->Process(carState->CurrenTime);

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

    led = new RgbLed(BOARD_LED_PIN);

    fileSystem = new FileSystem();
    fileSystem->Init();

    configFile = new ConfigFile(carState);

    if (configFile->Read() == false)
    {
        configFile->Write();
    };

    printf("Firmware version: %s\n", carState->Version);
    printf("Source protocol: %d\n", carState->SOURCE_PROTOCOL);
    printf("Destination protocol: %d\n", carState->DESTINATION_PROTOCOL);

    timeProvider = new TimeProvider(SDA_PIN, SCL_PIN, carState);
    timeProvider->Start();

    fuelRefillTracker = new FuelRefillTracker(carState);

    printf("Create webserver\n");
    webSocketSerial = new WebSocketSerial(carState);
    webServer = new WebServer(carState, configFile, timeProvider, fuelRefillTracker, webSocketSerial, SendImmediateSignalToDestination);
    webServer->CreateWebServer();
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
            {0x664},
            {0x744},
            {0x5E4},
            {0x4D4},
            {0xADC}
        };

        #if CONFIG_IDF_TARGET_ESP32
            sourceVanMessageSender = new HulpVanTx(VAN_RX_PIN, VAN_TX_PIN);
        #else
            sourceVanMessageSender = new LpCoreVanTx((gpio_num_t)VAN_RX_PIN, (gpio_num_t)VAN_TX_PIN, LpCoreVanTx::LP_VAN_125KBPS);
        #endif


        sourceTransportLayer = new VANTransportLayer(sourceVanMessageSender, VAN_RX_PIN, VAN_DATA_RX_LED_INDICATOR_PIN);
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

        #if BOARD_SECOND_CAN_TYPE == 1
            sourceCanMessageSender = new CanMessageSenderEsp32Idf(CAN2_RX_PIN, CAN2_TX_PIN, 1);
        #endif
        #if BOARD_SECOND_CAN_TYPE == 2
            sourceCanMessageSender = new CanMessageSenderMcp2515Idf(BOARD_MISO_PIN, BOARD_MOSI_PIN, BOARD_SCK_PIN, BOARD_CS_PIN, BOARD_SPI_INSTANCE);
        #endif

        sourceTransportLayer = new CANTransportLayer(sourceCanMessageSender);
        //sourceTransportLayer = new CANTransportLayerOnSerial();
        sourceProtocolHandler = new AEE2004ComfortBus(
            carState,
            sourceTransportLayer,
            new MessageScheduler(),
            configFile
        );
    }

    if (carState->DESTINATION_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2004))
    {
        printf("Destination AEE2004\n");
        destinationCanMessageSender = new CanMessageSenderEsp32Idf(CAN1_RX_PIN, CAN1_TX_PIN, 0);
        destinationTransportLayer = new CANTransportLayer(destinationCanMessageSender);

        destinationProtocolHandler = new AEE2004ComfortBus(
            carState,
            destinationTransportLayer,
            new MessageScheduler(),
            configFile
        );
    }

    if (carState->DESTINATION_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2010))
    {
        printf("Destination AEE2010\n");
        destinationCanMessageSender = new CanMessageSenderEsp32Idf(CAN1_RX_PIN, CAN1_TX_PIN, 0);
        destinationTransportLayer = new CANTransportLayer(destinationCanMessageSender);

        destinationProtocolHandler = new AEE2010ComfortBus(
            carState,
            destinationTransportLayer,
            new MessageScheduler(),
            timeProvider,
            configFile
        );
    }

    if (sourceProtocolHandler == nullptr || destinationProtocolHandler == nullptr)
    {
        printf("Error: sourceProtocolHandler or destinationProtocolHandler is null\n");
        return;
    }

    destinationTransportLayer->SetLoggerFunction(2, PrintMessageToWebSocket);

    psaDiagLib = new PsaDiagLib(destinationTransportLayer, webSocketSerial);
    webServer->SetPsaDiagLib(psaDiagLib);

    diagnosticsContainer = new DiagnosticsContainer(
        carState,
        destinationTransportLayer,
        configFile
    );
    diagnosticsContainer->RegisterMessageHandlers(SendImmediateSignalToDestination);

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

    cpu_config_t ReadSourceTaskConfig        = { .cpu_core = 0, .priority = 5, .stack_size = 20000 };
    cpu_config_t ReadDestinationTaskConfig   = { .cpu_core = 0, .priority = 1, .stack_size = 20000 };
    cpu_config_t SendToSourceTaskConfig      = { .cpu_core = 0, .priority = 2, .stack_size = 20000 };
    cpu_config_t SendToDestinationTaskConfig = { .cpu_core = 0, .priority = 4, .stack_size = 20000 };

    xTaskCreatePinnedToCore(
        ReadSourceFunction,             // Function to implement the task
        "ReadSource",                   // Name of the task
        ReadSourceTaskConfig.stack_size,// Stack size in words
        NULL,                           // Task input parameter
        ReadSourceTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &ReadSourceTask,                // Task handle.
        ReadSourceTaskConfig.cpu_core); // Core where the task should run

    xTaskCreatePinnedToCore(
        ReadDestinationFunction,             // Function to implement the task
        "ReadDestination",                   // Name of the task
        ReadDestinationTaskConfig.stack_size,// Stack size in words
        NULL,                                // Task input parameter
        ReadDestinationTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &ReadDestinationTask,                // Task handle.
        ReadDestinationTaskConfig.cpu_core); // Core where the task should run

    xTaskCreatePinnedToCore(
        SendToSourceFunction,             // Function to implement the task
        "SendToSource",                   // Name of the task
        SendToSourceTaskConfig.stack_size,// Stack size in words
        NULL,                             // Task input parameter
        SendToSourceTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &SendToSourceTask,                // Task handle.
        SendToSourceTaskConfig.cpu_core); // Core where the task should run

      xTaskCreatePinnedToCore(
        SendToDestinationFunction,             // Function to implement the task
        "SendToDestination",                   // Name of the task
        SendToDestinationTaskConfig.stack_size,// Stack size in words
        NULL,                                  // Task input parameter
        SendToDestinationTaskConfig.priority,  // Priority of the task (higher the number, higher the priority)
        &SendToDestinationTask,                // Task handle.
        SendToDestinationTaskConfig.cpu_core); // Core where the task should run
}
