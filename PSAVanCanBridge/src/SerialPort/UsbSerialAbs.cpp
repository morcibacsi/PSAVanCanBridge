/*
 * Written by Ryan Smith <computergeek125@gmail.com>
 * Licensed under the MIT license.  See LICENSE for details
 */

#include "UsbSerialAbs.h"
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3

void UsbSerAbs::begin(unsigned long baud, uint8_t config) {
    port->begin(baud);
}

void UsbSerAbs::begin(unsigned long baud) {
    port->begin(baud);
}

void UsbSerAbs::end() {
    port->end();
}

int UsbSerAbs::available(void) {
    return port->available();
}

int UsbSerAbs::peek(void) {
    return port->peek();
}

int UsbSerAbs::read(void) {
    return port->read();
}

int UsbSerAbs::availableForWrite(void) {
    return port->availableForWrite();
}

void UsbSerAbs::flush(void) {
    port->flush();
}

size_t UsbSerAbs::write(uint8_t n) {
    return port->write(n);
}

size_t UsbSerAbs::write(unsigned long n) {
    return port->write(n);
}

size_t UsbSerAbs::write(long n) {
    return port->write(n);
}

size_t UsbSerAbs::write(unsigned int n) {
    return port->write(n);
}

size_t UsbSerAbs::write(int n) {
    return port->write(n);
}
#endif