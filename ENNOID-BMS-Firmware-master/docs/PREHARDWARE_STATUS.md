# Pre-Hardware Status

## 1. Executive Summary

The firmware has been migrated away from the original ENNOID/LTC6803-era assumptions toward the new STM32F303 plus dual-chain LTC6812 hardware model. The migrated firmware now has a verified GNU Arm Embedded build path, and the Qt UI has a working monitoring-only path against a fake firmware endpoint.

At this stage, the project is still pre-hardware. The software builds, the monitoring protocol is exercised, and the UI can display representative pack data without real BMS hardware. Hardware validation has not happened yet, so none of the output behavior, power-latch behavior, sensing polarity, contactor behavior, or safety behavior should be treated as proven on a real board.

The config protocol remains intentionally blocked/guarded. Monitoring packets are compatible between the migrated firmware surface and the UI, but config read/write/store is still incompatible and must not be used yet.

## 2. Firmware Repo Status

Current firmware repo commit:

- `0965a67557c79aa3bbc2d2aae6f05be408bd17f9`

Verified GNU Arm build status:

- repo-local GNU Arm build path exists and links successfully
- official Arm GNU Toolchain path was used in validation
- linked build artifacts are produced

Expected / verified output artifacts:

- `build/firmware.elf`
- `build/firmware.hex`
- `build/firmware.bin`
- `build/firmware.map`

`syntax_check.sh` status:

- parse-only check passes
- limitation: it is not a full build, does not assemble/link, and does not validate target execution behavior

Implemented firmware features in the current migrated surface:

- dual isoSPI CELL/TEMP chain support
- 75-cell LTC6812 readout
- 75-temperature LTC6812 readout
- Enepaq temperature conversion
- TEMP sensor-bias S-output control
- cell open-wire diagnostics
- cell balancing control/reporting
- ISL28022 battery/current measurement support
- PA1 Vpack measurement path
- centralized fault model
- precharge and contact validation
- terminal diagnostics support
- EBMS UI monitoring compatibility packets

## 3. UI Repo Status

Current UI repo commit:

- `0965a67557c79aa3bbc2d2aae6f05be408bd17f9`

Build status:

- Qt/qmake build path is working
- app bundle path:
  - `ENNOID-BMS-ToolV6.00.app`

Fake firmware endpoint:

- `tools/fake_bms_firmware.py`

Current tested UI behavior in the pre-hardware path:

- connects over TCP
- realtime stream works after pressing `Stream Realtime Data`
- dashboard values decode correctly from the fake endpoint
- 75 cells display
- 75 pack / Enepaq temperature channels display
- AUX/local temperature count `0` is shown as unavailable
- IV, cell, and temperature graphs update

Config/status of settings pages:

- config pages are now guarded / monitoring-only for the migrated firmware path
- config read/write/store is blocked or warned rather than silently used

## 4. Cross-Repo Protocol Status

Supported monitoring packets:

- `COMM_EBMS_GET_VALUES = 157`
- `COMM_EBMS_GET_CELLS = 151`
- `COMM_EBMS_GET_AUX = 152`
- `COMM_EBMS_GET_EXP_TEMP = 153`
- optional `COMM_EBMS_GET_BMS_STATUS_EXT = 158`

Config compatibility status:

- config commands `154` to `156` remain incompatible
- the UI currently uses a `94`-field config blob
- the migrated firmware still has a legacy parser surface that expects `71` fields in a different order
- therefore config read/write/store must not be used yet

Fake firmware status:

- the fake firmware endpoint verifies the monitoring packet shapes and framing used by the UI
- it is suitable for pre-hardware monitoring validation only

## 5. Safety / Status Boundaries

This phase is pre-hardware build and monitoring validation only.

Clear boundaries:

- not vehicle-ready
- not safety-approved
- hardware outputs and polarities still require bench verification
- no accumulator / HV use until staged bring-up is complete

## 6. Remaining Unvalidated Items

- STM32 boot on the target board
- SWD flashing on the target board
- `+3V3` and power-latch behavior
- output default states at boot/reset/fault
- `AMS_OK` behavior
- isoSPI wake and timing behavior
- CELL daisy-chain order
- TEMP daisy-chain and channel order
- TEMP bias settle time
- ISL scaling and sign convention on real hardware
- Vpack ADC scaling on real hardware
- open-wire diagnostic behavior on the actual cell chain
- balancing thermal and electrical behavior
- CAN behavior on real hardware
- final thresholds and safety signoff

## 7. Recommended Next Steps

1. Tag both repos if not already tagged.
2. Preserve the current firmware and UI build instructions exactly as validated.
3. Start hardware bring-up in low-voltage-only conditions.
4. Verify SWD flashing, boot, and default outputs first.
5. Verify the CELL chain next.
6. Verify the TEMP chain after CELL is stable.
7. Verify the power monitor path after sensing order is confirmed.
8. Verify shutdown and `AMS_OK` behavior before any higher-risk tests.
9. Verify balancing last.
10. Only then revisit config protocol compatibility or a cleaner application-layer refactor.

## 8. Pointers To Detailed Docs

Firmware repo:

- `docs/build.md`
- `docs/bringup_procedure.md`
- `docs/fault_model.md`
- `docs/diagnostics.md`
- `docs/protocol_compatibility.md`
- `docs/application_layer_refactor_plan.md`

UI repo:

- `docs/fake_firmware_ui_test.md`
- `docs/migrated_firmware_ui_notes.md`

## 9. Known Caveats

- x-axis labels for 75 channels are visually cramped but functional
- UI config remains blocked
- fake firmware is not hardware validation
- `syntax_check.sh` is not a build
- the GNU build is verified, but flash/run on target is not yet verified
