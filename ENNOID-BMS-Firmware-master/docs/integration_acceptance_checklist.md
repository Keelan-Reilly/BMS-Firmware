# Integration Acceptance Checklist

## Runtime / UI

- UI detects migrated firmware via `COMM_BMS_GET_CAPABILITIES`.
- UI shows `75` cells from `COMM_EBMS_GET_CELLS`.
- UI shows `75` EXP_TEMP temperatures when valid.
- AUX count `0` is handled as valid/unavailable, not as failure.
- Legacy config toolbar actions are disabled for migrated firmware.
- Unknown firmware leaves config/update disabled.
- Bootloader/update mode shows only safe update workflows.

## Firmware safety

- Legacy config commands `150/154/155/156` are rejected loudly.
- TEMP-chain write semantics remain sensor-bias only.
- CELL-chain balancing is still isolated to the CELL chain.
- `PB11` remains `MasterOk`, not precharge.
- `PB10` remains `DischargePermission`, not a direct discharge relay.

## Config V2

- Capabilities report Config V2 support.
- `GET_CONFIG_V2` works.
- `VALIDATE_CONFIG_V2` returns explicit error codes.
- `SET_CONFIG_V2` is RAM-only.
- `STORE_CONFIG_V2` remains blocked until validated on hardware.

## Boot / update

- Firmware image size is checked against staged capacity.
- Tool rejects unknown targets before upload.
- Tool rejects too-large images before upload.
- Bootloader rejects zero-size images.
- Bootloader rejects CRC mismatch.
- Bootloader rejects invalid stack pointer.
- Bootloader rejects invalid reset handler.
- Bootloader rejects out-of-bounds copy targets.

## Validation scripts / tests

- `./scripts/syntax_check.sh`
- `./scripts/check_flash_layout.py`
- `python3 ./tools/ui_protocol_golden.py`
- `python3 ../ENNOID-BMS-Tool-master/tools/fake_bms_firmware.py --self-test`
- `python3 ../DieBieMS-Bootloader-master/tools/check_staged_image.py --self-test`

## Hardware-deferred items

- SWD flash / first-boot validation
- +3V3 latch / AMS_OK behavior
- isoSPI wake timing on real boards
- CELL chain physical ordering
- TEMP chain physical ordering
- TEMP settle time tuning
- ISL28022 scaling calibration
- PA1 Vpack scaling calibration
- Open-wire behavior with real filters
- Balancing thermal/electrical validation
