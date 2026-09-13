# ESP32-C6 VAN LP-core architecture

This document describes the current ESP32-C6 VAN implementation. It is a
maintenance guide to the code as it exists now, not a record of the debugging
work that produced it.

Read [`../VAN-BUS-FORMAT.md`](../VAN-BUS-FORMAT.md) first for the protocol:
E-Manchester encoding, frame fields, FCS, EOD, ACK, EOF, and ReplyRequest wire
semantics. This document explains how the firmware implements those rules and
does not duplicate the protocol specification.

## Purpose and design rule

VAN requires bus sampling, arbitration, ACK assertion, and ReplyRequest
handover at precise sub-frame boundaries. A FreeRTOS task on the main CPU
cannot provide that timing reliably. The ESP32-C6 LP core therefore polls the
bus continuously and performs the operations whose deadlines are measured in
VAN time slices.

The main architectural rule is:

> The main CPU performs expensive or non-time-critical preparation and
> decoding. The LP core performs deterministic, timing-critical bus
> interaction.

The LP core has limited processing time and RAM. Do not move CRC calculation,
general byte assembly, or application policy into it merely to keep VAN logic
in one place.

### Main-CPU responsibilities

- expose VAN through `IVanMessageSender`, `IVanMessageReceiver`, and
  `VANTransportLayer`;
- construct complete logical frames and calculate the 15-bit FCS;
- expand bytes into the raw ten-state representation used for transmission;
- build ACK and requested-reply match data;
- publish persistent configuration safely to shared LP RAM;
- dequeue raw LP receive captures, decode E-Manchester groups, reconstruct
  bytes, and validate FCS in the transport layer;
- queue application TX requests and translate completed receive data into
  `BusMessage` objects.

The main implementation files are:

- `src/lib/esp32_ulp_lpc_core_van_tx/LpCoreVanTx.cpp`
- `src/lib/esp32_ulp_lpc_core_van_tx/VanFrameBuilder.hpp`
- `src/lib/esp32_ulp_lpc_core_van_tx/VanEManchesterDecoder.hpp`
- `src/Platform/Esp/Protocol/VANTransportLayer.cpp`

### LP-core responsibilities

- continuously monitor bus level and recognize an incoming SOF after a
  sufficient recessive run;
- wait for EOF plus IFS before starting local TX;
- drive and sample GPIO at absolute cycle deadlines;
- transmit prepared normal and ReplyRequest frames;
- perform arbitration and release the bus on loss or failure;
- compare an incoming prefix against configured ACK and reply candidates;
- track E-Manchester phase far enough to recognize a legal EOD in real time;
- assert the second ACK time slice when required;
- handle both sides of the in-frame RTR ownership transfer;
- capture raw five-state groups for later main-CPU decoding.

The timing-critical implementation is `ulp/main.c`. Shared HP/LP layout and
fixed capacities are defined in
`src/lib/esp32_ulp_lpc_core_van_tx/VanLpShared.h`.

## Application-facing abstractions

`IVanMessageSender` keeps `VANTransportLayer` independent of the hardware TX
backend. Its core operations start the backend, submit a normal frame, submit a
ReplyRequest, and report whether another submission is possible. Its optional
LP-oriented operations configure ACK identifiers, requester ACK behavior, and
prepared requested-module reply slots. Backends without those capabilities use
the default no-op implementations.

`IVanMessageReceiver::ReceiveData()` blocks until one receiver item is
available. A successful item is the logical byte representation used throughout
the application:

```
SOF byte | identifier high byte | identifier low nibble + COM | DATA | packed FCS
```

The maximum representation is 33 bytes: SOF, two identifier/COM bytes, up to
28 data bytes, and two packed FCS bytes. A zero returned length means that no
complete decodable frame was produced.

`VANTransportLayer` owns a 15-entry FreeRTOS TX queue. Its TX task waits until
the sender accepts another frame, then dispatches normal, query, response-slot,
or ACK-slot operations. On receive it uses the configured
`IVanMessageReceiver`, validates length and FCS, and produces a `BusMessage`.

## Prepared frame representation

`VanFrameBuilder` is main-CPU-only code. `Build()` validates the 12-bit
identifier, four-bit command, and 0-28-byte payload; computes FCS over
identifier, COM, and DATA; and E-Manchester-expands every byte.

Each `VanLpFrame` contains up to 34 32-bit words. The low ten bits of each word
hold ten consecutive raw bus states, most significant state first. The builder
turns the last packed-FCS inverse into the `00` EOD violation and appends one
word containing two recessive ACK states plus eight recessive EOF states.

The same representation is used for:

- normal frames (`COM=8` or `COM=C`);
- outgoing ReplyRequests (`COM=F`, no DATA);
- complete requested-module replies (`COM=E`).

Keeping one representation avoids rebuilding or copying a frame in a
time-critical handover.

