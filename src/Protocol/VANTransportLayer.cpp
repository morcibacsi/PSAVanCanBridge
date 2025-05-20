#include <algorithm>
#include "VANTransportLayer.hpp"
#include <cstring>

VANTransportLayer::VANTransportLayer(uint8_t rxPin, uint8_t txPin, uint8_t dataRxLedIndicatorPin)
{
    _crcCalculator = new VanCrcCalculator();

    _vanTx = new LpCoreVanTx((gpio_num_t)rxPin, (gpio_num_t)txPin, LpCoreVanTx::LP_VAN_NETWORK_SPEED::LP_VAN_125KBPS);
    _vanTx->Start();

    _vanRx = new ESP32_RMT_VAN_RX(rxPin, dataRxLedIndicatorPin, VAN_LINE_LEVEL_HIGH, VAN_NETWORK_TYPE_COMFORT);
    //_vanRx = new ESP32_RMT_VAN_RX(rxPin, dataRxLedIndicatorPin, VAN_LINE_LEVEL_HIGH, VAN_NETWORK_TYPE_BODY);
    _vanRx->Start();

    _txQueue = xQueueCreate(TX_QUEUE_LENGTH, TX_QUEUE_ITEM_SIZE);
    xTaskCreate([](void* arg) {
        static_cast<VANTransportLayer*>(arg)->TxTask();
    }, "VAN-TxTask", 4096, this, 10, &_txTaskHandle);
}

void VANTransportLayer::SendMessage(const BusMessage& message, bool highPriority)
{
    if (highPriority)
    {
        // Send to the front of the queue
        xQueueSendToFront(_txQueue, &message, 0);
    }
    else
    {
        // Send to the back of the queue
        xQueueSendToBack(_txQueue, &message, 0);
    }
}

bool VANTransportLayer::ReceiveMessage(BusMessage& message)
{
    uint8_t vanMessageLength;
    uint8_t vanMessage[32];

    _vanRx->ReceiveData(&vanMessageLength, vanMessage);

    if (vanMessageLength == 0)
    {
        return false;
    }

    if (!_crcCalculator->IsCrcOk(vanMessage, vanMessageLength))
    {
        return false;
    }

    message.id = (vanMessage[1] << 8 | vanMessage[2]) >> 4;
    message.command = vanMessage[2] & 0x0F;

    std::memcpy(message.data, vanMessage + 3, vanMessageLength-2); //+3 to skip SOF+IDEN+COM, -2 to remove CRC from the data
    message.dataLength = vanMessageLength - 5; // -5 to remove SOF, IDEN, COM, CRC from the data
    message.crc = vanMessage[vanMessageLength - 2] << 8 | vanMessage[vanMessageLength - 1] << 0; // last two bytes of the data

    return true;
}

bool VANTransportLayer::IsBusAvailable()
{
    bool result = _vanTx->IsTxPossible();

    if (!result)
    {
        printf("VAN TX is not possible\n");
    }

    return result;
}

void VANTransportLayer::TxTask()
{
    BusMessage message;

    while (true)
    {
        if (xQueueReceive(_txQueue, &message, portMAX_DELAY) == pdTRUE)
        {
            if (!IsBusAvailable()) {
                // Optional delay to avoid busy-looping
                vTaskDelay(pdMS_TO_TICKS(2));

                message.retryCount++;

                if (message.retryCount <= MAX_RETRY_COUNT) {
                    xQueueSendToFront(_txQueue, &message, 0);
                } else {
                    printf("Message %03X dropped after %u retries\n", (unsigned int) message.id, message.retryCount);
                }

                continue;
            }

            switch (message.type)
            {
                case MessageType::Query:
                    //printf("Send query message: %03X\n", (unsigned int) message.id);
                    _vanTx->SendReplyRequestFrame(message.id);
                    break;
                case MessageType::Normal:
                    //printf("Send normal message: %03X\n", (unsigned int) message.id);
                    _vanTx->SendNormalFrame(message.id, message.data, message.dataLength, message.ack);
                    break;
                default:
                    break;
            }
        }
    }
}