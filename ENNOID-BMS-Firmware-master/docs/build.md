# Build And Flash Path

## Current Status

This repo now has a verified GNU Arm Embedded build path for the migrated STM32F303 firmware.

Primary local build path:

- `make -j4`

Checked-in GNU build files:

- build entrypoint: `Makefile`
- linker script: `gcc/STM32F303CC_APP_FLASH.ld`
- GNU startup file: `Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s`
- system file: `CubeMX/Src/system_stm32f3xx.c`

Legacy path:

- `MDK-ARM/DieBieMS.uvprojx` still exists, but it is not the authoritative migrated-firmware build path and still reflects older LTC6803-era project state unless separately updated.

## Verified Toolchain On This Mac

Official Arm GNU Toolchain path used for the successful build:

- `/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi`

Bin directory:

- `/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin`

Verified tools:

- `arm-none-eabi-gcc`
- `arm-none-eabi-objcopy`
- `arm-none-eabi-size`
- `arm-none-eabi-gdb`

The build was run with a PATH override so the incomplete Homebrew compiler was not used:

- `PATH="/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin:$PATH" make -j4`

The previous sysroot/header problem is resolved with this toolchain:

- `arm-none-eabi-gcc -v` reports `--with-newlib` and `--with-headers=yes`
- `<stdint.h>` resolves correctly from the official toolchain sysroot/newlib headers

## Build Commands

From the repo root:

- `PATH="/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin:$PATH" make clean`
- `PATH="/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin:$PATH" make -j4`

Optional size command:

- `PATH="/Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin:$PATH" arm-none-eabi-size build/firmware.elf`

Syntax-only parse check:

- `./scripts/syntax_check.sh`

## Target And Compiler Settings

Target MCU:

- `STM32F303CC`
- define: `STM32F303xC`

Compiler settings from `Makefile`:

- `-mcpu=cortex-m4`
- `-mthumb`
- `-mfloat-abi=soft`
- `-std=gnu99`
- `-DUSE_HAL_DRIVER`
- `-DSTM32F303xC`
- `-ffunction-sections`
- `-fdata-sections`
- `--specs=nano.specs`
- `--specs=nosys.specs`

Notes:

- `-mfloat-abi=soft` matches the checked-in GCC startup file, which declares `.fpu softvfp`.
- This GNU build path was verified without changing shutdown polarity, AMS_OK behavior, fault policy, LTC6812 commands, balancing behavior, TEMP sensor-bias behavior, or UI protocol behavior.

## Source List Strategy

The GNU source list is built from the migrated firmware surface and current runtime references, not from the stale Keil project.

Included source areas:

- main entry:
  - `Main/main.c`
- startup and system support:
  - `Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s`
  - `CubeMX/Src/system_stm32f3xx.c`
  - `Device/stm32f3xx_it.c`
  - `Device/stm32f3xx_hal_msp.c`
- HAL sources required by the current runtime:
  - ADC
  - CAN
  - Cortex
  - CRC
  - DMA
  - FLASH
  - GPIO
  - I2C
  - IWDG
  - PWR
  - RCC
  - SPI
  - UART
- hardware drivers:
  - `driverHWADC.c`
  - `driverHWEEPROM.c`
  - `driverHWI2C1.c`
  - `driverHWI2C2.c`
  - `driverHWPowerState.c`
  - `driverHWSPI1.c`
  - `driverHWStatus.c`
  - `driverHWSwitches.c`
  - `driverHWUART2.c`
- software drivers:
  - `driverSWADC128D818.c`
  - `driverSWDCDC.c`
  - `driverSWEMC2305.c`
  - `driverSWISL28022.c`
  - `driverSWLTC6812.c`
  - `driverSWPCAL6416.c`
  - `driverSWSHT21.c`
  - `driverSWSSD1306.c`
  - `driverSWStorageManager.c`
  - `driverSWUART2.c`
- modules:
  - `modCAN.c`
  - `modCommands.c`
  - `modConfig.c`
  - `modDelay.c`
  - `modDisplay.c`
  - `modEffect.c`
  - `modFlash.c`
  - `modHiAmp.c`
  - `modOperationalState.c`
  - `modPowerElectronics.c`
  - `modPowerState.c`
  - `modStateOfCharge.c`
  - `modTerminal.c`
  - `modUART.c`
- support libraries:
  - `libBuffer.c`
  - `libCRC.c`
  - `libGLCDFont.c`
  - `libGraphics.c`
  - `libLogos.c`
  - `libPacket.c`
  - `libRingbuffer.c`

Excluded as stale or not part of the active migrated build:

- `Drivers/SWDrivers/Src/driverSWLTC6803.c`
- `Drivers/SWDrivers/Src/driverSWCC1101.c`
- `Modules/Src/modMessage.c`
- `CubeMX/Src/main.c`
- `CubeMX/Src/stm32f3xx_hal_msp.c`
- `CubeMX/Src/stm32f3xx_it.c`
- `Device/startup_stm32f303xc.s`
- `Libraries/Scr/libFileStream.c`

Rationale:

- `driverSWLTC6812.c` is the active migrated cell and TEMP-chain driver.
- `driverSWLTC6803.c` is a legacy source and not part of the current migrated measurement path.
- `Device/startup_stm32f303xc.s` is Keil/ARMASM syntax, not GCC syntax.
- `modMessage.c` is not part of the active migrated GNU build surface.
- `libFileStream.c` depends on a legacy custom `FILE` shim member (`outputFunctionPointer`) that is not compatible with newlib `FILE`, and its active consumer is `modMessage.c`, which is excluded from this build.

## Narrow Fixes Needed For The Successful Build

Only two narrow source/build fixes were required after the official toolchain was available:

1. `Modules/Src/modTerminal.c`

- existing `extern` declarations were moved above helper functions that reference those symbols
- this fixed a real compile-order error
- no firmware behavior changed

2. `Makefile`

- `Libraries/Scr/libFileStream.c` was removed from the GNU source list
- this fixed a real portability/build error against newlib `FILE`
- no active migrated runtime behavior changed because the legacy consumer path is not included in this GNU build

No safety logic, output polarity, fault policy, LTC6812 behavior, balancing behavior, TEMP sensor-bias behavior, or UI protocol behavior was changed.

## Memory Layout

The GNU build does not use `CubeMX/STM32F303CC_FLASH.ld` unchanged because that script assumes one contiguous application image from `0x08000000`.

The repo's application layout reserves flash pages for EEPROM emulation:

- `0x08000000 .. 0x080007ff` : application vector table / early code
- `0x08000800 .. 0x08000fff` : EEPROM emulation page 0
- `0x08001000 .. 0x080017ff` : EEPROM emulation page 1
- `0x08001800 .. 0x08031fff` : main application body
- `0x08032000 ..` : bootloader region

That layout is encoded in:

- `gcc/STM32F303CC_APP_FLASH.ld`

This successful build did not require linker-script changes beyond the checked-in GNU linker script already present in the repo.

## Verified Output Artifacts

Successful build artifacts:

- `build/firmware.elf`
- `build/firmware.hex`
- `build/firmware.bin`
- `build/firmware.map`

Observed artifact sizes:

- `build/firmware.elf` : `1.6M`
- `build/firmware.hex` : `251K`
- `build/firmware.bin` : `95K`
- `build/firmware.map` : `1.2M`

## Verified Size Output

`arm-none-eabi-size build/firmware.elf` returned:

```text
   text    data     bss     dec     hex filename
  91308     148   26028  117484   1caec build/firmware.elf
```

## Known Build Warnings

The verified GNU build completed with these warnings:

- newlib/nosys syscall stubs are not implemented and will always fail:
  - `_close`
  - `_fstat`
  - `_getpid`
  - `_isatty`
  - `_kill`
  - `_lseek`
  - `_read`
  - `_write`
- linker warning:
  - `build/firmware.elf has a LOAD segment with RWX permissions`

These warnings did not block the linked build and were not changed in this phase.

## Flashing Notes

This document verifies build output generation, not board flashing.

Produced images:

- use `build/firmware.elf` for debugger loading
- use `build/firmware.hex` or `build/firmware.bin` for programmer workflows as appropriate

Before flashing hardware:

- confirm the board expects the EEPROM-reserved application layout encoded in `gcc/STM32F303CC_APP_FLASH.ld`
- confirm the bootloader/application boundary matches the target hardware
- avoid flashing over the reserved EEPROM emulation pages unless that is intentional

## Syntax Check Limitation

`./scripts/syntax_check.sh` remains a parse-only check.

What it does:

- runs host `clang`
- uses `-fsyntax-only`
- validates a selected migrated source surface

What it does not do:

- assemble startup code
- link against the STM32 memory map
- validate the GNU/newlib sysroot
- produce `.elf`, `.hex`, or `.bin`

Observed local result after the successful GNU build:

- final line: `syntax-check: all requested translation units parsed successfully`
- warnings emitted: `223 warnings generated`

Those warnings come from host `clang` parsing CMSIS ARM inline assembly helpers.