## Shared state and configuration publication

The public shared ABI uses fixed-size, naturally aligned structures. It has no
dynamic allocation:

- one normal/query TX outbox;
- two `VanLpConfig` banks;
- five ACK candidates and five reply candidates per configuration;
- five complete prepared reply frames;
- one four-entry receive queue;
- fixed diagnostic/result records.

ACK and reply configuration is persistent. It is not consumed by a matching
transaction. `VanLpConfig::enabledMask` assigns bits 0-4 to ACK candidates and
bits 5-9 to reply candidates. `matchMask[30][2]` is a bit-sliced accelerator:
for each of the first 30 raw states and each observed bus level, it identifies
the candidates that remain possible. One GPIO sample, one table lookup, and an
AND therefore update all ten candidates.

The reply match entries are derived from the same complete `COM=E` reply frames
that will be transmitted. They are not separately maintained request patterns.
ACK candidates are built from SOF, identifier, and the complete `COM=C` prefix.
If duplicate reply entries match, the lowest numbered slot wins.

Configuration publication uses two banks and monotonically increasing
published/applied generation counters. Main-CPU writers serialize with a short
critical section, write only the inactive bank after the previous generation
has been acknowledged, execute a hardware memory fence, and publish the new
generation. The LP core adopts and acknowledges a bank only between bus
transactions, then pins that pointer for the complete receive/TX transaction.
It never waits for a main-CPU lock.

The concrete boolean slot APIs are nonblocking and reject an update when the
previous generation is still in use. The `IVanMessageSender` void configuration
methods wait for at most 50 ms in task context. Do not call those compatibility
methods from a timing-sensitive parse or generate path.

## Continuous monitor and normal TX

The LP core runs `monitor_once()` continuously with interrupts disabled. A
pending local transmission does not replace or suspend receive monitoring.
Incoming SOF detection remains active while the outbox waits for bus idle.

An eight-slice recessive run is sufficient to arm incoming SOF recognition. A
local frame requires at least 16 recessive slices, representing EOF plus IFS,
and performs a final GPIO check immediately before driving dominant SOF. A
competing transmitter that starts after that check is handled by arbitration.

Normal TX proceeds as follows:

1. The main CPU builds the complete frame and copies it to the single shared
   outbox.
2. Publication flags the request without blocking the LP monitor.
3. After the idle requirement, the LP drives SOF immediately.
4. `transmit_prepared()` walks the prepared words, changes TX at absolute slice
   edges, and samples RX near each slice center.
5. A transmitted dominant state that is not observed is a hardware/timing
   abort. A transmitted recessive state observed dominant normally loses
   arbitration, so TX is released immediately.
6. The ACK states in the prepared tail are recessive. For a normal frame the
   LP accepts a dominant second ACK state without treating it as arbitration
   loss.
7. The bus is released on completion, arbitration loss, or any abort.

The shared outbox is intentionally not a second queue. `TrySendFrame()` rejects
a submission while it is occupied; `VANTransportLayer` provides the
application-level queueing.

ESP32-C6 input reflection can remain briefly dominant after a local
dominant-to-recessive transition even when the output latch is already
recessive. For normal TX only, that exact apparent arbitration loss is confirmed
again at three quarters of the same slice. Genuine contention remains dominant
and still wins. Moving this confirmation later leaves too little time to load
and drive the next word-boundary state.

## Incoming frame matching, EOD, and ACK

`receive_frame()` samples the first 30 raw states: SOF, identifier, and COM.
It validates SOF, maintains E-Manchester phase, captures the six raw groups,
and eliminates ACK/reply candidates with the bit-sliced match table.

After the prefix, `track_response()` samples the four logical states of each
five-state group and treats the mandatory inverse state specially. It searches
for the expected edge and recenters future sample deadlines, bounding phase
drift to one group. If the edge occurs in the short interval after edge polling
stops, the fallback brackets the transition and still corrects the phase.

EOD is accepted only as a `00` violation at the D/inverse position, after a
complete 15-bit FCS and on a whole-byte boundary. `11` is malformed. Legal EOD
positions follow the protocol rule `48/49 + 10*N` for 0-28 DATA bytes. The LP
does not calculate or validate FCS; it recognizes enough framing to act before
the real-time ACK deadline, captures the groups, and leaves FCS validation to
`VANTransportLayer`.

For a matching configured normal frame (`COM=C`), ACK handling is:

1. track the response through the second EOD state;
2. verify the first ACK state is recessive;
3. drive only the second ACK state dominant;
4. hold it for one bus time slice;
5. release the bus before recording diagnostics or publishing receive data.

ACK drive and release share the same path for configured normal-frame ACK and
requester-side ACK of an immediate ReplyRequest response. Requester ACK is
enabled by default at `Start()` and remains configurable through
`SetQueryRequesterAckEnabled()`.

