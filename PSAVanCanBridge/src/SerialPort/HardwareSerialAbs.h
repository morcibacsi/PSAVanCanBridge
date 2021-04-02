/*
 * Written by Ryan Smith <computergeek125@gmail.com>
 * Licensed under the MIT license.  See LICENSE for details
 */

#ifndef HardwareSerialAbs_H
#define HardwareSerialAbs_H
#define LIBRARY_VERSION_HWSERABS_H   "0.1.0-alpha"

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

class HwSerAbs : public AbsSer {
public:
    inline HwSerAbs(HardwareSerial &hsport) {
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
    HardwareSerial* port;
};
#endif