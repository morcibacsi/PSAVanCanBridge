#include "UlpVanTx.h"

#if CONFIG_IDF_TARGET_ESP32

#include "driver/gpio.h"
#include "esp32/ulp.h"
#include "driver/rtc_io.h"
#include "soc/rtc_periph.h"

#include <string.h>
#include "hulp.h"

#include "sdkconfig.h"
#if !(defined CONFIG_ESP32_ULP_COPROC_RESERVE_MEM) && defined(CONFIG_ULP_COPROC_RESERVE_MEM)
    #define CONFIG_ESP32_ULP_COPROC_RESERVE_MEM CONFIG_ULP_COPROC_RESERVE_MEM
#endif

#define STUFFED_VAL(byte) (((byte) & 0xF0) << 2) | ((~(byte) & 0x10) << 1) | (((byte) & 0x0F) << 1) | ((~(byte) & 0x01) << 0)
#define SPLIT_VAN_IDENTIFIER(iden, byte1, byte2) (*(byte1) = (uint8_t)(((iden) << 4 & 0xff00) >> 8), *(byte2) = (uint8_t)((iden) & 0xF))
#define VAN_FRAME_SOF 0x0E

UlpVanTx::UlpVanTx(gpio_num_t rxPin, gpio_num_t txPin, ULP_VAN_NETWORK_SPEED networkSpeed)
{
    _networkSpeed = networkSpeed;
    _rxPin = rxPin;
    _txPin = txPin;

    enum {
        LBL_WAIT_BEGIN,
        LABEL_WAIT_FOR_BUS_HI,
        LABEL_CHECK_FREE,
        LBL_NEXT_BYTE,
        LBL_NEXT_BIT,
        LBL_1BIT,
        LBL_DONE
    };

    #define VAN_DATA_ARR_OFFSET (((uint32_t)ulp_command - ((uint32_t)RTC_SLOW_MEM)) / 4)
    #define M_CHECK_BUS_HI() I_GPIO_READ(_rxPin), M_BL(LBL_WAIT_BEGIN, 1)

    const ulp_insn_t ulpProgram[] =
    {
        M_LABEL(LBL_WAIT_BEGIN),
            // Reset Pointer
            I_MOVI(R1, VAN_DATA_ARR_OFFSET),
            // Wait here until SoC sets flag to transmit
            I_LD(R0,R1,0),
            I_BL(-1, 1 << 15),

        // Clear the flag, leaving only the length, which we keep in R3
        I_ANDI(R3,R0,0x7FFF),
        I_ST(R3,R1,0),

        M_LABEL(LABEL_WAIT_FOR_BUS_HI),
            // While the GPIO is low, continue polling the pin
            I_GPIO_READ(_rxPin),
            I_BL(-1,1),

            // We have a high state, "move into the middle" of the bit
            M_DELAY_US_1_10(4),

            // Wait for 16 bits of high state (EOF + IFS period) to ensure the bus is free (16 * 8us)
            I_STAGE_RST(),
            M_LABEL(LABEL_CHECK_FREE),
                I_STAGE_INC(1),

                // Wait for 8us to "move into the next bit"
                M_DELAY_US_1_10(8),

                // If the GPIO is low, we need to wait for the bus to become free again
                I_GPIO_READ(_rxPin),
                M_BL(LABEL_WAIT_FOR_BUS_HI, 1),

                M_BSLE(LABEL_CHECK_FREE, 16),
                // We have 16 bits of high state, so we can start writing the data

            M_LABEL(LBL_NEXT_BYTE),
                // If this is the last byte then go to LBL_DONE (Decrement remaining bytes until done)
                I_SUBI(R3, R3, 1),
                M_BXF(LBL_DONE),
                // Increment pointer, load next byte, reset stage (bit counter)
                I_ADDI(R1, R1, 1),
                I_LD(R2, R1, 0),
                I_STAGE_RST(),

                M_LABEL(LBL_NEXT_BIT),
                    I_STAGE_INC(1),

                    // If bit == 1 then go to LBL_1BIT
                        I_ANDI(R0, R2, (1 << 15)),
                        I_LSHI(R2, R2, 1),
                        M_BGE(LBL_1BIT, (1 << 15)),

                    // Else 0 bit
                        I_GPIO_SET(_txPin, 0),
                        I_DELAY(10),

                        // If this is the 10th bit then go to LBL_NEXT_BYTE
                            M_BSGE(LBL_NEXT_BYTE, 10),
                        // Else delay a little more... and go to LBL_NEXT_BIT
                            I_DELAY(12),
                            M_BX(LBL_NEXT_BIT),

                M_LABEL(LBL_1BIT),
                    I_GPIO_SET(_txPin, 1),

                    // If the bus is low then we lost arbitration and go to LBL_WAIT_BEGIN
                        M_CHECK_BUS_HI(),
                        M_CHECK_BUS_HI(),

                    // If this is the 10th bit go to LBL_NEXT_BYTE
                        M_BSGE(LBL_NEXT_BYTE, 10),
                    // Not the 10th bit so delay while checking bus
                        //I_DELAY(8),
                        M_CHECK_BUS_HI(),
                        M_BX(LBL_NEXT_BIT),

        M_LABEL(LBL_DONE),
            I_GPIO_SET(_txPin, 1),
            M_BX(LBL_WAIT_BEGIN)
    };

    size_t programSize = sizeof(ulpProgram) / sizeof(ulp_insn_t);
    ESP_ERROR_CHECK(ulp_process_macros_and_load(0, ulpProgram, &programSize));
}

