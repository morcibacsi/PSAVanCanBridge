/*
 * esp-idf-ds3231 component library for the ESP32 family of microcontrollers
 * using ESP-IDF v5.0 or later.
 * Copyright 224 Jason M. Schwefel
 *
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/license/MIT
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "esp-idf-ds3231.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "soc/clk_tree_defs.h"
#include "esp_debug_helpers.h"

#define RTC_I2C_ADDRESS 0x68
#define RTC_DATA_LENGTH 0x13

enum flag_bitmask_e {
    RTC_BITMASK_OSCILLATOR_ENABLE = 0b10000000,
    RTC_BITMASK_BATTERY_BACKED = 0b01000000,
    RTC_BITMASK_CONVERT_TEMPERATURE = 0b00100000,
    RTC_BITMASK_SQAURE_WAVE_FREQ = 0b00011000,
    RTC_BITMASK_INTERRUPT_CONTROL = 0b00000100,
    RTC_BITMASK_ALARM2_ENABLE_FIRED = 0b00000010,
    RTC_BITMASK_ALARM1_ENABLE_FIRED = 0b00000001,
    RTC_BITMASK_OSCILLATOR_STOP = 0b10000000,
    RTC_BITMASK_32KHZ_ENABLE = 0b00001000,
};

typedef struct {
    rtc_handle_t* rtc_handle;
    gpio_isr_t isr_handler;
    void* params;
} rtc_alarm_isr_t;

// Bitfield mapping is LSB to MSB
#pragma pack(push, 1)
/**
 * @brief Union representing seconds, minututes or year of the DS3231. All three types use a compatable format.
 */
typedef union {
    uint8_t data;                /*!< Native type representation of the data.  */
    struct {
        unsigned ones       : 4; /*!< 1's digit, represented as BCD */
        unsigned tens       : 3; /*!< 10's digit, represented as BCD */
        unsigned alarm_rate : 1;
    };
} rtc_seconds_or_minutes_t;

/**
 * @brief Union representing seconds, minututes or year of the DS3231. All three types use a compatable format.
 */
typedef union {
    uint8_t data;          /*!< Native type representation of the data.  */
    struct {
        unsigned ones : 4; /*!< 1's digit, represented as BCD */
        unsigned tens : 4; /*!< 10's digit, represented as BCD */
    };
} rtc_years_t;

/**
 * @brief Union representing hours of the DS3231.
 */
typedef union {
    uint8_t data;                        /*!< Native type representation of the data.  */
    struct {
        unsigned ones               : 4; /*!< 1's digit, represented as BCD */
        unsigned tens               : 1; /*!< 10's digit, represented as single bit. BCD conversion compliant. */
        unsigned ampm_hour_twenties : 1; /*!< AM/PM **OR** 20's digit. When 0, AM. When 1 is PM if 'twelve_hour' is 0, 20's
                                            digit if 'twelve_hour' is 1 */
        unsigned twelve_hour        : 1; /*!< Represents 12 hour time if 0, 24 hour time if 1 */
        unsigned alarm_rate         : 1;
    };
} rtc_hours_t;

/**
 * @brief Union representing days of the DS3231.
 */
typedef union {
    uint8_t data;              /*!< Native type representation of the data.  */
    struct {
        unsigned day_date : 4; /*!< Repreresents the day of month or (only for alarms AND dy_dt is 0). If an alarm and dy_dt is 1,
                              represents the day of the week, 1-7 starting on Monday. Both vlaues are expressed in BCD format. */
        unsigned tens     : 2; /*!< 10's value of date. BCD conversion compliant */
        unsigned dy_dt    : 1; /*!< Determine if day_date represents a day of week (DOW) or day of month (DOM). If 0, it is DOM. If
                                  1, it is DOW. */
        unsigned alarm_rate : 1;
    };
} rtc_day_date_t;

/**
 *
 * @brief Union representing the month **AND** century flag of the DS3231.
 */
typedef union {
    uint8_t data;             /*!< Native type representation of the data.  */
    struct {
        unsigned ones    : 4; /*!< 1's digit, represented as BCD */
        unsigned tens    : 1; /*!< 10's digit, represented as single bit. BCD conversion compliant. */
        /** @cond */
        unsigned unused  : 2;
        /** @endcond */
        unsigned century : 1; /*!< Century is 1900 if 0, 2000 if 1 */
    };
} rtc_month_century_t;

/**
 * @brief Union representing the aging offset register (10h) of the DS3231.
 */
typedef union {
    int8_t data;           /*!< Native type representation of the data.  */
    struct {
        signed offset : 8; /*!< See the DS3231 data sheet for more details. */
    };
} rtc_aging_offset_t;

/**
 * @brief Union representing the temperature in degrees celcius registers (11h & 12h) of the DS3231.
 */
typedef union {
    int16_t data;                    /*!< Native type representation of the data.  */
    struct {
        signed temp             : 8; /*!< Unsigned int8 representing the whole number temperature in dregree celcius */
        /** @cond */
        unsigned unused         : 6; /*!< Not used in register  */
        /** @endcond */
        unsigned quarter_degree : 2; /*!< The DS3231 has 0.25 degree celcius resolution. */
    };
} rtc_temperature_t;
#pragma pack(pop)

static const char* TAG = "esp-idf-ds3231";

static QueueHandle_t alarm_event_queue = NULL;
static TaskHandle_t isr_task_handle = NULL;

uint8_t convert_to_bcd(uint8_t value);
static const char* byte_to_binary(int x);
esp_err_t set_flag(rtc_handle_t* rtc_handle, bool setEnabled, bool isUpper, enum flag_bitmask_e bitmask);

