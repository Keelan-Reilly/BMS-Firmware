# BMS Bring-Up Checklist

## Boot And Latch

- Verify `POWER_ENABLE` latches +3V3 as expected after boot.
- Verify `POWER_BUTTON` and `CHARGE_DETECT` inputs read correctly.
- Verify all safety permissions are inactive immediately after reset and before state transitions.

## Output Defaults

- Verify `MULTIPURPOSE_ENABLE` idle state leaves the downstream active-low signal deasserted.
- Verify `DISCHARGE_ENABLE` idle state leaves the downstream active-low signal deasserted.
- Verify `CHARGE_ENABLE` idle state is inactive.
- Verify `CHARGER_SAFETY` idle state is inactive.
- Verify `AMS_OK` stays low unless both `MULTIPURPOSE_ENABLE` and `DISCHARGE_ENABLE` are intentionally asserted through their active-low downstream paths.

## isoSPI And LTC Chains

- Verify `CS_CELL` and `CS_TEMP` are both high at idle.
- Verify selecting one isoSPI chain always deselects the other.
- Verify cell-chain LTC6812 readout succeeds with PEC clean on all 5 devices.
- Verify the Phase 12 cell-chain `ADOW` open-wire diagnostic runs with PEC-clean
  readback on both the pull-up and pull-down passes.
- Verify temp-chain LTC6812 readout succeeds with PEC clean on all 5 devices.
- Verify the LTC6812 command/PEC assumptions match the repo datasheet:
  `ADCV` bit layout from `datasheets/ltc6812-1-3.pdf` Table 37 / command table,
  `RDCVA`-`RDCVE` from Table 36, and 15-bit PEC from the "Packet Error Code"
  section on pp. 52-54.
- Verify the TEMP sensor-bias control commands match the LTC6812 configuration
  register tables: `WRCFGA` / `RDCFGA` and `WRCFGB` / `RDCFGB`, with `DCC1`-`DCC8`
  in CFGA byte 4, `DCC9`-`DCC12` in CFGA byte 5, and `DCC13`-`DCC15` in CFGB byte 0.
- Verify CELL balancing uses that same `DCC1`-`DCC15` mapping on the CELL chain
  only, with readback verification through `RDCFGA` / `RDCFGB`.
- Verify no balancing/config writes are sent to the TEMP chain.
- Verify the TEMP chain remains measurement-only in captures, with S pins used only
  as temporary sensor-bias enables.
- Verify CELL balancing is disabled when cell readout is invalid, when the
  open-wire diagnostic is invalid or faulted, and when the pack is in error,
  power-down, battery-dead, or external states.
- Verify all CELL balancing is disabled through the common disable path before or
  during shutdown/error handling.
- Verify the open-wire diagnostic remains measurement/diagnostic only and does not
  change shutdown permissions by itself.

## Voltage And Current Monitors

- Verify battery-side `Vbat` is read only through the ISL28022 on `I2C2` (`PA9`/`PA10`).
- Verify low-current path current is read only through the ISL28022.
- Verify ISL28022 comms failures increment the power-monitor error counter and invalidate `Vbat/current`.
- Verify `Vpack` on `PA1` tracks the load-side / precharge-bus voltage and is not confused with `Vbat`.
- Verify failed or disconnected `Vpack` ADC handling does not allow discharge-close decisions to pass.

## Communications

- Verify USB debug UART on `PA2`/`PA3`.
- Verify CAN communication on `PA11`/`PA12`.
- Verify CAN/USB activity alone does not assert safety permissions without local measurement validity.

## Safety Behavior

- Verify watchdog/fatal/error/power-down paths deassert all permissions.
- Verify injected cell undervoltage blocks discharge permission.
- Verify injected cell overvoltage blocks charge permission and escalates hard faults as configured.
- Verify temp-chain comms fault is surfaced and temperature coverage remains invalid/conservative.
- Verify invalid power-monitor readout does not leave stale `Vbat/current` marked valid.
- Verify `diag_faults` shows the expected fault bits for invalid cell readout,
  open-wire fault, TEMP coverage loss, ISL failure, and `Vpack` ADC failure.
- Verify `COMM_EBMS_GET_VALUES` now reports a non-zero coarse fault byte whenever
  a centralized active fault is present.
- Verify `MasterOk`, `DischargePermission`, `ChargePermission`, and
  `ChargerSafety` stay deasserted when the centralized fault model blocks them,
  even if the state machine still desires the path.

## TEMP Chain Conversion

- Verify the Enepaq sensor is treated as a voltage shunt reference, not as an NTC.
- Verify the TEMP-chain sensor-bias path uses a `680 ohm` current limit.
- Verify TEMP-chain S pins are only used to enable the sensor-bias MOSFETs and are
  not treated as cell-balancing controls.
- Verify CELL-chain balancing does not change TEMP sensor-bias behavior and does
  not require odd/even TEMP sequencing.
- Verify this board does not use odd/even anti-adjacent temp sequencing.
- Verify the TEMP measurement sequence is: enable required sensor-bias outputs,
  allow the bias to settle, issue `ADCV`, read voltages, then disable all TEMP
  sensor-bias outputs.
- Verify all TEMP-chain sensor-bias enables are turned back off after measurement
  and after any failed TEMP measurement path.
- Verify TEMP-chain raw voltages stay inside the Enepaq Table 5 range from
  `datasheets/Sony-Murata-VTC6-Li-ion-Battery-Module-With-Temperature-Sensor-Datasheet-.pdf`:
  2.44V at `-40C` down to 1.30V at `120C`.
- Verify interpolation between the Enepaq table points matches the datasheet curve.
- Verify any TEMP-chain channel outside 1.30V to 2.44V is marked invalid.
- Verify `temperatureReadoutValid` only becomes true when every required TEMP-chain
  channel converts validly on the same fresh read.
- Verify local STM32 NTC still reports board temperature only and is not treated as
  pack coverage.

## Remaining TODOs

- Final ISL28022 voltage/current calibration constants.
- Final `Vpack` ADC divider calibration.
- Bench validation of the LTC6812 ADOW normal-mode assumptions against the board's
  actual cell-input capacitance and wiring.
- Bench validation of the Phase 13 host-controlled CELL balancing path, including
  whether the board uses any LTC6812 `DTEN` / `DCTO` timer policy that would need
  explicit firmware handling.
- Physical mapping from the board's per-channel TEMP sensor-bias MOSFET topology onto the 5 x 15
  TEMP-chain channels.
- Bench validation of the final required TEMP-channel mask if not all 75 channels
  are populated in hardware.

## Reference

- See [safety_invariants.md](safety_invariants.md) for the Phase 7 invariants that new cleanup work must preserve.
- See [datasheet_validation.md](datasheet_validation.md) for the Phase 8 datasheet-backed assumptions.
- See [fault_model.md](fault_model.md) for the Phase 14 centralized fault bits and
  permission gating rules.
