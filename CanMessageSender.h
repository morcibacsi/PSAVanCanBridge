#pragma once

#ifndef _CanMessageSender_h
    #define _CanMessageSender_h

#include "driver/can.h"
#include "AbstractCanMessageSender.h"

class CanMessageSender : public AbstractCanMessageSender {
  private:
      uint8_t _rxPin;
      uint8_t _txPin;
      SemaphoreHandle_t canSemaphore;
  public:
      CanMessageSender(uint8_t rxPin, uint8_t txPin)
      {
          _rxPin = rxPin;
          _txPin = txPin;
          canSemaphore = xSemaphoreCreateMutex();
      }

      virtual void Init()
      {
          can_timing_config_t t_config; // = CAN_TIMING_CONFIG_125KBITS();
          t_config.brp = 32;
          t_config.tseg_1 = 15;
          t_config.tseg_2 = 4;
          t_config.sjw = 3;
          t_config.triple_sampling = false;

          //Filter all other IDs except MSG_ID
          can_filter_config_t f_config;

          //f_config.acceptance_code = (MSG_ID << 21);
          //f_config.acceptance_mask = ~(CAN_STD_ID_MASK << 21);
          f_config.single_filter = true;

          //Set to NO_ACK mode due to self testing with single module
          can_general_config_t g_config; // = CAN_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, CAN_MODE_NO_ACK)
          g_config.rx_io = (gpio_num_t)_rxPin;
          g_config.tx_io = (gpio_num_t)_txPin;
          g_config.clkout_io = (gpio_num_t)CAN_IO_UNUSED;
          g_config.tx_queue_len = 5;
          g_config.rx_queue_len = 5;
          g_config.alerts_enabled = CAN_ALERT_NONE;
          g_config.clkout_divider = 0;
          g_config.mode = CAN_MODE_NO_ACK;

          ESP_ERROR_CHECK(can_driver_install(&g_config, &t_config, &f_config));
          ESP_ERROR_CHECK(can_start());
      }

      virtual void SendMessage(unsigned long canId, uint8_t ext, uint8_t sizeOfByteArray, unsigned char *byteArray)
      {
          can_message_t tx_msg;
          tx_msg.data_length_code = sizeOfByteArray;
          tx_msg.identifier = canId;
          for (size_t i = 0; i < sizeOfByteArray; i++)
          {
              tx_msg.data[i] = byteArray[i];
          }
          //ESP_LOGI(EXAMPLE_TAG, "Send Msg 1 %d", i);
          xSemaphoreTake(canSemaphore, portMAX_DELAY);

          ESP_ERROR_CHECK(can_transmit(&tx_msg, portMAX_DELAY));

          xSemaphoreGive(canSemaphore);

      }

      virtual void ReadMessage(uint8_t *len, uint8_t *buf)
      {
          can_message_t rx_msg;
          ESP_ERROR_CHECK(can_receive(&rx_msg, portMAX_DELAY));
          *len = rx_msg.data_length_code;
          for (uint8_t i = 0; i < *len; i++)
          {
              buf[i] = rx_msg.data[i];
          }
      }

      //virtual unsigned long GetCanId(void)
      //{
      //    return CAN->getCanId();
      //}

      //virtual unsigned long Start(uint8_t speedset, const uint8_t clockset)
      //{
      //    return CAN->begin(speedset, clockset);
      //}

      //virtual uint8_t CheckReceive(void)
      //{
      //    return CAN->checkReceive();
      //}
 };

#endif
