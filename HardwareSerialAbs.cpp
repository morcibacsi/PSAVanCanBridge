/*
 * Written by Ryan Smith <computergeek125@gmail.com>
 * Licensed under the MIT license.  See LICENSE for details
 */

#include "HardwareSerialAbs.h"

void HwSerAbs::begin(unsigned long baud, uint8_t config) {
    port->begin(baud, config);
}

void HwSerAbs::begin(unsigned long baud) {
    port->begin(baud);
}

void HwSerAbs::end() {
    port->end();
}

int HwSerAbs::available(void) {
    return port->available();
}

int HwSerAbs::peek(void) {
    return port->peek();
}

int HwSerAbs::read(void) {
    return port->read();
}

int HwSerAbs::availableForWrite(void) {
    return port->availableForWrite();
}

void HwSerAbs::flush(void) {
    port->flush();
}

size_t HwSerAbs::write(uint8_t n) {
    return port->write(n);
}

size_t HwSerAbs::write(unsigned long n) {
    return port->write(n);
}

size_t HwSerAbs::write(long n) {
    return port->write(n);
}

size_t HwSerAbs::write(unsigned int n) {
    return port->write(n);
}

size_t HwSerAbs::write(int n) {
    return port->write(n);
}