# VAN LP transmit optimization

## Current status: optimized architecture restored; SOF failure under investigation

The user explicitly requires continued development of the optimized transmitter.
Do not wholesale-restore the baseline without an explicit user request. The
optimized LP code is restored in **80533b2** and matches the saved optimized ELF's
loadable sections byte-for-byte. Neither the original optimization nor its
rollback had been committed; both were working-tree changes. The restoration
used the recorded source diff, then verified the result against the saved binary,
rather than assuming an approximation was equivalent.

The next increment exposes existing completion diagnostics on HP for the two
reported normal IDs, 5E4 and 8A4. The earlier transport printed only selected
query results, so normal failures were invisible. It now reads completion before
another submission, tracks only accepted frames, and polls completion when the
queue is empty. `VAN_LP_TRACE_NORMAL_TX=0` disables this temporary trace. No LP
instructions, timing constants, GPIO operations or arbitration rules changed in
this diagnostic increment. LP binary SHA256 remains
`69b0f73308d965c44b53283da5dd3f264142a1eee2d6bec89f1b6ccc4a2c4cf4`.

### First difference and unresolved root cause

Expected SOF is `0000111101`, including inverse states. The capture lacks the
later dominant SOF state at raw index 8 and the subsequent identifier. The first
observable divergence is therefore within SOF, not DATA/FCS/ACK processing.
The image alone cannot identify the exact abort point: an early abort releases
the same line that should already be recessive during SOF slots 4–7.

The first execution difference is earlier: the optimized path schedules and
rewrites repeated dominant states and samples them; baseline normal TX used
NOP compensation and sampled only recessive states. The new deadline checks can
also exit before a later state is written. Neither a deadline violation nor a
readback/arbitration error is yet established as the hardware root cause.
Host tests do not model the cost of LP instructions or MMIO, so their success
does not resolve this. Changing a timing value now would stack an untested
hypothesis on an unidentified failure.

| Critical point | Inspection/result |
|---|---|
| IFS/start | Monitor retains continuous-high threshold and final idle read; optimized path starts SOF immediately, sets next deadline afterward |
| GPIO 0/1 | W1TC/W1TS address arithmetic agrees with installed C6 register definitions; saved assembly stores before sampling |
| 8 us states | Nominal 128-cycle deadlines; physical instruction/MMIO margins remain unmeasured |
| Buffer/index | Starts after SOF[0], descends bit 8 to 0, then increments pointer by four and resets bit to 9; both reported frames pass full transition-order tests |
| Arbitration | Scheduled half-slice read; dominant readback guard is new for normal TX; only ACK[1] permits recessive mismatch |
| Failure/release | Deadline/readback abort and arbitration loss release TX; HP previously omitted normal completion logging |
| EOD | Prepared positions unchanged; not reached in the reported capture |
| ACK | Last-word bits 9/8 unchanged; acceptance only in bit 8 |
| EOF | Same eight released states and final deadline; not reached in capture |
| Assembly correction | None yet: diagnostic increment deliberately leaves LP binary identical to the failed candidate |

Original optimizations retained: unified prepared loop, cached ten-state words
and length, direct GPIO masks, pointer increments, absolute deadlines and
compile-time TX roles. None is removed in this increment. Text remains 4062
bytes versus baseline 4558. Detailed instruction/load/branch comparison below
continues to apply; it is not a cycle-accurate timing model.

### Next hardware observation

Flash this diagnostic build and capture the same 5E4 or 8A4 attempt with its
`VAN normal ... completed:` serial line. Abort results include stage and raw_ts,
counted from SOF[0] with inverse states included. No startup identity check is
requested. Measure the initial low duration and whether the low state at slot 8
appears; distinguish an ordinary SOF release from a stretched initial pulse.

- `TX_EDGE_DEADLINE`: next output edge was already late; raw_ts identifies it.
- `TX_SAMPLE_DEADLINE`: center sample was late; raw_ts identifies that slice.
- `TX_DOMINANT_NOT_SEEN`: RX reported recessive while TX drove dominant.
- `ARBITRATION_LOST`: RX reported dominant at a transmitted recessive state.
- `NORMAL_TX_COMPLETED` with the same truncated capture: software reached EOF;
  investigate the electrical TX/RX path and measured waveform, not an abort.

These outcomes require different corrections. Keep this observation separate
from a timing correction so the next result discriminates between them.

Validation: full VAN host suite (including both supplied normal frames) and
`pio run -e esp32c6_v16` pass; embedded LP bytes and 19 shared addresses match.
Build log: `.pio/van_lp_tests/tx-sof-diagnostic.log`. Nothing was flashed here.
Root-cause correction remains pending the normal-TX completion evidence.

## Historical rollback (superseded by the restoration above)

