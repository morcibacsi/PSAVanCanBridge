# ESP32-C6 VAN ACK / immediate reply implementation

Protocol reference: `VAN-BUS-FORMAT.md`. Target: 125 kbit/s, 16 MHz LP core.
Implementation and host simulation are complete; bus timing has **not** been
validated on ESP32-C6 hardware by this change.

## Baseline and experimental changes

The committed baseline is `0147c88`; the relevant normal TX timing change is
`302e1ca` (unrolled NOPs), following `a48f4f8` (timing adjustment) and `f57b094`
(trailing word). The pre-task ACK/reply changes in the two implementation files
and public header were uncommitted. They were reviewed separately from this
baseline. Unrelated working-tree changes were left alone.

Preserved: CRC polynomial/init/inversion/packing, identifier/COM serialization,
SOF, ten-state E-Manchester words, the final CRC-word EOD operation, and the
trailing `0x3ff` word. That word is two ACK slots plus eight EOF slots. Normal
TX still uses the original GPIO helpers, nested word/bit loops and NOP macros,
including the historically named `DELAY_15_CYCLES` macro. Its body was not
retuned. The loss branch now permits a dominant second ACK slot instead of
misclassifying receiver ACK as arbitration loss. Other losses retain the
existing retry semantics, with monitoring resumed while waiting to retry.

Kept from the experiment: sharing frame preparation, the compatibility setter
names, and the idea of phase-aware EOD tracking. Corrected/replaced: ID-only
ACK matching (now complete COM=C), unsafe configuration publication, acceptance
of `11` as EOD, 16 us ACK assertion, matching the last query as a separate frame,
and the blocking IFS-only wait when TX is pending. Removed: the 16-entry ACK
arrays, single continuation-only reply buffer, copying replies into `VAN_DATA`,
late takeover after the entire COM field, and reply FCS calculated with COM=F.
Immediate reply FCS now covers the actual on-wire COM=E.

The old normal buffer had 33 words: 28 payload bytes need **34**, including
ACK/EOF. Bounds checks and the corrected capacity cover the full documented
0–28-byte payload range. Invalid arguments or a busy outbox cannot overwrite
an existing pending TX.

## Main CPU and persistent configuration

`VanFrameBuilder.hpp` contains the shared HP-only CRC/serialization/encoding
implementation. Normal frames, outgoing requests, and complete prepared reply
frames all use it. No CRC, encoding, data copying, or payload-length decoding
runs on LP during reception or reply takeover.

Each configuration bank contains five complete reply frames, one combined
prefix matching table, an enabled-candidate mask, and the requester-ACK option.
Each reply contains 34 fixed word slots and a word count; zero count disables
the slot. There are no separate request-prefix/response-continuation arrays.
RTR is always raw slice 28. The last word supplies ACK[2]+EOF[8], so redundant
per-frame RTR/ACK/EOF offsets are unnecessary.

The 30-by-2, 32-bit matching table is a deliberate comparison accelerator:
each entry holds the surviving candidates for one observed raw bus state.
HP derives reply candidates directly from their complete prepared frames and
ACK candidates from SOF/identifier/COM=C. It replaces ten comparisons per
sample with one table load and AND. This is the small auxiliary representation
needed to keep five ACKs and five replies within the LP sampling budget; it is
not an independently constructed request or response bitstream. Candidates
are eliminated immediately, and one GPIO sample serves all ten candidates.

Updates use two banks and 32-bit published/applied generations. HP only writes
the inactive bank after LP has acknowledged the preceding generation. HP
publishes after a hardware fence. LP acquires and pins a bank between
transactions, then acknowledges it with a fence; it never changes that pointer
during a receive/reply/request transaction. The old bank can then be reused.
HP task writers serialize with a short critical section. All shared control
accesses are volatile and publication/consumption has hardware memory fences.
LP never takes an HP lock or waits for an update.

Public C6 APIs:

| API | Behavior |
| --- | --- |
| `ConfigureAckFrame(slot, id, enabled)` | Set/replace/remove ACK slot 0–4; matches normal COM=C only. |
| `ConfigureReplyFrame(slot, id, data, bytes, enabled)` | Set/replace/remove reply slot 0–4; bytes 0–28. |
| `IsConfigurationReady()` | Previous published bank has been applied. |
| `TrySendFrame(id, data, bytes, command, query)` | Nonblocking single outbox: commands 8/C for normal, F with zero data for query. |
| `GetLastTxResult()` / `GetLastBusResult()` | Separate integer diagnostics for local TX and incoming transactions. |

The new boolean setters return false for invalid input, not started, or a
previous update still outstanding. Retry valid busy updates from task context.
The existing void setters remain compatible: `SetAckIdentifiers` replaces the
whole ACK set (maximum five); `SetRequestedReplyFrame` addresses reply slot 0;
`SetQueryRequesterAckEnabled` changes the persistent requester option. These
legacy configuration calls wait at most 50 ms for the preceding update and
cannot report a rejected update through their old void interface. Use the new
boolean slot APIs when application-level confirmation is needed. Do not call
the compatibility configuration setters from timing-sensitive parse/generate
paths. Query ACK defaults enabled after `Start()`.

