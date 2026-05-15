Authoritative contract lives in:

- [../../ENNOID-BMS-Firmware-master/docs/flash_update_contract.md](/Users/keelan/Desktop/BMS%20Firmware/ENNOID-BMS-Firmware-master/docs/flash_update_contract.md)

Bootloader-specific obligations:
- validate staged size, CRC, stack pointer, reset handler, and flash bounds before erase/copy
- never touch the bootloader region during application installs
- treat EEPROM erase/copy behavior as explicit and documented, not implicit
- require the firmware-side `scripts/check_flash_layout.py` ELF-section audit to pass before update/upload is considered software-safe
- note that the stronger flash-layout checker does not make the update transactional
- host-side checker: `python3 tools/check_staged_image.py --self-test` and `python3 tools/check_staged_image.py --app-bin <firmware.bin>`
