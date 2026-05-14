# Fake Firmware UI Test

## Purpose

`tools/fake_bms_firmware.py` provides a fake ENNOID-BMS firmware endpoint so the Qt UI can be exercised without live BMS hardware.

It is intended for monitoring-page testing only.

Supported monitoring commands:

- `COMM_FW_VERSION` (`0`)
- `COMM_EBMS_GET_VALUES` (`157`)
- `COMM_EBMS_GET_CELLS` (`151`)
- `COMM_EBMS_GET_AUX` (`152`)
- `COMM_EBMS_GET_EXP_TEMP` (`153`)
- `COMM_EBMS_GET_BMS_STATUS_EXT` (`158`)

Config commands are not emulated for real use:

- `COMM_EBMS_SET_MCCONF` (`154`)
- `COMM_EBMS_GET_MCCONF` (`155`)
- `COMM_EBMS_GET_MCCONF_DEFAULT` (`156`)
- `COMM_EBMS_STORE_CONF` (`150`)

If those are received, the fake endpoint sends a `COMM_PRINT` message saying config access is unsupported.

## Preferred Endpoint

Use the TCP connection page in the Qt UI.

The UI already supports TCP directly, and this avoids depending on serial drivers or real USB hardware.

Default TCP endpoint:

- host: `127.0.0.1`
- port: `65102`

## Run The Fake Firmware

From the UI repo root:

```sh
python3 tools/fake_bms_firmware.py
```

Optional flags:

```sh
python3 tools/fake_bms_firmware.py --fault
python3 tools/fake_bms_firmware.py --invalid-temps
python3 tools/fake_bms_firmware.py --port 65103
python3 tools/fake_bms_firmware.py --serial-pty
python3 tools/fake_bms_firmware.py --quiet
```

Run the built-in protocol self-test:

```sh
python3 tools/fake_bms_firmware.py --self-test
```

## Connect The UI

In ENNOID-BMS Tool:

1. Open the `Connection` page.
2. Use the `TCP` section.
3. Set server to `127.0.0.1`.
4. Set port to `65102` unless you started the script with another `--port`.
5. Click `Connect`.

The fake endpoint also answers `COMM_FW_VERSION`, which keeps the UI connected and out of the firmware-read timeout path.

The fake firmware reports version `6.0`, which matches the current supported version list in `res/info.xml`.

Expected connection behavior:

- the UI should connect without the limited-mode warning
- after the `COMM_FW_VERSION` handshake, the server should start logging periodic monitoring requests:
  - `157` for values
  - `151` for cells
  - `152` for AUX
  - `153` for pack temperature channels

## Expected Realtime Values

`COMM_EBMS_GET_VALUES` returns these monitoring values:

- pack voltage: `300.0 V`
- pack current: `-12.3 A`
- SoC: `56 %`
- cell high / average / low: `4.10 / 4.00 / 3.90 V`
- cell mismatch: `0.20 V`
- load-side voltage: `295.0 V`
- load-side current: `-12.3 A`
- charger voltage: `0.0 V`
- pack temp high / average / low: `28.0 / 24.0 / 20.0 C`
- BMS temp high / average / low: `35.0 / 30.0 / 25.0 C`
- humidity: `50.0 %`
- op state: `Load enabled`
- fault code: `0` by default, `1` with `--fault`

Note:

- the existing wire format stores SoC as an integer byte, so the fake endpoint returns `56` rather than fractional `55.5`

## Expected Cell Display

`COMM_EBMS_GET_CELLS` returns `75` cell voltages:

- `T1` cell value range is `3.700 V`
- final cell value range is `3.774 V`
- values increase by `0.001 V` per cell

This should exercise the 75-cell graph/list in the UI.

## Expected Temperature Display

`COMM_EBMS_GET_EXP_TEMP` returns `75` pack temperature channels by default:

- first temperature: `20.0 C`
- last temperature: `27.4 C`
- values increase by `0.1 C` per channel

This should exercise the migrated pack temperature channel display.

`COMM_EBMS_GET_AUX` returns:

- count `0`

That should exercise the UI's unavailable AUX/local temperature state.

## Fault Mode

Use:

```sh
python3 tools/fake_bms_firmware.py --fault
```

Behavior:

- `COMM_EBMS_GET_VALUES` returns a non-zero UI fault byte
- `COMM_EBMS_GET_BMS_STATUS_EXT` returns non-zero active and latched fault masks

This is useful for checking that the UI shows a faulted state without changing packet formats.

## Invalid Temperature Mode

Use:

```sh
python3 tools/fake_bms_firmware.py --invalid-temps
```

Behavior:

- `COMM_EBMS_GET_EXP_TEMP` returns count `0`
- `COMM_EBMS_GET_BMS_STATUS_EXT` reports zero expansion temperature channels

This is useful for checking the UI message:

- `No valid pack temperature data`

## Optional PTY Serial Mode

On macOS or Linux you can also create a pseudo-terminal endpoint:

```sh
python3 tools/fake_bms_firmware.py --serial-pty
```

The script prints a PTY path such as:

- `/dev/ttys012`

If that PTY appears in the Qt serial-port list, it can be used like a fake serial BMS endpoint.

TCP is still the preferred path because it is simpler and does not depend on how Qt enumerates pseudo-terminals on the host OS.

## Packet Framing

The script implements the same framing used by the UI `Packet` class:

- short packet start byte: `0x02`
- short packet length: `1` byte
- long packet start byte: `0x03`
- long packet length: `2` bytes, big-endian
- payload
- CRC16 over payload only
- end byte: `0x03`

Payload encoding matches `VByteArray`:

- integers are big-endian
- `Double32(scale)` is signed `int32` divided by `scale`
- `Double16(scale)` is signed `int16` divided by `scale`

## Limitations

- This is a monitoring simulator, not a full firmware emulator.
- Config read/write/store is intentionally not implemented.
- The UI currently does not decode `COMM_EBMS_GET_BMS_STATUS_EXT`, but the fake endpoint supports it for future use and direct protocol testing.
- SoC in the current values packet is integer-only.
- PTY serial mode is best-effort; TCP is the primary supported path.
