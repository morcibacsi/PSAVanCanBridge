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

#pragma once

#include <sys/time.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_I2C_TIMEOUT                                                                                                            \
    1000 /*!< @brief Timeout of I2C commands in ms. Defaul is 1000. Redefine as needed. For no timeout use -1                      \
          */

/**
 * @brief Enumeration of all the register values of the DS3231 RTC chip
 *
 */
enum rtc_register_e {
    RTC_REGISTER_TIME_SECONDS = 0x00,    /*!< Time - Seconds register */
    RTC_REGISTER_TIME_MINUTES = 0x01,    /*!< Time - Minutes register */
    RTC_REGISTER_TIME_HOURS = 0x02,      /*!< Time - Hours register */
    RTC_REGISTER_TIME_DAY = 0x03,        /*!< Time - Days (Day of Week) register */
    RTC_REGISTER_TIME_DATE = 0x04,       /*!< Time - Date (Day of Month register */
    RTC_REGISTER_TIME_MONTH = 0x05,      /*!< Time - Month register */
    RTC_REGISTER_TIME_YEAR = 0x06,       /*!< Time - Year register */
    RTC_REGISTER_ALARM1_SECONDS = 0x07,  /*!< Alarm 1 - Seconds register */
    RTC_REGISTER_ALARM1_MINUTES = 0x08,  /*!< Alarm 1 - Minutes register */
    RTC_REGISTER_ALARM1_HOURS = 0x09,    /*!< Alarm 1 - Hours register */
    RTC_REGISTER_ALARM1_DAY_DATE = 0x0A, /*!< Alarm 1 - Day or Date register */
    RTC_REGISTER_ALARM2_MINUTES = 0x0B,  /*!< Alarm 1 - Minutes register */
    RTC_REGISTER_ALARM2_HOURS = 0x0C,    /*!< Alarm 1 - Hours register */
    RTC_REGISTER_ALARM2_DAY_DATE = 0x0D, /*!< Alarm 1 - Day or Date register */
    RTC_REGISTER_CONTROL = 0x0E,         /*!< Control & Status registers */
    RTC_REGISTER_CONTROL_UPPER = 0x0E,   /*!< Upper Control & Status registers */
    RTC_REGISTER_CONTROL_LOWER = 0x0F,   /*!< Lower Control & Status registers */
    RTC_REGISTER_AGING_OFFSET = 0x10,    /*!< Aging Offset register */
    RTC_REGISTER_TEMPERATURE = 0x11      /*!< Temperature registers */
};

/**
 * @brief Enumeration of all the Days of the Week vaules as used by the DS3231 chip.
 *
 * @note The Day of Week values used by \b DS3231 and \b struct \b tm are different. In the DS3231, the values are 1 - 7 and
 * correspons to Monday to Sunday. In the struct tm, the values are 0 - 6 and corresponds to Sunday - Saturday. The
 ds3231_time_get function accounts for this. The ds3231_time_day_of_week_get does not.
 *
 */
enum rtc_day_of_week_e {
    RTC_MONDAY = 1,
    RTC_TUESDAY = 2,
    RTC_WEDNESDAY = 3,
    RTC_THURSDAY = 4,
    RTC_FRIDAY = 5,
    RTC_SATURDAY = 6,
    RTC_SUNDAY = 7
};

/**
 * @brief Enumeration of all the Days of the Week vaules as used by the POSIX time struct.
 *
 * @note The Day of Week values used by \b DS3231 and \b struct \b tm are different. In the DS3231, the values are 1 - 7 and
 * correspons to Monday to Sunday. In the struct tm, the values are 0 - 6 and corresponds to Sunday - Saturday. The
 ds3231_time_get function accounts for this. The ds3231_time_day_of_week_get does not.
 *
 */
enum tm_day_of_week_e {
    TM_SUNDAY = 0,
    TM_MONDAY = 1,
    TM_TUESDAY = 2,
    TM_WEDNESDAY = 3,
    TM_THURSDAY = 4,
    TM_FRIDAY = 5,
    TM_SATURDAY = 6
};

/**
 * @brief Enumation of the four frequency values availed for output as a square wave on the DS3231
 *
 */
