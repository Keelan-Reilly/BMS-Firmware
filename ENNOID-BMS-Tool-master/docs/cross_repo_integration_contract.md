Authoritative contract lives in:

- [../ENNOID-BMS-Firmware-master/docs/cross_repo_integration_contract.md](/Users/keelan/Desktop/BMS%20Firmware/ENNOID-BMS-Firmware-master/docs/cross_repo_integration_contract.md)

Tool-specific obligations:
- request `COMM_BMS_GET_CAPABILITIES` after firmware version
- classify connection mode before enabling config or update flows
- block legacy ENNOID config for migrated STM32F303/LTC6812 firmware
- reject update uploads when target identity or staged-size contract is unknown
- treat Config V2 as a fixed `112`-byte little-endian object with internal CRC bytes at `18..19` covering bytes `20..111`
- keep Config V2 persistent store hidden/blocked until firmware explicitly supports it
