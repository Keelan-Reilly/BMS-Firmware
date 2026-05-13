# Safety Invariants

These invariants describe the intended Phase 7 safety model and should remain
true during future cleanup work.

- `CS_CELL` and `CS_TEMP` must both idle high.
- The TEMP isoSPI chain is measurement-only; S outputs are used only as temporary sensor-bias enables.
- No balancing or config writes intended for the cell chain may be routed to `BMS_ISOSPI_CHAIN_TEMP`.
- TEMP-chain sensor-bias enables should be off at idle, enabled only long enough to measure, and turned back off after any success or failure path to avoid parasitic discharge.
- TEMP-chain temperature data is only considered valid after a fresh LTC6812 read succeeds with clean PEC and every required Enepaq conversion lands inside the validated datasheet range.
- Missing, stale, or out-of-range TEMP-chain data must never look safe; when battery/BMS temperature masks are enabled, invalid TEMP coverage blocks permissions through `dataHealthy`.
- `PB11` / `MULTIPURPOSE_ENABLE` is the `MasterOk` / multipurpose permission path, not a precharge relay output.
- `PB10` / `DISCHARGE_ENABLE` is a discharge permission into shutdown logic, not a direct discharge relay output.
- The downstream shutdown signals on `PB11` and `PB10` are active-low even though the MCU-side GPIO drive may be active-high through a MOSFET stage.
- `Vbat` comes from the ISL28022 on `I2C2` (`PA9`/`PA10`), not from `PA1`.
- `Vpack` on `PA1` is the load-side / precharge-bus voltage, not battery-side `Vbat`.
- Safety permissions default inactive after boot and after any explicit disable path.
- Invalid cell readout blocks permissions through the `dataHealthy` gating path.
- Missing temperature coverage blocks permissions whenever temperature masks are enabled.
- Invalid `Vbat` or invalid `Vpack` blocks discharge-path close decisions.
- Phase 7 syntax checks are only parse-time checks; they are not a full linked firmware build and are not hardware validation.