There are five ACK slots. A slot matches an identifier plus the complete
normal ACK-requesting command (`COM=C`), not merely an identifier.

## ReplyRequest requester role

An outgoing ReplyRequest is prepared as a complete `COM=F` frame. The LP sends
the prefix normally. At RTR it intends to send recessive. If a requested module
drives RTR dominant, this is expected ownership handover, not generic
arbitration failure.

The LP then:

1. releases its TX output immediately;
2. changes the captured COM group from the requested `COM=F` to the observed
   responder-owned `COM=E` form;
3. validates the responder's inverse state after RTR;
4. switches from transmit to phase-tracked receive without starting a new
   frame;
5. follows DATA and FCS to a legal EOD;
6. drives the second ACK state if requester ACK is enabled;
7. releases the bus so the responder can complete EOF;
8. publishes the combined frame for normal main-CPU receive decoding.

If RTR remains recessive, the LP completes the prepared request frame and
reports that there was no immediate response.

## ReplyRequest requested-module role

The main CPU prepares each configured response as one complete raw `COM=E`
frame, including SOF, identifier, COM, DATA, FCS, EOD, ACK, and EOF. Up to five
such replies are persistent in the active configuration bank.

The LP uses that same frame in both phases of the transaction:

- before RTR, its raw states are the comparison source for the incoming
  request prefix;
- one full slice before RTR, the LP resolves a matching slot and preloads its
  metadata;
- at RTR, it drives the prepared frame's dominant RTR state and takes
  ownership;
- after RTR, it continues from the same array and the already established
  word/bit index in `transmit_prepared()`.

There is no response buffer replacement, copy, CRC calculation, or index
reconstruction at takeover. Preserving this compare-to-transmit continuity is
essential to the RTR deadline. The responder releases both ACK states, samples
the second one to report whether the requester acknowledged, and then completes
EOF.

## Receive backends

The application can use either receiver through `IVanMessageReceiver`; protocol
handling above the transport does not depend on the backend.

### RMT receiver

`ESP32_RMT_VAN_RX` uses the ESP32 RMT peripheral to capture alternating level
durations. At 1 MHz RMT resolution it rounds durations to 8 us (comfort/125
KTS) or 16 us (body/62.5 KTS) slices, expands each duration into bus states,
skips each E-Manchester inverse position, and assembles logical bytes on the
main CPU. Completed RMT symbol buffers pass through a ring buffer. If the ISR
cannot reserve ring-buffer space, that capture is dropped and RX is restarted.

### LP-core receiver

`LpCoreVanTx` also implements `IVanMessageReceiver`. The LP core continuously
captures raw five-state groups while it performs the phase/EOD work already
needed for real-time ACK and handover. It does not assemble general receive
bytes. `LpCoreVanTx::ReceiveData()` copies one completed capture out of shared
RAM, advances the consumer index, and invokes `VanEManchesterDecoder` on the
main CPU.

The LP receive queue is a fixed four-entry single-producer/single-consumer ring.
The producer publishes `writeIndex` only after an entry is complete. If the
queue is full, the newest frame is dropped and `overflowCount` is incremented;
receive bookkeeping must never delay ACK, reply, or TX timing. Malformed and
occupancy counters are available through `GetReceiveDiagnostics()`.

The LP publishes frames it receives and complete frames produced locally, so
the selected receiver backend presents the same application representation.

Current ESP32-C6 application wiring in `src/Platform/Esp/main.cpp` uses
`LpCoreVanTx` as the sender and a separately started `ESP32_RMT_VAN_RX` as the
receiver. The alternative `VANTransportLayer(lpVan, lpVan)` wiring is present
but commented out. Thus LP receive is implemented and tested, but is not the
active application receive backend in the current configuration.

## 125 KTS and 62.5 KTS

Both speeds use the same frame format and E-Manchester representation. Only
physical time per bus state changes:

- 125 KTS: approximately 8 us per time slice;
- 62.5 KTS: approximately 16 us per time slice.

For LP operation, `LpCoreVanTx::Start()` queries the exact calibrated
`RTC_FAST` clock frequency with `esp_clk_tree_src_get_freq_hz()`. It passes the
frequency and selected `VanBusSpeed` to `VanBusTiming::TimeSliceCycles()`, which
rounds cycles per slice to the nearest integer. The main CPU publishes that
value once and the LP caches it before bus activity. Absolute TX, receive,
arbitration, and IFS deadlines derive from this cached slice count; they do not
assume a nominal 16 MHz oscillator.

The RMT backend selects 8 us or 16 us duration quantization from
`VAN_NETWORK_TYPE_COMFORT` or `VAN_NETWORK_TYPE_BODY` and doubles its maximum
accepted signal duration for the slower network.