UlpVanTx::~UlpVanTx()
{
}

void UlpVanTx::Start()
{
    ESP_ERROR_CHECK(rtc_gpio_init(_rxPin));
    rtc_gpio_set_direction(_rxPin, RTC_GPIO_MODE_INPUT_ONLY);

    ESP_ERROR_CHECK(rtc_gpio_init(_txPin));
    rtc_gpio_set_level(_txPin, 1);
    rtc_gpio_set_direction(_txPin, RTC_GPIO_MODE_OUTPUT_ONLY);

    ESP_ERROR_CHECK(ulp_run(0));
}

uint16_t UlpVanTx::Crc15(const uint8_t data[], const uint8_t length)
{
    const uint8_t order = 15;
    const uint16_t polynom = 0xF9D;
    const uint16_t xorValue = 0x7FFF;
    const uint16_t mask = 0x7FFF;

    uint16_t crc = 0x7FFF;

    for (uint8_t i = 0; i < length; i++)
    {
        uint8_t currentByte = data[i];

        // rotate one data byte including crcmask
        for (uint8_t j = 0; j < 8; j++)
        {
            bool bit = (crc & (1 << (order - 1))) != 0;
            if ((currentByte & 0x80) != 0)
            {
                bit = !bit;
            }
            currentByte <<= 1;

            crc = ((crc << 1) & mask) ^ (-bit & polynom);
        }
    }

    // perform xor and multiply result by 2 to turn 15 bit result into 16 bit representation
    return (crc ^ xorValue) << 1;
}

void UlpVanTx::InternalSendFrame(const uint8_t data[], const uint8_t length)
{
    // The first item (ulp_command[0]) is metadata, which is set last

    // Data
    // Shift these such that the MSB is in bit15, which simplifies logic for the ULP
    // ulp_command[1 + 0].val = (0x0E) << 6;

    uint8_t i = 0;

    while (i < length)
    {
        if (i == length - 1)
        {
            // the second byte of the CRC contains the EOD - a pair of zeros which commit an E-Manchester violation marking the end of transmitted data.
            ulp_command[1 + i].val = ((STUFFED_VAL(data[i])) & ((STUFFED_VAL(data[i]))-1)) << 6;
        }
        else
        {
            ulp_command[1 + i].val = (STUFFED_VAL(data[i])) << 6;
        }
        i++;
    }
    // EOF
    ulp_command[1 + i].val = 0x3FF << 6;

    // Now set metadata (flag + length) to begin transmission
    ulp_command[0].val = (1 << 15) | (length + 1);
}

void UlpVanTx::SendNormalFrame(const uint16_t identifier, const uint8_t data[], const uint8_t dataLength, const bool requireAck)
{
    // van frame length is length of data + 5 bytes (SOF, ID, COM, CRC, CRC)
    uint8_t vanFrame[dataLength + 5] = { 0 };

    uint8_t idenByte1, idenByte2;
    SPLIT_VAN_IDENTIFIER(identifier, &idenByte1, &idenByte2);

    // COM
    idenByte2 = idenByte2 << 4 | 0x08;

    if (requireAck)
        idenByte2 = idenByte2 | 0x04;

    // SOF
    vanFrame[0] = VAN_FRAME_SOF;

    // ID & COM
    vanFrame[1] = idenByte1;
    vanFrame[2] = idenByte2;

    // Data
    memcpy(&vanFrame[3], data, dataLength);

    // CRC - exclude SOF
    uint16_t crc = Crc15(&vanFrame[1], dataLength + 2);
    vanFrame[dataLength + 3] = (crc >> 8) & 0xFF;
    vanFrame[dataLength + 4] = (crc & 0xFF);

    InternalSendFrame(vanFrame, dataLength + 5);
}

void UlpVanTx::SendReplyRequestFrame(const uint16_t identifier)
{
    // van frame length is length of data + 5 bytes (SOF, ID, COM, CRC, CRC)
    uint8_t vanFrame[5] = { 0 };

    uint8_t idenByte1, idenByte2;
    SPLIT_VAN_IDENTIFIER(identifier, &idenByte1, &idenByte2);
    idenByte2 = idenByte2 << 4 | 0x0F;

    // SOF
    vanFrame[0] = VAN_FRAME_SOF;

    // ID & COM
    vanFrame[1] = idenByte1;
    vanFrame[2] = idenByte2;

    // CRC - exclude SOF
    uint16_t crc = Crc15(&vanFrame[1], 2);
    vanFrame[3] = (crc >> 8) & 0xFF;
    vanFrame[4] = (crc & 0xFF);

    InternalSendFrame(vanFrame, 5);

    /*
    uint8_t idenByte1, idenByte2;
    SPLIT_VAN_IDENTIFIER(identifier, &idenByte1, &idenByte2);
    // COM part of the message is F
    idenByte2 = idenByte2 << 4 | 0x0F;

    ulp_command[1 + 0].val  = (STUFFED_VAL(VAN_FRAME_SOF)) << 6;
    ulp_command[1 + 1].val  = (STUFFED_VAL(idenByte1)) << 6;
    //ulp_command[1 + 2].val  = (STUFFED_VAL(byte2)) << 6; //this is for testing
    ulp_command[1 + 2].val  = (STUFFED_VAL(idenByte2) | 1) << 6;
    // Now set metadata (flag + length) to begin
    ulp_command[0].val = (1 << 15) | 3;
    */
}
#endif