enum rtc_square_wave_freq_e {
    RTC_SQUARE_WAVE_FREQ_1000HZ = 0,
    RTC_SQUARE_WAVE_FREQ_1024HZ = 1,
    RTC_SQUARE_WAVE_FREQ_4096HZ = 2,
    RTC_SQUARE_WAVE_FREQ_8192HZ = 3
};

/**
 * @brief The alarm functionality of the DS3231 is veritile in it's functionaliity.
 *
 */
enum rtc_alarm_rate_e {
    RTC_ALARM_MATCH_EVERY_SECOND = 15,                   /*!< \b Alarm \b 1 \b Only - The alarm will fire every second. */
    RTC_ALARM_MATCH_SECONDS_A1_OR_EVERY_MINUTES_A2 = 14, /*!< \b Alarm \b 1 \b Only - The alarm will fire when the seconds value in
                                                            the alarm matches the seconds value of the DS3231.
                                                            \b Alarm \b 2 \b Only - The alarm will fire every minute when the
                                                            seconds value on the DS3231 hits 00.*/
    RTC_ALARM_MATCH_MINUTES = 12, /*!< The alarm will fire every hour when the minutes (and seconds for Alarm 1) value matches the
                                     the DS3231 minutes (and seconds for Alarm 1) value */
    RTC_ALARM_MATCH_HOURS = 8, /*!< The alarm will fire every day when the hour, minutes (and seconds for Alarm 1) value matches the
                                  the DS3231 hour, minutes (and seconds for Alarm 1) value  */
    RTC_ALARM_MATCH_DAY_DATE = 0,  /*!< The alarm will fire every day when the hour, minutes (and seconds for Alarm 1) value matches
                                      the the DS3231 hour, minutes (and seconds for Alarm 1) value */
    RTC_ALARM_MATCH_INVALID = -999 /*!<  */
};

enum rtc_intr_sqr_e { square = false, interrupts = true };

#pragma pack(push, 1)

/**
 * @brief Union of the 2 control and status registers  (0Eh and 0Fh) of the * DS3231.  All (flags are active if 1.)
 */
typedef union rtc_control_status_t {
    uint16_t data;                      /*!< Native type representation of the data. */
    struct {
        unsigned alarm1_enable     : 1; /*!< Alarm 1 Enable. */
        unsigned alarm2_enable     : 1; /*!< Alarm 2 Enable. */
        unsigned interrup_control  : 1; /*!< Interrupt/Square Wave Control. Square Wave is active when set to 1. See the DS3231 data
                                           sheet for more details. */
        // clang-format off
        unsigned square_freq : 2; /*!< Square wave frequncy control register (2 bits)
                                    |  BIT 1  |  BIT 0  | OUTPUT FREQ |
                                    | :-----: | :-----: | :---------: |
                                    |    0    |    0    | 1.000khZ    |
                                    |    0    |    1    | 1.024KhZ    |
                                    |    1    |    0    | 4.096KhZ    |
                                    |    1    |    1    | 8.192KhZ    | */
        // clang-format on

        unsigned convert_temp      : 1; /*!< Force a tempature conversion. See the DS3231 data sheet for more details */
        unsigned battery_square    : 1; /*!< Run the square wave when on battery.  See the DS3231 data sheet for more details */
        unsigned enable_oscillator : 1; /*!< Active 0 - Whien inactive, all register are read only */
        unsigned alarm1_flag      : 1; /*!< Flag indicating Alarm 1 triggered. If interrupts are enabled, one will be triggered/  */
        unsigned alarm2_flag      : 1; /*!< Flag indicating Alarm 2 triggered. If interrupts are enabled, one will be triggered/  */
        unsigned busy_flag        : 1; /*!< Temperature conversion is processing */
        unsigned enable_32kHz_out : 1; /*!< Enables output of 32.765kHZ square wave. Power on default is active */
        /** @cond */
        unsigned unused           : 3;
        /** @endcond */
        unsigned oscillaror_stop_flag : 1; /*!< Indicates the oscillator is stopped. Seethe DS3231 data sheet for more details. */
    };
} rtc_control_status_t;

/**
 * @brief Simple alias of i2c_master_dev_handle.
 *
 */