rtc_handle_t* ds3231_init_full(gpio_num_t SCL, gpio_num_t SDA) {
    esp_err_t err;
    i2c_master_bus_handle_t* bus_handle = (i2c_master_bus_handle_t*)malloc(sizeof(i2c_master_bus_handle_t));
    if (bus_handle == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'bus_handle'");
    }

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = SCL,
        .sda_io_num = SDA,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    err = i2c_new_master_bus(&i2c_mst_config, bus_handle);
    if (err != ESP_OK) {
        free(bus_handle);
        ESP_LOGE(TAG, "Failed to create I2C Master Bus entity.\nError: %s", esp_err_to_name(err));
    }

    return ds3231_init(bus_handle);
}

rtc_handle_t* ds3231_init(i2c_master_bus_handle_t* bus_handle) {
    i2c_device_config_t dev_cfg_read = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = RTC_I2C_ADDRESS,
        .scl_speed_hz = 200000,
    };

    rtc_handle_t* rtc_handle = (rtc_handle_t*)malloc(sizeof(rtc_handle_t));
    if (rtc_handle == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'rtc_handle'");
    }

    esp_err_t err = i2c_master_bus_add_device(*bus_handle, &dev_cfg_read, rtc_handle);
    if (err == ESP_OK) {
        err = i2c_master_probe(*bus_handle, RTC_I2C_ADDRESS, RTC_I2C_TIMEOUT);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to probe RTC over I2C bus.\nError: %s", esp_err_to_name(err));
            free(rtc_handle);
            return NULL;
        }
    }
    return rtc_handle;
}

uint8_t* ds3231_get_registers_raw(rtc_handle_t* rtc_handle) {
    ESP_LOGV(TAG, "Entering 'read_registers' function");
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    uint8_t* data_wr = (uint8_t*)malloc(1);
    if (data_wr == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_wr'");
    }
    data_wr[0] = 0x00;
    // Need to reset to the register address point back to the top (00h)
    esp_err_t err = i2c_master_transmit_receive(*rtc_handle, data_wr, 1, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    free(data_wr);
    // esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error retrieing RTC registers from ds3231.\nError: %s", esp_err_to_name(err));
        free(data_rd);
        return NULL;
    }

    for (int i = 0; i < RTC_DATA_LENGTH; i++) {
        ESP_LOGV(TAG, "Register received with value of: %02Xh - %02Xh - %s", i, data_rd[i], byte_to_binary(data_rd[i]));
    }

    return data_rd;
}

// rtc_registers_t* ds3231_get_registers(rtc_handle_t* rtc_handle) {
//     return (rtc_registers_t*)ds3231_get_registers_raw(rtc_handle);
// }

esp_err_t ds3231_register_set(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register, uint8_t bit_pattern) {
    uint8_t* registers = (uint8_t*)calloc(1, sizeof(uint8_t) * RTC_DATA_LENGTH);
    if (registers == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'registers'");
    }

    esp_err_t err = ds3231_send_byte_raw(rtc_handle, rtc_register, registers[rtc_register] & bit_pattern);
    free(registers);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting RTC control registers from ds3231.\nError: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t ds3231_send_byte_raw(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register, uint8_t rtc_data) {
    const uint8_t data_wr[2] = { rtc_register, rtc_data };
    ESP_LOGD(TAG, "Transmitting %02Xh 0x%02x to DS3231 RTC moodule.", data_wr[0], data_wr[1]);
    esp_err_t err = i2c_master_transmit(*rtc_handle, data_wr, 2, RTC_I2C_TIMEOUT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "An error occured duing write.\n Error: %s", esp_err_to_name(err));
    }
    // Reset the register pointer to 00h
    uint8_t* pointer = ds3231_get_registers_raw(rtc_handle);
    free(pointer);
    return err;
}

esp_err_t ds3231_set_registers_raw(rtc_handle_t* rtc_handle, uint8_t* rtc_data, size_t count) {
    esp_err_t err = i2c_master_transmit(*rtc_handle, rtc_data, count, RTC_I2C_TIMEOUT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "An error occured duing write.\n Error: %s", esp_err_to_name(err));
    }
    // Reset the register pointer 00h
    uint8_t* pointer = ds3231_get_registers_raw(rtc_handle);
    free(pointer);
    return err;
}

esp_err_t ds3231_time_tm_set(rtc_handle_t* rtc_handle, struct tm time) {
    size_t time_length = 0x08;
    uint8_t time_data[time_length];

    time_data[0x00] = RTC_REGISTER_TIME_SECONDS;
    time_data[0x01] = convert_to_bcd(time.tm_sec);
    time_data[0x02] = convert_to_bcd(time.tm_min);
    time_data[0x03] = convert_to_bcd(time.tm_hour);
    time_data[0x04] = convert_to_bcd(time.tm_wday) + 1;                                      // tm is 0 based, DS3231 is 1 based.
    time_data[0x05] = convert_to_bcd(time.tm_mday);
    time_data[0x06] = convert_to_bcd((time.tm_mon) + 1) + ((time.tm_year >= 100) ? 128 : 0); // tm is years since 1900
    time_data[0x07] = convert_to_bcd(time.tm_year >= 100 ? time.tm_year - 100 : time.tm_year);
    esp_err_t err = ds3231_set_registers_raw(rtc_handle, time_data, time_length);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting time from ds3231.\nError: %s", esp_err_to_name(err));
        uint8_t* get_registers_raw = ds3231_get_registers_raw(rtc_handle);
        err =
            ds3231_register_set(rtc_handle, RTC_REGISTER_CONTROL_LOWER, get_registers_raw[RTC_REGISTER_CONTROL_LOWER] & 0b01111111);
        free(get_registers_raw);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error Oscillator Stop flag from ds3231.\nError: %s", esp_err_to_name(err));
        }
    }
    return err;
}

