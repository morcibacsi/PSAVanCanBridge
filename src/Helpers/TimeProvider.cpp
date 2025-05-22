#include "TimeProvider.hpp"
#include <string.h>

TimeProvider::TimeProvider(uint8_t sdaPin, uint8_t sclPin, CarState* carState)
{
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    _carState = carState;
}

void TimeProvider::Start()
{
    if (!_carState->HAS_RTC || _started)
    {
        return;
    }

    // Initialize I2C
    i2c_master_bus_handle_t* bus_handle = (i2c_master_bus_handle_t*)malloc(sizeof(i2c_master_bus_handle_t));

    // Create the i2c_master_bus_config_t struct and assign values.
    i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = -1,
        .sda_io_num = (gpio_num_t)_sdaPin,
        .scl_io_num = (gpio_num_t)_sclPin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            // The DS3231 **requires** pullup resistors on all of its I/O pins.
            // Note: Some DS3231 boards have pullup resistors as part
            // of their design.
            .enable_internal_pullup = false,
        }
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, bus_handle));

    printf("I2C bus created\n");

    if (bus_handle == NULL)
    {
        printf("Failed to create I2C bus\n");
    }

    _rtc = ds3231_init(bus_handle);
    ds3231_debug_print_data(_rtc);

    //SetDateTime(2024, 12, 21, 13, 45, 0);

    _started = true;
}

bool TimeProvider::Process(unsigned long currentTime)
{
    if (!_carState->HAS_RTC)
    {
        return false;
    }

    uint8_t wday;
    if (currentTime - _previousTime >= 1000)
    {
        _previousTime = currentTime;

        time_t now;
        now = ds3231_time_unix_get(_rtc);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        _carState->Hour = timeinfo.tm_hour;
        _carState->Minute = timeinfo.tm_min;
        _carState->Second = timeinfo.tm_sec;
        _carState->MDay = timeinfo.tm_mday;
        _carState->Month = timeinfo.tm_mon + 1;
        _carState->Year = timeinfo.tm_year + 1900;

        //_rtc.getDateTime(&_dataBroker->Hour, &_dataBroker->Minute, &_dataBroker->Second, &_dataBroker->MDay, &_dataBroker->Month, &_dataBroker->Year, &wday);
        //_serialPort->println("TimeProvider::Process: " + String(_dataBroker->Hour) + ":" + String(_dataBroker->Minute) + ":" + String(_dataBroker->Second));
        return true;
    }

    return false;
}

void TimeProvider::SetDateTime(uint16_t year,  uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    if (!_carState->HAS_RTC)
    {
        return;
    }
    //_rtc.setDateTime(hour, minute, second, day, month, year, 0);

    struct tm timeinfo = {0};

    timeinfo.tm_year = year - 1900;  // Year is 1900-based
    timeinfo.tm_mon = month - 1;     // (0-based)
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;

    time_t time = mktime(&timeinfo);

    ESP_ERROR_CHECK(ds3231_time_time_t_set(_rtc, time));
}