typedef i2c_master_dev_handle_t rtc_handle_t;

#pragma pack(pop)

/**
 * @brief Initialize the I2C and the DS3231 RTC Module.
 * @param SCL [in] GPIO_NUM_X to use as I2C Clock.
 * @param SDA [in] GPIO_NUM_X to use as I2C Data.
 * @retval rtc_handle_t: Handle required for subesequent operations.
 * @retval NULL: The handle creation failed. Check serial monitor for ERROR condition.
 */
rtc_handle_t* ds3231_init_full(gpio_num_t SCL, gpio_num_t SDA);

/**
 * @brief Initialize the DS3231 RTC Module with I2C info.
 * @param *bus_handle [in] The i2c_master_bus_handle_t from i2c_new_master_bus call.
 * @retval rtc_handle_t: Handle required for subesequent operations.
 * @retval NULL: The handle creation failed. Check serial monitor for ERROR condition.
 */
rtc_handle_t* ds3231_init(i2c_master_bus_handle_t* bus_handle);

/**
 * @brief This is a testing function. It will real all registers and display their data
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 */
void ds3231_debug_print_data(rtc_handle_t* rtc_handle);

/**
 * @brief Queries the DS3231 for the contents of all registers (00h - 12h) as raw data.
 * @note The returned value must be freed by the caller to prevent memory leakage
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b uint8_t* - An array containing all of the register data.
 */
uint8_t* ds3231_get_registers_raw(rtc_handle_t* rtc_handle);

/**
 * @brief Sends sinlge unsinged 8-bit byte to a specific DS3231 register
 * @warning This is a potentially dangerous command. ANY data can be sent to ANY register. There is no checking performed.
 * @warning The inclustion of this function is to allow low-level access that the standard \a set_x and \a get_x function do not
 * provide.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param rtc_register [in] The register to be written to.
 * @param rtc_data [in] The data to be written to rtc_tregister.
 * @return @b esp_err_t - cascaded from i2c_master_transmit
 *      - \b ESP_OK: I2C master transmit-receive success
 *      - \b ESP_ERR_INVALID_ARG: I2C master transmit parameter invalid.
 *      - \b ESP_ERR_TIMEOUT: Operation timeout(larger than xfer_timeout_ms) because the bus is busy or hardware crash.
 */
esp_err_t ds3231_send_byte_raw(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register, uint8_t rtc_data);

/**
 * @brief Queries the DS3231 for the contents the minutes or seconds rgisters. Works on both Time and Alarm registers.
 * @note Alarm2 has no seconds register.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 *  * @param rtc_register [in] rtc_register_e The minutes/seconds register for Time, Alarm 1, Alarm 2

 * @return \b uint8_t - Single byte representing Base-10 minutes or seconds value.
 *      - Values: 0-59
 *      - Leap seconds are \b NOT accounted format
 */
int8_t ds3231_time_minutes_or_seconds_get(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register);

/**
 * @brief Queries the DS3231 for the contents the hours rgister. Works on both Time and Alarm registers.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param rtc_register [in] rtc_register_e The hours register for Time, Alarm 1, Alarm 2
 * @return \b uint8_t - Single byte representing Base-10 hours.
 *      - Values: 0-23.
 *      - Always in 24 hour format.
 */
int8_t ds3231_time_hours_get(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register);

/**
 * @brief Queries the DS3231 for the contents the day of week rgister. Works on both Time and Alarm registers.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param rtc_register [in] rtc_register_e The day of week register for Time, Alarm 1, Alarm 2
 * @return \b uint8_t - Single byte representing the Day of Week.
 *      - Values 1 - 7
 *      - Day counting starts on Monday (1 == Monday)
 */
int8_t ds3231_time_day_of_week_get(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register);

/**
 * @brief Queries the DS3231 for the contents the day of month rgister. Works on both Time and Alarm registers.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param rtc_register [in] rtc_register_e The day of month register for Time, Alarm 1, Alarm 2
 * @return \b uint8_t - Single byte representing the Day of Month.
 *      - Values 1 - 31
 */
int8_t ds3231_time_date_get(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register);

