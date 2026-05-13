# Application Layer Refactor Plan

## Scope

This phase is planning-only. It does not change active firmware behavior.

The goal is to replace the current ENNOID-heavy application layer with a clearer BMS controller layer while preserving:

- low-level hardware drivers
- LTC6812 CELL/TEMP chain separation
- conservative fault gating
- existing UI and terminal protocol compatibility

This plan assumes the migrated hardware and safety meanings already established in Phases 1-18:

- `PB11` is `MasterOk` / multipurpose permission, not precharge
- `PB10` is `DischargePermission`, not a direct discharge relay
- `PA1` is `Vpack`, not `Vbat`
- TEMP-chain `S` outputs are sensor-bias enables only, not balancing

## Current Application-Layer Responsibilities

### `modPowerElectronics`

This module currently mixes several responsibilities:

- measurement orchestration
  - cell-chain reads
  - TEMP-chain reads and Enepaq conversion
  - ISL28022 `Vbat` / current reads
  - `Vpack` ADC reads
  - one-shot measurement helpers
- derived pack statistics
  - min/avg/max cell voltage
  - min/avg/max temperature
  - throttle and pack SOA calculations
- safety/fault evaluation
  - active and latched fault masks
  - UI fault byte generation
- output permission gating
  - `MasterOk`
  - `DischargePermission`
  - `ChargePermission`
  - `ChargerSafety`
- precharge/contact validation
  - `Vbat`/`Vpack` ratio and delta checks
  - timeout and welded-contactor suspicion
- CELL balancing
  - candidate selection
  - CELL-chain-only DCC mask construction
  - balancing disable paths
- legacy compatibility surfaces
  - mirrored cell arrays
  - old-style wrappers whose names still imply old ENNOID semantics

`modPowerElectronics` is the main refactor target because it combines measurement collection, safety policy, state-derived behavior, actuator gating, and diagnostics data shaping in one file.

### `modOperationalState`

This module currently mixes:

- high-level operating-state transitions
- boot/idle/error/power-down behavior
- charger-connect/disconnect handling
- timeout handling
- direct calls into `modPowerElectronics`
- display/effect side effects

It is acting as both a state machine and a coordinator for UI/display behavior.

### `modCommands`

This module currently handles:

- packet decoding/dispatch
- legacy command handlers
- `COMM_EBMS_*` compatibility packet packing
- configuration read/write/store protocol
- firmware update packet handling
- serialization of internal state for UI consumption

Its main problem is not correctness, but that protocol shaping is coupled directly to broad application state structures.

### `modTerminal`

This module currently handles:

- terminal command registration and parsing
- diagnostics printing
- config editing commands
- measurement/debug visibility

It is already close to a diagnostics role, but it reaches into many unrelated modules directly.

### `modCAN`

This module currently handles:

- CAN init and periodic transmit scheduling
- receive/fragment handling
- simple BMS status transmit packets
- watchdog/ESC-related CAN behavior

Current CAN telemetry remains aggregate and does not fully represent the newer 75-cell / 75-temp / fault-mask model.

## Proposed Target Architecture

### `bmsMeasurements`

Responsibility:

- own all measurement acquisition and freshness/validity bookkeeping
- expose raw and converted measurement snapshots

Planned contents:

- CELL-chain read orchestration
- TEMP-chain read orchestration
- TEMP sensor-bias usage during measurement only
- open-wire diagnostic execution
- ISL28022 power-monitor reads
- `Vpack` ADC reads
- one-shot measurement helpers
- derived measurement aggregates
  - min/avg/max cell voltage
  - min/avg/max temperature
  - raw/converted temperature validity

This module should not decide final permissions.

### `bmsFaults`

Responsibility:

- convert measurement validity, thresholds, and contact-state observations into explicit faults

Planned contents:

- active fault mask
- latched fault mask
- primary fault selection
- UI fault-byte mapping
- precharge timeout / welded-contact fault evaluation
- balancing-allowed fault gate

This module should not write outputs directly.

### `bmsOutputs`

Responsibility:

- own desired versus effective output permissions
- apply centralized fault gating
- write the switch driver

Planned contents:

- desired permission state
- effective permission state after gating
- output update function
- conservative disable-all path

This module should be the only application-layer writer of:

- `MasterOk`
- `DischargePermission`
- `ChargePermission`
- `ChargerSafety`

