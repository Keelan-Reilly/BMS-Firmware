# Flash Update Contract

## Address map

- `FLASH_BASE = 0x08000000`
- `FLASH_END = 0x0803FFFF`
- `page size = 0x800` bytes
- `app vector start = 0x08000000`
- `EEPROM page 0 = 0x08000800`
- `EEPROM page 1 = 0x08001000`
- `app body start = 0x08001800`
- `staged app start = 0x08019000`
- `bootloader start = 0x08032000`
- `bootloader end exclusive = 0x08040000`

## Capacities

- `max staged image size = 0x19000 = 102400 bytes`
- `max application image size = 0x32000 = 204800 bytes`
- Usable linked application body region after EEPROM hole:
  - `0x08001800 .. 0x08031FFF`
  - size `0x30800 = 198656 bytes`

## Historical staged-image format

The current raw staged-image layout is:

1. `uint32 big-endian app_size`
2. `uint16 big-endian CRC16-CCITT-FALSE over payload bytes`
3. `payload bytes` copied to application flash starting at `0x08000000`

Notes:
- This is not a self-describing package format yet.
- Tool and bootloader must agree that the CRC algorithm is `CRC16-CCITT-FALSE`.
- Endianness is big-endian because the existing packet helpers write big-endian integers.
- Firmware-side `scripts/check_flash_layout.py` is required before update/upload and now checks both `firmware.bin` size and allocated `firmware.elf` section placement.
- The flash-layout checker does not make the update transactional.

## Erase/copy behavior

- Tool uploads raw bytes into the staged region at `0x08019000`.
- Bootloader validates metadata and payload before erase/copy.
- Bootloader erases destination pages starting at `0x08000000`.
- Current behavior does erase through the EEPROM pages if the application image spans them.
- EEPROM preservation is therefore **not guaranteed** by the current raw-image update flow and must be treated as an explicit risk until a package format or split-copy strategy is introduced.

## Required bootloader validation

Before erase/copy:

- `size > 0`
- `size <= 0x19000`
- `size <= destination app size limit`
- CRC16 matches exactly
- staged payload source stays inside `0x08019000 .. 0x08031FFF`
- destination payload stays inside `0x08000000 .. 0x08031FFF`
- source/destination do not overlap bootloader
- initial stack pointer is inside SRAM `0x20000000 .. 0x20009FFF`
- reset handler is inside application flash, not in EEPROM, not in bootloader

## App / bootloader transitions

- Application requests install by sending `COMM_JUMP_TO_BOOTLOADER` after staging completes.
- Bootloader runs from `0x08032000`.
- Current bootloader return path is `NVIC_SystemReset()` after copy, not a manual vector jump back into the app.

## Current repo agreement targets

These files must stay aligned:

- Firmware linker:
  - [gcc/STM32F303CC_APP_FLASH.ld](/Users/keelan/Desktop/BMS%20Firmware/ENNOID-BMS-Firmware-master/gcc/STM32F303CC_APP_FLASH.ld)
- Firmware update command handlers:
  - [Modules/Src/modCommands.c](/Users/keelan/Desktop/BMS%20Firmware/ENNOID-BMS-Firmware-master/Modules/Src/modCommands.c)
- Tool upload flow:
  - [commands.cpp](/Users/keelan/Desktop/BMS%20Firmware/ENNOID-BMS-Tool-master/commands.cpp)
  - [pages/pagefirmware.cpp](/Users/keelan/Desktop/BMS%20Firmware/ENNOID-BMS-Tool-master/pages/pagefirmware.cpp)
- Bootloader flash constants and validation:
  - [Modules/Inc/modFlash.h](/Users/keelan/Desktop/BMS%20Firmware/DieBieMS-Bootloader-master/Modules/Inc/modFlash.h)
  - [Main/main.c](/Users/keelan/Desktop/BMS%20Firmware/DieBieMS-Bootloader-master/Main/main.c)

## Validation requirement

Before any update/upload workflow is treated as safe in software:

- `scripts/check_flash_layout.py --root .` must pass
- The script must confirm:
  - `firmware.bin` fits the staged region
  - `firmware.bin` fits the application region
  - allocated ELF flash sections stay inside:
    - `0x08000000..0x080007ff`
    - `0x08001800..0x08031fff`
  - no allocated ELF section overlaps:
    - `0x08000800..0x08000fff`
    - `0x08001000..0x080017ff`
    - `0x08032000..0x0803ffff`
  - load addresses for copied sections do not land in forbidden flash

This validation does not change the current update behavior:

- Bootloader update is still non-transactional.
- EEPROM/config pages are still not preserved by the current raw staged-image flow.
