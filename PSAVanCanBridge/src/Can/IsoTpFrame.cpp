#include "IsoTpFrame.h"
#include <cstring>

#include <Arduino.h>
#ifdef ISO_TP_DEBUG
#endif

IsoTpFrame::IsoTpFrame(ICanMessageSender *canInterface, uint16_t txId, uint16_t rxId, uint16_t internalProcessInterval)
{
    _canInterface = canInterface;

    _txMsg.tx_id = txId;
    _rxMsg.rx_id = rxId;

    _txMsg.Buffer = (uint8_t *)calloc(MAX_MSGBUF, sizeof(uint8_t));
    _rxMsg.Buffer = (uint8_t *)calloc(MAX_MSGBUF, sizeof(uint8_t));

    _internalProcessInterval = internalProcessInterval;
}

// Send Flow Control (FC) Frame
uint8_t IsoTpFrame::send_fc()
{
    _txMsg.fc_status = ISOTP_FC_CTS;
    _txMsg.blocksize = 0;
    _txMsg.min_sep_time = 0;

    uint8_t TxBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // FC message high nibble = 0x3 , low nibble = FC Status
    TxBuf[0] = (N_PCI_FC | _txMsg.fc_status);
    TxBuf[1] = _txMsg.blocksize;

    /* fix wrong separation time values according spec */
    if ((_txMsg.min_sep_time > 0x7F) &&
        ((_txMsg.min_sep_time < 0xF1) || (_txMsg.min_sep_time > 0xF9)))
        {
            _txMsg.min_sep_time = 0x7F;
        }

    TxBuf[2] = _txMsg.min_sep_time;

    return _canInterface->SendMessage(_txMsg.tx_id, 0, 3, TxBuf);
}

 // Send Single Frame (SF) Message
uint8_t IsoTpFrame::send_sf()
{
    uint8_t TxBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    // SF message high nibble = 0x0 , low nibble = Length
    TxBuf[0] = (N_PCI_SF | _txMsg.len);
    memcpy(TxBuf + 1, _txMsg.Buffer, _txMsg.len);

    return _canInterface->SendMessage(_txMsg.tx_id, 0, _txMsg.len+1, TxBuf); // Add PCI length
}

 // Send First Frame (FF)
uint8_t IsoTpFrame::send_ff()
{
    uint8_t TxBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    _txMsg.seq_id = 1;

    TxBuf[0] = (N_PCI_FF | ((_txMsg.len&0x0F00) >> 8));
    TxBuf[1] = (_txMsg.len&0x00FF);
    memcpy(TxBuf + 2, _txMsg.Buffer, 6);             // Skip 2 Bytes PCI
    return _canInterface->SendMessage(_txMsg.tx_id, 0, 8, TxBuf);       // First Frame has full length
}

 // Send Consecutive Frame (CF) Message