It should preserve existing output polarity and AMS_OK hardware semantics.

### `bmsStateMachine`

Responsibility:

- own operating-state transitions and timers

Planned contents:

- former `modOperationalState` transition logic
- charger connect/disconnect policy
- error/power-down handling
- precharge progression state decisions
- state-derived desired permission requests

This module should not directly touch low-level switch polarity.

### `bmsProtocol`

Responsibility:

- provide a stable application-facing protocol surface for:
  - UART packet commands
  - EBMS compatibility packets
  - optional future extended status packets
  - CAN telemetry data shaping

Planned contents:

- protocol serializers from clean BMS model structs
- legacy packet compatibility shims
- non-breaking future extension commands

This module should avoid embedding business logic about safety decisions.

### `bmsDiagnostics`

Responsibility:

- expose read-only diagnostics over terminal and related debug surfaces

Planned contents:

- `diag`
- `diag_cells`
- `diag_temp`
- `diag_power`
- `diag_faults`
- `diag_precharge`
- `diag_balance`
- safe one-shot measurement command wrappers

This module should depend on measurement/fault/output/state APIs rather than reaching into internal globals.

## Old-To-New Responsibility Mapping

### From `modPowerElectronics`

Move to `bmsMeasurements`:

- cell read/update helpers
- temp read/update helpers
- Enepaq conversion helpers
- raw/converted measurement validity tracking
- ISL28022 read/update
- `Vpack` read/update
- one-shot measurement functions
- open-wire diagnostic data surfaces

Move to `bmsFaults`:

- `modPowerElectronicsEvaluateFaults(...)`
- UI fault-byte generation
- threshold checks
- invalid-data fault checks
- precharge/contact diagnostic fault checks

Move to `bmsOutputs`:

- output permission gating currently inside `modPowerElectronicsUpdateSwitches(...)`
- disable-all permission handling

Move to `bmsStateMachine`:

- precharge completion queries used only for state progression
- any state-specific “should allow charge/discharge/balance” policy that is really state ownership

Keep low-level CELL balancing control in `driverSWLTC6812`, but move balancing selection/orchestration to `bmsMeasurements` or a future `bmsBalancing` helper owned by the controller layer. If balancing stays small, it can remain under `bmsMeasurements`; if it grows, split it later.

### From `modOperationalState`

Move to `bmsStateMachine`:

- state enum handling
- transition rules
- precharge timers
- charger disconnect handling
- delayed disable / terminate behavior

Move display/effect side effects out of the core state machine over time, ideally behind optional hooks or a small presentation adapter.

### From `modCommands`

Move to `bmsProtocol`:

- EBMS packet serializers
- extended status command serializers
- fault-byte protocol mapping
- config blob compatibility glue

Keep packet framing and transport integration nearby unless a separate transport abstraction becomes necessary later.

### From `modTerminal`

Move to `bmsDiagnostics`:

- all read-only diagnostics printers
- safe one-shot measurement commands
- fault/precharge/balance debug commands

Keep generic terminal parser infrastructure where it is if that is lower-risk.

### From `modCAN`

Move data shaping to `bmsProtocol`:

- build clean telemetry snapshots from the controller model

Keep low-level CAN scheduling and transmit mechanics in `modCAN` unless there is a broader transport refactor.

## Reuse Plan For Existing Low-Level Drivers

These should be preserved and reused rather than rewritten:

- `driverSWLTC6812`
  - CELL-chain readout
  - TEMP-chain readout
  - TEMP sensor-bias control
  - CELL balancing control
  - open-wire diagnostic support
- `driverSWISL28022`
  - `Vbat` / current reads
- `driverHWADC`
  - `Vpack` ADC reads
- `driverHWSwitches`
  - output permission pin writes/readback
- `driverHWIsoSPI` / SPI1 path
  - isoSPI transport and chain selection

These drivers already encode hardware-specific knowledge and should remain the hardware boundary below the new controller layer.

## Legacy Paths To Retire Or Deprecate

### Old LTC6803 paths

Deprecate and remove from active application logic:

- `driverSWLTC6803`
- `driverLTC6803CellsTypedef` usage in application code
- any mirrored 12-cell-era balancing assumptions

Short term:

- allow compatibility mirrors only where needed to avoid destabilizing the rest of the app

Long term:

- remove application-layer dependence on LTC6803-derived types entirely

