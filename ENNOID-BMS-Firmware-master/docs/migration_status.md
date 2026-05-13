Current merged state:
- GPIO pin contract migrated
- dual isoSPI chain abstraction added
- LTC6812 cell-chain readout added
- LTC6812 temperature-chain raw voltage readout added
- shutdown outputs migrated to permission semantics
- ISL28022 Vbat/current validity added
- Vpack ADC semantics clarified
- syntax-check script added

Known TODOs:
- full linked build still not available
- Enepaq voltage-to-temperature curve still missing
- LTC6812 open-wire diagnostics not migrated
- LTC6812 hardware UV/OV flags not migrated
- balancing not migrated to LTC6812
- hardware bring-up validation still required