The current ESP32-C6 bootstrap explicitly constructs both LP TX and RMT RX for
125 KTS/comfort VAN. Supporting 62.5 KTS in the classes does not by itself
select it for a board or protocol at runtime.

## Timing-critical implementation details

The current transmitter is deadline-based, not a train of source-level NOP
macros. Timing depends on both explicit cycle deadlines and the generated
instructions between a deadline and the physical GPIO access.

Important implementation choices are:

- `mcycle` is read with an inline RISC-V CSR instruction;
- signed deadline subtraction handles 32-bit counter wrap;
- LP interrupts are disabled so an interrupt cannot stretch ACK or RTR timing;
- direct 32-bit LP GPIO input and W1TS/W1TC MMIO accesses avoid IDF helper
  calls, bitfield code, and stack spills in the hot path;
- absolute deadlines prevent per-sample processing cost from accumulating as
  drift;
- inverse-edge resynchronization recenters receive sampling once per
  five-state group;
- a 16-cycle lateness guard fails a transaction instead of emitting badly
  timed bus states;
- ACK drive is scheduled 12 LP cycles early to compensate for the instructions
  and MMIO store after the wait, and ACK release adds a five-cycle hold because
  its generated path is shorter;
- diagnostic writes are kept out of the dominant ACK pulse and normal
  successful inner loops where practical.

The 12-cycle ACK advance, five-cycle hold extension, and 16-cycle deadline
slack are LP-cycle constants, while slice-relative deadlines scale with bus
speed. Re-evaluate their physical meaning at both 125 and 62.5 KTS when changing
clocking, compiler options, or the hot-path instruction sequence.

Apparently harmless C cleanup can change register allocation, branches, loads,
function inlining, and the delay between `wait_until()` and a GPIO transition.
Inspect generated RISC-V assembly and validate with a logic analyzer whenever
changing `wait_until()`, `sample_inverse()`, `track_response()`,
`transmit_prepared()`, `receive_frame()`, GPIO helpers, or their nearby control
flow. Host simulation verifies protocol decisions but does not model real
instruction fetch, MMIO synchronization, transceiver delay, or electrical bus
timing.

## Invariants and gotchas

- Keep general E-Manchester decoding, byte assembly, FCS validation, and frame
  construction on the main CPU.
- Keep LP/shared structures fixed-size and allocation-free.
- Never disable incoming monitoring merely because normal TX is pending or
  waiting for IFS.
- Start local TX only after the full idle requirement and a final bus-level
  check.
- EOD is a phase-specific `00` E-Manchester violation. Never search for an
  arbitrary pair of dominant states.
- A missed inverse edge observed by the center sample must still correct future
  receive phase; accepting the level without recentering can create false EOD.
- `11` at the D/inverse position is malformed, not EOD.
- Requester loss of recessive RTR to dominant is expected ownership transfer;
  it must enter same-frame response tracking before generic arbitration-loss
  handling.
- A requested-module reply uses one complete prepared frame for both prefix
  comparison and post-RTR transmission. Do not split those representations.
- Resolve reply metadata before the narrow RTR takeover window.
- ACK is the second state of a two-state field. Verify the first state is
  recessive, drive only the second, and release promptly.
- ACK timing depends on surrounding generated instructions, not just an
  explicit wait or constant.
- Normal TX has a deliberate late confirmation only for stale RX after a local
  dominant-to-recessive transition. Do not apply it indiscriminately to other
  roles or recessive arbitration states.
- ACK and reply configuration persists until explicitly replaced or disabled.
- The limits of five ACK entries, five reply entries, 28 DATA bytes, 34 frame
  words, and four queued receive captures are deliberate shared-memory and
  timing bounds.
- Adopt a newly published configuration only between transactions; never swap
  configuration banks during a frame.
- On an unexpected level, malformed phase, missed deadline, invalid length, or
  arbitration loss, fail toward releasing the bus before diagnostics.
- A software completion result proves the state machine completed. Physical
  GPIO and transceiver timing still require hardware observation when a timing
  path changes.

## Diagnostics and validation

`VanLpResult` separates local TX completion from continuously observed bus
results. Abort details encode the stage in the high 16 bits and zero-based raw
time-slice index in the low 16 bits. Optional arbitration, GPIO, EOD-index, and
bounded receive-pair diagnostics are read by the main CPU after the critical
operation; they must not become unbounded logging in LP timing paths.

Run the host suite after changing framing, matching, state transitions, shared
layout, or speed scaling:

```
python test/van_lp/run_tests.py
```

The suite compiles the actual LP C routines into a GPIO/cycle simulator and
covers both speeds, all slots and lengths, ACK, RTR handover, malformed frames,
deadlines, counter wrap, drift, queue overflow, and pending-TX monitoring. It
does not replace an ESP32-C6 firmware build, embedded-LP verification, generated
assembly inspection, or logic-analyzer validation of timing-sensitive changes.