/**
 * @brief Queries the DS3231 for the contents the Month rgister. Works only on Time.
 * @note Neither Alarm 1 nor Alarm 2 have a Month register.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b uint8_t - Single byte representing the Month.
 *      - Values 1 - 12
 */
int8_t ds3231_time_month_get(rtc_handle_t* rtc_handle);

/**
 * @brief Queries the DS3231 for the contents the Year rgister. Works only on Time.
 * @note Neither Alarm 1 nor Alarm 2 have a Year register.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b uint16_t - Two byte representing the Year.
 *      - Values 1900 - 2099
 */
int16_t ds3231_time_year_get(rtc_handle_t* rtc_handle);

/**
 * @brief Queries the DS3231 for the contents the Aging Offset register.
 * @note Please see the DS3231 Data Sheet for specifics of the Aging Offset
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b int8_t - Signed single byte representing the aging offset.
 */
int8_t ds3231_aging_offset_get(rtc_handle_t* rtc_handle);

/**
 * @brief Queries the DS3231 for the contents the Temperature registers.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b float - Float representing the the temperature in 0.25° C increments.
 */
float ds3231_temperature_get(rtc_handle_t* rtc_handle);

/**
 * @brief Queries the DS3231 for the contents the Contol and Status registers.
 * @note Please see the DS3231 Data Sheet for full descriptions of all of the control and status values.
 * @note The returned value must be freed by the caller to prevent memory leakage
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b rtc_control_status_t* - Pointer to stuct containing all of the control and status register values.
 */
rtc_control_status_t* ds3231_control_status_flags_get(rtc_handle_t* rtc_handle);

/**
 * @brief Queries the DS3231 for the current time value and is returned as \b struct \b tm.
 * @note The returned value must be freed by the caller to prevent memory leakage
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b struct \b tm* - Pointer to struct tm populated with the current time.
 */
struct tm* ds3231_time_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the time on the DS3231 usinf a \b time_t value
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param time [in] time_t that is set to the time to be set on on the DS3231
 * @return @b esp_err_t - cascaded from i2c_master_transmit
 */
esp_err_t ds3231_time_time_t_set(rtc_handle_t* rtc_handle, time_t time);

/**
 * @brief Queries the DS3231 for the current time value and is returned as \b struct \b tm.
 * @note Even though the ESP32 family is 32-bits, time_t is 64-bit and is Y2038 compliant,
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b time_t - Unsigned integer value representing number of seconds since Jan 1, 1970 00:00:00
 */
time_t ds3231_time_unix_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the time on the DS3231 using a \b time_t value
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param unix_time [in] long that is set to the time, in seconds since Jan 1, 1970 @ 00:00:00 to be set on on the DS3231
 * @return @b esp_err_t - cascaded from i2c_master_transmit
 */
esp_err_t ds3231_time_unix_set(rtc_handle_t* rtc_handle, long unix_time);

/**
 * @brief Sets the time of the DS3231 using a \b struct \b tm value
 * @note the \b time value \b MUST have \b BOTH the day of the week (tm.tm_wday) and day of month (tm.tm_mday) populated to
 * correctlt set the time on the DS3231
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param time [in] struct tm that has its fields set the desired time values.
 * @return @b esp_err_t - cascaded from i2c_master_transmit
 */
esp_err_t ds3231_time_tm_set(rtc_handle_t* rtc_handle, struct tm time);

/**
 * @brief Creates the GPIO Pin interrupt and queue and assigns the user-defined function and parameters for handling interrupts
 * generated by the DS3231 on it's  INT pin.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param INT [in] gpio_num_t The GPIO Pin that the DS3231 INT pin is connected to.
 * @param isr [in] gpio_isr_t User-defined function to be invoded at every interrupt event.
 * @param params [in] void* Pointer to parameters that are passed to \b isr,
 * @return \b esp_err_t - cascaded from gpio_isr_handler_add
 */
esp_err_t ds3231_alarm_isr_create(rtc_handle_t* rtc_handle, gpio_num_t INT, gpio_isr_t isr, void* params);

/**
 * @brief Deletes alarm isr previously created by \b ds3231_alarm_isr_create
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param INT [in] gpio_num_t The GPIO Pin that the DS3231 INT pin is connected to.
 * @return \b esp_err_t - cascaded from gpio_isr_handler_delete
 */