The user's logic-analyzer test showed only the initial four dominant SOF slices,
followed by recessive bus, instead of either complete normal frame:
`0E 5E4 C 20 1E 8A 50` or `0E 8A4 8 8F 47 FF 38 31 0F FF 39 FC`.
This rejects the proposed normal-TX replacement despite its host-test results.
The screenshot alone does not identify the exact abort stage or establish whether
the release was an explicit abort versus the start of the SOF recessive run.
The replacement's deadline/readback checks and generated timing remain suspects,
not a confirmed diagnosis. No timing limit was loosened to mask the failure.

`ulp/main.c` has been restored byte-for-byte from **b731b97**, restoring the
measured NOP normal transmitter and baseline ACK/requester/responder behavior.
The temporary role selection and normal-deadline wrapper/tests were removed.
The 496-byte saving is withdrawn. The analysis below is retained as the history
of the rejected candidate; it does not describe the current production path.

New regression cases send both reported frames through the actual monitor's
normal-TX branch and check completion, all GPIO transitions in order, independent
encoding, and the supplied CRCs (8A50 and 39FC). These do not emulate NOP timing.
The full VAN host suite passes; the restored source retains its two existing
signed/unsigned comparison warnings. Hardware retesting is still required to
confirm the flashed result. No firmware was flashed by this change.

Restoration build: `pio run -e esp32c6_v16` passed (log:
`.pio/van_lp_tests/tx-restore-baseline.log`). The rebuilt LP's loadable sections
including executable code match the saved pre-optimization ELF byte-for-byte
and at the same addresses. Embedded LP bytes and all 19 used HP/LP shared
symbol addresses also match. This verifies restoration locally, without asking
the user to repeat a startup identity check.

## Phase 1 — findings before implementation

The user selected **b731b97** as the behavioral baseline, superseding the
attachment's 971420e reference. The LP source and HP sender initially match
b731b97; unrelated working-tree changes are outside this work.

Baseline normal TX is a separate 758-byte function in the current -Os RISC-V
build. Each ten-slice word is loaded ten times. It reloads the volatile length
at word boundaries, reloads pin numbers, and computes GPIO masks in its loop.
The IDF GPIO bitfield helpers compile into register reads and stack byte
stores/reloads even for write-one set/clear operations. GPIO output changes
only on transitions; repeated levels instead execute 20 NOPs. State-dependent
NOP blocks compensate the remaining paths. The misleading DELAY_15_CYCLES
macro contains ten NOPs; DELAY_90_CYCLES consequently contains 85.

Normal TX samples only recessive output, after a short NOP delay and helper
setup. Its arbitration sample is not an explicit half-slice deadline. A
dominant second ACK slot is accepted; other recessive mismatches abort. Retry
and pending-TX monitoring are managed outside the transmitter.

Requester and responder already share an always-inlined deadline transmitter:
one aligned word load per ten slices, direct GPIO mask writes, a scheduled
mid-slice sample, and absolute next-edge deadlines. RTR takeover is a special
branch before generic arbitration loss. The responder enters the same prepared
array at word 2/bit 0 after driving RTR; it does not copy a continuation buffer.
All failure paths release TX. EOD receive synchronization is separate.

Baseline LP text/data/BSS: **4558 / 28 / 2324 = 6910 bytes**. Captured artifacts:
`.pio/van_lp_tests/tx-review/before.elf` and `before.asm`. Full baseline build
log: `.pio/van_lp_tests/tx-review-before.log`.

### Increment selected

Reuse the existing deadline transmitter for normal frames using a compile-time
role. This removes the duplicate NOP engine and its transition compensation;
each normal slice gets the same edge/sample deadlines. Keep the packed ten-bit
word representation, HP preparation, persistent tables, ownership, and retries.
This explicitly changes normal arbitration sampling to the nominal half-slice
point and adds dominant-output mismatch/deadline aborts, matching the existing
prepared transmitter's fail-recessive behavior. Those timing/fault differences
must be called out and checked on hardware; this is not cycle-identical NOP
substitution. Compare generated code and behavior before keeping the change.

### Alternatives evaluated

- Merely caching words/masks in the NOP engine shortens its measured timing;
  compensating with guessed NOP counts is not a sound optimization.
- One byte per slice uses 340 bytes per frame instead of 136 (2244 additional
  bytes across the outbox and ten banked replies). GPIO-operation words use
  1360 bytes/frame. Neither is justified while word extraction is only shift
  plus mask; the larger encodings put severe pressure on the 8128-byte LP RAM.
- A dedicated assembly TX loop would duplicate working ownership/abort logic
  and toolchain-specific constraints. First evaluate the existing direct-MMIO
  deadline loop; assembly is not justified without a remaining measured issue.
- ACK pulses and receive synchronization should not be routed through a generic
  frame abstraction. They already use direct writes and absolute deadlines.
