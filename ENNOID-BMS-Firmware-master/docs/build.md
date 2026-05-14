# Build And Flash Path

## Current Status

This repo now has a checked-in GNU Arm build path for the migrated STM32F303 firmware:

- build entrypoint: `Makefile`
- linker script: `gcc/STM32F303CC_APP_FLASH.ld`
- startup file used by GNU build: `Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s`
- system file used by GNU build: `CubeMX/Src/system_stm32f3xx.c`

The build was verified up to the point where the local toolchain failed before compiling project code. The remaining blocker is external to the repo: the installed `arm-none-eabi-gcc` on this Mac was built `--without-headers`, so it has no usable `arm-none-eabi` C library headers/sysroot.

## Preferred Local Build Path

Use GNU Arm Embedded as the main local build path for the migrated firmware.

Build command:

- `make -j4`

Clean command:

- `make clean`

Syntax-only parse check:

- `./scripts/syntax_check.sh`

## Required Toolchain

Required binaries:

- `arm-none-eabi-gcc`
- `arm-none-eabi-objcopy`
- `arm-none-eabi-size`
- `make`

### Verified On This Mac

Present in this environment:

- `/opt/homebrew/bin/arm-none-eabi-gcc`
- `/opt/homebrew/bin/arm-none-eabi-objcopy`
- `/opt/homebrew/bin/arm-none-eabi-size`
- `/usr/bin/make`

However, the installed compiler is not a complete bare-metal toolchain. `arm-none-eabi-gcc -v` reports it was configured with:

- `--without-headers`

And `arm-none-eabi-gcc -xc -E -Wp,-v - </dev/null` shows these include directories are missing:

- `.../arm-none-eabi/sys-include`
- `.../arm-none-eabi/include`

That causes the first real build to fail on:

- `fatal error: stdint.h: No such file or directory`

### Install Guidance For This Mac

Do not rely on the current Homebrew `arm-none-eabi-gcc` bottle alone if it is built without headers.

Use one of these approaches:

1. Install a complete Arm bare-metal toolchain package from Arm Developer for macOS Apple silicon.
2. Or install a Homebrew-packaged `arm-none-eabi` sysroot/newlib setup if and when one is available and matches `arm-none-eabi-gcc`.

What was verified locally:

- `brew search --formula arm-none-eabi` shows `arm-none-eabi-binutils`, `arm-none-eabi-gcc`, `arm-none-eabi-gdb`
- `brew search --formula newlib` did not show a matching Arm bare-metal newlib formula in this environment

Because of that, the most reliable path on this Mac is likely the official Arm GNU Toolchain package rather than the current compiler-only Homebrew install.

Official Arm sources used for this conclusion:

- Arm GNU Toolchain downloads: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
- Legacy GNU Arm Embedded downloads page noting newer content moved to Arm GNU Toolchain downloads: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

## Target And Build Settings

Target MCU:

- `STM32F303CC`
- define: `STM32F303xC`

Compiler settings in `Makefile`:

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

- `-mfloat-abi=soft` was chosen conservatively to match the checked-in GCC startup file, which declares `.fpu softvfp`.
- The build does not try to change runtime behavior, safety logic, LTC6812 packet behavior, or shutdown polarity.

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
  - all files under `Libraries/Scr/`

Excluded as stale or not part of the active migrated build:

- `Drivers/SWDrivers/Src/driverSWLTC6803.c`
- `Drivers/SWDrivers/Src/driverSWCC1101.c`
- `Modules/Src/modMessage.c`
- `CubeMX/Src/main.c`
- `CubeMX/Src/stm32f3xx_hal_msp.c`
- `CubeMX/Src/stm32f3xx_it.c`
- `Device/startup_stm32f303xc.s`

Rationale:

- `driverSWLTC6812.c` is the active migrated cell/TEMP-chain driver.
- `driverSWLTC6803.c` is still present in headers for legacy compatibility but is not used by the current migrated measurement path.
- `Device/startup_stm32f303xc.s` is Keil/ARMASM syntax, not GCC syntax.
- `modTerminal.c` and `modFlash.c` remain included because `modCommands.c` still dispatches terminal commands and firmware update packets.

## Memory Layout

The GNU build does not use `CubeMX/STM32F303CC_FLASH.ld` unchanged because that script assumes one contiguous application image from `0x08000000`.

The repo's actual application layout reserves flash pages for EEPROM emulation:

- `0x08000000 .. 0x080007ff` : application vector table / early code
- `0x08000800 .. 0x08000fff` : EEPROM emulation page 0
- `0x08001000 .. 0x080017ff` : EEPROM emulation page 1
- `0x08001800 .. 0x08031fff` : main application body
- `0x08032000 ..` : bootloader region

That layout is encoded in:

- `gcc/STM32F303CC_APP_FLASH.ld`

## Expected Output Artifacts

When a complete GNU Arm toolchain is installed, `make -j4` is intended to generate:

- `build/firmware.elf`
- `build/firmware.hex`
- `build/firmware.bin`
- `build/firmware.map`

And print size output with:

- `arm-none-eabi-size build/firmware.elf`

## Build Attempt Performed

Command run:

- `make -j4`

Observed result on this machine:

- build started correctly
- source list expansion worked
- startup/system/linker path resolved
- compilation stopped immediately because the local toolchain could not find `<stdint.h>`

Exact failure:

- `/opt/homebrew/Cellar/arm-none-eabi-gcc/16.1.0/lib/gcc/arm-none-eabi/16.1.0/include/stdint.h:11:16: fatal error: stdint.h: No such file or directory`

This is a toolchain packaging issue, not a firmware source regression.

## Syntax Check Limitation

`./scripts/syntax_check.sh` remains a parse-only check.

What it does:

- runs host `clang`
- uses `-fsyntax-only`
- validates a selected migrated source surface

What it does not do:

- assemble startup code
- link against the STM32 memory map
- verify newlib/sysroot completeness
- produce `.elf`, `.hex`, or `.bin`

Observed local result:

- final line: `syntax-check: all requested translation units parsed successfully`
- warnings emitted: `223 warnings generated`

Those warnings come from host `clang` parsing CMSIS ARM inline assembly helpers.

## Keil Status

The old MDK project remains in the repo but is legacy/stale for the migrated build surface:

- `MDK-ARM/DieBieMS.uvprojx`

This phase does not update Keil as the main build path.

Important mismatch:

- migrated GNU build uses `driverSWLTC6812.c`
- stale Keil project still references `driverSWLTC6803.c`

Treat the GNU `Makefile` path as the intended reproducible local path going forward once a complete Arm bare-metal toolchain is installed.

## Flashing Notes

Expected debug/programming path:

- ST-Link over SWD

Typical flash artifacts to use:

- `build/firmware.hex`
- or `build/firmware.bin`

Before flashing:

- verify bootloader/application split still matches the target board
- verify EEPROM emulation pages are preserved
- verify BOOT0 and reset wiring are correct
- verify SWDIO, SWCLK, NRST, VTref, and GND access

## Honest Summary

What is now verified:

- the repo contains a real GNU Arm build definition
- the build definition uses the migrated LTC6812-era source surface
- the linker script respects the repo's reserved flash layout
- `syntax_check.sh` still passes after the build-system changes

What is not yet verified on this machine:

- a fully linked `build/firmware.elf`
- generated `.hex` and `.bin` artifacts
- final firmware size

Remaining blocker:

- install a complete `arm-none-eabi` toolchain with standard headers and bare-metal sysroot/newlib for macOS Apple silicon
