# Cross-Repo Integration Contract

## Historical lineage

This stack contains inherited assumptions from three eras:

1. VESC / DieBieMS packet framing and staged-update patterns.
2. ENNOID firmware/tool config and UI assumptions.
3. Migrated STM32F303 + dual LTC6812 + 75-cell + 75-temp firmware.

The migrated stack is authoritative for hardware truth. Legacy ENNOID / DieBieMS behavior is compatibility-only and must never be silently assumed for the migrated STM32F303/LTC6812 target.

## Repo responsibilities

Firmware repo:
- Owns hardware truth, sensing semantics, fault policy, output permissions, and packet payloads.
- Must reject legacy config on migrated hardware.
- Must report identity/capabilities before config/update is enabled in the tool.

Tool repo:
- Owns presentation, workflow gating, file validation, and operator-facing warnings.
- Must not assume hardware profile from firmware version alone.
- Must use capabilities before enabling migrated config or update flows.

Bootloader repo:
- Owns staged-image validation and copy/install safety.
- Must match the firmware linker layout and the tool upload contract.

## Runtime monitoring packets

`COMM_EBMS_GET_VALUES = 157`
- Big-endian framing inside the standard packet envelope.
- Payload order:
  - `float32 x1e3 packVoltage`
  - `float32 x1e3 packCurrent`
  - `uint8 SoC`
  - `float32 x1e3 cellVoltageHigh`
  - `float32 x1e3 cellVoltageAverage`
  - `float32 x1e3 cellVoltageLow`
  - `float32 x1e3 cellVoltageMismatch`
  - `float16 x1e1 loadSideVoltage`
  - `float16 x1e1 loadSideCurrent`
  - `float16 x1e1 chargerVoltage`
  - `float16 x1e1 tempBattHigh`
  - `float16 x1e1 tempBattAverage`
  - `float16 x1e1 tempBattLow`
  - `float16 x1e1 tempBMSHigh`
  - `float16 x1e1 tempBMSAverage`
  - `float16 x1e1 tempBMSLow`
  - `float16 x1e1 humidity`
  - `uint8 operationalState`
  - `uint8 balancingCount`
  - `uint8 coarseFaultCode`
  - `6 x float32 x1e3 reserved lifetime counters`

`COMM_EBMS_GET_CELLS = 151`
- `uint8 count`
- `count x float16 x1e3 cellVoltage`
- Migrated target reports `count = 75`.

`COMM_EBMS_GET_AUX = 152`
- `uint8 count`
- Migrated target reports `count = 0`.

`COMM_EBMS_GET_EXP_TEMP = 153`
- `uint8 count`
- `count x float16 x1e1 temperatureC`
- Migrated target reports `count = 75` when valid, `0` when invalid/unavailable.

`COMM_EBMS_GET_BMS_STATUS_EXT = 158`
- Optional extension packet.
- Payload order:
  - `uint8 fwMajor`
  - `uint8 fwMinor`
  - `uint8 cellCount`
  - `uint8 tempCount`
  - `uint32 activeFaultMask`
  - `uint32 latchedFaultMask`
  - `uint16 measurementFlags`
  - `uint8 activeBalancingCount`
  - `uint8 openWireFaultCount`
  - `uint8 coarseFaultCode`
  - `uint8 operationalState`

## Legacy config quarantine

Legacy ENNOID config commands are unsafe on migrated firmware:

- `COMM_EBMS_STORE_CONF = 150`
- `COMM_EBMS_SET_MCCONF = 154`
- `COMM_EBMS_GET_MCCONF = 155`
- `COMM_EBMS_GET_MCCONF_DEFAULT = 156`

Known mismatch:
- Tool legacy config blob historically serializes about `94` fields.
- Firmware legacy parser historically expects about `71` fields in a different order.

Rule:
- Migrated firmware must not partially parse, partially apply, or partially store legacy config.
- Tool must not render legacy config workflows for migrated firmware.

## Capabilities / identity packet

`COMM_BMS_GET_CAPABILITIES = 159`

Payload:
- `uint32 magic = 0x424D5332` (`BMS2`)
- `uint8 payloadVersion = 1`
- `uint8 firmwareType`
- `uint8 firmwareMajor`
- `uint8 firmwareMinor`
- `uint16 firmwarePatch`
- `uint16 hardwareProfile`
- `uint8 hardwareProfileVersion`
- `uint8 configSchemaVersion`
- `uint8 cellCount`
- `uint8 tempCount`
- `uint8 cellChainDeviceCount`
- `uint8 tempChainDeviceCount`
- `uint32 featureFlags`
- `uint32 appStartAddress`
- `uint32 appBodyStartAddress`
- `uint32 eepromPage0Address`
- `uint32 eepromPage1Address`
- `uint32 stagedUpdateAddress`
- `uint32 bootloaderAddress`
- `uint32 maxStagedImageSize`
- `uint32 maxApplicationImageSize`
- `uint8 updateCrcType`
- `uint8[3] reserved`

