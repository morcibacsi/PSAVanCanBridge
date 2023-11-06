/*
 * Written by Ryan Smith <computergeek125@gmail.com>
 * Licensed under the MIT license.  See LICENSE for details
 */

#include "BluetoothSerialAbs.h"

static BluetoothSerAbs* thisBluetoothSerial;

static esp_spp_cb_t custom_spp_callback = NULL;

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
    if(event == ESP_SPP_SRV_OPEN_EVT){
        thisBluetoothSerial->SetConnected(true);
    }

    if(event == ESP_SPP_CLOSE_EVT ){
        thisBluetoothSerial->SetConnected(false);
    }
}

void BluetoothSerAbs::SetConnected(bool connected){
    isConnected = connected;
}

void BluetoothSerAbs::begin(unsigned long baud, uint8_t config) {
    thisBluetoothSerial = this;
    port->begin(name);
}

void BluetoothSerAbs::begin(unsigned long baud) {
    thisBluetoothSerial = this;
    port->begin(name);

    custom_spp_callback = callback;
    port->register_callback(&custom_spp_callback);
}

void BluetoothSerAbs::end() {
    port->end();
}

int BluetoothSerAbs::available(void) {
    if (isConnected)
    {
        return port->available();
    }
    return 0;
}

int BluetoothSerAbs::peek(void) {
    if (isConnected)
    {
        return port->peek();
    }
    return 0;
}

int BluetoothSerAbs::read(void) {
    if (isConnected)
    {
        return port->read();
    }
    return 0;
}

int BluetoothSerAbs::availableForWrite(void) {
    if (isConnected)
    {
        return port->available();
    }
    return 0;
}

void BluetoothSerAbs::flush(void) {
    if (isConnected)
    {
        port->flush();
    }
}

size_t BluetoothSerAbs::write(uint8_t n) {
    if (isConnected)
    {
        return port->write(n);
    }
    return 0;
}

size_t BluetoothSerAbs::write(unsigned long n) {
    if (isConnected)
    {
        return port->write(n);
    }
    return 0;
}

size_t BluetoothSerAbs::write(long n) {
    if (isConnected)
    {
        return port->write(n);
    }
    return 0;
}

size_t BluetoothSerAbs::write(unsigned int n) {
    if (isConnected)
    {
        return port->write(n);
    }
    return 0;
}

size_t BluetoothSerAbs::write(int n) {
    if (isConnected)
    {
        return port->write(n);
    }
    return 0;
}