esp_err_t ds3231_time_unix_set(rtc_handle_t* rtc_handle, long unix_time) {
    time_t time = unix_time;
    esp_err_t err = ds3231_time_time_t_set(rtc_handle, time);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting time from ds3231.\nError: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t ds3231_time_time_t_set(rtc_handle_t* rtc_handle, time_t time) {
    struct tm* tm = gmtime(&time);
    esp_err_t err = ds3231_time_tm_set(rtc_handle, *tm);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting time from ds3231.\nError: %s", esp_err_to_name(err));
    }
    return err;
}

int ds3231_set_esp_with_rtc(rtc_handle_t* rtc_handle) {
    struct timeval rtc_time;
    rtc_time.tv_sec = ds3231_time_unix_get(rtc_handle);
    return settimeofday(&rtc_time, NULL);
}

int8_t ds3231_time_minutes_or_seconds_get(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        rtc_seconds_or_minutes_t intermediate = (rtc_seconds_or_minutes_t)data_rd[rtc_register];
        free(data_rd);
        return (intermediate.tens * 10) + intermediate.ones;
    }
    free(data_rd);
    return -1;
}

int8_t ds3231_time_hours_get(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        rtc_hours_t intermediate = (rtc_hours_t)data_rd[rtc_register];
        free(data_rd);
        return (intermediate.ampm_hour_twenties * 20) + (intermediate.tens * 10) + intermediate.ones;
    }
    free(data_rd);
    return -1;
}

int8_t ds3231_time_day_of_week_get(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    rtc_day_date_t intermediate = (rtc_day_date_t)data_rd[rtc_register];
    if (err == ESP_OK) {
        if (rtc_register == RTC_REGISTER_TIME_DAY) {
            free(data_rd);
            return intermediate.day_date;
        } else {
            if (intermediate.dy_dt) {
                free(data_rd);
                return intermediate.day_date;
            }
        }
    }
    free(data_rd);
    return -1;
}

int8_t ds3231_time_date_get(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        rtc_day_date_t intermediate = (rtc_day_date_t)data_rd[rtc_register];
        if (rtc_register == RTC_REGISTER_TIME_DATE) {
            free(data_rd);
            return (intermediate.tens * 10) + intermediate.day_date;
        } else if (!intermediate.dy_dt) {
            free(data_rd);
            return (intermediate.tens * 10) + intermediate.day_date;
        }
    }
    free(data_rd);
    return -1;
}

int8_t ds3231_time_month_get(rtc_handle_t* rtc_handle) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        rtc_month_century_t intermediate = (rtc_month_century_t)data_rd[RTC_REGISTER_TIME_MONTH];
        // Power On/Reset may have the month value at 0x00. While this is expected behavior on the DS3132, the strftime() call will
        // shit itself when doing the conversion and crash the the ESP32 with a LoadStoreError panic. tm.tm_mon is the only member
        // that is not 0 based.
        if (intermediate.ones == 0) {
            intermediate.ones = 1;
        }
        free(data_rd);
        return (intermediate.tens * 10) + intermediate.ones;
    }
    free(data_rd);
    return -1;
}

int16_t ds3231_time_year_get(rtc_handle_t* rtc_handle) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        rtc_years_t intermediate = (rtc_years_t)data_rd[RTC_REGISTER_TIME_YEAR];
        rtc_month_century_t intermediate2 = (rtc_month_century_t)data_rd[RTC_REGISTER_TIME_MONTH];
        free(data_rd);
        return 1900 + (intermediate2.century * 100) + (intermediate.tens * 10) + intermediate.ones;
    }
    free(data_rd);
    return -1;
}

int8_t ds3231_aging_offset_get(rtc_handle_t* rtc_handle) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        rtc_aging_offset_t intermediate = (rtc_aging_offset_t)((int8_t)data_rd[RTC_REGISTER_AGING_OFFSET]);
        free(data_rd);
        return intermediate.offset;
    }
    free(data_rd);
    return -1;
}

float ds3231_temperature_get(rtc_handle_t* rtc_handle) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        rtc_temperature_t intermediate;
        int16_t data = ((int16_t)(((int16_t)data_rd[RTC_REGISTER_TEMPERATURE + 1] << 8) | data_rd[RTC_REGISTER_TEMPERATURE]));
        intermediate.data = data;
        free(data_rd);
        return intermediate.temp + intermediate.quarter_degree * 0.25;
    }
    free(data_rd);
    return -9999;
}

rtc_control_status_t* ds3231_control_status_flags_get(rtc_handle_t* rtc_handle) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        rtc_control_status_t* statusP = (rtc_control_status_t*)calloc(1, sizeof(rtc_control_status_t));
        rtc_control_status_t status =
            (rtc_control_status_t)((uint16_t)(((uint16_t)data_rd[RTC_REGISTER_CONTROL + 1] << 8) | data_rd[RTC_REGISTER_CONTROL]));
        free(data_rd);
        statusP->data = status.data;
        return statusP;
    }
    free(data_rd);
    return NULL;
}

struct tm* ds3231_time_get(rtc_handle_t* rtc_handle) {
    struct tm* rtcTime_s = (struct tm*)calloc(1, sizeof(struct tm));
    if (rtcTime_s == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'rtcTime_s'");
    }
    rtcTime_s->tm_year = ds3231_time_year_get(rtc_handle) - 1900;
    rtcTime_s->tm_mon = ds3231_time_month_get(rtc_handle) - 1;
    rtcTime_s->tm_mday = ds3231_time_date_get(rtc_handle, RTC_REGISTER_TIME_DATE);
    int8_t dow = ds3231_time_day_of_week_get(rtc_handle, RTC_REGISTER_TIME_DAY);
    rtcTime_s->tm_wday = dow == 7 ? 0 : dow;
    rtcTime_s->tm_hour = ds3231_time_hours_get(rtc_handle, RTC_REGISTER_TIME_HOURS);
    rtcTime_s->tm_min = ds3231_time_minutes_or_seconds_get(rtc_handle, RTC_REGISTER_TIME_MINUTES);
    rtcTime_s->tm_sec = ds3231_time_minutes_or_seconds_get(rtc_handle, RTC_REGISTER_TIME_SECONDS);
    return rtcTime_s;
}