- Do not unroll full frames or move CRC/encoding onto LP. No new HP preparation
  or shared ABI is needed for the selected change.

## Implemented comparison with b731b97

`ulp/main.c` now selects a compile-time normal/requester/responder role in
`transmit_prepared`. Normal TX uses the same prepared-word executor as the two
existing modes. The old normal transmitter and all its NOP macros are removed.
The monitor still drives the first SOF state immediately after its final idle
check, then enters at word 0/bit 8. The existing requester follows that pattern;
the responder still enters at word 2/bit 0 after taking RTR dominant.

This one increment also supplies the useful loop optimizations: one volatile
word load per ten states, cached length, precomputed masks, and compiler-generated
pointer increments. Further pointer rewrites or assembly were not warranted.
The role has no dispatch branch or function call per slice in the generated code.
ACK pulse generation remains separate. No processing moved from HP to LP.

`LpCoreVanTx.cpp`, `VanFrameBuilder.hpp`, and `VanLpShared.h` need no changes.
HP still performs CRC, E-Manchester expansion, prefix-mask preparation, and
publication of immutable frames/configuration. The aligned 32-bit words still
contain ten bus states each. No shared fields, table capacities, linker settings,
board pins, schedules, allocations or task behavior changed.

### Generated instructions

Artifacts use the installed ESP-IDF 5.4.1 RISC-V compiler at `-Os`, with
`rv32imac_zicsr_zifencei`. The before/after ELF and disassembly are under
`.pio/van_lp_tests/tx-review/`. After disassembly includes source annotations.

| Operation | Baseline normal TX | New normal TX |
|---|---|---|
| Frame load | Ten loads per ten-state word | One load per word |
| Extract state | Shift/mask plus repeated indexed access | `srl`, `andi`; cached word |
| Advance word | Index and volatile length handling | Pointer `addi 4`; cached length |
| GPIO write | Transition-dependent IDF helper and NOP alternative | Every slice: stack load of base, shift, subtract, MMIO store |
| GPIO sample | IDF helper, only for recessive state | MMIO load and mask, every scheduled sample |
| Delay | Multiple state-dependent NOP blocks | CSR/subtract/branch deadline loop |
| Role selection | Separate normal function | Constant-folded; no requester RTR branch in normal loop |

In the after ELF, normal word loading is at `0x50000bb8`, extraction at
`0x50000bbc`, GPIO write at `0x50000be0`, sample at `0x50000c0e`, and loop advance
at `0x50000c3c`. The ordinary successful non-final-word slice takes 24 executed
instructions if each of the two waits executes once; each extra wait iteration
adds three. Dominant and recessive success paths both have that count, despite
different branch outcomes. A word boundary adds pointer/index/reset/branch plus
the next load. Final-word ACK checks add branches. These are instruction counts,
**not measured processor cycles**: branch, CSR, MMIO and instruction-fetch costs
must be measured on the LP core.

The write sequence is identical for 0 and 1: the level chooses W1TC versus W1TS
by address arithmetic. The compiler retains one stack load of the GPIO base per
slice; forcing registers or expanding the shared frame format solely to remove
that load is not justified. No per-slice multiplication remains on successful
paths; raw-index multiplication is confined to abort diagnostics.

The initial deadline expires before the GPIO store by several instructions;
the sample deadline similarly precedes the actual input load. Nominal half-slice
sampling therefore means a scheduled center, not a claim of exact 4.000 us
electrical sampling. Fixed overhead largely cancels between consecutive edges,
but first SOF, word boundaries and late waits require measurement.

Requester RTR still branches directly to release, inverse-state sampling and
response tracking. The responder still releases through prepared recessive
states for ACK and continues through EOF. ACK[1] alone permits dominant input
while transmitting recessive. The final deadline precedes explicit release.
Generated receive/ACK code retains direct writes and timed release; the separate
`track_response` function remains 710 bytes. Register placement in inlined paths
can change even when their C behavior is unchanged, so their hardware timing
remains part of regression validation.

### Timing and behavior differences

- The calibrated slice length, half-slice calculation, 16-cycle deadline slack,
  IFS thresholds and ACK pulse duration are unchanged. Normal TX now actually
  uses the calibrated deadline timing instead of the fixed NOP engine.
- Normal arbitration is sampled at the scheduled half-slice point instead of
  the old helper/NOP-dependent point. The arbitration rule and retry policy are
  unchanged, but a disturbance confined to one sampling window may be classified
  differently. This is an intentional timing difference, not exact equivalence.
- Normal TX now aborts on missed deadlines and on dominant output read back high,
  just as the existing requester/responder executor does. These are additional
  fault checks; they return protocol/timing abort rather than normal completion.
  Every such path releases TX. Ordinary arbitration loss still returns its
  distinct result and retains pending normal TX according to the retry limit.
- Normal frames still report normal completion whether ACK[1] is present or
  absent. No new normal-TX ACK-result API is introduced.
