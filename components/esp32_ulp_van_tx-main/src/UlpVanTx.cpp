#include "UlpVanTx.h"

#if CONFIG_IDF_TARGET_ESP32

#include "driver/gpio.h"
#include "esp32/ulp.h"
#include "driver/rtc_io.h"
#include "soc/rtc_periph.h"

#include <string.h>

#include "sdkconfig.h"
#if !(defined CONFIG_ESP32_ULP_COPROC_RESERVE_MEM) && defined(CONFIG_ULP_COPROC_RESERVE_MEM)
    #define CONFIG_ESP32_ULP_COPROC_RESERVE_MEM CONFIG_ULP_COPROC_RESERVE_MEM
#endif

#define STUFFED_VAL(byte) ((((byte) & 0xF0) << 2) | ((~(byte) & 0x10) << 1) | (((byte) & 0x0F) << 1) | ((~(byte) & 0x01) << 0))
#define SPLIT_VAN_IDENTIFIER(iden, byte1, byte2) (*(byte1) = (uint8_t)(((iden) << 4 & 0xff00) >> 8), *(byte2) = (uint8_t)((iden) & 0xF))
#define VAN_FRAME_SOF 0x0E
#define ULP_COMMAND_PENDING (1 << 15)
#define ULP_COMMAND_LENGTH_MASK 0x7FFF
#define ULP_ACK_IDENTIFIER_DISABLED 0xFFFF

static_assert(STUFFED_VAL(0x40) == 0x0121,
              "STUFFED_VAL must expand and group the complete input byte");

