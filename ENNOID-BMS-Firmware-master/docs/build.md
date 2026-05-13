# Build And Flash Path

## Current Status

This repository currently has:

- a syntax-only parser check: `./scripts/syntax_check.sh`
- Keil MDK project files
- CubeMX project files
- an STM32F303 linker script
- STM32F303 startup assembly

It does **not** currently have a tested repo-local GCC or CMake build path.

## What Was Verified

Commands checked during Phase 17:

- `./scripts/syntax_check.sh`
- `arm-none-eabi-gcc --version`

Results:

- `syntax_check.sh` parses the selected translation units successfully
- `arm-none-eabi-gcc` was not installed in this environment

Important limitation:

- `syntax_check.sh` is parse-only validation, not a full linked firmware build

## Recommended Application Build Path

Use the Keil project:

- `MDK-ARM/DieBieMS.uvprojx`

Why this project is the best current source of truth:

- it targets `STM32F303CC`
- it includes the application sources under `Main`, `Modules`, `Drivers`, and `Libraries`
- it includes the device startup file `Device/startup_stm32f303xc.s`
- it carries the application include paths and `USE_HAL_DRIVER,STM32F303xC` defines

Related CubeMX metadata also exists:

- `CubeMX/STM32F303CCTx.ioc`
- `CubeMX/MDK-ARM/STM32F303CCTx.uvprojx`
- `CubeMX/STM32F303CC_FLASH.ld`

Those files are useful for regeneration/reference, but the migrated firmware work is
currently centered on the top-level `MDK-ARM/DieBieMS.uvprojx` application project.

## Keil Build Steps

1. Open `MDK-ARM/DieBieMS.uvprojx` in Keil MDK-ARM v5.
2. Select the `DieBieMS` target.
3. Confirm the target device is `STM32F303CC`.
4. Build the target in Keil.
5. Inspect the Keil output window for compile, assemble, and link success.

Expected output directory from the project metadata:

- `MDK-ARM/DieBieMS/`

Typical artifacts to look for after a successful Keil build:

- `.axf`
- `.hex` if enabled in Keil post-build/output settings
- `.bin` if enabled in Keil post-build/output settings
- `.map` / listing artifacts depending on project settings

## Flashing Notes

This repository README states:

- application flash base: `0x08000000`
- bootloader flash base: `0x08032000`

Recommended flashing/debug path:

- ST-Link over SWD
- target MCU: STM32F303CC-class device with at least 256 kB flash

Board-level notes to verify on hardware before relying on flashing:

- `BOOT0` strapped for normal flash boot
- SWDIO / SWCLK / NRST access available
- target power and ground stable during programming

## Why A GCC Build Was Not Added Here

Although the repo contains promising pieces:

- `CubeMX/STM32F303CC_FLASH.ld`
- `Device/startup_stm32f303xc.s`
- CMSIS/HAL source trees

there is not yet a tested repo-local build description that pins:

- the exact source file list
- the exact preprocessor defines
- the full include path set
- any required assembler/linker flags
- output artifact generation

Adding a GCC/CMake path without proving it builds would be misleading, so this
phase documents the current truthful state instead.

## Open Questions

- Whether `MDK-ARM/DieBieMS.uvprojx` or the CubeMX MDK project should be treated as
  the single long-term project source of truth
- Whether a future repo-local GCC build should link against the existing flash map
  from `modFlash.c` / bootloader assumptions
- Which exact post-build artifacts (`.hex`, `.bin`) are enabled in the maintainer's
  Keil configuration