- SOF, identifier, COM, DATA, FCS, EOD, both ACK states and all eight EOF states
  use the same prepared bits. Rewriting a repeated GPIO level creates no intended
  extra edge. EOD and EOF are not shortened.
- All five ACK entries and five reply entries remain available. Pending local
  TX still monitors incoming traffic; configuration swaps remain between frames.
  Reply compare and continuation use the same full frame. Requester ACK handling,
  bounded diagnostics and completion ownership are unchanged.

### Memory

| LP ELF section | b731b97 | Optimized | Change |
|---|---:|---:|---:|
| Text | 4558 | 4062 | -496 |
| Data | 28 | 28 | 0 |
| BSS | 2324 | 2324 | 0 |
| Total reported by `size` | 6910 | 6414 | -496 |

Text shrinks by 10.9%. The old 758-byte function disappears while `main` grows
from 1872 to 2134 bytes with the inline normal path. These are section sizes,
not a runtime stack high-water measurement. The reserved/padded LP binary stays
8128 bytes. Frame arrays, configuration banks and diagnostic buffers are
unchanged. Symbol addresses move with code size; regenerated HP linker exports
are checked against the resulting LP ELF.

## Validation performed

- Full existing project-owned VAN LP host suite passed before and after changes.
  This includes independent encoding/CRC checks, all identifiers and payload
  lengths, all configuration slots, ACK/RTR modes, malformed frames, timing
  failures, wrap/drift and pending-TX monitoring.
- Added 464 normal-output cases: lengths 0–28, COM 8/C, payload patterns
  00/FF/AA/55, with and without remote ACK. Recorded GPIO transitions match the
  independent reference through EOF, across the 32-bit cycle-counter wrap.
- Tested a dominant competitor at every recessive state of a maximum-length
  normal frame; only ACK[1] is accepted. Added dominant-readback failure,
  late-edge abort and invalid-length checks; all release TX.
- Added actual-monitor arbitration tests for retry exhaustion and subsequent
  successful retry after a fresh idle interval. Existing monitor tests still
  service ACK/reply traffic while a normal frame is queued.
- `python test/van_lp/run_tests.py`: pass, no compiler warnings in the final run.
  Fake time validates scheduling logic, not RISC-V execution-time margins.
- `pio run -e esp32c6_v16`: successful baseline and optimized builds. Logs are
  `.pio/van_lp_tests/tx-review-before.log` and `tx-review-after.log`.
- Local artifact verification: all 8128 embedded LP bytes match firmware ELF and
  flash image; all 19 used shared-symbol addresses match. This is a local build
  check and imposes no startup identity check on the user.
- Scoped `git diff --check`: passed. Vendor component tests requiring their own
  targets/hardware were not run. No device was flashed or electrically tested.

## Hardware validation still required

Compare the same traffic against b731b97, probing TXD and bus RX together where
possible. Use short and 28-byte normal frames, 00/FF/AA/55 data, with and without
ACK, plus the established 0x564 immediate reply and configured responder mode.

1. Measure 8 us state spacing for both polarities, repeated runs and every
   ten-state word boundary. Check first SOF separately and drift over a maximum
   frame. Compare scheduled sampling with transceiver propagation delay.
2. For N payload bytes, check EOD at raw slots `48+10N` and `49+10N`, ACK[0] at
   `50+10N`, ACK[1] at `51+10N`, and eight EOF slots immediately afterward.
   Raw indices count from SOF[0], including inverse states. For a 27-byte reply,
   EOD is 318/319 and ACK[1] is 321.
3. Force a competing dominant state during recessive normal TX. Verify release,
   the new sample point, no continued transmission, and retry only after idle.
   Check dominant ACK[1] is accepted while ACK[0]/DATA/EOF contention is not.
4. Verify incoming SOF can still win before local TX starts. Measure the monitor's
   16-slice continuous-high threshold to TX, including its existing conservative
   behavior after a completed transmission; it is not a new exact IFS-edge timer.
5. Check requester RTR slot 28 release/takeover, inverse slot 29, response data
   from slot 30, correct ACK[1] position/8 us width and release afterward.
6. Check responder RTR takeover and the first transmitted inverse/data states;
   verify ACK release/sampling and all EOF states. Repeat while normal TX waits.
7. For temporary timing instrumentation only, an otherwise unused board-safe
   debug GPIO can mark immediately before `read_bus()` in `sample_at` and after
   the read. That instrumentation perturbs timing: measure its added cost and
   remove it afterward. No permanent debug writes were added.

Until these measurements pass, the change is a structurally tested optimization
candidate, not a newly hardware-proven baseline. The baseline commit remains
untouched. To restore this optimization's firmware behavior, restore only
`ulp/main.c` from b731b97 and rebuild; do not reset unrelated working-tree work.
