# Diagnostics

## Phase 11 scope

Phase 11 adds read-only bring-up diagnostics for the migrated firmware.

Phase 14 extends those diagnostics with centralized fault visibility.

These diagnostics are intended for bench visibility only:

- they do not assert or clear safety outputs
- they do not write LTC registers directly
- they do not trigger balancing
- they do not alter state-machine transitions
- they do not force precharge or discharge-close decisions
- they do not replace safety validation

## Terminal commands

Use the existing terminal command path from the UI or serial console.

### `diag`

Prints a bring-up summary:

- operational state
- cell readout validity / error count / count
- cell open-wire validity / fault count / diagnostic error count
- temperature readout validity / error count / count
- Vbat / current / Vpack validity
- power-monitor validity / error count
- active fault count / primary fault / UI fault byte / active fault mask
- a compact pack summary

### `diag_faults`

Prints centralized fault-model diagnostics:

- `activeFaultMask`
- `latchedFaultMask`
- `activeFaultCount`
- primary fault bit name
- coarse UI fault byte
- per-fault active / latched state for every Phase 14 fault bit

### `diag_cells`

Prints cell-chain diagnostics:

- total cell count
- `cellVoltageReadoutValid`
- `cellVoltageReadoutErrorCount`
- `cellVoltageReadoutCount`
- `cellOpenWireValid`
- `cellOpenWireFaultCount`
- `cellOpenWireDiagnosticErrorCount`
- `cellBalancingValid`
- `cellBalancingErrorCount`
- `cellBalancingActiveCount`
- LTC6812 cell-chain `lastReadValid`
- LTC6812 cell-chain `lastReadPECErrors`
- LTC6812 cell-chain open-wire `lastDiagnosticPECErrors`
- LTC6812 CELL-chain balance-config `lastConfigPECErrors`
- min / average / max / mismatch cell voltage
- first few and last few cell voltages with raw codes and device/channel indices
- first few and last few open-wire flags
- first few and last few balancing flags

### `diag_temp`

Prints TEMP-chain diagnostics:

- total temp channel count
- `temperatureReadoutValid`
- `temperatureReadoutErrorCount`
- `temperatureReadoutCount`
- LTC6812 TEMP-chain `lastReadValid`
- LTC6812 TEMP-chain `lastReadPECErrors`
- battery temp high / average / low
- BMS temp high / average / low
- first few and last few raw TEMP voltages
- first few and last few converted temperatures
- first few and last few per-channel validity flags

Notes:

- the TEMP-chain status currently shares the same `driverSWLTC6812TempChainStatus`
  surface used by voltage reads and TEMP sensor-enable config readback
- TEMP-chain S outputs are measurement-only sensor-bias enables, not balancing
- CELL-chain balancing status is separate and uses the LTC6812 DCC bits on the
  CELL chain only

### `diag_power`

Prints power-monitor diagnostics:

- Vbat from the master ISL28022
- current from the master ISL28022
- Vpack from PA1 ADC
- validity flags for all three
- aggregate `powerMonitorReadoutValid`
- `powerMonitorReadoutErrorCount`
- ChargeDetect GPIO state
- charger-current detection state
- power-button GPIO / debounced state

### `diag_precharge`

Prints precharge/contact validation diagnostics:

- operational state
- precharge elapsed time
- configured low-current precharge timeout
- `Vbat` and `Vpack`
- validity flags for both
- minimum plausible `Vbat` threshold used for ratio evaluation
- `Vpack / Vbat` ratio
- completion ratio threshold
- `Vbat - Vpack` delta
- `prechargeMeasurementValid`
- `prechargeComplete`
- active `PRECHARGE_TIMEOUT` and `WELDED_CONTACTOR_SUSPECT` fault status
- reminder that `PB11` is `MasterOk`, not a direct precharge relay output

### `diag_outputs`

Prints output-permission diagnostics:

- `masterOkDesired`
- `disChargeDesired`
- `disChargeLCAllowed`
- `chargeDesired`
- `chargeAllowed`
- `chargerSafetyDesired`
- GPIO readback for:
  - Multipurpose / MasterOk permission
  - Discharge permission
  - Charge permission
  - Charger safety permission

Interpretation:

- the GPIO readback shows the MCU-side pin level
- for the migrated shutdown path, the downstream hardware contract for some
  lines is active-low after the external stage, so the command also prints that
  note explicitly

### `diag_isospi`

Prints isoSPI diagnostics:

- currently selected chain if any
- `CS_CELL` raw GPIO state
- `CS_TEMP` raw GPIO state
- whether both chip-selects are idle high
- reminder that the TEMP chain uses S outputs only as temporary sensor-bias
  enables

## Interpreting invalid flags

- `cellVoltageReadoutValid=false`: do not trust the cell-voltage list as fresh
  or safe
- `cellOpenWireValid=false`: do not trust the latest ADOW open-wire result; it is
  missing, stale, or the command/readback failed
- `cellOpenWireFaultCount > 0`: one or more cell-input connections looked suspect
  under the datasheet ADOW comparison; this is bring-up diagnostics only and not
  final shutdown policy yet
- `cellBalancingValid=false`: the last CELL-chain balance mask write/readback did
  not verify cleanly, so any active balancing state should be treated as unknown
  until a later successful disable or balance update
- `activeFaultMask != 0`: one or more centralized fault conditions are active and
  output permissions are being gated conservatively from that mask
- `prechargeMeasurementValid=false`: do not trust the current Vbat/Vpack
  precharge-complete decision
- `prechargeComplete=false`: the load-side bus has not yet been validated as close
  enough to pack voltage for discharge-path close
- `temperatureReadoutValid=false`: do not trust pack temperature coverage
- `vBatReadoutValid=false`, `currentReadoutValid=false`, `vPackReadoutValid=false`:
  the respective measurement failed or was rejected
- `powerMonitorReadoutValid=false`: the combined Vbat/current/Vpack monitor set
  is not healthy
- LTC6812 `lastReadPECErrors > 0`: a PEC mismatch occurred on the last tracked
  chain transaction

## Limitations

- diagnostics reflect the current firmware state and last tracked readback
  status; they are not a substitute for oscilloscope or DMM verification
- the UI fault byte is intentionally coarse; use `diag_faults` for the detailed
  fault-bit view
- some desired/effective output splits are inferred from desired flags plus GPIO
  readback, not from downstream contactor feedback
- welded-contactor suspicion is still a conservative heuristic based on Vbat/Vpack
  relationship, not direct contactor feedback
- syntax checking only confirms the code parses; it is not a full firmware build

## Validation note

`./scripts/syntax_check.sh` or
`./ENNOID-BMS-Firmware-master/scripts/syntax_check.sh`
is syntax-only validation, not full build or hardware validation.

## Protocol note

- Phase 16 adds optional `COMM_EBMS_GET_BMS_STATUS_EXT` for detailed
  capability/fault/validity telemetry without changing the existing UI packet IDs
  `150..157`.
