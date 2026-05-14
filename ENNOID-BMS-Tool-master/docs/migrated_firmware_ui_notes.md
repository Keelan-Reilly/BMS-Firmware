# Migrated Firmware UI Notes

This UI build is intended to act as a monitoring front-end for the migrated BMS firmware.

## Monitoring Supported

- `COMM_EBMS_GET_VALUES` (157)
- `COMM_EBMS_GET_CELLS` (151)
- `COMM_EBMS_GET_EXP_TEMP` (153)
- `COMM_EBMS_GET_AUX` (152) as optional/legacy data

## Temperature Handling

- The migrated firmware exposes 75 Enepaq temperature channels through `COMM_EBMS_GET_EXP_TEMP`.
- The UI treats `EXP_TEMP` as a flat list of pack temperature channels.
- AUX temperatures are treated as legacy/local/optional data and not as pack coverage.
- Packet 157 aggregate temperature fields remain in use for:
  - Pack aggregate high/average/low
  - Local BMS/controller high/average/low

## Config Safety

- BMS config read/write/store is currently disabled for the migrated firmware UI flow.
- The current UI config blob and the migrated firmware config parser are not compatible.
- No config serialization, field remapping, or compatibility shim is attempted in this phase.
- The Settings -> General -> Sensors page is shown as legacy NTC configuration only, with warning text and disabled legacy temperature fields.

## Current UI Limits

- The UI shows `EXP_TEMP` channels as a flat list and does not yet provide a 75-channel physical mapping or grouping view.
- `COMM_EBMS_GET_AUX` may legitimately return `count=0`, which is shown as unavailable local/AUX data.
