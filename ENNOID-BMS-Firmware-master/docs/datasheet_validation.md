# Phase 8-9 Datasheet Validation

This note records the repo-local datasheets used for the Phase 8 temperature-chain
validation work. It is intentionally limited to assumptions that were checked
directly against files already present in this repository.

## Datasheet Paths

- `datasheets/ltc6812-1-3.pdf`
- `datasheets/LTC6820-2.pdf`
- `datasheets/stm32f303vc-4.pdf`
- `datasheets/Sony-Murata-VTC6-Li-ion-Battery-Module-With-Temperature-Sensor-Datasheet-.pdf`
- `ENNOID-BMS-Firmware-master/CubeMX/STM32F303CCTx.pdf`

## LTC6812

- `ADCV` command layout was checked against `ltc6812-1-3.pdf` Table 36 and Table 37
  plus the "Measuring Cell Voltages (ADCV Command)" section on pp. 21-23.
- `ADOW` command layout for Phase 12 open-wire diagnostics was checked against the
  same command table and the "Open Wire Check (ADOW Command)" section on pp. 23-24:
  `ADOW = 0 | 1 | MD[1] | MD[0] | PUP | 1 | DCP | 1 | CH[2:0]`.
- Read-command opcodes `RDCVA` through `RDCVE` were checked against Table 36 on p. 59.
- The five cell register groups for 15 cells were checked against Tables 40-44 on
  pp. 62-63.
- Raw cell/GPIO conversion of `100uV` per LSB was checked against the "ADC Range and
  Resolution" section on p. 21 and Table 55 on p. 67.
- PEC handling was checked against the "Packet Error Code" section on pp. 52-54:
  initial PEC seed `0x0010`, polynomial
  `x^15 + x^14 + x^10 + x^8 + x^7 + x^4 + x^3 + 1`, and a stuffed zero LSB in the
  transmitted 16-bit field.
- Wake-up behavior was reviewed against the "Waking Up the Serial Interface" and
  "Waking a Daisy Chain" text on pp. 50-52.
- TEMP-chain sensor-bias enable control uses the same configuration-register command
  table: `WRCFGA`, `RDCFGA`, `WRCFGB`, and `RDCFGB`.
- The 15-cell discharge-control map used for TEMP sensor bias was checked against the
  LTC6812 configuration-register field tables: `DCC1`-`DCC8` in CFGA byte 4,
  `DCC9`-`DCC12` in CFGA byte 5, and `DCC13`-`DCC15` in CFGB byte 0.
- Configuration writes use per-device 6-byte payloads plus per-device PEC, and
  readback verification is possible by re-reading CFGA/CFGB and checking both PEC
  and the requested DCC bits.
- Phase 13 reuses those same configuration-register commands for CELL-chain
  balancing only. The mapping is the same as the TEMP sensor-bias path, but the
  public firmware APIs remain separate: CELL balancing writes target
  `BMS_ISOSPI_CHAIN_CELL` only, while TEMP sensor-bias writes target
  `BMS_ISOSPI_CHAIN_TEMP` only.
- Phase 13 balancing remains host-controlled. No new `DCP = 1` ADCV path was added,
  and no new discharge-timer policy was introduced. `DTEN` / `DCTO` behavior
  therefore remains an explicit follow-up question for later review.
- The Phase 12 open-wire algorithm follows the datasheet sequence directly:
  run the 15-cell `ADOW` command at least twice with `PUP = 1`, read cells once,
  then run it at least twice with `PUP = 0`, read cells once, and compare
  `CELLPU(n) - CELLPD(n)` for cells 2..15.
- The datasheet's open-wire criteria were preserved as diagnostic-only status:
  if `CELLPU(1) == 0`, flag the first measured cell connection; if
  `CELLPU(n+1) - CELLPD(n+1) < -400mV`, flag cell `n`; if `CELLPD(15) == 0`,
  flag the last measured cell connection.