UlpVanTx::UlpVanTx(gpio_num_t rxPin, gpio_num_t txPin, ULP_VAN_NETWORK_SPEED networkSpeed)
{
    _networkSpeed = networkSpeed;
    _rxPin = rxPin;
    _txPin = txPin;
    _ulpProgramInstructionCount = 0;
    ulp_ack_state[ULP_ACK_CONFIGURED_IDENTIFIER].word = ULP_ACK_IDENTIFIER_DISABLED;
    ulp_ack_state[ULP_ACK_LAST_SAMPLED_IDENTIFIER].val = 0;
    ulp_ack_state[ULP_ACK_LAST_SAMPLED_COM].val = 0;
    ulp_ack_state[ULP_ACK_LAST_EOD_CANDIDATE_GROUP].val = 0;
    ulp_ack_state[ULP_ACK_COUNT].word = 0;

    enum {
        LBL_MONITOR_HIGH,
        LBL_RX_SOF,
        LBL_RX_IDENTIFIER_NEXT,
        LBL_RX_IDENTIFIER_SAMPLE,
        LBL_RX_IDENTIFIER_ACCUM,
        LBL_RX_IDENTIFIER_DONE,
        LBL_RX_MATCH,
        LBL_RX_MATCHED,
        LBL_RX_ABORT,
        LBL_RX_COM_FALL_WAIT,
        LBL_RX_COM_FALL_FOUND,
        LBL_RX_COM_RISE_WAIT,
        LBL_RX_COM_EDGE_FOUND,
        LBL_RX_TRACK_NORMAL,
        LBL_RX_TRACK_D,
        LBL_RX_EDGE_SEARCH,
        LBL_RX_EDGE_UNCHANGED,
        LBL_RX_EOD,
        LBL_RX_ACK,
        LABEL_WAIT_FOR_BUS_HI,
        LABEL_CHECK_FREE,
        LBL_BEGIN_TX,
        LBL_NEXT_BYTE,
        LBL_NEXT_BIT,
        LBL_1BIT,
        LBL_DONE
    };

    #define VAN_DATA_ARR_OFFSET (((uint32_t)ulp_command - ((uint32_t)RTC_SLOW_MEM)) / 4)
    // M_DELAY_US accounts for the DELAY instruction itself, not the ALU and
    // branch instructions around it. These path-specific delays compensate
    // for the FSM ULP's roughly six RTC-fast-clock cycles per instruction.
    #define M_RX_ID_SAMPLE(label) M_DELAY_US_1_10(4), I_GPIO_READ(_rxPin), M_BX(label)
    #define M_CHECK_BUS_HI() I_GPIO_READ(_rxPin), M_BL(LABEL_WAIT_FOR_BUS_HI, 1)

    const ulp_insn_t ulpProgram[] =
    {
            // Do not assume startup occurs during an idle part of the bus.
            M_BX(LABEL_WAIT_FOR_BUS_HI),

        M_LABEL(LBL_MONITOR_HIGH),
            // Every path entering the monitor has already released TX.
            I_MOVI(R1, VAN_DATA_ARR_OFFSET),

            // A dominant edge takes priority over a pending local command.
            I_GPIO_READ(_rxPin),
            M_BL(LBL_RX_SOF, 1),

            // If no TX is pending, keep monitoring rather than halting the ULP.
            I_LD(R0,R1,0),
            M_BL(LBL_MONITOR_HIGH, ULP_COMMAND_PENDING),
            M_BX(LABEL_WAIT_FOR_BUS_HI),

        // A dominant level after qualified recessive idle belongs to SOF, but
        // the 8 us idle poll can discover it anywhere in SOF's four dominant
        // slices. Lock to the following low-to-high transition instead. From
        // that edge, six SOF slices remain before the identifier begins.
        M_LABEL(LBL_RX_SOF),
            I_GPIO_READ(_rxPin),
            M_BL(LBL_RX_SOF, 1),
            // Preserve the validated eight-cycle prefix timing formerly
            // occupied by the temporary GPIO25 REG_WR instruction.
            I_DELAY(2),
            // Branch/initialization plus the identifier sampler's own 3 us
            // delay fill the rest of the 52 us edge-to-first-sample interval.
            // Hardware capture showed the final identifier inverse sampled
            // one state early (0x4728 instead of 0x4729); center the complete
            // identifier window one microsecond later.
            M_DELAY_US_1_10(45),

        // Keep the identifier exactly as the ULP sees it: 15 expanded states.
        M_LABEL(LBL_RX_IDENTIFIER_NEXT),
            I_MOVI(R3, 0),
            I_STAGE_RST(),
        M_LABEL(LBL_RX_IDENTIFIER_SAMPLE),
            M_RX_ID_SAMPLE(LBL_RX_IDENTIFIER_ACCUM),

        M_LABEL(LBL_RX_IDENTIFIER_ACCUM),
            I_STAGE_INC(1),
            I_ADDR(R3, R3, R3),
            I_ADDR(R3, R3, R0),
            M_BSGE(LBL_RX_IDENTIFIER_DONE, 15),
            M_BX(LBL_RX_IDENTIFIER_SAMPLE),

        // Sample and validate COM[0] before spending time on the table lookup.
        M_LABEL(LBL_RX_IDENTIFIER_DONE),
            M_DELAY_US_1_10(3),
            I_GPIO_READ(_rxPin),
            // COM=C starts with two recessive states. Reject immediately if
            // the first state is not recessive.
            M_BL(LBL_RX_ABORT, 1),
            I_MOVI(R1, RTC_WORD_OFFSET(ulp_ack_state)),

        // One comparison is the deliberately bounded identifier matcher.
        M_LABEL(LBL_RX_MATCH),
            I_LD(R2, R1, ULP_ACK_CONFIGURED_IDENTIFIER),
            I_ST(R3, R1, ULP_ACK_LAST_SAMPLED_IDENTIFIER),
            I_SUBR(R0, R2, R3),
            M_BXZ(LBL_RX_MATCHED),
            M_BX(LBL_RX_ABORT),

        // COM=C is raw 11001. Rather than sampling its remaining states with
        // another relative-delay train, find both known physical edges. The
        // falling edge starts COM[2]; the rising edge is the mandatory
        // COM-D/inverse transition and becomes the body phase anchor.
        M_LABEL(LBL_RX_MATCHED),
            I_STAGE_RST(),

        M_LABEL(LBL_RX_COM_FALL_WAIT),
            I_STAGE_INC(1),
            I_GPIO_READ(_rxPin),
            M_BL(LBL_RX_COM_FALL_FOUND, 1),
            // Thirty captured 0x8C4 frames timed out one poll immediately
            // before the physical fall. One additional bounded poll covers
            // the measured 0.88..2.19 us miss without changing later logic.
            M_BSLE(LBL_RX_COM_FALL_WAIT, 5),
            M_BX(LBL_RX_ABORT),

        M_LABEL(LBL_RX_COM_FALL_FOUND),
            // COM[0] was already sampled recessive before the identifier
            // lookup. Hardware shows the legitimate COM[1]/COM[2] fall can
            // be present at the first poll, so that first bounded observation
            // is valid and must not be rejected as an early edge.
            I_STAGE_RST(),

        M_LABEL(LBL_RX_COM_RISE_WAIT),
            I_STAGE_INC(1),
            I_GPIO_READ(_rxPin),
            M_BGE(LBL_RX_COM_EDGE_FOUND, 1),
            M_BSLE(LBL_RX_COM_RISE_WAIT, 6),
            M_BX(LBL_RX_ABORT),

        M_LABEL(LBL_RX_COM_EDGE_FOUND),
            // Reject a rise too close to the fall: COM[2] and COM[3] must both
            // be dominant. The observed rise is COM D/inverse, not an
            // instruction-timed approximation of it.
            M_BSLT(LBL_RX_ABORT, 4),
            I_MOVI(R0, 0x19),
            I_ST(R0, R1, ULP_ACK_LAST_SAMPLED_COM),

        M_LABEL(LBL_RX_TRACK_NORMAL),
            I_MOVI(R3, 0),
            I_ST(R3, R1, ULP_ACK_LAST_EOD_CANDIDATE_GROUP),
            M_DELAY_US_10_100(32),

        // Read D near its center. The instructions before the first edge poll
        // advance close to the expected D/inverse boundary. Three bounded
        // polls cover the rest of inverse without waiting far enough to lock
        // to an unrelated inverse/next-A transition.
        M_LABEL(LBL_RX_TRACK_D),
            I_GPIO_READ(_rxPin),
            I_MOVR(R2, R0),
            I_ADDI(R3, R3, 1),
            I_STAGE_RST(),

        M_LABEL(LBL_RX_EDGE_SEARCH),
            I_STAGE_INC(1),
            I_GPIO_READ(_rxPin),
            I_SUBR(R0, R0, R2),
            M_BXZ(LBL_RX_EDGE_UNCHANGED),

            // A real D/inverse transition was observed. Rebase the next D
            // sample on this observation, bounding phase error to one group
            // instead of accumulating it from SOF through the whole frame.
            M_DELAY_US_10_100(32),
            M_BX(LBL_RX_TRACK_D),

        M_LABEL(LBL_RX_EDGE_UNCHANGED),
            M_BSLE(LBL_RX_EDGE_SEARCH, 2),
            // No transition throughout the bounded inverse window. D=1 is
            // the malformed 11 pair (or a missing inverse edge). D=0 is a
            // potential EOD 00 and must pass all legal-position checks.
            I_MOVR(R0, R2),
            M_BGE(LBL_RX_ABORT, 1),
            M_BX(LBL_RX_EOD),

        M_LABEL(LBL_RX_EOD),
            // Legal EOD groups are exactly 4, 6, ... 60 for 0..28 DATA bytes.
            I_ST(R3, R1, ULP_ACK_LAST_EOD_CANDIDATE_GROUP),
            I_MOVR(R0, R3),
            M_BL(LBL_RX_ABORT, 4),
            M_BGE(LBL_RX_ABORT, 61),
            I_ANDI(R0, R3, 1),
            M_BGE(LBL_RX_ABORT, 1),

        M_LABEL(LBL_RX_ACK),
            // EOD has ended only a few microseconds before this point. Leave
            // TX recessive through the remainder of ACK[0], drive only ACK[1]
            // dominant, then release before EOF. These are the only timing
            // constants to tune from the production Saleae waveform.
            // The 5 us setting placed ACK[1] 7.12..9.81 us after EOD, with a
            // 9.14 us average. Advance it by 1 us to center ACK[0] nearer one
            // VAN time slice without changing receive tracking.
            M_DELAY_US_1_10(4),
            I_GPIO_SET(_txPin, 0),
            // Keep the existing 6 us dominant-hold calibration unchanged;
            // ACK notification begins only after the release instruction.
            M_DELAY_US_1_10(6),
            I_GPIO_SET(_txPin, 1),
            // Publish only after ACK[1] has been driven and TX released.
            // R1 still points at ulp_ack_state; the configured identifier is
            // never cleared, so another matching frame can be ACKed normally.
            I_LD(R0, R1, ULP_ACK_COUNT),
            I_ADDI(R0, R0, 1),
            I_ST(R0, R1, ULP_ACK_COUNT),

        M_LABEL(LBL_RX_ABORT),
            M_BX(LABEL_WAIT_FOR_BUS_HI),

        M_LABEL(LABEL_WAIT_FOR_BUS_HI),
            // While the GPIO is low, continue polling the pin
            I_GPIO_READ(_rxPin),
            I_BL(-1,1),

            // Move away from the edge, then qualify a continuous recessive
            // interval. The loop runs much faster than one VAN slice so a
            // dominant DATA slice cannot be missed through phase aliasing.
            M_DELAY_US_1_10(4),

            // About 47 iterations at ~2.75 us each gives the required EOF+IFS
            // interval at 125 kbit/s.
            I_STAGE_RST(),
            M_LABEL(LABEL_CHECK_FREE),
                I_STAGE_INC(1),
                I_GPIO_READ(_rxPin),
                M_BL(LABEL_WAIT_FOR_BUS_HI, 1),

                M_BSLE(LABEL_CHECK_FREE, 46),
                // After idle qualification, transmit only if a command is pending.
                I_MOVI(R1, VAN_DATA_ARR_OFFSET),
                I_LD(R0, R1, 0),
                M_BL(LBL_MONITOR_HIGH, ULP_COMMAND_PENDING),
                // We have 16 high states and a pending command: start TX.

        M_LABEL(LBL_BEGIN_TX),
            // R1 and R0 were loaded immediately above while checking the
            // pending flag; avoid duplicating those two instructions.
            I_ANDI(R3, R0, ULP_COMMAND_LENGTH_MASK),
            I_ST(R3, R1, 0),

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

                    // Any recessive-TX/dominant-bus mismatch is ordinary
                    // arbitration loss. For a ReplyRequest this also cleanly
                    // releases the bus when a responder takes ownership; this
                    // node no longer tracks or ACKs that immediate response.
                        M_CHECK_BUS_HI(),
                        M_CHECK_BUS_HI(),

                    // If this is the 10th bit go to LBL_NEXT_BYTE
                        M_BSGE(LBL_NEXT_BYTE, 10),
                    // Not the 10th bit so delay while checking bus
                        //I_DELAY(8),
                        M_CHECK_BUS_HI(),
                        M_BX(LBL_NEXT_BIT),

        M_LABEL(LBL_DONE),
            // The prepared ACK/EOF word ends recessive, so TX is already released.
            M_BX(LBL_MONITOR_HIGH)
    };

    size_t programSize = sizeof(ulpProgram) / sizeof(ulp_insn_t);
    ESP_ERROR_CHECK(ulp_process_macros_and_load(0, ulpProgram, &programSize));
    _ulpProgramInstructionCount = programSize;
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

bool UlpVanTx::SetAckIdentifier(uint16_t identifier, bool enabled)
{
    if (identifier > 0x0FFF)
        return false;

    uint16_t expanded = 0;
    if (enabled)
    {
        uint8_t high = (identifier >> 4) & 0xFF;
        uint8_t low = (identifier & 0x0F) << 4;
        expanded = ((STUFFED_VAL(high) & 0x03FF) << 5)
                 | ((STUFFED_VAL(low) >> 5) & 0x001F);
    }
    // One aligned RTC word replaces the complete 15-state comparison value.
    // The ULP's single I_LD sees either the old or the new identifier.
    ulp_ack_state[ULP_ACK_CONFIGURED_IDENTIFIER].word =
        enabled ? expanded : ULP_ACK_IDENTIFIER_DISABLED;
    return true;
}

uint16_t UlpVanTx::GetConfiguredAckIdentifier() const
{
    return uint16_t(ulp_ack_state[ULP_ACK_CONFIGURED_IDENTIFIER].word);
}

uint16_t UlpVanTx::GetAckCount() const
{
    // Read one aligned RTC word; ULP I_ST puts PC metadata in its upper half.
    return uint16_t(ulp_ack_state[ULP_ACK_COUNT].word);
}

uint16_t UlpVanTx::GetLastSampledIdentifier() const
{
    return ulp_ack_state[ULP_ACK_LAST_SAMPLED_IDENTIFIER].val & 0x7FFF;
}

uint8_t UlpVanTx::GetLastSampledCom() const
{
    return ulp_ack_state[ULP_ACK_LAST_SAMPLED_COM].val & 0x1F;
}

uint8_t UlpVanTx::GetLastEodCandidateGroup() const
{
    return ulp_ack_state[ULP_ACK_LAST_EOD_CANDIDATE_GROUP].val & 0xFF;
}

uint16_t UlpVanTx::GetUlpProgramInstructionCount() const
{
    return _ulpProgramInstructionCount;
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
            ulp_command[1 + i].val = ((STUFFED_VAL(data[i])) & (STUFFED_VAL(data[i])-1)) << 6;
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
    ulp_command[0].val = ULP_COMMAND_PENDING | (length + 1);
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