esp_err_t ds3231_alarm_isr_delete(rtc_handle_t* rtc_handle, gpio_num_t INT);

/**
 * @brief Queries the DS3231 for the set value of Alarm 1.
 * @attention Due to the difference on how the DS3231 and \b struct \b tm store day of week/month values, a -1 will appear in
 * the tm.tm_wday or tm.tm_mday (or both) to indicate that those values are invalid
 * @attention If \a tm.tm_wday or \a tm.tm_mday has a -1 and the other have is >= 0,
 * this a normal condition as only the Day of the Week \b or Day of the Month may be used at any given time per alarm.
 * @note The returned value must be freed by the caller to prevent memory leakage
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b struct \b tm* - Pointer to struct tm populated with the Alarm 1 time.
 */
struct tm* ds3231_alarm1_time_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the value of Alarm 1 to fire on the same day each week. Alarm 1 supports a seconds value, where as Alarm 2 does not.
 * Otherwise they are functioanally the same.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param dow [in] enum rtc_day_of_week_e. (This emum coresponds with how the DS3231 represents the doy of week.)
 * @param hour [in] int8_t represening hours from 00 - 23.
 * @param minutes [in] int8_t representing minutes from 00 - 59.
 * @param seconds [in] int8_t representing seconds from 00 - 59.
 * @return @b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm1_day_of_week_set(rtc_handle_t* rtc_handle, enum rtc_day_of_week_e dow, int8_t hour, int8_t minutes,
                                        int8_t seconds);

/**
 * @brief Sets the value of Alarm 1 to fire on the same day each month. Alarm 1 supports a seconds value, where as Alarm 2 does not.
 * Otherwise they are functioanally the same.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param day [in] int8_t representing the day of the month.
 * @note If the day of the month does not exist in the current month, i.e. February 30th, the alarm will not fire that month. It is
 * recomended the logic be included in the alarm ISR to adjust the alarm entry after it fires.
 * @param hour [in] int8_t represening hours from 00 - 23.
 * @param minutes [in] int8_t representing minutes from 00 - 59.
 * @param seconds [in] int8_t representing seconds from 00 - 59.
 * @return @b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm1_day_of_month_set(rtc_handle_t* rtc_handle, int8_t day, int8_t hour, int8_t minutes, int8_t seconds);

/**
 * @brief Returs a boolean value representing the isEnabled status of the Alarm 1
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true - Alarm 1 is enabled.
 * @retval false - Alarm 1 is disabled.
 */
bool ds3231_alarm1_enable_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the enable/disabled state of Alarm 1
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param isEnabled [in] bool that enables/disables Alarm 1
 * @return @b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm1_enable_flag_set(rtc_handle_t* rtc_handle, bool isEnabled);

/**
 * @brief Gets the status of the Alarm 1 Fired flag. If alarm interrupts are configured, one will be triggered. This flag remains
 * set until is manually unset.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true - Alarm 1 has fired.
 * @retval false - Alarm 1 had NOT fired.
 */
bool ds3231_alarm1_fired_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Gets the Alarm 1 trigger condition as described in \b rtc_alarm_rate_e.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b enum \b rtc_alarm_rate_e - Enum identifying the trigger condition,
 */
enum rtc_alarm_rate_e ds3231_alarm1_rate_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the Alarm 1 trigger condition as described in \b rtc_alarm_rate_e.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param alarm_rate [in] Enum identifying the trigger condition.
 * @return @b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm1_rate_set(rtc_handle_t* rtc_handle, enum rtc_alarm_rate_e alarm_rate);

/**
 * @brief Resets the Alarm 1 Fired flag to zero.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return @b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm1_fired_flag_reset(rtc_handle_t* rtc_handle);

/**
 * @brief Queries the DS3231 for the set value of Alarm 2.
 * @attention Due to the difference on how the DS3231 and \b struct \b tm store day of week/month values, a -1 will appear in the
 * tm.tm_wday or tm.tm_mday (or both) to indicate that those values are invalid
 * @attention If \a tm.tm_wday or \a tm.tm_mday has a -1 and the other have is >= 0,
 * this a normal condition as only the Day of the Week \b or Day of the Month may be used at any given time per alarm.
 * @note The returned value must be freed by the caller to prevent memory leakage
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b struct \b tm* - Pointer to struct tm populated with the Alarm 2 time.
 */
