# Telecoding diagnostics

The telecoding page accepts the existing KWP-HAB and UDS ECU JSON format. Optional diagnostic_actions and diagnostic_tests may be arrays or objects keyed by action ID. Existing definitions without those fields retain their zone controls.

An action supplies request (hex), optional positive_prefix, label, button_style, requires_unlock, confirm, start_session, stop_session and ecu_resets. response_handler can be raw, dtc or clear_faults. DTC parsers are psa_kwp_compact, kwp_2byte_status and uds_1902; select the parser matching the ECU response format. Optional dtc_definitions/dtc_base_definitions and dtc_fault_types add descriptions. Raw ECU responses remain visible.

A diagnostic test can supply a single request or a steps array with request/positive_prefix/response_handler per step. RD45-specific handlers rd45_c1, rd45_a5, rd45_c0 and rd45_actuator_80 run only when requested by the JSON. auto_stop_request, auto_stop_positive_prefix and auto_stop_ms define the actuator STOP behavior.

Zones marked read_only disable editing and writes. Numeric, missing and previously undeclared tab IDs are supported, and loading another ECU rebuilds every zone state. Read replies must echo the requested zone and the correct KWP/UDS service. Read All continues after an unsupported zone.

Run node test/web/telecoding.test.cjs. These are simulated response tests, not physical ECU validation. Firmware builds regenerate the embedded page through scripts/prebuild_script.py.