time_t ds3231_time_unix_get(rtc_handle_t* rtc_handle) {
    struct tm* rtcTime_s = ds3231_time_get(rtc_handle);
    time_t rtcTime = mktime(rtcTime_s);
    free(rtcTime_s);
    return rtcTime;
}

// TODO: Combine 'ds3231_alarm1_time_get' and 'ds3231_alarm1_time_get'
struct tm* ds3231_alarm1_time_get(rtc_handle_t* rtc_handle) {
    struct tm* rtcTime_s = (struct tm*)calloc(1, sizeof(struct tm));
    if (rtcTime_s == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'rtcTime_s'");
    }

    int8_t date = ds3231_time_date_get(rtc_handle, RTC_REGISTER_ALARM1_DAY_DATE);
    if (date == -1) {
        int8_t dow = ds3231_time_day_of_week_get(rtc_handle, RTC_REGISTER_ALARM1_DAY_DATE);
        rtcTime_s->tm_wday = dow == 7 ? 0 : dow;
        rtcTime_s->tm_mday = -1;
    } else {
        rtcTime_s->tm_mday = date;
        rtcTime_s->tm_wday = -1;
    }
    rtcTime_s->tm_hour = ds3231_time_hours_get(rtc_handle, RTC_REGISTER_ALARM1_HOURS);
    rtcTime_s->tm_min = ds3231_time_minutes_or_seconds_get(rtc_handle, RTC_REGISTER_ALARM1_MINUTES);
    rtcTime_s->tm_sec = ds3231_time_minutes_or_seconds_get(rtc_handle, RTC_REGISTER_ALARM1_SECONDS);
    return rtcTime_s;
}

struct tm* ds3231_alarm2_time_get(rtc_handle_t* rtc_handle) {
    struct tm* rtcTime_s = (struct tm*)calloc(1, sizeof(struct tm));
    if (rtcTime_s == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'rtcTime_s'");
    }

    int8_t date = ds3231_time_date_get(rtc_handle, RTC_REGISTER_ALARM2_DAY_DATE);
    if (date == -1) {
        int8_t dow = ds3231_time_day_of_week_get(rtc_handle, RTC_REGISTER_ALARM2_DAY_DATE);
        rtcTime_s->tm_wday = dow == 7 ? 0 : dow;
        rtcTime_s->tm_mday = -1;
    } else {
        rtcTime_s->tm_mday = date;
        rtcTime_s->tm_wday = -1;
    }
    rtcTime_s->tm_hour = ds3231_time_hours_get(rtc_handle, RTC_REGISTER_ALARM2_HOURS);
    rtcTime_s->tm_min = ds3231_time_minutes_or_seconds_get(rtc_handle, RTC_REGISTER_ALARM2_MINUTES);
    return rtcTime_s;
}

