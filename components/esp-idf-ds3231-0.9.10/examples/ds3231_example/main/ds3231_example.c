#include <stdio.h>
#include "esp-idf-ds3231.h"

void app_main(void) {
    // Allocte memory for the pointer of i2c_master_bus_handle_t
    i2c_master_bus_handle_t* bus_handle = (i2c_master_bus_handle_t*)malloc(sizeof(i2c_master_bus_handle_t));
    // Create the i2c_master_bus_config_t struct and assign values.
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = -1,
        .scl_io_num = CONFIG_DS3231_SCL,
        .sda_io_num = CONFIG_DS3231_SDA,
        .glitch_ignore_cnt = 7,
        // The DS3231 **requires** pullup resistors on all of its I/O pins. Note: Some DS3231 boards have pullup resistors as part
        // of their design.
        .flags.enable_internal_pullup = true,
    };
    i2c_new_master_bus(&i2c_mst_config, bus_handle);
    rtc_handle_t* rtc_handle = ds3231_init(bus_handle);

    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;
    now = ds3231_time_unix_get(rtc_handle);
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    printf("The current time from the DS3231 RTC moldue is: %s\n", strftime_buf);
}
