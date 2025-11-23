#pragma once

#ifdef PIO_INI_BOARD_ESP32_C6_V16
    #include "BoardConfig_v16.h"
#endif
#ifdef PIO_INI_BOARD_ESP32_v13_CAN
    #include "BoardConfig_v13_can.h"
    #include "lib/CanMessageSender/CanMessageSenderMcp2515Idf.h"
#endif
#ifdef PIO_INI_BOARD_ESP32_v14_CAN
    #include "BoardConfig_v14_can.h"
    #include "lib/CanMessageSender/CanMessageSenderMcp2515Idf.h"
#endif
#ifdef PIO_INI_BOARD_ESP32_v15_CAN
    #include "BoardConfig_v15_can.h"
    #include "lib/CanMessageSender/CanMessageSenderMcp2515Idf.h"
#endif
#ifdef PIO_INI_BOARD_ESP32_v15_VAN_ULP
    #include "BoardConfig_v15_van_ulp.h"
    #include "lib/esp32_hulp_van_tx/HulpVanTx.hpp"
#endif