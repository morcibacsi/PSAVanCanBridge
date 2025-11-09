#include <string.h>
#include "CanMessageSenderMcp2515Idf.h"
#include "driver/gpio.h"

static const char *TAG = "CAN_MCP2515";

CanMessageSenderMcp2515Idf::CanMessageSenderMcp2515Idf(uint8_t misoPin, uint8_t mosiPin, uint8_t clkPin, uint8_t csPin, spi_host_device_t spiHost)
{
    spi_bus_config_t buscfg = {};
    buscfg.miso_io_num = misoPin;
    buscfg.mosi_io_num = mosiPin;
    buscfg.sclk_io_num = clkPin;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    // Initialize SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(spiHost, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 1 * 1000 * 1000; // 1 MHz safe startup, MCP2515 allows up to 10 MHz
    devcfg.mode = 0;
    devcfg.spics_io_num = csPin;
    devcfg.queue_size = 7;

    _spiHandle = nullptr;
    ESP_ERROR_CHECK(spi_bus_add_device(spiHost, &devcfg, &_spiHandle));

    // Create MCP2515 object with this handle
    _mcp2515 = new MCP2515(&_spiHandle);

    canSemaphore = xSemaphoreCreateMutex();
}

void CanMessageSenderMcp2515Idf::Init()
{
    Reset();
}

uint8_t CanMessageSenderMcp2515Idf::SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, const uint8_t byteArray[])
{
    can_frame message;
    message.can_id = canId;
    message.can_dlc = sizeOfByteArray;
    memcpy(message.data, byteArray, sizeOfByteArray);

   uint8_t result = 0;
    if (xSemaphoreTake(canSemaphore, pdMS_TO_TICKS(5)) == pdTRUE)
    //if (xSemaphoreTake(canSemaphore, portMAX_DELAY) == pdTRUE)
    {
        if (_mcp2515->sendMessage(&message) == MCP2515::ERROR_OK)
        {
            //printf("Message queued for transmission\n");
            result = 1;
        }
        else
        {
            uint8_t err = _mcp2515->getErrorFlags();
            //printf("CAN send error: 0x%02X \r\n", err);
        }
        xSemaphoreGive(canSemaphore);
    }

    return result;
}

bool CanMessageSenderMcp2515Idf::ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf)
{
    *canId = 0;
    *len = 0;

    can_frame rx_frame;
    bool result = false;

    if (xSemaphoreTake(canSemaphore, pdMS_TO_TICKS(5)) == pdTRUE)
    //if (xSemaphoreTake(canSemaphore, portMAX_DELAY) == pdTRUE)
    {
        MCP2515::ERROR readResult = _mcp2515->readMessage(&rx_frame);

        if (readResult == MCP2515::ERROR_OK)
        {
            *canId = rx_frame.can_id;
            *len = rx_frame.can_dlc;
            for (int i = 0; i < rx_frame.can_dlc; i++)
            {
                buf[i] = rx_frame.data[i];
            }
            result = true;
        }
        else
        {
            uint8_t err = _mcp2515->getErrorFlags();
            //printf("CAN receive error: 0x%02X \r\n", err);
        }
        xSemaphoreGive(canSemaphore);
    }
    return result;
}

void CanMessageSenderMcp2515Idf::Reset()
{
    MCP2515::ERROR ret;

    _mcp2515->reset();

    ret = _mcp2515->setBitrate(CAN_125KBPS, MCP_8MHZ);
    if (ret != MCP2515::ERROR_OK)
    {
        ESP_LOGE(TAG, "Failed to set bitrate");
        return;
    }

    ret =_mcp2515->setNormalMode();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set normal mode");
        return;
    }
}