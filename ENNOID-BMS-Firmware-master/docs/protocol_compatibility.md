# Protocol Compatibility

## Phase 10 scope

This phase adds firmware-side compatibility for the existing ENNOID-BMS Tool
`COMM_EBMS_*` packet IDs without changing the UI repo and without removing the
legacy `COMM_*` handlers.

The compatibility layer is intentionally narrow:

- no low-level LTC6812 / isoSPI changes
- no shutdown polarity changes
- no AMS_OK changes
- no balancing implementation changes
- Phase 14 adds a coarse UI fault-byte mapping without changing the packet layout
- no expansion of the legacy config blob for 75-channel temperature mapping

## Packet IDs

The firmware now recognizes these ENNOID-BMS Tool packet IDs:

- `150`: `COMM_EBMS_STORE_CONF`
- `151`: `COMM_EBMS_GET_CELLS`
- `152`: `COMM_EBMS_GET_AUX`
- `153`: `COMM_EBMS_GET_EXP_TEMP`
- `154`: `COMM_EBMS_SET_MCCONF`
- `155`: `COMM_EBMS_GET_MCCONF`
- `156`: `COMM_EBMS_GET_MCCONF_DEFAULT`
- `157`: `COMM_EBMS_GET_VALUES`

Legacy packet IDs remain supported unchanged.

## Handler map

### `COMM_EBMS_GET_VALUES`

Firmware handler: `modCommandsSendEBMSValuesPacket()`

Payload returned:

- `float32 * 1e3`: pack voltage
- `float32 * 1e3`: pack current
- `uint8`: SoC
- `float32 * 1e3`: cell voltage high
- `float32 * 1e3`: cell voltage average
- `float32 * 1e3`: cell voltage low
- `float32 * 1e3`: cell voltage mismatch
- `float16 * 1e1`: low-current load voltage
- `float16 * 1e1`: low-current load current
- `float16 * 1e1`: charger voltage placeholder
- `float16 * 1e1`: battery temperature high
- `float16 * 1e1`: battery temperature average
- `float16 * 1e1`: battery temperature low
- `float16 * 1e1`: BMS temperature high
- `float16 * 1e1`: BMS temperature average
- `float16 * 1e1`: BMS temperature low
- `float16 * 1e1`: humidity
- `uint8`: operational state
- `uint8`: balance-active flag
- `uint8`: coarse fault-state byte
- `6 x float32 * 1e3`: Ah / Wh counters placeholders

Known placeholders:

- charger voltage: `0.0`
- Ah / Wh counters: `0.0`

Rationale:

- the current pack state does not expose a distinct charger-voltage signal
- the firmware now exposes only a coarse 1-byte fault category, not the full
  internal fault bitmask
- the firmware tracks SoC and remaining capacity, but not the UI's expected
  cumulative Ah / Wh counters

Fault-byte mapping:

- `0`: no active centralized fault
- `1`: cell voltage fault category
- `2`: cell read / open-wire fault category
- `3`: temperature fault category
- `4`: ISL / Vpack read-invalid fault category
- `5`: precharge / welded-contactor suspicion category
- `6`: internal-fatal category

### `COMM_EBMS_GET_CELLS`

Firmware handler: `modCommandsSendEBMSCellsPacket()`

Payload returned:

- `uint8`: cell count = `75`
- `75 x float16 * 1e3`: cell voltages

75-cell behavior:

- when `cellVoltageReadoutValid == true`, return the latest LTC6812 cell-chain
  voltages for all 75 cells
- when `cellVoltageReadoutValid == false`, return count `75` with all values
  forced to `0.0V`

Deliberate limitation:

- no negative-voltage balancing marker is used in the EBMS compatibility path
- this avoids implying a 75-cell balancing status format that does not yet
  exist

### `COMM_EBMS_GET_EXP_TEMP`

Firmware handler: `modCommandsSendEBMSExpansionTempPacket()`

Payload returned:

- `uint8`: temperature count
- `count x float16 * 1e1`: converted temperatures in degrees C

75-temp behavior:

- when `temperatureReadoutValid == true`, return count `75` and the converted
  Enepaq temperatures from the TEMP chain
- when `temperatureReadoutValid == false`, return count `0`

Safety rationale:

- returning count `0` is safer for this UI than emitting fake pack temperatures
- local STM32 / board temperatures are not substituted for pack coverage

### `COMM_EBMS_GET_AUX`

Firmware handler: `modCommandsSendEBMSAuxPacket()`

Payload returned:

- `uint8`: count = `0`

Current choice:

- pack temperatures are carried by `COMM_EBMS_GET_EXP_TEMP`
- AUX is left empty rather than duplicating or mislabeling temperatures

### Config aliases

Firmware handlers:

- `COMM_EBMS_SET_MCCONF` -> existing config write path
- `COMM_EBMS_GET_MCCONF` -> existing config read path
- `COMM_EBMS_GET_MCCONF_DEFAULT` -> existing default-config read path
- `COMM_EBMS_STORE_CONF` -> existing config store path

Known limitation:

- the legacy config blob is not expanded in this phase for 75-channel
  temperature mapping or capability data
- future work should add a new dedicated protocol command for detailed
  temperature-channel capabilities, required masks, and physical mapping

## 75-cell / 75-temp summary

- `GET_VALUES`: remains aggregate-only and stays UI-compatible through
  placeholders where needed
- `GET_CELLS`: now supports 75 cells directly
- `GET_EXP_TEMP`: now supports 75 pack temperatures directly
- `GET_AUX`: intentionally remains empty
- config protocol: still legacy and not suitable for detailed 75-channel temp
  configuration

## Known limitations

- only a coarse 1-byte fault category is exposed to the UI path
- no charger-voltage field in current pack-state model
- no cumulative Ah / Wh counters exposed to this packet
- no detailed temp capability / mapping command yet
- config blob compatibility is preserved as-is rather than redesigned in this
  phase
