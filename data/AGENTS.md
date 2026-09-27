# Setup web UI (`data/index.html`)

This directory contains the browser UI embedded in ESP firmware. `index.html` is the board setup page. Read this file before changing the setup UI, its fixtures, or the matching HTTP handlers.

## Runtime model

- The setup page is plain HTML/CSS/JavaScript with no framework or bundler. Shared visual rules are in `styles.css`; `index.html` also contains legacy inline styles.
- All device calls use the relative base `api/`, except OTA upload, which currently posts to the absolute path `/api/update`. Relative URLs are important because the IIS development copy is mounted at `/vancansetup/`, while firmware serves the page at `/`.
- At startup the page fetches `GET api/config.json`, stores the object in the global `jsonData`, and generates the form from that object. There is no separately maintained front-end schema.
- `setupSections` gives known config paths their section and order. Any leaf returned by firmware but not listed there is rendered in **Additional settings**. `SUPPORTED_SOURCE_PROTOCOLS`, `SUPPORTED_DESTINATION_PROTOCOLS`, and `MILEAGE_AT_CMB_TRIP_RESET` are metadata/internal fields and are not rendered as ordinary controls.
- Control type is inferred from the JSON value: booleans become switches, most numbers/strings become inputs, arrays become rows of numeric inputs, nested objects use dot-delimited IDs, and VIN is specially converted between an ASCII string and a byte array. Named numeric fields have explicit select options. The legacy `DATETIME` config leaf is not rendered; clock controls live in the top status card.
- Source/destination protocol options come from `SUPPORTED_SOURCE_PROTOCOLS` and `SUPPORTED_DESTINATION_PROTOCOLS`, so the compiled board configuration controls what the user can choose. Protocol values are `0=None`, `1=AEE2001`, `2=AEE2004`, and `3=AEE2010`.
- The dictionary contains English, French, and Brazilian Portuguese entries, but the current helpers deliberately read only `entry.en`; the rendered setup page is therefore English-only. Missing labels fall back to the config path with underscores replaced by spaces.

## Sections and conditional visibility

The normal sections are Protocol setup, Vehicle identity, Bridge behavior, Parking aid distances, Clock, AEE2010, and the fallback Additional settings section.

`updateConditionalSections()` runs after form generation and whenever source protocol, destination protocol, or parking-aid type changes:

- Show the AEE2010 section only when destination is AEE2010.
- Show Clock when destination is AEE2010 or source is AEE2004.
- Show parking-distance arrays only for source AEE2001 plus parking aid type `1` (PSA VAN).
- Show `QUERY_AC_STATUS` only for AEE2001 -> AEE2004.
- Show `SEND_AC_FAN_CHANGES_TO_DISPLAY` and `SEND_AC_CHANGES_TO_DISPLAY` for an AEE2004 destination.
- Show `EMULATE_DISPLAY_ON_SOURCE` for an AEE2001 source.
- Never show `EMULATE_DISPLAY_ON_DESTINATION` (it is retained in JSON and round-tripped).
- Show `GENERATE_POPUP_FOR_DOOR_STATUS` for an AEE2010 destination.
- Show `ENABLE_PARKING_AID_SOUND_FROM_SPEAKER` only for source AEE2001 plus PSA VAN parking aid.

Hidden controls remain in the DOM and are still gathered on save, so changing visibility must not be treated as removing a config value.

## User actions and API contract

| UI action | Request | Behavior |
| --- | --- | --- |
| Page load | `GET api/config.json` | Builds the complete form from the returned live configuration. Failure opens the custom error modal. |
| Device status | `GET api/time` | Starts immediately, uses an 800 ms abort timeout, and schedules the next request 1 second after the previous request finishes. Response supplies date/time and `firmware_version`. |
| Save configuration | `POST api/config` with JSON | Recursively gathers every rendered control back into `jsonData`, preserving the supported-protocol arrays, then shows success/failure in the custom modal. |
| Set time | `POST api/time` with `{year, month, day, hour, minute, second}` | The main action captures the browser clock at click time. A collapsed manual editor is the offline fallback when that clock is inaccurate. This is independent of Save configuration. |
| Reboot | `GET api/reboot` | Immediate; there is currently no confirmation dialog. |
| Get VIN | `GET api/getVin?radioType=<value>` | Warns that protocol changes require save/restart, starts a config save, triggers `ImmediateSignal::StartVinRead`, then reloads after 300 ms. The save is not awaited before the GET. |
| Export settings | Browser download | Gathers the current form and downloads `psa-van-can-settings-YYYYMMDD-HHMMSS.json`; it does not write to the device. |
| Import settings | Browser file picker | Parses a JSON object, preserves supported-protocol arrays if the import omitted them, rebuilds the form, and asks the user to click Save. It does not write automatically. |
| Update firmware | `POST /api/update`, `application/octet-stream` | Sends the raw selected `.bin` after confirmation. The device checks image magic and target chip, writes the next OTA partition, selects it, and reboots after success. |

