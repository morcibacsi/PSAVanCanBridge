#pragma once

#include "ISerial.hpp"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class UartSerial : public ISerial
{
public:
    explicit UartSerial(uart_port_t port)
        : _port(port) {}

    esp_err_t begin(int baudrate) override
    {
        uart_config_t config = {};
        config.baud_rate = baudrate;
        config.data_bits = UART_DATA_8_BITS;
        config.parity    = UART_PARITY_DISABLE;
        config.stop_bits = UART_STOP_BITS_1;
        config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
        config.source_clk = UART_SCLK_DEFAULT;

        ESP_ERROR_CHECK(uart_driver_install(_port, 2048, 0, 0, nullptr, 0));
        ESP_ERROR_CHECK(uart_param_config(_port, &config));

        return ESP_OK;
    }

    void end() override
    {
        uart_driver_delete(_port);
    }

    int available() override
    {
        size_t length;
        uart_get_buffered_data_len(_port, &length);
        return static_cast<int>(length);
    }

    int read(uint8_t* buffer, size_t length, TickType_t timeout) override
    {
        return uart_read_bytes(_port, buffer, length, timeout);
    }

    int readByte(uint8_t* byte, TickType_t timeout) override
    {
        return uart_read_bytes(_port, byte, 1, timeout);
    }

    int write(const uint8_t* data, size_t length) override
    {
        return uart_write_bytes(_port, data, length);
    }

    int writeByte(uint8_t byte) override
    {
        return uart_write_bytes(_port, reinterpret_cast<const char*>(&byte), 1);
    }

    void flush() override
    {
        uart_wait_tx_done(_port, portMAX_DELAY);
    }

private:
    uart_port_t _port;
};