struct tm* ds3231_alarm2_time_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the value of Alarm 2 to fire on the same day each week. Alarm 2 supports a seconds value, where as Alarm 2 does not.
 * Otherwise they are functioanally the same.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param dow [in] enum rtc_day_of_week_e. (This emum coresponds with how the DS3231 represents the doy of week.)
 * @param hour [in] int8_t represening hours from 00 - 23.
 * @param minutes [in] int8_t representing minutes from 00 - 59.
 * @return @b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm2_day_of_week_set(rtc_handle_t* rtc_handle, enum rtc_day_of_week_e dow, int8_t hour, int8_t minutes);

/**
 * @brief Sets the value of Alarm 2 to fire on the same day each month. Alarm 2 supports a seconds value, where as Alarm 2 does not.
 * Otherwise they are functioanally the same.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param day [in] int8_t representing the day of the month.
 * @note If the day of the month does not exist in the current month, i.e. February 30th, the alarm will not fire that month. It is
 * recomended the logic be included in the alarm ISR to adjust the alarm entry after it fires.
 * @param hour [in] int8_t represening hours from 00 - 23.
 * @param minutes [in] int8_t representing minutes from 00 - 59.
 * @return @b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm2_day_of_month_set(rtc_handle_t* rtc_handle, int8_t day, int8_t hour, int8_t minutes);

/**
 * @brief Returs a boolean value representing the isEnabled status of the Alarm 2
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true - Alarm 2 is enabled.
 * @retval false - Alarm 2 is disabled.
 */
bool ds3231_alarm2_enable_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the enable/disabled state of Alarm 2
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param isEnabled [in] bool that enables/disables Alarm 2
 * @return @b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm2_enable_flag_set(rtc_handle_t* rtc_handle, bool isEnabled);

/**
 * @brief Gets the status of the Alarm 2 Fired flag. If alarm interrupts are configured, one will be triggered. This flag remains
 * set until is manually unset.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true - Alarm 2 has fired.
 * @retval false - Alarm 2 had NOT fired.
 */
bool ds3231_alarm2_fired_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Gets the Alarm 2 trigger condition as described in \b rtc_alarm_rate_e.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b enum \b rtc_alarm_rate_e - Enum identifying the trigger condition,
 */
enum rtc_alarm_rate_e ds3231_alarm2_rate_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the Alarm 2 trigger condition as described in \b rtc_alarm_rate_e.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param alarm_rate [in] Enum identifying the trigger condition.
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm2_rate_set(rtc_handle_t* rtc_handle, enum rtc_alarm_rate_e alarm_rate);

/**
 * @brief Resets the Alarm 2 Fired flag to zero.
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_alarm2_fired_flag_reset(rtc_handle_t* rtc_handle);

/**
 * @brief Synchronizes the time on the ESP32 with the time from thr DS3231
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval 0 Success
 * @retval -1 Failure
 */
int ds3231_set_esp_with_rtc(rtc_handle_t* rtc_handle);

/**
 * @brief Gets the status of the Oscillartor Flag
 *
 * @note The driver is aware that on the harware that Enable is Logic 0 and will return true for Logic 0
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true Enabled
 * @retval false Disabled
 */
bool ds3231_enable_oscillator_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Setss the status of the Oscillartor Flag
 *
 * @note The driver is aware that on the harware that Enable is Logic 0 and will set Logic 0 for \b isEnmabled \b = \b true
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param isEnabled [in] bool that enables/disables the Oscillaror Flag
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_enable_oscillator_flag_set(rtc_handle_t* rtc_handle, bool isEnabled);

/**
 * @brief Gets the status of the Battery Backed Scquare Wave Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true Enabled
 * @retval false Disabled
 */
bool ds3231_battery_backed_square_wave_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the status of the Batter Backed Square Wave Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param isEnabled [in] bool that enables/disables the Battery Backed Square Wave Flag
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_battery_backed_square_wave_flag_set(rtc_handle_t* rtc_handle, bool isEnabled);

