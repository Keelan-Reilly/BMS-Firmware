# Hardware Bring-Up Procedure

This procedure is for bench bring-up support only.

- It does not replace schematic review, DMM checks, or oscilloscope verification.
- `./scripts/syntax_check.sh` is syntax-only validation, not a full firmware build.

## Power-On Checks

1. Verify the board powers correctly and `+3V3` is present.
2. Verify `POWER_ENABLE` latches the supply as expected.
3. Verify the debug/programming header and ST-Link/SWD wiring are correct.
4. Verify all output permissions are inactive by default after reset.

Recommended command:

- `diag`

## isoSPI Idle Checks

1. Confirm `CS_CELL` is high at idle.
2. Confirm `CS_TEMP` is high at idle.
3. Confirm neither chain remains selected between transactions.

Recommended command:

- `diag_isospi`

## CELL-Chain Read

1. Trigger a one-shot CELL measurement.
2. Confirm `cellVoltageReadoutValid=true`.
3. Confirm PEC errors are zero or understood.
4. Inspect first/last sample cells and aggregate min/avg/max.

Recommended commands:

- `measure_cells_once`
- `diag_cells`

## TEMP-Chain Read

1. Trigger a one-shot TEMP measurement.
2. Confirm `temperatureReadoutValid=true` only when required coverage is present.
3. Confirm TEMP raw voltages and converted temperatures look plausible.
4. Confirm TEMP sensor-bias enables are not left on after the read path.

Recommended commands:

- `measure_temp_once`
- `diag_temp`

## Power Monitor And Precharge Bus

1. Trigger a one-shot power measurement.
2. Confirm `Vbat`, current, and `Vpack` validity flags.
3. Confirm `Vpack` tracks the load-side bus, not battery-side `Vbat`.
4. Confirm the precharge ratio and delta are plausible for the current state.

Recommended commands:

- `measure_power_once`
- `diag_power`
- `diag_precharge`

## Inputs

1. Verify `ChargeDetect` changes when a charger is present.
2. Verify `PowerButton` raw and debounced state.

Recommended command:

- `diag_power`

## Output Permissions

1. Confirm `MasterOk`, `DischargePermission`, `ChargePermission`, and
   `ChargerSafety` desired/effective states remain conservative by default.
2. Confirm error and power-down paths deassert them.
3. Confirm `AMS_OK` behavior follows the existing hardware equation rather than any
   old direct-relay assumption.

Recommended commands:

- `diag_outputs`
- `diag_faults`

## Injected Fault Checks

1. Inject cell UV/OV conditions and confirm the centralized fault model responds.
2. Simulate TEMP invalid / missing coverage and confirm permissions block.
3. Verify open-wire diagnostics report suspicious channels conservatively.
4. Verify precharge timeout / welded-contact suspicion show up in diagnostics.

Recommended commands:

- `diag_faults`
- `diag_cells`
- `diag_temp`
- `diag_precharge`

## Balancing Checks

1. Verify balancing is not active when cell data is invalid or open-wire is faulted.
2. Verify only CELL-chain balancing state is reported.
3. Verify TEMP-chain S outputs remain measurement-only and are not used for balancing.

Recommended commands:

- `diag_balance`
- `diag_cells`
- `diag_temp`

## Error / Watchdog / Shutdown Checks

1. Verify error, battery-dead, and power-down paths deassert permissions.
2. Verify diagnostics show validity flags beside the affected measurements.
3. Verify no bench command in this procedure forces safety outputs directly.

Recommended commands:

- `diag`
- `diag_faults`
- `diag_outputs`