void ds3231_debug_print_data(rtc_handle_t* rtc_handle) {
    // Used to reset the register pointer.
    uint8_t* pointer = ds3231_get_registers_raw(rtc_handle);
    free(pointer);
    ESP_LOGI(TAG, "The Time Seconds value: %i", ds3231_time_minutes_or_seconds_get(rtc_handle, RTC_REGISTER_TIME_SECONDS));
    ESP_LOGI(TAG, "The Time Minutes value: %i", ds3231_time_minutes_or_seconds_get(rtc_handle, RTC_REGISTER_TIME_MINUTES));
    ESP_LOGI(TAG, "The Time Hours value: %i", ds3231_time_minutes_or_seconds_get(rtc_handle, RTC_REGISTER_TIME_HOURS));
    ESP_LOGI(TAG, "The Time Day of Week value: %i", ds3231_time_day_of_week_get(rtc_handle, RTC_REGISTER_TIME_DAY));
    ESP_LOGI(TAG, "The Time Date value: %i", ds3231_time_date_get(rtc_handle, RTC_REGISTER_TIME_DATE));
    ESP_LOGI(TAG, "The Time Month value: %i", ds3231_time_month_get(rtc_handle));
    ESP_LOGI(TAG, "The Time Year value: %i", ds3231_time_year_get(rtc_handle));
    ESP_LOGI(TAG, "The Aging Offset value: %i", ds3231_aging_offset_get(rtc_handle));
    ESP_LOGI(TAG, "The Tamperature value: %.2f", ds3231_temperature_get(rtc_handle));

    // rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);

    ESP_LOGI(TAG, "The Enable Oscillator flag is set to: %s", ds3231_enable_oscillator_flag_get(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Batter Backed Square Ware flag is set to: %s",
             ds3231_battery_backed_square_wave_flag_get(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Convert Temperature flag is set to: %s", ds3231_convert_temp_flag_get(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Square Wave Freqency value is set to: %i", ds3231_square_wave_freq_get(rtc_handle));
    ESP_LOGI(TAG, "The Interrupt/SQW flag is set to: %s",
             ds3231_interrupt_square_wave_control_flag_get(rtc_handle) ? "Interrupt" : "Square Wave");
    ESP_LOGI(TAG, "The Alarm 2 Rate flag is set to: %d", ds3231_alarm2_rate_get(rtc_handle));
    ESP_LOGI(TAG, "The Alarm 2 Enable flag is set to: %s", ds3231_alarm2_enable_flag_get(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Alarm 1 Rate flag is set to: %d", ds3231_alarm1_rate_get(rtc_handle));
    ESP_LOGI(TAG, "The Alarm 1 Enable flag is set to: %s", ds3231_alarm1_enable_flag_get(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Oscillator Stop flag is set to: %s", ds3231_get_oscillator_stop_flag(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Enbale 32khz flag is set to: %s", ds3231_32kHz_out_enable_flag_get(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Busy flag is set to: %s", ds3231_busy_flag_get(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Alarm 2 Fire flag is set to: %s", ds3231_alarm2_fired_flag_get(rtc_handle) ? "True" : "False");
    ESP_LOGI(TAG, "The Alarm 1 Fire flag is set to: %s", ds3231_alarm1_fired_flag_get(rtc_handle) ? "True" : "False");
    char strftime_buf[64];
    struct tm* current_time = ds3231_time_get(rtc_handle);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", current_time);
    free(current_time);
    ESP_LOGI(TAG, "The current UTC date/time is: %s", strftime_buf);
    ESP_LOGI(TAG, "The Unix time value: %lli", ds3231_time_unix_get(rtc_handle));
    struct tm* alarm1_time = ds3231_alarm1_time_get(rtc_handle);
    ESP_LOGI(TAG, "Alarm 1 is set to - Day of week: %d \t Day of Month: %d \t Hour: %d \t Minutes: %d \t Seconds: %d",
             alarm1_time->tm_wday, alarm1_time->tm_mday, alarm1_time->tm_hour, alarm1_time->tm_min, alarm1_time->tm_sec);
    free(alarm1_time);
    struct tm* alarm2_time = ds3231_alarm2_time_get(rtc_handle);
    ESP_LOGI(TAG, "Alarm 2 is set to - Day of week: %d \t Day of Month: %d \t Hour: %d \t Minutes: %d", alarm2_time->tm_wday,
             alarm2_time->tm_mday, alarm2_time->tm_hour, alarm2_time->tm_min);
    free(alarm2_time);
}

esp_err_t set_alarm(rtc_handle_t* rtc_handle, bool isAlarm1, int8_t dom, int8_t dow, int8_t hr, int8_t min, int8_t sec) {
    size_t alarm_length;
    int8_t alarm_register;
    if (isAlarm1) {
        alarm_length = 0x05;
        alarm_register = RTC_REGISTER_ALARM1_SECONDS;
    } else {
        alarm_length = 0x04;
        alarm_register = RTC_REGISTER_ALARM2_MINUTES;
    }

    uint8_t alarm_data[alarm_length];
    int8_t counter = 0x00;

    alarm_data[counter++] = alarm_register;
    if (isAlarm1) {
        alarm_data[counter++] = convert_to_bcd(sec);
    }
    alarm_data[counter++] = convert_to_bcd(min);
    alarm_data[counter++] = convert_to_bcd(hr);
    if (dom == -1) {
        alarm_data[counter++] = convert_to_bcd(dow) | 0b01000000;
    } else {
        alarm_data[counter++] = convert_to_bcd(dom) & 0b10111111;
    }
    esp_err_t err = ds3231_set_registers_raw(rtc_handle, alarm_data, alarm_length);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting alarm 1 dow from ds3231.\nError: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t ds3231_alarm1_day_of_week_set(rtc_handle_t* rtc_handle, enum rtc_day_of_week_e dow, int8_t hour, int8_t minutes,
                                        int8_t seconds) {
    esp_err_t err = set_alarm(rtc_handle, true, -1, dow, hour, minutes, seconds);
    return err;
}

esp_err_t ds3231_alarm1_day_of_month_set(rtc_handle_t* rtc_handle, int8_t day, int8_t hour, int8_t minutes, int8_t seconds) {
    esp_err_t err = set_alarm(rtc_handle, true, day, -1, hour, minutes, seconds);
    return err;
}

esp_err_t ds3231_alarm2_day_of_week_set(rtc_handle_t* rtc_handle, enum rtc_day_of_week_e dow, int8_t hour, int8_t minutes) {
    esp_err_t err = set_alarm(rtc_handle, false, -1, dow, hour, minutes, -1);
    return err;
}

esp_err_t ds3231_alarm2_day_of_month_set(rtc_handle_t* rtc_handle, int8_t day, int8_t hour, int8_t minutes) {
    esp_err_t err = set_alarm(rtc_handle, false, day, -1, hour, minutes, -1);
    return err;
}

static const char* byte_to_binary(int x) {
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

esp_err_t set_flag(rtc_handle_t* rtc_handle, bool setEnabled, bool isUpper, enum flag_bitmask_e bitmask) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }

    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        size_t data_length = 2;
        uint8_t data_wr[2];
        uint8_t control_register;
        if (isUpper) {
            control_register = RTC_REGISTER_CONTROL_UPPER;
        } else {
            control_register = RTC_REGISTER_CONTROL_LOWER;
        }
        uint8_t csr = data_rd[control_register];
        free(data_rd);
        data_wr[0x00] = control_register;

        if ((isUpper) & (bitmask == RTC_BITMASK_OSCILLATOR_ENABLE)) {
            if (setEnabled) {
                data_wr[0x01] = csr & ~bitmask;
            } else {
                data_wr[0x01] = csr | bitmask;
            }
        } else {
            if (setEnabled) {
                data_wr[0x01] = csr | bitmask;
            } else {
                data_wr[0x01] = csr & ~bitmask;
            }
        }

        err = ds3231_set_registers_raw(rtc_handle, data_wr, data_length);
        uint8_t reset_data[1];
        reset_data[0] = 0x00;
        err = ds3231_set_registers_raw(rtc_handle, reset_data, 1);
    }
    return err;
}

bool ds3231_enable_oscillator_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    return (csr->enable_oscillator);
}

esp_err_t ds3231_enable_oscillator_flag_set(rtc_handle_t* rtc_handle, bool isEnabled) {
    return set_flag(rtc_handle, !isEnabled, true, RTC_BITMASK_OSCILLATOR_ENABLE);
}

bool ds3231_battery_backed_square_wave_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    return csr->battery_square;
}

esp_err_t ds3231_battery_backed_square_wave_flag_set(rtc_handle_t* rtc_handle, bool isEnabled) {
    return set_flag(rtc_handle, isEnabled, true, RTC_BITMASK_BATTERY_BACKED);
}

bool ds3231_convert_temp_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    return csr->convert_temp;
}

esp_err_t ds3231_convert_temp_flag_set(rtc_handle_t* rtc_handle, bool isEnabled) {
    return set_flag(rtc_handle, isEnabled, true, RTC_BITMASK_CONVERT_TEMPERATURE);
}

enum rtc_square_wave_freq_e ds3231_square_wave_freq_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    return csr->square_freq;
}

// TODO I think this could be better. Works for now.
esp_err_t ds3231_square_wave_freq_set(rtc_handle_t* rtc_handle, enum rtc_square_wave_freq_e frequency) {
    typedef union {
        uint8_t data;
        struct {
            unsigned d1          : 3;
            unsigned square_freq : 2;
            unsigned d2          : 3;
        };
    } square_t;

    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    if (err == ESP_OK) {
        size_t data_length = 2;
        uint8_t data_wr[2];
        uint8_t control_register;
        control_register = RTC_REGISTER_CONTROL_UPPER;
        square_t csr;
        csr.data = data_rd[control_register];
        free(data_rd);
        csr.square_freq = frequency;

        data_wr[0x00] = control_register;
        data_wr[0x01] = csr.data;

        err = ds3231_set_registers_raw(rtc_handle, data_wr, data_length);
    }
    return err;
}

enum rtc_intr_sqr_e ds3231_interrupt_square_wave_control_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    bool result = csr->interrup_control;
    free(csr);
    return result;
}

