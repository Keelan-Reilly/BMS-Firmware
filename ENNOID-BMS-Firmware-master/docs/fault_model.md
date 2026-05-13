# Fault Model

Phase 14 adds a centralized conservative fault model in `modPowerElectronics`.

This model is intentionally simple:

- one active fault bitmask
- one latched fault bitmask
- one primary-fault indicator
- one coarse UI fault byte

It does not change shutdown polarity or the AMS_OK hardware equation.

## Fault Bits

- `BMS_FAULT_CELL_OV_SOFT`
- `BMS_FAULT_CELL_OV_HARD`
- `BMS_FAULT_CELL_UV_SOFT`
- `BMS_FAULT_CELL_UV_HARD`
- `BMS_FAULT_CELL_READ_INVALID`
- `BMS_FAULT_CELL_OPEN_WIRE`
- `BMS_FAULT_TEMP_OVER_LIMIT`
- `BMS_FAULT_TEMP_READ_INVALID`
- `BMS_FAULT_TEMP_SENSOR_INVALID`
- `BMS_FAULT_ISL_READ_INVALID`
- `BMS_FAULT_VPACK_READ_INVALID`
- `BMS_FAULT_PRECHARGE_TIMEOUT`
- `BMS_FAULT_WELDED_CONTACTOR_SUSPECT`
- `BMS_FAULT_INTERNAL_FATAL`

## Source Signals

- Cell voltage faults come from the validated LTC6812 CELL-chain aggregates and the
  existing config thresholds.
- Cell read invalid comes from `cellVoltageReadoutValid == false`.
- Cell open-wire fault comes from `cellOpenWireValid == false` or
  `cellOpenWireFaultCount > 0`.
- Temperature read invalid is raised when temperature coverage is required by the
  config masks and `temperatureReadoutValid == false`.
- Temperature sensor invalid is raised when a required TEMP-chain channel is not
  valid after conversion.
- Temperature over-limit currently uses the existing conservative `70C` aggregate
  ceiling already used elsewhere in the firmware.
- ISL read invalid is raised when `Vbat`, current, or the combined power-monitor
  validity is false.
- Vpack read invalid is raised when `vPackReadoutValid == false`.
- Precharge completion is now evaluated from fresh `Vbat` and `Vpack` only:
  `Vbat` and `Vpack` must both be valid, `Vbat` must be above a plausible minimum,
  and `Vpack / Vbat` must meet the configured `minimalPrechargePercentage`
  threshold.
- Precharge timeout still follows `OP_STATE_ERROR_PRECHARGE`.
- Welded contactor suspect is raised only when fresh `Vbat` / `Vpack` data is
  valid, the firmware expects the main output path to be open, and `Vpack`
  remains near pack voltage.
- Internal fatal is currently reserved for generic `OP_STATE_ERROR` cases that do
  not already map to a more specific active fault.

## Permission Effects

- `MasterOk` is blocked by critical measurement, comms, contact-state, hard cell,
  and internal faults.
- `DischargePermission` is blocked by the `MasterOk` fault set plus soft cell
  undervoltage.
- `ChargePermission` is blocked by the `MasterOk` fault set plus soft cell
  overvoltage.
- `ChargerSafety` follows the gated charge-permission result, not charger detect
  alone.
- CELL balancing is blocked by cell-read invalid, open-wire fault, hard cell
  voltage faults, contact-state faults, and internal fatal faults.

The state machine still sets the desired permissions. The final GPIO-facing
permission outputs are then gated by the centralized fault mask.

Current precharge/contact constants:

- ratio threshold: use config `minimalPrechargePercentage` when it is in `(0, 1]`,
  otherwise fall back to a conservative `0.80`
- minimum plausible `Vbat` for ratio evaluation: `max(noOfCells * cellHardUnderVoltage, 10V)`
- welded-contactor absolute floor: `10V`

These fallback constants need final safety review and bench calibration.

## Latched Behavior

- `activeFaultMask` reflects the current evaluation.
- `latchedFaultMask` ORs in every active fault and is only cleared on reset or
  reinitialization in the current firmware.

No runtime fault-clear command is added in this phase.

## UI And Diagnostics

- `diag` shows fault count, primary fault, UI fault byte, and the active mask.
- `diag_faults` shows the full active and latched masks plus every named fault bit.
- `diag_precharge` shows `Vbat`, `Vpack`, ratio, delta, validity, timer, and the
  active precharge/contact fault status.
- `COMM_EBMS_GET_VALUES` now uses a coarse non-zero UI fault byte rather than the
  old fixed zero placeholder.

## Open Questions

- Whether the current `70C` TEMP over-limit should become a config-backed limit.
- Whether latched faults should later gain an explicit clear policy or command.
- Whether `BMS_FAULT_INTERNAL_FATAL` should be driven by a dedicated watchdog or
  internal-self-test source instead of the current conservative fallback.
- Whether the precharge fallback constants (`0.80`, `10V`) should move into
  reviewed config fields instead of remaining firmware defaults.
- Whether the welded-contactor suspicion rule needs tighter filtering once bench
  data exists for the real load bus discharge behavior.