/**
 * @brief Gets the status of the Convert Temp TemperatureFlag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true Enabled
 * @retval false Disabled
 */
bool ds3231_convert_temp_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the status of the Convert Temperature Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param isEnabled [in] bool that enables/disables the Convert Temperature Flag
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_convert_temp_flag_set(rtc_handle_t* rtc_handle, bool isEnabled);

/**
 * @brief Gets the current (1 of 4) value of the square wave output
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return enum rtc_square_wave_freq_e Square Wave frequency
 */
enum rtc_square_wave_freq_e ds3231_square_wave_freq_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the (1 of 4) square wave output
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param frequency [in] rtc_square_wave_freq_e The desired frequency.
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_square_wave_freq_set(rtc_handle_t* rtc_handle, enum rtc_square_wave_freq_e frequency);

/**
 * @brief Gets the status of the Interupt/Square Wave Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true Alarm Interrupts are enabled
 * @retval false Square Wave is output constantly
 */
enum rtc_intr_sqr_e ds3231_interrupt_square_wave_control_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the status of the  Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param isEnabled [in] bool that enables/disables the  Flag
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */

esp_err_t ds3231_interrupt_square_wave_control_flag_set(rtc_handle_t* rtc_handle, enum rtc_intr_sqr_e isEnabled);

/**
 * @brief Gets the status of the Ocsillator Stop Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true Enabled
 * @retval false Disabled
 */
bool ds3231_get_oscillator_stop_flag(rtc_handle_t* rtc_handle);

/**
 * @brief Resets the status of the Ocsillator Stop Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */

esp_err_t ds3231_oscillator_stop_flag_reset(rtc_handle_t* rtc_handle);

/**
 * @brief Gets the status of the 32kHz Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true Enabled
 * @retval false Disabled
 */
bool ds3231_32kHz_out_enable_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Sets the status of the 32kHz Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param isEnabled [in] bool that enables/disables the 32kHz Flag
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */

esp_err_t ds3231_32kHz_out_enable_flag_set(rtc_handle_t* rtc_handle, bool isEnabled);

/**
 * @brief Gets the status of the Busy Flag
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @retval true Enabled
 * @retval false Disabled
 */
bool ds3231_busy_flag_get(rtc_handle_t* rtc_handle);

/**
 * @brief Similar in concept to ds3231_set_registers_raw. Allows for the setting of a single register with a single byte of data.
 * @warning This is a potentially dangerous command as no sanity checks are performed on the data. This function is provided in the
 * event that the other function do not provide desired functionality.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param rtc_register [in] enum rtc_register_e Register to write to
 * @param bit_pattern [in] uint8_t Value to write
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_register_set(rtc_handle_t* rtc_handle, enum rtc_register_e rtc_register, uint8_t bit_pattern);

/**
 * @brief Similar in concept to ds3231_register_set. Allows for the setting of a multiple successive registers with an array of
 * data.
 *
 * @warning This is a potentially dangerous command as no sanity checks are performed on the data. This function is provided in the
 * event that the other function do not provide desired functionality.
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 * @param rtc_data [in] uint8_t array with the date to write to the register(s).
 * @param count [in] size_t the number of elements in rtc_data
 * @return \b esp_err_t - cascaded from i2c_master_transmit.
 */
esp_err_t ds3231_set_registers_raw(rtc_handle_t* rtc_handle, uint8_t* rtc_data, size_t count);

/**
 * @brief Gets all of the time, alarm and control/status registers. Data is output as ESP_LOGI
 * @note This is a good function to call as a check on that data being set on the DS3231.
 * @note If the debug level is set to VERBOSE, there will a list of each register and its value in both hex and binary for each get
 * AND set operation.
 */
void ds3231_debug_print_data(rtc_handle_t* rtc_handle);

/**
 * @brief Sets, gets, resets and gets all flags from the control/status regiters. Data is output as ESP_LOGI
 * @warning This should only be used for debugging/testing a DS3231. Existing setting will be overwritten
 *
 * @param rtc_handle [in] rtc_handle_t* returned from ds3231_init.
 */
void ds3231_debug_test_set(rtc_handle_t* rtc_handle);

#ifdef __cplusplus
}
#endif