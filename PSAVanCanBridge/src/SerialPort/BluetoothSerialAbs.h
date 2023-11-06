/*
 * Written by Ryan Smith <computergeek125@gmail.com>
 * Licensed under the MIT license.  See LICENSE for details
 */

#ifndef BluetoothSerialAbs_H
#define BluetoothSerialAbs_H
#define LIBRARY_VERSION_BLUETOOTSERABS_H   "0.1.0-alpha"

/*
#ifdef ARDUINO_ARCH_SAMD
#warning "This library often does not compile properly on SAMD boards.  Consider using UartSerialAbs instead."
#endif
*/

#include <inttypes.h>
#include "AbstractSerial.h"
#include <BluetoothSerial.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class BluetoothSerAbs : public AbsSer {
public:
    inline BluetoothSerAbs(BluetoothSerial &blport, String localName) {
        port = &blport;
        name = localName;
    }
    bool begin(String localName=String());
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

    void SetConnected(bool connected);
private:
    BluetoothSerial* port;
    String name;
    bool isConnected;
};
#endif