## LP monitoring and in-frame ownership

One continuous monitor recognizes incoming SOF and observes bus idle even when
the outbox is occupied. A pending normal/query frame starts only after at least
16 recessive slices (EOF+IFS), with a final GPIO check before driving SOF.
Incoming ACK/reply handling never consumes or replaces that pending frame.
There is one outbox, not a new queue: the second local submission is rejected
while the first remains pending. Duplicate reply matches select the lowest slot.

After a normal ACK prefix matches, LP tracks E-Manchester phase through DATA
and FCS. Only `00` at the fourth-bit/inverse positions can indicate EOD; `11`
aborts. Legal EOD positions are 48/49 + 10*N for N=0..28. The minimum FCS and
whole-byte alignment are checked without deriving a DLC. The scan is bounded.
LP verifies the first ACK slot is recessive, drives only the second ACK slot,
and releases at its next boundary. CRC validity is not checked on LP.

A requester sends COM=F. Dominant RTR at raw slice 28 causes immediate release
and same-frame receive tracking. LP validates the responder's COM inversion,
then tracks to EOD and ACKs, without matching the identifier again or consulting
HP. If RTR remains recessive, the prepared no-data request's FCS/EOD/ACK/EOF is
completed. Disabling requester ACK still permits tracking the immediate reply.

As responder, LP selects the complete reply array and fetches its length at
RAK (slice 26), leaving the critical R/W-to-RTR interval free of candidate
selection work. After R/W matches, the same SOF-based word/bit cursor advances
once into RTR. Its preloaded dominant RTR state is driven before setting up
the remaining transmit loop, giving that setup a full slice. The inlined
transmitter continues the same array's inverse, DATA, FCS and EOD.
It releases for both ACK slots, samples the second
for diagnostics, and finishes all eight EOF slices regardless of ACK presence.
No buffer copy or index reconstruction occurs at handover.

Unexpected encoding, invalid lengths, missed deadlines and lost arbitration
release the bus. LP interrupts are disabled for this continuously running
polling participant. There is no LP logging or allocation.

## Timing review, memory and validation

New paths use absolute `mcycle` deadlines (128 cycles per 8 us), rather than
adding a full delay after each sample's work. Mandatory inversion edges
re-center receive timing once per five-state group, bounding clock drift.
Deadline lateness beyond 16 cycles aborts; edge errors beyond a quarter slice
also abort. These are implementation limits requiring hardware characterization,
not proof of a physical timing tolerance.

RISC-V disassembly was inspected. The matcher has no division, CRC or allocation
calls. Reply transmission is inlined and candidate selection precedes R/W.
The new GPIO operations use C6 LP_IO input and W1TS/W1TC registers directly:
the IDF bitfield helpers under `-Os` otherwise introduced calls and stack spills.
Normal TX deliberately retains its original helper/NOP path. The LP build uses
`-Os -march=rv32imac_zicsr_zifencei` with the installed ESP-IDF toolchain.

Memory at five plus five entries:

| Storage | Bytes |
| --- | ---: |
| Five complete reply slots per bank | 700 |
| Combined ACK/reply prefix table per bank | 240 |
| Enabled mask and requester option per bank | 8 |
| Two complete configuration banks | 1,896 |
| HP shadow configuration | 948 (ordinary HP RAM) |
| One-shot LP TX word buffer | 136 |

The reviewed LP ELF has 3,696 text bytes, 20 data bytes and 2,112 BSS bytes:
5,828 total, within the 8,128-byte reservation, with room for stack/runtime.
Five entries in each set did not need to be reduced.

Run `python test/van_lp/run_tests.py`. The harness compiles the actual LP C
routines against simulated GPIO/cycle functions and independently checks the
builder against a logical-bit CRC/encoding reference. Passed coverage includes
all 256 byte expansions, all 4,096 identifiers, every payload length, the
documented `3D54` FCS example, all five ACK and reply slots together, replacement,
removal, invalid input, wrong COM, partial matches, EOD legality, exact simulated
ACK slot/width, responder takeover and complete output, requester immediate and
unanswered requests, ACK absent/present, clock drift, counter wrap, injected
lateness, pending TX coexistence, and bank publication during an active frame.
The simulator does not model RISC-V instruction, memory, or GPIO latency. The
legacy normal NOP loop's physical timing is explicitly outside its assertions.

The `esp32c6_v16` firmware build passed. Existing repository warnings are not
timing evidence. Before production deployment, capture SOF centering, normal TX
and arbitration, EOD with 0/28-byte payloads, normal ACK placement/8 us width/
release, RTR takeover and its inversion/first DATA state, requester ACK,
responder ACK release and EOF, back-to-back minimum-IFS frames, and incoming
traffic/configuration updates while normal TX is pending. Exercise all five
candidates, temperature/LP clock variation, and transceiver propagation delay.
No firmware was flashed and no on-vehicle validation was performed here.
