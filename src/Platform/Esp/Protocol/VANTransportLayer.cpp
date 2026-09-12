#include <algorithm>
#include <cstring>

#include "Protocol/VANTransportLayer.hpp"

#ifdef CONFIG_IDF_TARGET_ESP32C6
// Temporary HP-side completion tracing for the trip-computer request.
// Override with -DVAN_LP_TRACE_QUERY_ID=0 to disable, or another VAN ID.
#ifndef VAN_LP_TRACE_QUERY_ID
//#define VAN_LP_TRACE_QUERY_ID 0x564
#define VAN_LP_TRACE_QUERY_ID 0
#endif

static const char* VanQueryResultName(VanLpResult result)
{
    switch (result)
    {
        case VAN_LP_QUERY_NO_RESPONSE: return "NO_IMMEDIATE_RESPONSE";
        case VAN_LP_QUERY_RESPONSE_ACKED: return "RESPONSE_ACK_SENT";
        case VAN_LP_QUERY_RESPONSE_RECEIVED: return "RESPONSE_RECEIVED_ACK_DISABLED";
        case VAN_LP_ARBITRATION_LOST: return "ARBITRATION_LOST";
        case VAN_LP_ABORT: return "PROTOCOL_OR_TIMING_ABORT";
        default: return "UNEXPECTED_RESULT";
    }
}

static const char* VanAbortStageName(uint32_t detail)
{
    switch (static_cast<VanLpAbortStage>(detail >> 16))
    {
        case VAN_LP_ABORT_TX_LENGTH: return "TX_LENGTH";
        case VAN_LP_ABORT_TX_EDGE_DEADLINE: return "TX_EDGE_DEADLINE";
        case VAN_LP_ABORT_TX_SAMPLE_DEADLINE: return "TX_SAMPLE_DEADLINE";
        case VAN_LP_ABORT_TX_DOMINANT_NOT_SEEN: return "TX_DOMINANT_NOT_SEEN";
        case VAN_LP_ABORT_RTR_INVERSE_TIMING: return "RTR_INVERSE_TIMING";
        case VAN_LP_ABORT_RTR_INVERSE_INVALID: return "RTR_INVERSE_INVALID";
        case VAN_LP_ABORT_RX_SAMPLE_DEADLINE: return "RX_SAMPLE_DEADLINE";
        case VAN_LP_ABORT_RX_INVERSE_TIMING: return "RX_INVERSE_TIMING";
        case VAN_LP_ABORT_RX_EOD_INVALID: return "RX_EOD_INVALID";
        case VAN_LP_ABORT_RX_SCAN_LIMIT: return "RX_SCAN_LIMIT";
        case VAN_LP_ABORT_ACK_FIRST_DEADLINE: return "ACK_FIRST_DEADLINE";
        case VAN_LP_ABORT_ACK_FIRST_DOMINANT: return "ACK_FIRST_DOMINANT";
        case VAN_LP_ABORT_ACK_DRIVE_DEADLINE: return "ACK_DRIVE_DEADLINE";
        case VAN_LP_ABORT_ACK_RELEASE_DEADLINE: return "ACK_RELEASE_DEADLINE";
        case VAN_LP_ABORT_TX_END_DEADLINE: return "TX_END_DEADLINE";
        default: return "UNSPECIFIED";
    }
}
#endif

VANTransportLayer::VANTransportLayer(IVanMessageSender* vanMessageSender, uint8_t rxPin, uint8_t dataRxLedIndicatorPin)
{
    _vanMessageSender = vanMessageSender;
    _crcCalculator = new VanCrcCalculator();

    _vanMessageSender->Start();

    _vanRx = new ESP32_RMT_VAN_RX(rxPin, dataRxLedIndicatorPin, VAN_LINE_LEVEL_HIGH, VAN_NETWORK_TYPE_COMFORT);
    //_vanRx = new ESP32_RMT_VAN_RX(rxPin, dataRxLedIndicatorPin, VAN_LINE_LEVEL_HIGH, VAN_NETWORK_TYPE_BODY);
    _vanRx->Start();

    _txQueue = xQueueCreate(TX_QUEUE_LENGTH, TX_QUEUE_ITEM_SIZE);
    xTaskCreate([](void* arg) {
        static_cast<VANTransportLayer*>(arg)->TxTask();
    }, "VAN-TxTask", 4096, this, 10, &_txTaskHandle);
}

