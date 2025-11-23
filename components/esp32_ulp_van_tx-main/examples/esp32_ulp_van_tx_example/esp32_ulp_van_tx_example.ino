#include <arduino.h>
#include <stdint.h>
#include <UlpVanTx.h>
#include <esp32_arduino_rmt_van_rx.h>

// Pins used for RX and TX must be in the RTC domain.
// Input/output : 0,2,4,12,13,14,15,25,26,27,32,33
// Input only   : 34,35,36,37,38,39

#define VAN_RX_PIN GPIO_NUM_33
#define VAN_TX_PIN GPIO_NUM_32

UlpVanTx* ulpVanTx;

TaskHandle_t VANReadDataTask;

ESP32_RMT_VAN_RX *VAN_RX;

const uint8_t VAN_DATA_RX_RMT_CHANNEL = 0;
const uint8_t VAN_DATA_RX_PIN = 33;
const uint8_t VAN_DATA_RX_LED_INDICATOR_PIN = 2;

uint8_t vanMessageLength;
uint8_t vanMessage[34];

uint32_t lastMillis = 0;

void IRAM_ATTR VANReadDataTaskFunction(void * parameter)
{
    VAN_RX = new ESP32_RMT_VAN_RX();
    VAN_RX->Init(VAN_DATA_RX_RMT_CHANNEL, VAN_DATA_RX_PIN, VAN_DATA_RX_LED_INDICATOR_PIN, VAN_LINE_LEVEL_HIGH, VAN_NETWORK_TYPE_COMFORT);

    for (;;)
    {
        VAN_RX->Receive(&vanMessageLength, vanMessage);

        if (vanMessageLength > 0)
        {
            if(!VAN_RX->IsCrcOk(vanMessage, vanMessageLength))
            {
              printf("CRC ERROR: ");
            }
            {
                for (size_t i = 0; i < vanMessageLength; i++)
                {
                    if (i != vanMessageLength - 1)
                    {
                        printf("%02X ", vanMessage[i]);
                    }
                    else
                    {
                        printf("%02X", vanMessage[i]);
                    }
                }
                printf("\n");
            }
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

void setup()
{
    Serial.begin(500000);

    printf("ESP32 Arduino VAN bus monitor\n");

    xTaskCreatePinnedToCore(
        VANReadDataTaskFunction,        // Function to implement the task
        "VANReadDataTask",              // Name of the task
        20000,                          // Stack size in words
        NULL,                           // Task input parameter
        1,                              // Priority of the task (higher the number, higher the priority)
        &VANReadDataTask,               // Task handle.
        0);                             // Core where the task should run


    ulpVanTx = new UlpVanTx(VAN_RX_PIN, VAN_TX_PIN, ULP_VAN_125KBPS);
    ulpVanTx->Start();
}

void SendExternalTemperature(int temperature)
{
    uint8_t packet[7] = { 0x0F, 0x07, 0x81, 0x1D, 0xA4 ,0x93, temperature * 2 + 0x50 };
    ulpVanTx->SendNormalFrame(0x8A4, packet, sizeof(packet), false);
}

void ShowPopupMessage(int messageId)
{
    uint8_t packet[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, messageId, 0xFF, 0xFF, 0xFF, 0xFF,0xFF,0xFF };
    ulpVanTx->SendNormalFrame(0x524, packet, 16, 0);
}

void BsiEvent()
{
    // This forces the EMF to send a 0x564 frame
    uint8_t packet[2] = { 0x52, 0x08 };
    ulpVanTx->SendNormalFrame(0x8C4, packet, sizeof(packet), true);
}

void loop()
{
    //ulpVanTx->SendReplyRequestFrame(0x564);
    SendExternalTemperature(-7);
    //ShowPopupMessage(0x18);
    //BsiEvent();

    vTaskDelay(500 / portTICK_PERIOD_MS);
}