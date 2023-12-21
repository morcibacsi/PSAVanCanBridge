#pragma once

#ifndef DebugPrint_h
    #define DebugPrint_h

#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>
#include "../../Config.h"

#ifdef PRINT_DEBUG
///*
    static size_t debug_print(const __FlashStringHelper *f)
    {
        return Serial.print(f);
    }

    static size_t debug_print(const String &s)
    {
        return Serial.print(s);
    }

    //size_t debug_print(const char[] c)
    //{
    //    return Serial.print(c);
    //}

    static size_t debug_print(char c)
    {
        return Serial.print(c);
    }

    static size_t debug_print(unsigned char c, int v = DEC)
    {
        return Serial.print(c, v);
    }

    static size_t debug_print(int i, int v = DEC)
    {
        return Serial.print(i, v);
    }

    static size_t debug_print(unsigned int i, int v = DEC)
    {
        return Serial.print(i, v);
    }

    static size_t debug_print(long l, int v = DEC)
    {
        return Serial.print(l, v);
    }

    static size_t debug_print(unsigned long l, int v = DEC)
    {
        return Serial.print(l, v);
    }

    static size_t debug_print(long long l, int v = DEC)
    {
        return Serial.print(l, v);
    }

    static size_t debug_print(unsigned long long l, int v = DEC)
    {
        return Serial.print(l, v);
    }

    static size_t debug_print(double d, int v = 2)
    {
        return Serial.print(d, v);
    }

    static size_t debug_print(const Printable& p)
    {
        return Serial.print(p);
    }

    static size_t debug_print(struct tm * timeinfo, const char * format = NULL)
    {
        return Serial.print(timeinfo, format);
    }

    static size_t debug_println(const __FlashStringHelper *f)
    {
        return Serial.println(f);
    }

    static size_t debug_println(const String &s)
    {
        return Serial.println(s);
    }

    //size_t debug_println(const char[] c)
    //{
    //    return Serial.println(c);
    //}
    static size_t debug_println(char c)
    {
        return Serial.println(c);
    }

    static size_t debug_println(unsigned char c, int v = DEC)
    {
        return Serial.println(c, v);
    }

    static size_t debug_println(int i, int v = DEC)
    {
        return Serial.println(i, v);
    }

    static size_t debug_println(unsigned int i, int v = DEC)
    {
        return Serial.println(i, v);
    }

    static size_t debug_println(long l, int v = DEC)
    {
        return Serial.println(l, v);
    }

    static size_t debug_println(unsigned long l, int v = DEC)
    {
        return Serial.println(l, v);
    }

    static size_t debug_println(long long l, int v = DEC)
    {
        return Serial.println(l, v);
    }

    static size_t debug_println(unsigned long long l, int v = DEC)
    {
        return Serial.println(l, v);
    }

    static size_t debug_println(double d, int v = 2)
    {
        return Serial.println(d, v);
    }

    static size_t debug_println(const Printable& p)
    {
        return Serial.println(p);
    }

    static size_t debug_println(struct tm * timeinfo, const char * format = NULL)
    {
        return Serial.println(timeinfo, format);
    }

    static size_t debug_println(void)
    {
        return Serial.println();
    }
//*/
#else
///*
    static size_t debug_print(const __FlashStringHelper *f)
    {
        return 0;
    }

    static size_t debug_print(const String &s)
    {
        return 0;
    }

    //size_t debug_print(const char[] c)
    //{
    //    return Serial.print(c);
    //}

    static size_t debug_print(char c)
    {
        return 0;
    }

    static size_t debug_print(unsigned char c, int v = DEC)
    {
        return 0;
    }

    static size_t debug_print(int i, int v = DEC)
    {
        return 0;
    }

    static size_t debug_print(unsigned int i, int v = DEC)
    {
        return 0;
    }

    static size_t debug_print(long l, int v = DEC)
    {
        return 0;
    }

    static size_t debug_print(unsigned long l, int v = DEC)
    {
        return 0;
    }

    static size_t debug_print(long long l, int v = DEC)
    {
        return 0;
    }

    static size_t debug_print(unsigned long long l, int v = DEC)
    {
        return 0;
    }

    static size_t debug_print(double d, int v = 2)
    {
        return 0;
    }

    static size_t debug_print(const Printable& p)
    {
        return 0;
    }

    static size_t debug_print(struct tm * timeinfo, const char * format = NULL)
    {
        return 0;
    }

    static size_t debug_println(const __FlashStringHelper *f)
    {
        return 0;
    }

    static size_t debug_println(const String &s)
    {
        return 0;
    }

    //size_t debug_println(const char[] c)
    //{
    //    return Serial.println(c);
    //}

    static size_t debug_println(char c)
    {
        return 0;
    }

    static size_t debug_println(unsigned char c, int v = DEC)
    {
        return 0;
    }

    static size_t debug_println(int i, int v = DEC)
    {
        return 0;
    }

    static size_t debug_println(unsigned int i, int v = DEC)
    {
        return 0;
    }

    static size_t debug_println(long l, int v = DEC)
    {
        return 0;
    }

    static size_t debug_println(unsigned long l, int v = DEC)
    {
        return 0;
    }

    static size_t debug_println(long long l, int v = DEC)
    {
        return 0;
    }

    static size_t debug_println(unsigned long long l, int v = DEC)
    {
        return 0;
    }

    static size_t debug_println(double d, int v = 2)
    {
        return 0;
    }

    static size_t debug_println(const Printable& p)
    {
        return 0;
    }

    static size_t debug_println(struct tm * timeinfo, const char * format = NULL)
    {
        return 0;
    }

    static size_t debug_println(void)
    {
        return 0;
    }
//*/
#endif

static void printArray(String name, uint8_t* data, int length)
{
    char tmp[3];
    debug_print(name);
    for (int i = 0; i < length; i++)
    {
        snprintf(tmp, 3, "%02X", data[i]);
        debug_print(tmp);
        debug_print(" ");
    }
    debug_println();
}

#endif