- Table 14 only guarantees the normal-mode two-pass `ADOW` algorithm through
  `<=10nF` external C-pin capacitance. Phase 12 therefore records the result as
  bring-up diagnostics only and does not upgrade it into final shutdown policy.

## LTC6820

- SPI mode selection was checked against `LTC6820-2.pdf` Table 4 on p. 12 and the
  accompanying phase/polarity text on p. 13. `POL = 1`, `PHA = 1` is SPI mode 3.
- The master-side recommendation for `SLOW` tied low was checked against the
  "Slow Mode" section on p. 13: the `SLOW` pin has no effect in master mode and is
  recommended tied to `GND`.
- Enable/idle timing was checked against the `EN` pin description on p. 9:
  with `EN` low the device enters IDLE after about 5.7ms of inactivity and wakes in
  less than 8us after `CS` falls on the master side.
- Bias-current and comparator-threshold assumptions were checked against p. 11:
  `IBIAS` is held at 2V, `IP/IM` drive current is `20 * IB`, and the comparator
  threshold is `0.5 * VICMP`.
- The schematic-style example on p. 11 uses `RB1 = 1.21k` and `RB2 = 787R`, giving
  `RBIAS = 2k`, `IB = 1mA`, `IDRV = 20mA`, `VICMP = 788mV`, and
  `VTCMP = 394mV`. The repo comment using approximately `1.2k + 806R = 2.006k`
  remains close in intent but should be treated as an approximate board-value
  statement, not the exact datasheet example.

## Enepaq Temperature Sensor

- The conversion source is Table 5 on p. 3 of
  `Sony-Murata-VTC6-Li-ion-Battery-Module-With-Temperature-Sensor-Datasheet-.pdf`.
- The validated transfer range is `2.44V @ -40C` down to `1.30V @ 120C`.
- The same section states that the signal is non-linear and that linear
  interpolation between the tabulated points is acceptable.
- The same datasheet describes the sensor as a temperature-variable voltage shunt
  reference.
- Page 4 notes that the sensor is a temperature-variable shunt reference and gives
  the suggested stack-measurement topology using differential cell inputs.
- The board clarified for Phase 8 does not use the Enepaq shared adjacent-balancing-
  switch topology. Instead, each TEMP channel uses its own MOSFET bias path, with
  the corresponding LTC6812 S output acting only as a temporary sensor-bias enable.
- The board-level sensor bias uses a `680 ohm` current limit, and all TEMP-chain
  bias enables should be turned off after measurement to avoid parasitic discharge.
- Because this board has per-channel sensor-bias enables, no odd/even anti-adjacent
  TEMP sequencing is required for this hardware.
- Phase 9 now performs the measurement sequence as: enable TEMP-chain sensor bias,
  allow a short settle delay, issue `ADCV`, read the C-input voltages, then disable
  all TEMP-chain sensor-bias enables before temperatures are accepted as valid.

## STM32F303

- `stm32f303vc-4.pdf` Table 9 confirms SPI support on the MCU family, and the pin
  tables show `PA1` as `ADC1_IN2`, matching the `Vpack` ADC path used in the
  migrated firmware.
- `ENNOID-BMS-Firmware-master/CubeMX/STM32F303CCTx.pdf` is retained in-tree as the
  board-local STM32 reference copy for the generated target.

## Open Questions

- Bench review of whether the board asserts the LTC6812 discharge timer path in a
  way that would require explicit `DTEN` / `DCTO` policy during CELL balancing.
- Physical ordering of TEMP-chain channels versus real Enepaq module/sensor wiring.
- Bench validation of the open-wire algorithm against the real board capacitance,
  especially if the C-pin wiring presents more than the datasheet's `<=10nF`
  normal-mode assumption.
- Exact bench-proven settle time between asserting the TEMP-chain sensor-bias enables
  and starting `ADCV`.
- Bench validation of the wake-up method across mixed `IDLE` / `READY` daisy-chain
  states.
- Bench validation of the final required TEMP-channel mask before using anything
  less conservative than "all channels required".