esp_err_t ds3231_interrupt_square_wave_control_flag_set(rtc_handle_t* rtc_handle, enum rtc_intr_sqr_e isEnabled) {
    return set_flag(rtc_handle, isEnabled, true, RTC_BITMASK_INTERRUPT_CONTROL);
}

bool ds3231_alarm2_enable_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    bool result = csr->alarm2_enable;
    free(csr);
    return result;
}

esp_err_t ds3231_alarm2_enable_flag_set(rtc_handle_t* rtc_handle, bool isEnabled) {
    return set_flag(rtc_handle, isEnabled, true, RTC_BITMASK_ALARM2_ENABLE_FIRED);
}

bool ds3231_alarm1_enable_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    bool result = csr->alarm1_enable;
    free(csr);
    return result;
}

esp_err_t ds3231_alarm1_enable_flag_set(rtc_handle_t* rtc_handle, bool isEnabled) {
    return set_flag(rtc_handle, isEnabled, true, RTC_BITMASK_ALARM1_ENABLE_FIRED);
}

bool ds3231_get_oscillator_stop_flag(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    bool result = csr->oscillaror_stop_flag;
    free(csr);
    return result;
}

esp_err_t ds3231_oscillator_stop_flag_reset(rtc_handle_t* rtc_handle) {
    return set_flag(rtc_handle, false, false, RTC_BITMASK_OSCILLATOR_STOP);
}

bool ds3231_32kHz_out_enable_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    bool result = csr->enable_32kHz_out;
    free(csr);
    return result;
}

esp_err_t ds3231_32kHz_out_enable_flag_set(rtc_handle_t* rtc_handle, bool isEnabled) {
    return set_flag(rtc_handle, isEnabled, false, RTC_BITMASK_32KHZ_ENABLE);
}

bool ds3231_busy_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    bool result = csr->busy_flag;
    free(csr);
    return result;
}

bool ds3231_alarm2_fired_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    bool result = csr->alarm2_flag;
    free(csr);
    return result;
}

esp_err_t ds3231_alarm2_fired_flag_reset(rtc_handle_t* rtc_handle) {
    return set_flag(rtc_handle, false, false, RTC_BITMASK_ALARM2_ENABLE_FIRED);
}

bool ds3231_alarm1_fired_flag_get(rtc_handle_t* rtc_handle) {
    rtc_control_status_t* csr = ds3231_control_status_flags_get(rtc_handle);
    bool result = csr->alarm1_flag;
    free(csr);
    return result;
}

esp_err_t ds3231_alarm1_fired_flag_reset(rtc_handle_t* rtc_handle) {
    return set_flag(rtc_handle, false, false, RTC_BITMASK_ALARM1_ENABLE_FIRED);
}

uint8_t convert_to_bcd(uint8_t value) {
    uint8_t tens;
    uint8_t ones;

    tens = (uint8_t)((value / 10) << 4);
    ones = (uint8_t)(value % 10);
    return tens | ones;
}

enum rtc_alarm_rate_e get_alarm_rate(rtc_handle_t* rtc_handle, uint8_t alarm_number) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    uint8_t alarm_register = RTC_REGISTER_ALARM1_SECONDS;
    if (err == ESP_OK) {
        uint8_t temp_register_value = 0b00000000;
        if (alarm_number == 1) {
            // The type used does not really matter, as long it contains 'unsigned alarm_rate : 1' for the MSB
            rtc_day_date_t intermediate = (rtc_day_date_t)data_rd[alarm_register++];
            temp_register_value = temp_register_value + (int8_t)pow(2, 0) * intermediate.alarm_rate;
        } else {
            alarm_register = alarm_register + 4;
        }
        for (int i = 1; i < 4; i++) {
            rtc_day_date_t intermediate = (rtc_day_date_t)data_rd[alarm_register++];
            temp_register_value = temp_register_value + (int8_t)pow(2, i) * intermediate.alarm_rate;
        }
        free(data_rd);
        return temp_register_value;
    }
    free(data_rd);
    return RTC_ALARM_MATCH_INVALID;
}