Current migrated hardware profile:
- `hardwareProfile = 1`
- Meaning: `STM32F303 + dual isoSPI + 5x LTC6812 CELL + 5x LTC6812 TEMP + 75 cells + 75 Enepaq temps`

Current feature flags:
- `bit0` migrated LTC6812 model
- `bit1` dual isoSPI
- `bit2` EXP_TEMP support
- `bit3` AUX count zero is valid
- `bit4` Config V2 supported
- `bit5` Config V2 RAM write supported
- `bit6` Config V2 persistent store supported
- `bit7` bootloader/update flow supported
- `bit8` legacy config supported

Current migrated application reports:
- legacy config supported = false
- Config V2 supported = true
- Config V2 persistent store = false for now

## Config V2

Reserved/implemented packet IDs:
- `COMM_BMS_GET_CONFIG_V2 = 160`
- `COMM_BMS_SET_CONFIG_V2 = 161`
- `COMM_BMS_STORE_CONFIG_V2 = 162`
- `COMM_BMS_GET_CONFIG_DEFAULT_V2 = 163`
- `COMM_BMS_VALIDATE_CONFIG_V2 = 164`
- `COMM_BMS_GET_CONFIG_SCHEMA_V2 = 165`

Config V2 wire object is versioned and fixed-length.

Wire contract:
- Total size: `112` bytes
- Endian: little-endian
- Internal CRC field offset: bytes `18..19`
- Internal CRC coverage: bytes `20..111`
- Bytes `0..17` are outside the internal Config V2 body CRC and are protected by the outer packet CRC on the wire
- Persistent store remains intentionally blocked for the migrated target

Header:
- `uint32 magic = 0x43464732` (`CFG2`)
- `uint16 schemaVersion = 1`
- `uint16 payloadLength`
- `uint32 generation`
- `uint16 hardwareProfile`
- `uint8 cellCount`
- `uint8 tempCount`
- `uint16 flags`
- `uint16 bodyCrc`

Body includes:
- Cell OV/UV thresholds
- Charge/discharge/hard temperature limits
- Precharge threshold + timeout
- Required cell/temp masks, 75 bits each packed into 10 bytes
- Balance-allowed mask, 75 bits packed into 10 bytes
- Vpack / ISL / current calibration fields
- Current sign enum
- Open-wire / balancing / telemetry options

Current implementation status:
- `GET_CONFIG_V2`: implemented
- `GET_CONFIG_DEFAULT_V2`: implemented
- `GET_CONFIG_SCHEMA_V2`: implemented
- `VALIDATE_CONFIG_V2`: implemented
- `SET_CONFIG_V2`: RAM-only implemented
- `STORE_CONFIG_V2`: intentionally rejected for now

Firmware validation rules currently enforced:
- magic
- schema version
- exact payload length
- hardware profile
- exact cell/temp counts
- body CRC
- threshold ordering
- plausible ranges
- packed-mask top-bit hygiene above channel 75
- nonzero calibration gains
- valid current-sign enum
- reserved bytes zero

Config V2 result codes:
- `0 OK`
- `1 unsupported version`
- `2 bad magic`
- `3 bad length`
- `4 bad CRC`
- `5 wrong hardware profile`
- `6 invalid cell count`
- `7 invalid temp count`
- `8 invalid threshold order`
- `9 invalid threshold range`
- `10 invalid mask`
- `11 invalid calibration`
- `12 store failed`
- `13 readback failed`
- `14 unsupported in current mode`

## Bootloader / update contract

See [flash_update_contract.md](./flash_update_contract.md).

Current update assumptions:
- Tool uploads raw `.bin` into the staged area using the historical `size + CRC16 + payload` format.
- Tool must reject files larger than the capabilities-reported staged size.
- Bootloader must verify size, CRC, staged/destination bounds, initial stack pointer, and reset handler before erase/copy.

## Compatibility matrix

| Connected target | Monitoring | Legacy config | Config V2 | Update |
| --- | --- | --- | --- | --- |
| Legacy ENNOID firmware + legacy tool path | Allowed | Allowed if detected compatible | N/A | Historical behavior |
| Migrated application + no capabilities | Allowed cautiously | Blocked | Blocked | Blocked |
| Migrated application + capabilities, monitoring-only | Allowed | Blocked | Blocked | Allowed only after size/layout validation |
| Migrated application + capabilities, Config V2 | Allowed | Blocked | Allowed | Allowed only after size/layout validation |
| Bootloader/update mode | Not applicable | Blocked | Blocked | Allowed |
| Unknown / unsupported target | Minimal only | Blocked | Blocked | Blocked |
