# UI Protocol Test

This document covers firmware-side compatibility checks for the migrated
`COMM_EBMS_*` packet handlers against the existing Qt UI repo.

## Scope

Covered commands:

- `COMM_EBMS_STORE_CONF = 150`
- `COMM_EBMS_GET_CELLS = 151`
- `COMM_EBMS_GET_AUX = 152`
- `COMM_EBMS_GET_EXP_TEMP = 153`
- `COMM_EBMS_SET_MCCONF = 154`
- `COMM_EBMS_GET_MCCONF = 155`
- `COMM_EBMS_GET_MCCONF_DEFAULT = 156`
- `COMM_EBMS_GET_VALUES = 157`
- `COMM_EBMS_GET_BMS_STATUS_EXT = 158` as a firmware-only optional extension

The golden test does not require hardware. It verifies packet IDs, payload
ordering, count bytes, signedness, scaling, and framing against the current UI
decoder order.

## How To Run

From the firmware repo root:

```bash
python3 tools/ui_protocol_golden.py
```

If the UI repo is not in the default sibling location, point at it explicitly:

```bash
python3 tools/ui_protocol_golden.py --ui-root ../ENNOID-BMS-Tool-master
```

## What The Script Checks

For packet payloads it verifies:

- `GET_VALUES`
  - exact field order used by `commands.cpp`
  - `float32 * 1e3` fields for pack and cell summary values
  - `float16 * 1e1` fields for load and temperature values
  - trailing-bytes check
- `GET_CELLS`
  - packet ID
  - one-byte count
  - 75 positive `int16 / 1e3` cell voltages
  - no trailing bytes
- `GET_AUX`
  - packet ID
  - one-byte `count=0`
- `GET_EXP_TEMP`
  - valid case: one-byte count plus 75 `int16 / 1e1` temperatures
  - invalid case: `count=0`
- `GET_BMS_STATUS_EXT`
  - firmware-defined extension packet shape for packet `158`

For framing it verifies the same VESC/DieBieMS packet envelope used by the UI
and firmware:

- start byte `0x02` for payloads up to 256 bytes
- payload length
- payload bytes
- CRC16
- terminating byte `0x03`

## Known Current Mismatch

The existing UI config serializer does not match the firmware's legacy config
blob for commands `154..156`.

Current status:

- The UI serializes a much larger XML-defined field list.
- The firmware still implements the older legacy config struct layout.
- The first serialized field already diverges:
  - UI starts with `noOfCellsSeries`
  - firmware expects `noOfCells`

This is reported by `tools/ui_protocol_golden.py` as a mismatch. It is not
fixed here because this task is verification-only.

## Invalid Data Behavior In The UI

Current firmware-side compatibility behavior:

- Invalid cell data:
  - `GET_CELLS` still returns `count=75`
  - each cell value is encoded as `0.0V`
- Invalid temperature data:
  - `GET_EXP_TEMP` returns `count=0`
- `GET_AUX` currently returns `count=0`
- The `GET_VALUES` fault byte is coarse and should be treated as summary-only,
  not full fault detail

## What Still Requires Hardware

This script does not validate:

- real serial transport timing
- UI connection behavior against a live COM/TTY device
- actual measured values from LTC6812, ISL28022, or PA1 ADC
- config round-trip safety on live hardware
- any transport-layer issues caused by OS serial drivers

## Fake Endpoint

No fake serial/TCP firmware endpoint is included in this phase.

Reason:

- the golden-packet verifier is enough to validate payload compatibility
  without changing firmware behavior
- the current task is protocol verification, not transport emulation
