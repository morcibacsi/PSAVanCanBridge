# KWP-HAB full telecoding transactions

Some RD45 firmware tracks participation of the complete configuration set before secured traceability can finalize telecoding. Writing an isolated configuration zone can leave F303 present. This patch ports the working transaction sequence from the author's local 3.7.0 changes; it is not a claim that every PSA KWP ECU requires this sequence.

Opt in by adding kwp_hab_telecoding_transaction to an ECU descriptor. The RD45 configuration is shown in rd45-kwp-transaction.example.json. This file is a descriptor fragment to merge into the matching ECU JSON, not a standalone ECU definition. Do not apply it to an unrelated ECU.

Both a single-zone Write and Write Modified Zones capture current B0/B1/B2/B3/B8/A0 payloads, replace only deliberately edited zones, validate lengths, write B0 -> B1 -> B2 -> B3 -> B8, reread A0 immediately and write that fresh A0 as the final step. Edited zones are read back before the UI marks them synchronized. No VIN, captured payload, traceability counter or security key is embedded.

A negative or wrong-zone write acknowledgement aborts the transaction. Invalid/duplicate transaction zones and invalid lengths are rejected. Failed writes still close the session; the operation is not atomic and preceding writes cannot be rolled back automatically. Empty transactions perform no I/O. Other KWP descriptors keep the ordinary write path, and UDS retains its existing traceability mechanism.

Use read_only: true for the A0 zone in the full ECU descriptor so users cannot edit the finalizer directly.

Run node test/web/kwp-transaction.test.cjs. Tests simulate fresh A0 changes, full ordering, preservation of unmodified values, length rejection and incorrect acknowledgements. F303 behavior requires physical RD45 validation after a local firmware build.
