/*
 * Written by Ryan Smith <computergeek125@gmail.com>
 * Licensed under the MIT license.  See LICENSE for details
 */

#ifndef USBSerialAbs_H
#define USBSerialAbs_H
#define LIBRARY_VERSION_USBSerAbs_H   "0.1.0-alpha"

#ifdef ARDUINO_ARCH_SAMD
#warning "This library often does not compile properly on SAMD boards.  Consider using UartSerialAbs instead."
#endif

#include <inttypes.h>
#include "AbstractSerial.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
class UsbSerAbs : public AbsSer {
public:
    inline UsbSerAbs(HWCDC &hsport) {
        port = &hsport;
    }
    void begin(unsigned long baud, uint8_t config);
    void begin(unsigned long baud);
    void end();
    int available(void);
    int peek(void);
    int read(void);
    int availableForWrite(void);
    void flush(void);
    using Print::write;
    size_t write(uint8_t n);
    size_t write(unsigned long n);
    size_t write(long n);
    size_t write(unsigned int n);
    size_t write(int n);
    inline operator bool() {return port;}

private:
    HWCDC* port;
};
#endif
#endif