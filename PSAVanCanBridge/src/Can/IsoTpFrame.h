#pragma once

#ifndef _IsoTpFrame_h
    #define _IsoTpFrame_h

#include <inttypes.h>
#include "ICanMessageSender.h"

//#define ISO_TP_DEBUG

typedef enum {
  ISOTP_IDLE = 0,
  ISOTP_SEND,
  ISOTP_SEND_FF,
  ISOTP_SEND_CF,
  ISOTP_WAIT_FIRST_FC,
  ISOTP_WAIT_FC,
  ISOTP_WAIT_DATA,
  ISOTP_FINISHED,
  ISOTP_ERROR
} isotp_states_t;

/* N_PCI type values in bits 7-4 of N_PCI bytes */
#define N_PCI_SF  0x00  /* single frame */
#define N_PCI_FF  0x10  /* first frame */
#define N_PCI_CF  0x20  /* consecutive frame */
#define N_PCI_FC  0x30  /* flow control */

#define FC_CONTENT_SZ 3 /* flow control content size in byte (FS/BS/STmin) */

/* Flow Status given in FC frame */
#define ISOTP_FC_CTS  0   /* clear to send */
#define ISOTP_FC_WT 1     /* wait */
#define ISOTP_FC_OVFLW  2 /* overflow */

/* Timeout values */
#define TIMEOUT_SESSION  500 /* Timeout between successfull send and receive */
#define TIMEOUT_FC       250 /* Timeout between FF and FC or Block CF and FC */
#define TIMEOUT_CF       250 /* Timeout between CFs                          */
#define MAX_FCWAIT_FRAME  10

#define MAX_MSGBUF 128

struct IsoTpMessage_t
{
  uint16_t len=0;
  isotp_states_t tp_state=ISOTP_IDLE;
  uint16_t seq_id=1;
  uint8_t fc_status=ISOTP_FC_CTS;
  uint8_t blocksize=0;
  uint8_t min_sep_time=0;
  uint32_t tx_id=0;
  uint32_t rx_id=0;
  uint8_t *Buffer;
};

class IsoTpFrame
{
    unsigned long currentTime;
    unsigned long _previousInternalProcessTime;

    uint16_t _internalProcessInterval;
    bool _receiveFinishCalled = false;

    uint8_t  fc_wait_frames = 0;
    uint8_t  rxBuffer[8];
    uint16_t rest;

    uint32_t wait_fc = 0;
    uint32_t wait_cf = 0;
    uint32_t wait_session = 0;

    uint8_t blockBoundary = false;

    uint8_t send_fc();
    uint8_t send_sf();
    uint8_t send_ff();
    uint8_t send_cf();

    void fc_delay(uint8_t sep_time);

    uint8_t rcv_fc();
    uint8_t rcv_sf();
    uint8_t rcv_ff();
    uint8_t rcv_cf();

    protected:
    struct IsoTpMessage_t _rxMsg;
    struct IsoTpMessage_t _txMsg;

    ICanMessageSender *_canInterface;
    IsoTpFrame(ICanMessageSender *canInterface, uint16_t txId, uint16_t rxId, uint16_t internalProcessInterval);
    virtual void InternalProcess() = 0;
    virtual void ReceiveFinished() = 0;

    public:

    uint8_t Send(uint8_t *byteArray, uint8_t sizeOfByteArray);
    virtual uint8_t Receive(unsigned long millis, uint16_t canId, uint8_t len, uint8_t* buf);
    uint8_t Process(unsigned long millis);
 };

#endif
