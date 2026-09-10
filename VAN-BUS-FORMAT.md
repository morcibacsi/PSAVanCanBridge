VAN bus frame format

The VAN bus used by PSA is based on ISO 11519-3. At 125 kbit/s, the fundamental bus time slice is 8 µs. Logical 0 is the dominant bus state and logical 1 is the recessive bus state.

It is important not to confuse VAN's Enhanced Manchester / E-Manchester coding with other Manchester-like encodings. In particular, a logical 0 is not represented by a long dominant period followed by a shorter recessive period, and a logical 1 is not the inverse of such a pulse-width encoding.

For the purposes of this implementation, think in terms of 8 µs bus time slices:

logical 0 = dominant for 8 µs
logical 1 = recessive for 8 µs

VAN groups the logical data into groups of four bits. The first three bits are transmitted normally. The fourth logical bit is followed by its logical inverse:

logical bits:       A B C D
bus time slices:    A B C D !D

Therefore four logical bits occupy five 8 µs time slices:

4 logical bits = 5 time slices = 40 µs

Another way of describing the same encoding, used in the original VAN controller documentation, is "three NRZ bits followed by one Manchester-coded bit": that fourth bit occupies two bus time slices, D followed by !D.

This is deterministic encoding and is different from CAN's data-dependent bit stuffing.

For example:

Logical nibble:     1000
On the bus:         10001
                         ^
                         inversion of the fourth logical bit

Logical nibble:     1010
On the bus:         10101

Logical nibble:     0001
On the bus:         00010
Frame fields

A VAN frame has the following general layout:

+-----+------------+-----+----------+-----+-----+-----+-----+
| SOF | Identifier | COM |   DATA   | FCS | EOD | ACK | EOF |
+-----+------------+-----+----------+-----+-----+-----+-----+

The field sizes are:

Field	Logical content	Bus time slices	Time at 125 kbit/s
SOF	special framing sequence	10 TS	80 µs
Identifier	12 bits	15 TS	120 µs
COM	4 bits	5 TS	40 µs
DATA	0–28 bytes	10 TS/byte	80 µs/byte
FCS	15-bit CRC	18 TS	144 µs
EOD	special violation	2 TS	16 µs
ACK	2 TS	2 TS	16 µs
EOF	8 recessive states	8 TS	64 µs

There must then be an inter-frame spacing of at least another 8 recessive time slices, although IFS is not part of the frame itself.

Thus, for a normal frame containing N data bytes:

total frame length = 60 + 10*N time slices
                   = 480 µs + 80 µs*N
SOF — Start Of Frame

The SOF occupies 10 bus time slices and is always:

0000111101

When represented in the same unencoded byte-oriented format used by the existing code and VAN analyzers, this corresponds to:

logical representation: 0000 1110 = 0x0E

E-Manchester:
0000 -> 00001
1110 -> 11101

SOF on bus:
00001 11101
= 0000111101

The protocol-level SOF should therefore be considered a 10-time-slice field, even if internally it is conveniently represented as the logical value 0x0E.

Identifier

The identifier is 12 logical bits, transmitted MSB first. E-Manchester expansion makes it 15 bus time slices.

The identifier is also used for arbitration: dominant 0 wins over recessive 1, so a numerically lower identifier has higher priority.

For example:

Identifier 0x8C4

logical:
1000 1100 0100

E-Manchester:
10001 11001 01001
COM / command field

COM consists of exactly four logical bits, transmitted in this order:

EXT  RAK  R/W  RTR

After E-Manchester encoding, the four logical bits occupy five bus time slices. The bit meanings are: EXT is normally 1/recessive; RAK=1 requests an acknowledgement; R/W=0 means write while 1 means read/reply-request semantics; RTR=0 means the frame contains data, while RTR=1 means no data is present.

For example, COM 0xC is:

C = 1100

EXT = 1
RAK = 1   ACK requested
R/W = 0   write
RTR = 0   frame contains data

E-Manchester:
1100 -> 11001

