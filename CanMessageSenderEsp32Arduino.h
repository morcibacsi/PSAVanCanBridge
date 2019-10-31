#pragma once

#ifndef _CanMessageSenderEsp32Arduino_h
    #define _CanMessageSenderEsp32Arduino_h

#include <ESP32CAN.h>
#include <CAN_config.h>
#include "AbstractCanMessageSender.h"

class CanMessageSenderEsp32Arduino : public AbstractCanMessageSender {
  private:
      const int rx_queue_size = 10;
      CAN_device_t CAN_cfg;

      SemaphoreHandle_t canSemaphore;
  public:
      CanMessageSenderEsp32Arduino(uint8_t rxPin, uint8_t txPin)
      {
          CAN_cfg.speed = CAN_SPEED_125KBPS;
          CAN_cfg.tx_pin_id = (gpio_num_t)txPin;
          CAN_cfg.rx_pin_id = (gpio_num_t)rxPin;

          canSemaphore = xSemaphoreCreateMutex();
      }

      virtual void Init()
      {
          CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
          ESP32Can.CANInit(CAN_cfg);
      }

      virtual void SendMessage(uint16_t canId, uint8_t ext, uint8_t sizeOfByteArray, unsigned char *byteArray)
      {
          CAN_frame_t tx_frame;
          tx_frame.FIR.B.FF = CAN_frame_std;
          tx_frame.MsgID = canId;
          tx_frame.FIR.B.DLC = sizeOfByteArray;

          for (size_t i = 0; i < sizeOfByteArray; i++)
          {
              tx_frame.data.u8[i] = byteArray[i];
          }

          xSemaphoreTake(canSemaphore, portMAX_DELAY);
          ESP32Can.CANWriteFrame(&tx_frame);
          xSemaphoreGive(canSemaphore);
      }

      virtual void ReadMessage(uint16_t *canId, uint8_t *len, uint8_t *buf)
      {
          CAN_frame_t rx_frame;

          if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {

              if (rx_frame.FIR.B.FF == CAN_frame_std) {
                  //printf("New standard frame");
              }
              else {
                  //printf("New extended frame");
              }

              if (rx_frame.FIR.B.RTR == CAN_RTR) {
                  //printf(" RTR from 0x%08X, DLC %d\r\n", rx_frame.MsgID, rx_frame.FIR.B.DLC);
              }
              else {
                  *canId = rx_frame.MsgID;
                  *len = rx_frame.FIR.B.DLC;
                  //printf(" from 0x%08X, DLC %d, Data ", rx_frame.MsgID, rx_frame.FIR.B.DLC);
                  for (int i = 0; i < rx_frame.FIR.B.DLC; i++) {
                      buf[i] = rx_frame.data.u8[i];
                      //printf("0x%02X ", rx_frame.data.u8[i]);
                  }
                  //printf("\n");
              }
          }
      }
 };

#endif