### Old precharge/discharge wrappers

Deprecate wrappers whose names suggest old relay semantics, especially where they obscure the migrated permission model.

Examples already identified:

- `modPowerElectronicsSetPreCharge(...)`
- `modPowerElectronicsSetDisCharge(...)`
- `modPowerElectronicsSetCharge(...)`

These should become clearer desired-permission APIs under `bmsOutputs` or `bmsStateMachine`.

### Display / buzzer / HiAmp paths

Do not remove immediately, but assess separation:

- `modDisplay`
- `modEffect`
- `modHiAmp`

Current evidence:

- `modDisplay` and `modEffect` are still coupled into operational-state behavior
- `modHiAmp` is still initialized and tasked from `main.c`

Plan:

- first separate core BMS decisions from presentation/peripheral side effects
- only retire these modules if they are proven unused on current hardware

### Other legacy assumptions to avoid carrying forward

- old single-device or 12-cell assumptions
- negative-voltage balancing markers in cell telemetry
- old direct precharge relay semantics

## Proposed Migration Order

### Step 1: Introduce clean read-only model interfaces

Add narrow headers around existing globals/structs so new code can read:

- measurement snapshot
- fault snapshot
- output desired/effective snapshot
- state snapshot

Do this without changing behavior.

### Step 2: Extract `bmsMeasurements`

Move measurement acquisition and validity logic out of `modPowerElectronics` first.

Reason:

- most of the hardware-specific sequencing already exists
- it can be moved with less safety-policy churn than outputs or state transitions

### Step 3: Extract `bmsFaults`

Move centralized fault evaluation next.

Reason:

- Phase 14 already created a mostly centralized policy point
- this is a strong seam for safe extraction

### Step 4: Extract `bmsOutputs`

Move desired/effective permission gating and switch updates into a dedicated outputs layer.

Reason:

- once faults are separate, output gating becomes much easier to reason about

### Step 5: Extract `bmsStateMachine`

Move the transition engine out of `modOperationalState`.

Reason:

- by then, state logic can depend on stable measurement/fault/output APIs instead of `modPowerElectronics` internals

### Step 6: Extract `bmsDiagnostics`

Move terminal diagnostics to read from the clean snapshots.

Reason:

- diagnostics should be the easiest consumer once the model APIs exist

### Step 7: Extract `bmsProtocol`

Move packet and CAN serializers last.

Reason:

- protocol compatibility is easiest to preserve once the internal model is already stable

## Risks

### Safety regression risk

The largest risk is moving permission and state logic without preserving conservative gating.

Mitigation:

- extract measurement and fault logic before output logic
- keep disable-all paths explicit during every step
- maintain “invalid blocks permission” semantics during the entire refactor

### Hidden legacy coupling

`modPowerElectronics`, `modOperationalState`, `modTerminal`, and `modCommands` currently share assumptions through global data and naming rather than narrow APIs.

Mitigation:

- add snapshot accessors before moving logic
- replace direct struct access gradually, not all at once

### Presentation-side coupling

`modDisplay`, `modEffect`, and possibly `modHiAmp` are intertwined with operational flow.

Mitigation:

- separate core control decisions from side effects before deleting anything

### Protocol compatibility risk

UI and CAN payloads must continue to match existing consumers.

Mitigation:

- keep `bmsProtocol` as a compatibility layer
- do not change packet ordering while internal refactor is underway

## Recommended Future Phase Breakdown

1. Add clean snapshot/accessor layer without behavior change
2. Move measurement logic into `bmsMeasurements`
3. Move fault logic into `bmsFaults`
4. Move output gating into `bmsOutputs`
5. Move state machine into `bmsStateMachine`
6. Move diagnostics into `bmsDiagnostics`
7. Move UI/CAN serialization into `bmsProtocol`
8. Remove remaining LTC6803-era application dependencies
9. Review and retire unused display/effect/HiAmp paths if hardware confirms they are not needed

## Open Questions

- Whether balancing should remain inside `bmsMeasurements` or become its own future `bmsBalancing` module
- Whether CAN telemetry should stay transport-owned in `modCAN` or be fully data-owned by `bmsProtocol`
- Whether `modDisplay`, `modEffect`, and `modHiAmp` are required on the target hardware or should become optional integrations
- How much legacy config-blob structure must remain frozen for tool compatibility before a cleaner protocol can replace it