The R/W and RTR combinations are particularly relevant to reply requests:

R/W RTR
 0   0   normal write/data frame
 1   0   read/reply data
 1   1   request for another module to supply data
DATA

DATA is a sequence of bytes transmitted MSB first. Each byte consists of two logical nibbles, therefore each byte expands from 8 logical bits to 10 bus time slices.

For example:

8A = 1000 1010
     10001 10101

21 = 0010 0001
     00101 00010

40 = 0100 0000
     01001 00001

A critical property of VAN is that the frame contains no data-length field. There is no CAN-style DLC telling the receiver how many bytes of DATA follow the COM field. The receiver must determine the end of the variable-length portion from the EOD framing violation.

FCS

The Frame Check Sequence is a 15-bit CRC, calculated over the identifier, COM and DATA fields. SOF is not part of the CRC calculation.

The VAN CRC generator polynomial is:

g(x) =
x^15 + x^11 + x^10 + x^9 + x^8 + x^7
     + x^4 + x^3 + x^2 + 1

The remainder is initialized to 0x7FFF, and the CRC bits are inverted before transmission.

Because the FCS contains 15 logical bits, its E-Manchester representation occupies:

4 + 4 + 4 + 3 logical bits

5 + 5 + 5 + 3 bus time slices
= 18 time slices

This detail is important when parsing the end of the frame.

EOD — End Of Data

There is no explicit DATA length. Instead, the end of the transmitted data/FCS portion is marked by an intentional E-Manchester violation.

Normally the fourth logical bit of a group must be followed by its inverse:

fourth bit 0 -> 01
fourth bit 1 -> 10

At the end of the FCS, VAN instead transmits:

00

These two dominant time slices form the EOD sequence.

This has an important implementation consequence:

Do not simply scan the raw signal for any two consecutive zeroes. Consecutive dominant bits can legitimately occur elsewhere in the three NRZ positions of an E-Manchester group. EOD must be detected relative to the current E-Manchester phase: it occurs at the position where the next Manchester pair is expected, and 00 violates the required 01 or 10 transition.

The parser therefore has to maintain the E-Manchester group position while receiving. Once the expected Manchester pair is 00, EOD has been reached.

Since the FCS immediately precedes EOD and is always 15 logical bits / 18 bus time slices, after locating EOD the implementation can separate the variable DATA field from the fixed-size FCS even though there was no length information in the frame header.

ACK

ACK occupies two bus time slices.

The transmitting module normally releases the bus recessive during the ACK field:

11 = no acknowledgement

When an acknowledgement is required (RAK=1), a receiver acknowledges the frame by driving the second ACK time slice dominant:

10 = positive acknowledgement
11 = acknowledgement absent

Thus, although it is often called the "ACK bit", the entire ACK field is two time slices and the actual receiver-generated acknowledgement occurs in its second time slice.

EOF

EOF is eight consecutive recessive time slices:

11111111

Its duration at 125 kbit/s is 64 µs.

After EOF, the bus must remain recessive for the required inter-frame spacing before another frame begins.

Complete example

Use this corrected example:

0E 8C 4C 8A 21 40 3D 54

SOF:         0E
Identifier:  8C4
COM:         C
DATA:        8A 21 40
FCS:         3D 54

The byte/nibble-oriented logical representation is:

0000 1110 1000 1100 0100 1100 1000 1010
0010 0001 0100 0000 0011 1101 0101 0100

The field boundaries are:

       SOF          ID             COM
       |            |               |
       v            v               v
0000 1110 | 1000 1100 0100 | 1100 |
           8 C 4              C

              DATA                         packed FCS
              |                                |
              v                                v
1000 1010 0010 0001 0100 0000 | 0011 1101 0101 0100
   8A       21        40              3D       54

The corresponding actual bus time-slice sequence is:

SOF
00001 11101
|---------|
   10 TS

Identifier
10001 11001 01001
|----------------|
       15 TS

COM
11001
|---|
 5 TS

DATA
10001 10101 00101 00010 01001 00001
|-----------------------------------|
                 30 TS

