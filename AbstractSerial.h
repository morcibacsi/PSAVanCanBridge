/*
 * Written by Ryan Smith <computergeek125@gmail.com>
 * Licensed under the MIT license.  See LICENSE for details
 */

#ifndef AbstractSerial_H
#define AbstractSerial_H
#define LIBRARY_VERSION_ABSSER_H   "0.1.0-alpha"

#include <inttypes.h>
#include "Stream.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

/**
 * This library is intended to provide a Serial class that wraps around the 
 * various UART, USBSerial, and other interesting serial ports seen in the 
 * Arduino ecosystem.  This is done by creating an API-compliant child class to
 * this abstract class.  This child class can be passed in to a function or 
 * library in abstract, so the program does not have to maintain its own wrapper
 * code to make multiple types of serial port work.
 *
 * This base library will include code to abstract a few different serial ports
 * and other stream-like devices that need to be accessed beyond their stream-
 * like capabilities.  Basically, this library's functions wrap the actual
 * serial port's functions one-to-one.
 * 
 * This class was based on the public interface of HardwareSerial.  If there are
 * any gaping incompatibilities with other kinds of ports, please open an issue 
 * on GitHub!
 * <https://github.com/computergeek125/arduino-abstract-serial>
 */

class AbsSer : public Stream {
public:
    virtual void begin(unsigned long baud, uint8_t config) =0;
    virtual void begin(unsigned long baud) =0;
    virtual void end() =0;
    virtual int available(void) =0;
    virtual int peek(void) =0;
    virtual int read(void) =0;
    virtual int availableForWrite(void) =0;
    virtual void flush(void) =0;
    using Print::write;
    virtual size_t write(uint8_t n) =0;
    virtual size_t write(unsigned long n) =0;
    virtual size_t write(long n) =0;
    virtual size_t write(unsigned int n) =0;
    virtual size_t write(int n) =0;
    virtual operator bool() =0;
};
#endif