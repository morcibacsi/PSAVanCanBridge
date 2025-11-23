#ifndef UlpVanTx_h
#define UlpVanTx_h

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32

#include "driver/gpio.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "soc/rtc_periph.h"
#include <stdint.h>
#include "hulp.h"

static RTC_SLOW_ATTR ulp_var_t ulp_command[1 + 33];

typedef enum {
    ULP_VAN_62K5BPS  = 0,
    ULP_VAN_125KBPS  = 1,
} ULP_VAN_NETWORK_SPEED;

class UlpVanTx
{
    private:
        gpio_num_t _rxPin;
        gpio_num_t _txPin;
        ULP_VAN_NETWORK_SPEED _networkSpeed;

        uint16_t Crc15(const uint8_t data[], const uint8_t length);
        void InternalSendFrame(const uint8_t data[], const uint8_t length);
    public:
        UlpVanTx(gpio_num_t rxPin, gpio_num_t txPin, ULP_VAN_NETWORK_SPEED networkSpeed);
        ~UlpVanTx();
        void Start();
        void SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t length, const bool requireAck);
        void SendReplyRequestFrame(const uint16_t identifier);
};

#endif
#endif