esp_err_t set_alarm_rate(rtc_handle_t* rtc_handle, uint8_t alarm_number, enum rtc_alarm_rate_e alarm_rate) {
    uint8_t* data_rd = (uint8_t*)malloc(RTC_DATA_LENGTH);
    if (data_rd == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'data_rd'");
    }
    esp_err_t err = i2c_master_receive(*rtc_handle, data_rd, RTC_DATA_LENGTH, RTC_I2C_TIMEOUT);
    uint8_t alarm_register = RTC_REGISTER_ALARM1_SECONDS;
    if (err == ESP_OK) {
        if (alarm_number == 1) {
            // The type used does not really matter, as long it contains 'unsigned alarm_rate : 1' for the MSB
            rtc_day_date_t intermediate = (rtc_day_date_t)data_rd[alarm_register];
            intermediate.alarm_rate = alarm_rate & 0b00000001;
            ds3231_send_byte_raw(rtc_handle, alarm_register++, intermediate.data);
        } else {
            alarm_register = alarm_register + 4;
        }

        for (int i = 1; i < 4; i++) {
            rtc_day_date_t intermediate = (rtc_day_date_t)data_rd[alarm_register];
            intermediate.alarm_rate = (alarm_rate >> i) & 0b00000001;
            ds3231_send_byte_raw(rtc_handle, alarm_register++, intermediate.data);
        }
    }
    free(data_rd);
    return err;
}

esp_err_t ds3231_alarm1_rate_set(rtc_handle_t* rtc_handle, enum rtc_alarm_rate_e alarm_rate) {
    return set_alarm_rate(rtc_handle, 1, alarm_rate);
}

esp_err_t ds3231_alarm2_rate_set(rtc_handle_t* rtc_handle, enum rtc_alarm_rate_e alarm_rate) {
    return set_alarm_rate(rtc_handle, 2, alarm_rate);
}

enum rtc_alarm_rate_e ds3231_alarm1_rate_get(rtc_handle_t* rtc_handle) {
    return get_alarm_rate(rtc_handle, 1);
}

enum rtc_alarm_rate_e ds3231_alarm2_rate_get(rtc_handle_t* rtc_handle) {
    return get_alarm_rate(rtc_handle, 2);
}

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    xQueueSendFromISR(alarm_event_queue, arg, NULL);
}

static void gpio_task_example(void* arg) {
    rtc_alarm_isr_t* isr = arg;
    while (true) {
        if (xQueueReceive(alarm_event_queue, NULL, portMAX_DELAY)) {
            // printf("GPIO %d was pressed. The state is %d\n", 6, gpio_get_level(6));
            // ESP_LOGI(TAG, "ISR Addr: %lu", (uint32_t)isr->isr_handler);
            void (*user_isr)(void*);
            user_isr = isr->isr_handler;
            user_isr(isr->params);
            if (ds3231_alarm1_fired_flag_get(isr->rtc_handle)) {
                ESP_LOGD(TAG, "Resetting Alarm 1 Flag");
                ds3231_alarm1_fired_flag_reset(isr->rtc_handle);
            } else if (ds3231_alarm2_fired_flag_get(isr->rtc_handle)) {
                ESP_LOGD(TAG, "Resetting Alarm 2 Flag");
                ds3231_alarm2_fired_flag_reset(isr->rtc_handle);
            } else {
                ESP_LOGW(TAG,
                         "No alarm flags have been set, this not an alarm\n****** This is likely due to having interrupts turned "
                         "off on the DS3231 and an active ISR on the ESP32 ******");
            }
        }
    }
    free(isr);
}

esp_err_t ds3231_alarm_isr_delete(rtc_handle_t* rtc_handle, gpio_num_t INT) {
    vTaskDelete(isr_task_handle);
    return gpio_isr_handler_remove(INT);
}
esp_err_t ds3231_alarm_isr_create(rtc_handle_t* rtc_handle, gpio_num_t INT, gpio_isr_t isr, void* params) {
    // Configure INT pin on ESP

    gpio_config_t interrupt_pin_enable = { .pull_down_en = 0,
                                           .pull_up_en = 1,
                                           .intr_type = GPIO_INTR_NEGEDGE,
                                           .mode = GPIO_MODE_INPUT,
                                           .pin_bit_mask = (uint64_t)1 << INT };
    gpio_config(&interrupt_pin_enable);

    // alarm_event_queue = xQueueCreate(10, sizeof(rtc_alarm_isr_t*));
    alarm_event_queue = xQueueCreate(10, 0);
    if (alarm_event_queue == NULL) {
        ESP_LOGE("DANGER", "Danger Will Robinson. DANGER!");
    }

    rtc_alarm_isr_t* isr_params = (rtc_alarm_isr_t*)malloc(sizeof(rtc_alarm_isr_t));
    if (isr_params == NULL) {
        esp_backtrace_print(6);
        esp_system_abort("Fatal Error. Unable to allocate memory for 'isr_params'");
    }

    isr_params->rtc_handle = rtc_handle;
    isr_params->isr_handler = isr;
    isr_params->params = params;

    // TODO Adjust the stack size
    BaseType_t xReturned = xTaskCreate(gpio_task_example, "gpio_task_example", 4096, isr_params, 10, &isr_task_handle);
    if (xReturned != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate failed to create ISR task.");
        return ESP_FAIL;
    }

    gpio_install_isr_service(1);
    esp_err_t err = gpio_isr_handler_add(INT, gpio_isr_handler, NULL);

    return err;
}

