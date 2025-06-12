#include <stdint.h>
#include <stdbool.h>
#include "ulp_lp_core_gpio.h"
#include "ulp_lp_core_utils.h"

volatile uint32_t VAN_DATA[33];
volatile uint32_t VAN_DATA_LENGTH = 0;
volatile uint32_t VAN_START_TX = 0;
volatile uint32_t VAN_TX_FINISHED = 1;
volatile uint32_t SET_VAN_RX_PIN = 0;
volatile uint32_t SET_VAN_TX_PIN = 0;
volatile uint32_t VAN_START_APP = 0;
volatile uint32_t VAN_RETRY_COUNT = 0;
volatile uint32_t VAN_FRAME_TYPE = 0;

volatile lp_io_num_t VAN_RX_PIN;
volatile lp_io_num_t VAN_TX_PIN;

#define VAN_FRAME_TYPE_NORMAL 0
#define VAN_FRAME_TYPE_QUERY 1

#define NOP1()  __asm__ __volatile__("nop")
#define NOP2()  NOP1(); NOP1()
#define NOP4()  NOP2(); NOP2()
#define NOP8()  NOP4(); NOP4()
#define NOP10()  NOP4(); NOP4(); NOP2()
#define DELAY_10_CYCLES() NOP8(); NOP1(); NOP1()
#define DELAY_15_CYCLES() NOP8(); NOP1(); NOP1()
#define DELAY_20_CYCLES() DELAY_10_CYCLES(); DELAY_10_CYCLES()
#define DELAY_25_CYCLES() DELAY_10_CYCLES(); DELAY_10_CYCLES(); NOP1(); NOP1(); NOP1(); NOP1(); NOP1()
#define DELAY_50_CYCLES() DELAY_25_CYCLES(); DELAY_25_CYCLES()
#define DELAY_75_CYCLES() DELAY_50_CYCLES(); DELAY_25_CYCLES()
#define DELAY_90_CYCLES() DELAY_50_CYCLES(); DELAY_25_CYCLES(); DELAY_15_CYCLES()
#define DELAY_100_CYCLES() DELAY_50_CYCLES(); DELAY_50_CYCLES()

bool wait_for_start()
{
    while (VAN_START_APP == 0)
    {
    }
    return true;
}

bool wait_for_bus_high_125kbps()
{
    // While the GPIO is low, continue polling the pin
    while (ulp_lp_core_gpio_get_level(VAN_RX_PIN) == 0)
    {
        // Busy wait
    }

    // We have a high state, "move into the middle" of the bit
    ulp_lp_core_delay_us(4);

    // Wait for 16 bits of high state (EOF + IFS period) to ensure the bus is free (16 * 8us)
    int i = 0;
    while (i < 16)
    {
        i++;
        ulp_lp_core_delay_us(7);

        if (ulp_lp_core_gpio_get_level(VAN_RX_PIN) == 0)
        {
            // If the GPIO is low, we need to wait for the bus to become free again
            return false;
        }
    }

    return true;
}

bool transmit_data_125kbps()
{
    // warning: this function is written by observing the logic analyzer output to match the timings of a 125kbps signal
    // in case of any changes in this function, please verify the timings with a logic analyzer (1 bit = 8us)
    // every instruction takes some cycle so in case of modification the ulp_lp_core_delay_cycles() should be adjusted
    uint8_t bitToWrite;
    uint8_t prevBit = 0xFF; // invalid state to force first write
    uint8_t busLevel;

    for (int i = 0; i < VAN_DATA_LENGTH; i++)
    {
        for (int bitCounter = 9; bitCounter >= 0; bitCounter--)
        {
            bitToWrite = (VAN_DATA[i] >> bitCounter) & 1;

            if (bitToWrite != prevBit)
            {
                ulp_lp_core_gpio_set_level(VAN_TX_PIN, bitToWrite);
                prevBit = bitToWrite;
            }
            else
            {
                DELAY_20_CYCLES();
            }

            if (bitToWrite == 1)
            {
                DELAY_10_CYCLES();
                busLevel = ulp_lp_core_gpio_get_level(VAN_RX_PIN);

                if (busLevel == 0)
                {
                    // If the bus is low then we lost arbitration, we need to stop the transmission
                    return false;
                }

                //DELAY_75_CYCLES();
                ///*
                if (bitCounter != 0)
                {
                    DELAY_75_CYCLES();
                }
                else
                {
                    DELAY_50_CYCLES();
                    DELAY_10_CYCLES();
                }
                //*/
            }
            else
            {
                //DELAY_100_CYCLES();
                ///*
                if (bitCounter != 0)
                {
                    DELAY_90_CYCLES();
                    NOP8();
                }
                else
                {
                    DELAY_90_CYCLES();
                }
                //*/
            }
        }
    }

    ulp_lp_core_gpio_set_level(VAN_TX_PIN, 1);
    return true;
}

int main(void)
{
    // Wait for the main application to start the ULP
    // This is needed otherwise the ULP would reset its variables, and we couldn't set the RX-TX pins
    wait_for_start();

    VAN_RX_PIN = SET_VAN_RX_PIN;
    VAN_TX_PIN = SET_VAN_TX_PIN;

    // GPIO initialization
    ulp_lp_core_gpio_init(VAN_RX_PIN);
    ulp_lp_core_gpio_input_enable(VAN_RX_PIN);

    ulp_lp_core_gpio_init(VAN_TX_PIN);
    ulp_lp_core_gpio_output_enable(VAN_TX_PIN);
    ulp_lp_core_gpio_set_output_mode(VAN_TX_PIN, RTCIO_LL_OUTPUT_NORMAL);

    uint8_t retryCount = 0;
    while (1)
    {
        // Wait for transmission start signal
        while (VAN_START_TX == 0)
        {
        }
        retryCount = 0;
        VAN_START_TX = 0;

        start_tx:
        if (wait_for_bus_high_125kbps())
        {
            bool result = transmit_data_125kbps();
            if (result == false && VAN_FRAME_TYPE != VAN_FRAME_TYPE_QUERY)
            {
                retryCount++;
                if (retryCount < VAN_RETRY_COUNT)
                {
                    goto start_tx;
                }
            }
        }
        else
        {
            goto start_tx;
            /*
            retryCount++;
            if (retryCount > VAN_RETRY_COUNT)
            {
            }
            */
        }
        VAN_TX_FINISHED = 1;
    }

    /* ulp_lp_core_halt() is called automatically when main exits */
    return 0;
}