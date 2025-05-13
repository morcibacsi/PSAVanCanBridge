#pragma once

#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"
#include "esp_err.h"
//#include "lib/esp_ws28xx/esp_ws28xx.h"


class RgbLed
{
private:
    // GPIO assignment
    // Numbers of the LED in the strip
    #define LED_STRIP_LED_COUNT 1
    // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
    #define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)

    uint8_t led_state_off = 0;
    led_strip_handle_t led_strip;

public:
    RgbLed(uint8_t ledPin)
    {
         led_strip = configure_led(ledPin);
    }

    void blink_led(void)
    {
        if (led_state_off) {
            /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
            for (int i = 0; i < LED_STRIP_LED_COUNT; i++) {
                ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, 5, 0, 0));
            }
            /* Refresh the strip to send data */
            ESP_ERROR_CHECK(led_strip_refresh(led_strip));
        } else {
            /* Set all LED off to clear all pixels */
            ESP_ERROR_CHECK(led_strip_clear(led_strip));
        }
        led_state_off = !led_state_off;
    }

    led_strip_handle_t configure_led(uint8_t ledPin)
    {
        // LED strip general initialization, according to your led board design
        led_strip_config_t strip_config = {
            .strip_gpio_num = ledPin, // The GPIO that connected to the LED strip's data line
            .max_leds = LED_STRIP_LED_COUNT,      // The number of LEDs in the strip,
            .led_model = LED_MODEL_WS2812,        // LED strip model
            .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color order of the strip: GRB
            .flags = {
                .invert_out = false, // don't invert the output signal
            }
        };

        // LED strip backend configuration: RMT
        led_strip_rmt_config_t rmt_config = {
            .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
            .resolution_hz = LED_STRIP_RMT_RES_HZ, // RMT counter clock frequency
            .mem_block_symbols = 64,               // the memory size of each RMT channel, in words (4 bytes)
            .flags = {
                .with_dma = false, // DMA feature is available on chips like ESP32-S3/P4
            }
        };

        // LED Strip object handle
        led_strip_handle_t led_strip;
        ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
        return led_strip;
    }
};