void ds3231_debug_test_set(rtc_handle_t* rtc_handle) {
    ESP_LOGI(TAG, "The Batter Backed Square Ware flag is set to: %s",
             ds3231_battery_backed_square_wave_flag_get(rtc_handle) ? "True" : "False");
    ds3231_battery_backed_square_wave_flag_set(rtc_handle, true);
    ESP_LOGI(TAG, "The Batter Backed Square Ware flag is set to: %s",
             ds3231_battery_backed_square_wave_flag_get(rtc_handle) ? "True" : "False");
    ds3231_battery_backed_square_wave_flag_set(rtc_handle, false);
    ESP_LOGI(TAG, "The Batter Backed Square Ware flag is set to: %s",
             ds3231_battery_backed_square_wave_flag_get(rtc_handle) ? "True" : "False");

    ESP_LOGI(TAG, "The Convert Temperature flag is set to: %s", ds3231_convert_temp_flag_get(rtc_handle) ? "True" : "False");
    ds3231_convert_temp_flag_set(rtc_handle, true);
    ESP_LOGI(TAG, "The Convert Temperature flag is set to: %s", ds3231_convert_temp_flag_get(rtc_handle) ? "True" : "False");

    ESP_LOGI(TAG, "The Square Wave Freqency value is set to: %i", ds3231_square_wave_freq_get(rtc_handle));
    ds3231_square_wave_freq_set(rtc_handle, RTC_SQUARE_WAVE_FREQ_1024HZ);
    ESP_LOGI(TAG, "The Square Wave Freqency value is set to: %i", ds3231_square_wave_freq_get(rtc_handle));
    ds3231_square_wave_freq_set(rtc_handle, RTC_SQUARE_WAVE_FREQ_8192HZ);
    ESP_LOGI(TAG, "The Square Wave Freqency value is set to: %i", ds3231_square_wave_freq_get(rtc_handle));

    ESP_LOGI(TAG, "The Interrupt/SQW flag is set to: %s",
             ds3231_interrupt_square_wave_control_flag_get(rtc_handle) ? "True" : "False");
    ds3231_interrupt_square_wave_control_flag_set(rtc_handle, false);
    ESP_LOGI(TAG, "The Interrupt/SQW flag is set to: %s",
             ds3231_interrupt_square_wave_control_flag_get(rtc_handle) ? "True" : "False");
    ds3231_interrupt_square_wave_control_flag_set(rtc_handle, true);
    ESP_LOGI(TAG, "The Interrupt/SQW flag is set to: %s",
             ds3231_interrupt_square_wave_control_flag_get(rtc_handle) ? "True" : "False");

    ESP_LOGI(TAG, "The Alarm 2 Enable flag is set to: %s", ds3231_alarm2_enable_flag_get(rtc_handle) ? "True" : "False");
    ds3231_alarm2_enable_flag_set(rtc_handle, true);
    ESP_LOGI(TAG, "The Alarm 2 Enable flag is set to: %s", ds3231_alarm2_enable_flag_get(rtc_handle) ? "True" : "False");
    ds3231_alarm2_enable_flag_set(rtc_handle, false);
    ESP_LOGI(TAG, "The Alarm 2 Enable flag is set to: %s", ds3231_alarm2_enable_flag_get(rtc_handle) ? "True" : "False");

    ESP_LOGI(TAG, "The Alarm 1 Enable flag is set to: %s", ds3231_alarm1_enable_flag_get(rtc_handle) ? "True" : "False");
    ds3231_alarm1_enable_flag_set(rtc_handle, true);
    ESP_LOGI(TAG, "The Alarm 1 Enable flag is set to: %s", ds3231_alarm1_enable_flag_get(rtc_handle) ? "True" : "False");
    ds3231_alarm1_enable_flag_set(rtc_handle, false);
    ESP_LOGI(TAG, "The Alarm 1 Enable flag is set to: %s", ds3231_alarm1_enable_flag_get(rtc_handle) ? "True" : "False");

    ESP_LOGI(TAG, "The Oscillator Stop flag is set to: %s", ds3231_get_oscillator_stop_flag(rtc_handle) ? "True" : "False");
    // Oscillator Stop Flag may only be set to 0
    ds3231_oscillator_stop_flag_reset(rtc_handle);
    ESP_LOGI(TAG, "The Oscillator Stop flag is set to: %s", ds3231_get_oscillator_stop_flag(rtc_handle) ? "True" : "False");

    ESP_LOGI(TAG, "The Enbale 32khz flag is set to: %s", ds3231_32kHz_out_enable_flag_get(rtc_handle) ? "True" : "False");
    ds3231_32kHz_out_enable_flag_set(rtc_handle, false);
    ESP_LOGI(TAG, "The Enbale 32khz flag is set to: %s", ds3231_32kHz_out_enable_flag_get(rtc_handle) ? "True" : "False");
    ds3231_32kHz_out_enable_flag_set(rtc_handle, true);
    ESP_LOGI(TAG, "The Enbale 32khz flag is set to: %s", ds3231_32kHz_out_enable_flag_get(rtc_handle) ? "True" : "False");

    ESP_LOGI(TAG, "The Enable Oscillator flag is set to: %s", ds3231_enable_oscillator_flag_get(rtc_handle) ? "True" : "False");
    ds3231_enable_oscillator_flag_set(rtc_handle, false);
    ESP_LOGI(TAG, "The Enable Oscillator flag is set to: %s", ds3231_enable_oscillator_flag_get(rtc_handle) ? "True" : "False");
    ds3231_enable_oscillator_flag_set(rtc_handle, true);
    ESP_LOGI(TAG, "The Enable Oscillator flag is set to: %s", ds3231_enable_oscillator_flag_get(rtc_handle) ? "True" : "False");
}
