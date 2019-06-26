/*
 * Written by Ryan Smith <computergeek125@gmail.com>
 * Licensed under the MIT license.  See LICENSE for details
 */

#include "BluetoothSerialAbs.h"
void BluetoothSerAbs::begin(unsigned long baud, uint8_t config) {
    port->begin(name);
}

void BluetoothSerAbs::begin(unsigned long baud) {
    port->begin(name);
}

void BluetoothSerAbs::end() {
    port->end();
}

int BluetoothSerAbs::available(void) {
    return port->available();
}

int BluetoothSerAbs::peek(void) {
    return port->peek();
}

int BluetoothSerAbs::read(void) {
    return port->read();
}

int BluetoothSerAbs::availableForWrite(void) {
    return port->available();
}

void BluetoothSerAbs::flush(void) {
    port->flush();
}

size_t BluetoothSerAbs::write(uint8_t n) {
    return port->write(n);
}

size_t BluetoothSerAbs::write(unsigned long n) {
    return port->write(n);
}

size_t BluetoothSerAbs::write(long n) {
    return port->write(n);
}

size_t BluetoothSerAbs::write(unsigned int n) {
    return port->write(n);
}

size_t BluetoothSerAbs::write(int n) {
    return port->write(n);
}