The custom modal is implemented by `showCustomConfirm()`. It is used for both confirmation and one-button result messages; do not replace it with blocking browser dialogs without a deliberate UI decision.

## Firmware integration

- Routes and handlers live in `src/Platform/Esp/Helpers/WebServer/WebServer.cpp`. The device serves `/` and `/index.html` from the same embedded `index_html` byte array.
- `GET /api/config.json` calls `ConfigFile::GetAsJson()`, which serializes current `CarState` plus the board-supported protocol arrays. It does not simply return the on-flash file.
- `POST /api/config` writes `/littlefs/settings.json` through `ConfigFile::SaveJson()` and immediately calls `ConfigFile::Read()` to refresh config-backed `CarState` fields.
- Source/destination handlers and transports are constructed from those fields during startup in `src/Platform/Esp/main.cpp`. Saving a new protocol value updates state but does not rebuild the runtime graph; a reboot is required for protocol changes to take full effect.
- `GET /api/time` also refreshes the web server's last-request timestamp. `POST /api/time` delegates to `TimeProvider::SetDateTime()`.
- The device serves HTML/CSS as gzip-compressed arrays from `src/Helpers/WebServer/gzipped_webpage_data.h`. `scripts/prebuild_script.py`, registered as a PlatformIO pre-script, regenerates that header from `.html`, `.css`, and `.js` files under `data/` using deterministic gzip level 9. Do not hand-edit the generated header; run a normal firmware build after web changes.

## Local IIS development fixture

Use `http://localhost/vancansetup/` to inspect the page. `data/api/config.json` is the static development response that makes the generated form usable in IIS. Keep it structurally representative of `ConfigFile::GetAsJson()`, especially value types, nested `AEE2010`, protocol metadata arrays, VIN bytes, and four-element parking-distance arrays.

IIS currently has no `data/api/time` fixture. Consequently the status values remain `-`, `GET api/time` returns 404, and the page logs an error on every polling cycle. This is expected in the current local fixture, not proof of a firmware failure. Device mutation endpoints (config POST, time POST, reboot, VIN request, and OTA) are firmware behavior and should not be exercised during ordinary IIS UI inspection. Also note that the absolute OTA URL resolves to `/api/update`, outside `/vancansetup/`, in this hosted layout.

## Change checklist

- Add a new persistent setting to `CarState`, `ConfigFile::Read()`, and `ConfigFile::GetAsJson()` as well as to the UI fixture. Preserve its JSON type because control generation and value gathering depend on it.
- Add the config path to `setupSections` when it belongs in a known section; otherwise it will intentionally appear under Additional settings.
- Add select mappings for enum-like numeric values. Without one, a numeric leaf becomes a generic input.
- Update the label/tip dictionary, remembering that only English is currently rendered.
- Revisit `updateConditionalSections()` if availability depends on protocol or another selector. Verify both visible and hidden combinations and restore the fixture defaults after testing.
- Never put bus work or long synchronous operations in an HTTP path. Setup requests run outside the timing-critical protocol handlers, and changes must preserve the firmware's real-time constraints described by the repository-level `AGENTS.md`.
- Test with the IIS URL at desktop and narrow widths, check the browser console, and perform a firmware build so the gzip header is regenerated. Do not click Reboot, Get VIN, Set time, Save, or Update firmware on a live device unless that mutation is explicitly intended.
- Keep the compact RTC/browser time comparison near the top and the single mobile fixed Save action available without scrolling. Hide the in-flow Save button at mobile widths so two Save configuration actions are not shown at once.
- Keep the mobile setup header and generated field spacing compact: reduced header typography/padding and tighter label, tip, control, card, and field gaps are intentional to maximize usable phone-screen space while retaining practical touch targets.
- Treat `STA_WIFI_PASSWORD` carefully: the UI renders it as a password input, but it is still present in config JSON and settings exports.