FCS
00110 11010 01010 010
|--------------------|
        18 TS

EOD
00

ACK
11

EOF
11111111

Or on one line:

00001 11101 | 10001 11001 01001 | 11001 |
10001 10101 00101 00010 01001 00001 |
00110 11010 01010 010 | 00 | 11 | 11111111
\_________/   \_________________/   \___/
    SOF              ID              COM

More compactly, matching the original capture representation:

00001 11101 10001 11001 01001 11001
10001 10101 00101 00010 01001 00001
00110 11010 01010 01000 11 11111111

The final:

... 01000 11 11111111

must be interpreted as:

... 010 | 00 | 11 | 11111111
        |    |       |
        |    |       +-- EOF
        |    +---------- ACK absent
        +--------------- EOD

The 00 is therefore not a normal E-Manchester stuffing pair. It is the intentional E-Manchester violation marking EOD.

For this sample COM=C means RAK is set, so an acknowledgement is requested. The captured ACK field is nevertheless 11, meaning that no receiver actually pulled the second ACK slot dominant.

The FCS itself is only 15 logical bits. In this sample the displayed 3D54 is the convenient 16-bit packed representation associated with those 15 bits; the actual FCS bus portion ends after the first 15 logical CRC bits, and EOD follows immediately. That is why the final apparent five-bit group does not obey normal E-Manchester encoding.

Frame ownership / frame types

The distinction between the different VAN frame types is important for the ULP implementation because a single frame can be generated by more than one ECU. The VAN controller documentation explicitly describes this in-frame handover mechanism.

Normal frame without ACK. A transmitting/master module generates SOF, identifier, COM, DATA, FCS, EOD, the absent ACK sequence and EOF. RAK is 0/dominant, so receivers must not acknowledge. The ACK field therefore remains 11.
Normal frame with ACK. The transmitting/master module generates SOF, identifier, COM, DATA, FCS and EOD. RAK is 1/recessive. During the ACK field the transmitter releases the bus; a receiver that accepts the frame pulls the second ACK time slice dominant. The transmitting module then generates EOF. Thus essentially the entire frame comes from the transmitter except for the dominant acknowledgement state supplied by the receiver.
Reply request with immediate reply. This is an in-frame handover between two modules. The requesting module sends SOF, identifier and the first three COM bits EXT, RAK and R/W. For a reply request, R/W is recessive (1). The requester also presents RTR as recessive (1). If the requested module can answer immediately, it simultaneously drives RTR dominant (0). Because dominant wins over recessive on VAN, the bus sees RTR=0. This dominant RTR is effectively the requested module taking ownership of the remainder of the data-producing part of the frame. The requested module then transmits DATA, FCS and EOD. During ACK it releases the bus, and the original requesting module—now the receiver of the returned data—asserts the positive acknowledgement by driving the second ACK time slice dominant. The requested/replying module then sends EOF.

In ASCII form, the immediate reply handover is:

REQUESTING MODULE:
SOF | IDENTIFIER | EXT RAK R/W | RTR=1 ............... | ACK | .....
                                      ^
                                      |
                                  releases bus

REQUESTED MODULE:
................................. RTR=0 | DATA | FCS | EOD | .. | EOF
                                      ^
                                      |
                           dominant overrides RTR=1

FRAME ON BUS:
SOF | IDENTIFIER | EXT RAK R/W | RTR=0 | DATA | FCS | EOD | ACK | EOF

The key point for the ULP implementation is that the switch from requester to responder happens inside the COM field at RTR. There is no new SOF and no new frame between the request and the immediate response.

For completeness, VAN also supports a deferred-reply case. If the requested module does not pull RTR dominant, RTR stays recessive and there is no immediate DATA field. The requesting module finishes the request frame with FCS/EOD/ACK/EOF; the requested module can send the actual reply later as a separate reply frame.

This version should give Codex the low-level details it needs without allowing it to substitute a CAN-style stuffing algorithm or a different Manchester encoding.