uint8_t IsoTpFrame::send_cf()
{
    uint8_t TxBuf[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint16_t len = 7;

    TxBuf[0] = (N_PCI_CF | (_txMsg.seq_id & 0x0F));

    if(_txMsg.len > 7)
    {
        len = 7;
    }
    else
    {
        len = _txMsg.len;
    }

    memcpy(TxBuf + 1, _txMsg.Buffer, len);         // Skip 1 Byte PCI

    //return _canInterface->SendMessage(_txMsg.tx_id, len + 1, TxBuf); // Last frame is probably shorter than 8 -> Signals last CF Frame
    return _canInterface->SendMessage(_txMsg.tx_id, 0, 8, TxBuf);     // Last frame is probably shorter than 8, pad with 00
}

void IsoTpFrame::fc_delay(uint8_t sep_time)
{
    /*
    * 0x00 - 0x7F: 0 - 127ms
    * 0x80 - 0xF0: reserved
    * 0xF1 - 0xF9: 100us - 900us
    * 0xFA - 0xFF: reserved
    * default 0x7F, 127ms
    */
    if(sep_time <= 0x7F)
    {
        delay(sep_time);
    }
    else if ((sep_time >= 0xF1) && (sep_time <= 0xF9))
    {
        delayMicroseconds((sep_time-0xF0)*100);
    }
    else
    {
        delay(0x7F);
    }
}

uint8_t IsoTpFrame::Send(uint8_t *byteArray, uint8_t sizeOfByteArray)
{
    if (_txMsg.tp_state == ISOTP_IDLE || _txMsg.tp_state == ISOTP_ERROR)
    {
        _receiveFinishCalled = false;

        _txMsg.tp_state = ISOTP_SEND;
        _txMsg.len = sizeOfByteArray;
        memcpy(_txMsg.Buffer, byteArray, sizeOfByteArray);
        return 1;
    }

    return 0;
}

// Receive Flow Control (FC) frame
uint8_t IsoTpFrame::rcv_fc()
{
    uint8_t retval=0;

    if (_rxMsg.tp_state != ISOTP_WAIT_FC && _rxMsg.tp_state != ISOTP_WAIT_FIRST_FC)
        return 0;

    /* get communication parameters only from the first FC frame */
    if (_rxMsg.tp_state == ISOTP_WAIT_FIRST_FC)
    {
        _rxMsg.blocksize = rxBuffer[1];
        _rxMsg.min_sep_time = rxBuffer[2];

        /* fix wrong separation time values according spec */
        if (
            (_rxMsg.min_sep_time > 0x7F) &&
            ((_rxMsg.min_sep_time < 0xF1) || (_rxMsg.min_sep_time > 0xF9))
           )
        {
            _rxMsg.min_sep_time = 0x7F;
        }
    }

    #ifdef ISO_TP_DEBUG
    Serial.print(F("FC frame: FS "));
    Serial.print(rxBuffer[0]&0x0F);
    Serial.print(F(", Blocksize "));
    Serial.print(_rxMsg.blocksize);
    Serial.print(F(", Min. separation Time "));
    Serial.println(_rxMsg.min_sep_time);
    #endif

    switch (rxBuffer[0] & 0x0F)
    {
        case ISOTP_FC_CTS:
            _rxMsg.tp_state = ISOTP_SEND_CF;
            break;

        case ISOTP_FC_WT:
            fc_wait_frames++;
            if(fc_wait_frames >= MAX_FCWAIT_FRAME)
            {
                #ifdef ISO_TP_DEBUG
                Serial.println(F("FC wait frames exceeded."));
                #endif
                fc_wait_frames=0;
                _rxMsg.tp_state = ISOTP_IDLE;
                retval = 1;
            }
            #ifdef ISO_TP_DEBUG
            Serial.println(F("Start waiting for next FC"));
            #endif
            break;

        case ISOTP_FC_OVFLW:
            #ifdef ISO_TP_DEBUG
            Serial.println(F("Overflow in receiver side"));
            #endif

        default:
            _rxMsg.tp_state = ISOTP_IDLE;
            retval = 1;
    }
    return retval;
}

// Receive Single Frame (SF) frame
uint8_t IsoTpFrame::rcv_sf()
{
    /* get the SF_DL from the N_PCI byte */
    _rxMsg.len = rxBuffer[0] & 0x0F;

    /* copy the received data bytes */
    memcpy(_rxMsg.Buffer, rxBuffer + 1, _rxMsg.len); // Skip PCI, SF uses len bytes
    _rxMsg.tp_state = ISOTP_FINISHED;

    return 0;
}

// Receive First Frame (FF) frame
uint8_t IsoTpFrame::rcv_ff()
{
    _rxMsg.seq_id = 1;

    /* get the FF_DL */
    _rxMsg.len = (rxBuffer[0] & 0x0F) << 8;
    _rxMsg.len += rxBuffer[1];
    rest = _rxMsg.len;

    /* copy the first received data bytes */
    memcpy(_rxMsg.Buffer, rxBuffer+2, 6); // Skip 2 bytes PCI, FF must have 6 bytes!
    rest -= 6; // Restlength

    _rxMsg.tp_state = ISOTP_WAIT_DATA;

    #ifdef ISO_TP_DEBUG
    Serial.print(F("First frame received with message length: "));
    Serial.println(rest);
    Serial.println(F("Send flow controll."));
    Serial.print(F("ISO-TP state: ")); Serial.println(_rxMsg.tp_state);
    #endif

    /* send our first FC frame with Target Address*/
    return send_fc();
}

// Receive Consecutive Frame (CF) frame
uint8_t IsoTpFrame::rcv_cf()
{
    //Handle Timeout
    //If no Frame within 250ms change State to ISOTP_IDLE

    uint32_t delta = currentTime - wait_cf;     // currentTime was millis()

    if ((delta >= TIMEOUT_FC) && _rxMsg.seq_id > 1)
    {
        #ifdef ISO_TP_DEBUG
        Serial.println(F("CF frame timeout during receive wait_cf="));
        Serial.print(wait_cf); Serial.print(F(" delta="));
        Serial.println(delta);
        #endif
        _rxMsg.tp_state = ISOTP_IDLE;
        return 1;
    }
    wait_cf = currentTime; // currentTime was millis()

    #ifdef ISO_TP_DEBUG
    Serial.print(F("ISO-TP state: ")); Serial.println(_rxMsg.tp_state);
    Serial.print(F("CF received with message rest length: "));
    Serial.println(rest);
    #endif

    if (_rxMsg.tp_state != ISOTP_WAIT_DATA)
    {
        return 0;
    }

    if ((rxBuffer[0] & 0x0F) != (_rxMsg.seq_id & 0x0F))
    {
        #ifdef ISO_TP_DEBUG
        Serial.print(F("Got sequence ID: ")); Serial.print(rxBuffer[0] & 0x0F);
        Serial.print(F(" Expected: ")); Serial.println(_rxMsg.seq_id & 0x0F);
        #endif
        _rxMsg.tp_state = ISOTP_IDLE;
        _rxMsg.seq_id = 1;
        return 1;
    }

    if(rest <= 7) // Last Frame
    {
        memcpy(_rxMsg.Buffer + 6 + 7*(_rxMsg.seq_id-1), rxBuffer + 1, rest);   // 6 Bytes in FF +7
        _rxMsg.tp_state = ISOTP_FINISHED;                                       // per CF skip PCI
        #ifdef ISO_TP_DEBUG
        Serial.print(F("Last CF received with seq. ID: "));
        Serial.println(_rxMsg.seq_id);
        #endif
    }
    else
    {
        #ifdef ISO_TP_DEBUG
        Serial.print(F("CF received with seq. ID: "));
        Serial.println(_rxMsg.seq_id);
        #endif
        memcpy(_rxMsg.Buffer + 6 + 7*(_rxMsg.seq_id-1), rxBuffer + 1, 7);   // 6 Bytes in FF +7
                                                                        // per CF
        rest -= 7; // Got another 7 Bytes of Data;
    }

    _rxMsg.seq_id++;

    return 0;
}

uint8_t IsoTpFrame::Receive(unsigned long millis, uint16_t canId, uint8_t len, uint8_t* buf)
{
    currentTime = millis;

    uint8_t n_pci_type = 0;
    /*
    uint32_t delta = 0;
    wait_session = currentTime;
    #ifdef ISO_TP_DEBUG
    Serial.println(F("Start receive..."));
    #endif
    msg->tp_state=ISOTP_IDLE;
    */

    if(canId == _rxMsg.rx_id)
    {
        memcpy(&rxBuffer, buf, len);

        #ifdef ISO_TP_DEBUG
        Serial.println(F("rxId OK!"));
        #endif

        n_pci_type = buf[0] & 0xF0;

        switch (n_pci_type)
        {
            case N_PCI_FC:
                #ifdef ISO_TP_DEBUG
                Serial.println(F("FC"));
                #endif
                /* rx path: fc frame */
                rcv_fc();
                break;

            case N_PCI_SF:
                #ifdef ISO_TP_DEBUG
                Serial.println(F("SF"));
                #endif
                /* rx path: single frame */
                rcv_sf();
                //msg->tp_state=ISOTP_FINISHED;
                break;

            case N_PCI_FF:
                #ifdef ISO_TP_DEBUG
                Serial.println(F("FF"));
                #endif
                /* rx path: first frame */
                rcv_ff();
                //msg->tp_state=ISOTP_WAIT_DATA;
                break;

            case N_PCI_CF:
                #ifdef ISO_TP_DEBUG
                Serial.println(F("CF"));
                #endif
                /* rx path: consecutive frame */
                rcv_cf();
                break;
        }

        // reset rxBuffer
        memset(rxBuffer, 0, sizeof(rxBuffer));

        if (_rxMsg.tp_state == ISOTP_FINISHED && !_receiveFinishCalled)
        {
            _receiveFinishCalled = true;
            ReceiveFinished();
        }

        return 1;
    }
    return 0;
}

uint8_t IsoTpFrame::Process(unsigned long millis)
{
    if (currentTime - _previousInternalProcessTime > _internalProcessInterval)
    {
        _previousInternalProcessTime = currentTime;

        InternalProcess();
    }
    #ifdef ISO_TP_DEBUG
    //Serial.print(F("ISO-TP process id: ")); Serial.println(_txMsg.tx_id, HEX);
    //Serial.print(F("ISO-TP process state: ")); Serial.println(_txMsg.tp_state);
    #endif

    uint32_t delta = 0;
    uint8_t retval = 0;

    //_txMsg.tp_state = ISOTP_SEND;

    if(_txMsg.tp_state == ISOTP_WAIT_FIRST_FC || _txMsg.tp_state == ISOTP_WAIT_FC)
    {
        retval = rcv_fc();
    }

    if (_txMsg.tp_state != ISOTP_IDLE && _txMsg.tp_state != ISOTP_ERROR)
    {
        blockBoundary = false;

        #ifdef ISO_TP_DEBUG
        Serial.print(F("ISO-TP State: ")); Serial.println(_txMsg.tp_state);
        Serial.print(F("Length      : ")); Serial.println(_txMsg.len);
        #endif

        switch(_txMsg.tp_state)
        {
            case ISOTP_IDLE:
            break;

            case ISOTP_SEND:
                if(_txMsg.len <= 7)
                {
                    #ifdef ISO_TP_DEBUG
                    Serial.println(F("Send SF"));
                    #endif
                    retval = send_sf();
                    _txMsg.tp_state = ISOTP_IDLE;
                }
                else
                {
                    #ifdef ISO_TP_DEBUG
                    Serial.println(F("Send FF"));
                    #endif
                    if(!(retval = send_ff())) // FF complete
                    {
                        _txMsg.Buffer += 6;
                        _txMsg.len -= 6;
                        _txMsg.tp_state = ISOTP_WAIT_FIRST_FC;
                        fc_wait_frames = 0;
                        wait_fc = millis;
                    }
                }
                break;

            case ISOTP_WAIT_FIRST_FC:
                #ifdef ISO_TP_DEBUG
                Serial.println(F("Wait first FC"));
                #endif
                delta = millis - wait_fc;
                if(delta >= TIMEOUT_FC)
                {
                    #ifdef ISO_TP_DEBUG
                    Serial.print(F("FC timeout during receive"));
                    Serial.print(F(" wait_fc="));
                    Serial.print(wait_fc);
                    Serial.print(F(" delta="));
                    Serial.println(delta);
                    #endif
                    _txMsg.tp_state = ISOTP_IDLE;
                    retval = 1;
                }
                break;

            case ISOTP_WAIT_FC:
                #ifdef ISO_TP_DEBUG
                Serial.println(F("Wait FC"));
                #endif
                break;

            case ISOTP_SEND_CF:
                #ifdef ISO_TP_DEBUG
                Serial.println(F("Send CF"));
                #endif
                while((_txMsg.len > 7) && !blockBoundary)
                {
                    fc_delay(_txMsg.min_sep_time);
                    if(!(retval = send_cf()))
                    {
                        #ifdef ISO_TP_DEBUG
                        Serial.print(F("Send Seq "));
                        Serial.println(_txMsg.seq_id);
                        #endif
                        if(_txMsg.blocksize > 0)
                        {
                            #ifdef ISO_TP_DEBUG
                            Serial.print(F("Blocksize trigger "));
                            Serial.print(_txMsg.seq_id % _txMsg.blocksize);
                            #endif
                            if(!(_txMsg.seq_id % _txMsg.blocksize))
                            {
                                blockBoundary = true;
                                _txMsg.tp_state = ISOTP_WAIT_FC;
                                #ifdef ISO_TP_DEBUG
                                Serial.println(F(" yes"));
                                #endif
                            }
                            #ifdef ISO_TP_DEBUG
                            else Serial.println(F(" no"));
                            #endif
                        }
                        _txMsg.seq_id++;
                        if (_txMsg.blocksize < 16)
                            _txMsg.seq_id %= 16;
                        else
                            _txMsg.seq_id %= _txMsg.blocksize;
                            _txMsg.Buffer += 7;
                            _txMsg.len -= 7;
                            #ifdef ISO_TP_DEBUG
                            Serial.print(F("Length      : "));
                            Serial.println(_txMsg.len);
                            #endif
                    }
                }
                if(!blockBoundary)
                {
                    fc_delay(_txMsg.min_sep_time);
                    #ifdef ISO_TP_DEBUG
                    Serial.print(F("Send last Seq "));
                    Serial.println(_txMsg.seq_id);
                    #endif
                    retval = send_cf();
                    _txMsg.tp_state = ISOTP_IDLE;
                }
                break;

                default:
                    break;
        }
        /*
        if(_txMsg.tp_state == ISOTP_WAIT_FIRST_FC || _txMsg.tp_state == ISOTP_WAIT_FC)
        {
            if(can_receive())
            {
                #ifdef ISO_TP_DEBUG
                Serial.println(F("Send branch:"));
                #endif
                if(rxId == _txMsg.rx_id)
                {
                    retval = rcv_fc();
                    memset(rxBuffer, 0, sizeof(rxBuffer));
                    #ifdef ISO_TP_DEBUG
                    Serial.println(F("rxId OK!"));
                    #endif
                }
            }
        }
        */
    }

    return retval;
}
