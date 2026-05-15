#!/usr/bin/env python3
"""
Host-side checker for the DieBieMS/ENNOID staged application update contract.

The bootloader stores:
    uint32 size   (big-endian)
    uint16 crc16  (big-endian, CCITT-FALSE)
    uint8[size] application image
"""

from __future__ import annotations

import argparse
import struct
import sys


FLASH_BASE = 0x08000000
EEPROM_PAGE0 = 0x08000800
APP_BODY_START = 0x08001800
STAGED_START = 0x08019000
BOOTLOADER_START = 0x08032000
FLASH_END = 0x08040000
SRAM_START = 0x20000000
SRAM_END = 0x2000A000
STAGED_MAX = BOOTLOADER_START - STAGED_START
APP_MAX = BOOTLOADER_START - FLASH_BASE
HEADER_SIZE = 6


def crc16_ccitt_false(data: bytes) -> int:
    checksum = 0
    for byte in data:
        checksum ^= byte << 8
        for _ in range(8):
            if checksum & 0x8000:
                checksum = ((checksum << 1) ^ 0x1021) & 0xFFFF
            else:
                checksum = (checksum << 1) & 0xFFFF
    return checksum & 0xFFFF


def validate_payload(payload: bytes) -> list[str]:
    errors: list[str] = []

    if not payload:
        return ["application payload is empty"]
    if len(payload) > STAGED_MAX:
        errors.append(f"payload size {len(payload)} exceeds staged max {STAGED_MAX}")
    if len(payload) > APP_MAX:
        errors.append(f"payload size {len(payload)} exceeds application max {APP_MAX}")
    if len(payload) < 8:
        errors.append("payload is too short for a vector table")
        return errors

    initial_sp, reset_handler = struct.unpack("<II", payload[:8])
    if not (SRAM_START <= initial_sp < SRAM_END):
        errors.append(f"initial stack pointer 0x{initial_sp:08X} is outside SRAM")
    if not (APP_BODY_START <= reset_handler < BOOTLOADER_START):
        errors.append(f"reset handler 0x{reset_handler:08X} is outside the application body")
    if EEPROM_PAGE0 <= reset_handler < APP_BODY_START:
        errors.append(f"reset handler 0x{reset_handler:08X} points into EEPROM/reserved flash")
    if BOOTLOADER_START <= reset_handler < FLASH_END:
        errors.append(f"reset handler 0x{reset_handler:08X} points into the bootloader")
    if FLASH_BASE + len(payload) > BOOTLOADER_START:
        errors.append("destination application range would overlap the bootloader")

    return errors


def validate_staged_blob(blob: bytes) -> list[str]:
    errors: list[str] = []
    if len(blob) < HEADER_SIZE:
        return ["blob is shorter than the staged metadata header"]

    size, crc = struct.unpack(">IH", blob[:HEADER_SIZE])
    payload = blob[HEADER_SIZE:]

    if size == 0:
        errors.append("staged size is zero")
    if size != len(payload):
        errors.append(f"staged size field {size} does not match payload length {len(payload)}")
    if size > STAGED_MAX:
        errors.append(f"staged size {size} exceeds staged max {STAGED_MAX}")
    if crc16_ccitt_false(payload) != crc:
        errors.append("staged CRC16 does not match payload")

    errors.extend(validate_payload(payload))
    return errors


def build_staged_blob(app_payload: bytes) -> bytes:
    return struct.pack(">IH", len(app_payload), crc16_ccitt_false(app_payload)) + app_payload


def run_self_test() -> int:
    good_payload = bytearray(256)
    good_payload[:8] = struct.pack("<II", 0x20002000, 0x08001840)
    good_blob = build_staged_blob(bytes(good_payload))
    if validate_staged_blob(good_blob):
        print("self-test failed: good image rejected", file=sys.stderr)
        return 1

    bad_crc = bytearray(good_blob)
    bad_crc[5] ^= 0x01
    if "staged CRC16 does not match payload" not in validate_staged_blob(bytes(bad_crc)):
        print("self-test failed: bad CRC not detected", file=sys.stderr)
        return 1

    bad_sp_payload = bytearray(good_payload)
    bad_sp_payload[:4] = struct.pack("<I", 0x10000000)
    if not any("stack pointer" in err for err in validate_payload(bytes(bad_sp_payload))):
        print("self-test failed: bad stack pointer not detected", file=sys.stderr)
        return 1

    bad_reset_payload = bytearray(good_payload)
    bad_reset_payload[4:8] = struct.pack("<I", 0x08032020)
    if not any("bootloader" in err or "application body" in err for err in validate_payload(bytes(bad_reset_payload))):
        print("self-test failed: bad reset handler not detected", file=sys.stderr)
        return 1

    oversize_payload = bytes(STAGED_MAX + 1)
    if not any("staged max" in err for err in validate_payload(oversize_payload)):
        print("self-test failed: oversize image not detected", file=sys.stderr)
        return 1

    print("self-test passed")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate a bootloader staged-image blob or application payload.")
    parser.add_argument("--staged-blob", help="Path to a raw staged blob: 4-byte BE size, 2-byte BE CRC16, then payload.")
    parser.add_argument("--app-bin", help="Path to an application binary payload to validate.")
    parser.add_argument("--self-test", action="store_true", help="Run built-in validation vectors.")
    args = parser.parse_args()

    if args.self_test:
        return run_self_test()

    if bool(args.staged_blob) == bool(args.app_bin):
        parser.error("pass exactly one of --staged-blob or --app-bin")

    if args.staged_blob:
        blob = open(args.staged_blob, "rb").read()
        errors = validate_staged_blob(blob)
    else:
        payload = open(args.app_bin, "rb").read()
        errors = validate_payload(payload)

    if errors:
        for err in errors:
            print(f"error: {err}", file=sys.stderr)
        return 1

    print("image validation passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