uint8_t VANTransportLayer::SendMessage(const BusMessage& message, bool highPriority)
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

    return 1;
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
    bool result = _vanMessageSender->IsTxPossible();

    if (!result)
    {
        printf("VAN TX is not possible\n");
    }

    return result;
}

void VANTransportLayer::TxTask()
{
    BusMessage message;
#ifdef CONFIG_IDF_TARGET_ESP32C6
    bool queryResultPending = false;
    // On C6 this transport is wired to LpCoreVanTx in Platform/Esp/main.cpp.
    // This TX task owns submission: read the completed result before it can
    // submit another frame and allow LP to overwrite VAN_TX_RESULT.
    auto reportCompletedQuery = [&]()
    {
        if (queryResultPending && _vanMessageSender->IsTxPossible())
        {
            const VanLpResult result = static_cast<LpCoreVanTx*>(_vanMessageSender)->GetLastTxResult();
            queryResultPending = false;
            if (result == VAN_LP_ABORT)
            {
                const uint32_t detail = static_cast<LpCoreVanTx*>(_vanMessageSender)->GetLastTxAbortDetail();
                printf("VAN query %03X completed: result=%u (%s) stage=%s raw_ts=%u\n",
                       static_cast<unsigned>(VAN_LP_TRACE_QUERY_ID), static_cast<unsigned>(result),
                       VanQueryResultName(result), VanAbortStageName(detail),
                       static_cast<unsigned>(detail & 0xffffu));
            }
            else if (result == VAN_LP_QUERY_RESPONSE_ACKED || result == VAN_LP_QUERY_RESPONSE_RECEIVED)
            {
                const uint32_t eodTs = static_cast<LpCoreVanTx*>(_vanMessageSender)->GetLastTxEodTs();
                printf("VAN query %03X completed: result=%u (%s) eod_ts=%u\n",
                       static_cast<unsigned>(VAN_LP_TRACE_QUERY_ID), static_cast<unsigned>(result),
                       VanQueryResultName(result), static_cast<unsigned>(eodTs));
            }
            else
            {
                printf("VAN query %03X completed: result=%u (%s)\n",
                       static_cast<unsigned>(VAN_LP_TRACE_QUERY_ID),
                       static_cast<unsigned>(result), VanQueryResultName(result));
            }
            VanLpRxTrace trace;
            for (uint8_t i = 0; static_cast<LpCoreVanTx*>(_vanMessageSender)->GetLastTxRxTrace(i, trace); ++i)
                printf("VAN RX ts=%u pair=%u%u center=%u shift=%ld dlate=%ld ilate=%ld\n",
                       static_cast<unsigned>(trace.rawTs), static_cast<unsigned>((trace.pair >> 2) & 3),
                       static_cast<unsigned>(trace.pair & 3), static_cast<unsigned>(trace.centerOffset),
                       static_cast<long>(trace.correction), static_cast<long>(trace.fourthLate),
                       static_cast<long>(trace.inverseLate));
        }
    };
#endif

    while (true)
    {
        TickType_t queueWait = portMAX_DELAY;
#ifdef CONFIG_IDF_TARGET_ESP32C6
        reportCompletedQuery();
        // A query can complete while the queue is empty. Yield for one tick
        // rather than waiting forever for a subsequent queued message.
        if (queryResultPending)
        {
            queueWait = 1;
        }
#endif
        if (xQueueReceive(_txQueue, &message, queueWait) == pdTRUE)
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

#ifdef CONFIG_IDF_TARGET_ESP32C6
            // Completion may also occur during xQueueReceive/IsBusAvailable.
            reportCompletedQuery();
#endif
            switch (message.type)
            {
                case MessageType::Query:
                    //printf("Send query message: %03X\n", (unsigned int) message.id);
                    _vanMessageSender->SendReplyRequestFrame(message.id);
#ifdef CONFIG_IDF_TARGET_ESP32C6
                    queryResultPending = VAN_LP_TRACE_QUERY_ID != 0 && message.id == VAN_LP_TRACE_QUERY_ID;
#endif
                    break;
                case MessageType::Normal:
                    //printf("Send normal message: %03X\n", (unsigned int) message.id);
                    _vanMessageSender->SendNormalFrame(message.id, message.data, message.dataLength, message.ack);
                    break;
                default:
                    break;
            }
        }
    }
}
