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
- Verify temp-chain LTC6812 readout succeeds with PEC clean on all 5 devices.
- Verify no balancing/config writes are sent to the TEMP chain.
- Verify the TEMP chain remains read-only in captures.

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

## Remaining TODOs

- Open-wire diagnostics migration to LTC6812.
- Final Enepaq voltage-to-temperature curve and conversion validation.
- Final ISL28022 voltage/current calibration constants.
- Final `Vpack` ADC divider calibration.
