#!/usr/bin/env python3
from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


FLASH_BASE = 0x08000000
FLASH_END = 0x0803FFFF
EEPROM_PAGE_0_START = 0x08000800
EEPROM_PAGE_0_END = 0x08000FFF
EEPROM_PAGE_1_START = 0x08001000
EEPROM_PAGE_1_END = 0x080017FF
APPVEC_START = 0x08000000
APPVEC_END = 0x080007FF
APP_BODY_START = 0x08001800
APP_BODY_END = 0x08031FFF
STAGED_APP_START = 0x08019000
BOOTLOADER_START = 0x08032000
BOOTLOADER_END_EXCLUSIVE = 0x08040000

EXPECTED = {
    "APPVEC_ORIGIN": APPVEC_START,
    "APPVEC_LENGTH": 0x00000800,
    "APPFLASH_ORIGIN": APP_BODY_START,
    "APPFLASH_LENGTH": 0x00030800,
}


@dataclass
class MemoryRegion:
    name: str
    origin: int
    length: int

    @property
    def end_inclusive(self) -> int:
        return self.origin + self.length - 1


@dataclass
class SectionInfo:
    name: str
    size: int
    vma: int
    lma: int
    flags: str

    @property
    def vma_end(self) -> int:
        return self.vma + self.size - 1

    @property
    def lma_end(self) -> int:
        return self.lma + self.size - 1


def parse_size_literal(value: str) -> int:
    value = value.strip()
    if value.lower().endswith("k"):
        return int(value[:-1], 0) * 1024
    if value.lower().endswith("m"):
        return int(value[:-1], 0) * 1024 * 1024
    return int(value, 0)


def parse_linker(linker_text: str) -> dict[str, MemoryRegion]:
    regions: dict[str, MemoryRegion] = {}
    for match in re.finditer(
        r"^\s*(\w+)\s*\(([^)]*)\)\s*:\s*ORIGIN\s*=\s*([^,]+),\s*LENGTH\s*=\s*([^\s]+)",
        linker_text,
        re.MULTILINE,
    ):
        name = match.group(1)
        origin = parse_size_literal(match.group(3))
        length = parse_size_literal(match.group(4))
        regions[name] = MemoryRegion(name=name, origin=origin, length=length)
    if not regions:
        raise SystemExit("failed to parse linker MEMORY regions")
    return regions


def run_tool(command: list[str]) -> str:
    result = subprocess.run(command, check=True, capture_output=True, text=True)
    return result.stdout


def parse_objdump_sections(elf_path: Path, objdump: str) -> list[SectionInfo]:
    output = run_tool([objdump, "-h", str(elf_path)])
    sections: list[SectionInfo] = []
    current: SectionInfo | None = None

    section_re = re.compile(
        r"^\s*(\d+)\s+(\S+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+([0-9A-Fa-f]+)\s+[0-9A-Fa-f]+\s+\S+"
    )

    for line in output.splitlines():
        section_match = section_re.match(line)
        if section_match:
            current = SectionInfo(
                name=section_match.group(2),
                size=int(section_match.group(3), 16),
                vma=int(section_match.group(4), 16),
                lma=int(section_match.group(5), 16),
                flags="",
            )
            sections.append(current)
            continue
        if current is not None and line.startswith("                  "):
            current.flags = line.strip()
            current = None

    return sections


def range_overlap(start: int, end: int, other_start: int, other_end: int) -> bool:
    return start <= other_end and other_start <= end


def in_region(start: int, end: int, region: MemoryRegion) -> bool:
    return start >= region.origin and end <= region.end_inclusive


def classify_flash_range(start: int, end: int) -> tuple[str, list[str]]:
    issues: list[str] = []
    if start < FLASH_BASE or end > FLASH_END:
        issues.append("outside valid flash")

    if range_overlap(start, end, EEPROM_PAGE_0_START, EEPROM_PAGE_0_END):
        issues.append("overlaps EEPROM page 0")
    if range_overlap(start, end, EEPROM_PAGE_1_START, EEPROM_PAGE_1_END):
        issues.append("overlaps EEPROM page 1")
    if range_overlap(start, end, BOOTLOADER_START, FLASH_END):
        issues.append("overlaps bootloader region")

    if start >= APPVEC_START and end <= APPVEC_END:
        return "flash:app-vectors", issues
    if start >= APP_BODY_START and end <= APP_BODY_END:
        return "flash:app-body", issues
    if not issues:
        issues.append("not in an allowed flash region")
    return "flash:forbidden", issues


def classify_ram_range(start: int, end: int, regions: dict[str, MemoryRegion]) -> tuple[str, list[str]]:
    ram = regions.get("RAM")
    ccmram = regions.get("CCMRAM")
    if ram and in_region(start, end, ram):
        return "ram:sram", []
    if ccmram and in_region(start, end, ccmram):
        return "ram:ccmram", []
    return "ram:invalid", ["outside valid SRAM/CCMRAM"]


def inspect_sections(sections: list[SectionInfo], regions: dict[str, MemoryRegion]) -> tuple[list[dict], list[str]]:
    rows: list[dict] = []
    failures: list[str] = []

    for section in sections:
        if section.size == 0 or "ALLOC" not in section.flags:
            continue

        reason_parts: list[str] = []
        status = "OK"

        if FLASH_BASE <= section.vma <= FLASH_END:
            region_label, issues = classify_flash_range(section.vma, section.vma_end)
            reason_parts.extend(issues)
        else:
            region_label, issues = classify_ram_range(section.vma, section.vma_end, regions)
            reason_parts.extend(issues)

        if "LOAD" in section.flags and FLASH_BASE <= section.lma <= FLASH_END:
            _, load_issues = classify_flash_range(section.lma, section.lma_end)
            reason_parts.extend(f"LMA {issue}" for issue in load_issues)

        if reason_parts:
            status = "FAIL"
            failures.append(f"{section.name}: " + "; ".join(reason_parts))

        rows.append(
            {
                "section": section.name,
                "vma_start": section.vma,
                "vma_end": section.vma_end,
                "size": section.size,
                "lma_start": section.lma,
                "region": region_label,
                "status": status,
                "reason": ", ".join(reason_parts) if reason_parts else "clean",
            }
        )

    return rows, failures


def print_table(rows: list[dict]) -> None:
    print("Allocated ELF sections:")
    print("section               vma_start    vma_end      size     lma_start    region            status reason")
    for row in rows:
        print(
            f"{row['section']:<20} "
            f"0x{row['vma_start']:08X} "
            f"0x{row['vma_end']:08X} "
            f"0x{row['size']:06X} "
            f"0x{row['lma_start']:08X} "
            f"{row['region']:<16} "
            f"{row['status']:<5} "
            f"{row['reason']}"
        )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", default=Path(__file__).resolve().parents[1], type=Path)
    parser.add_argument("--bin", default=None, type=Path)
    parser.add_argument("--elf", default=None, type=Path)
    args = parser.parse_args()

    root = args.root.resolve()
    linker = root / "gcc" / "STM32F303CC_APP_FLASH.ld"
    regions = parse_linker(linker.read_text())

    failures: list[str] = []

    for region_name, expected_value in EXPECTED.items():
        memory_name, field_name = region_name.split("_", 1)
        region = regions[memory_name]
        actual = region.origin if field_name == "ORIGIN" else region.length
        if actual != expected_value:
            failures.append(f"{region_name}: expected 0x{expected_value:08X}, got 0x{actual:08X}")

    staged_max = BOOTLOADER_START - STAGED_APP_START
    app_max = BOOTLOADER_START - FLASH_BASE

    bin_path = args.bin or (root / "build" / "firmware.bin")
    elf_path = args.elf or (root / "build" / "firmware.elf")
    if not bin_path.exists():
        failures.append(f"firmware.bin not found at {bin_path}")
        size = None
    else:
        size = bin_path.stat().st_size
        if size > staged_max:
            failures.append(f"firmware.bin too large for staged region: {size} > {staged_max}")
        if size > app_max:
            failures.append(f"firmware.bin too large for application region: {size} > {app_max}")

    if not elf_path.exists():
        failures.append(f"firmware.elf not found at {elf_path}")
        rows = []
    else:
        objdump = shutil.which("arm-none-eabi-objdump")
        if not objdump:
            failures.append("arm-none-eabi-objdump not found in PATH")
            rows = []
        else:
            rows, section_failures = inspect_sections(parse_objdump_sections(elf_path, objdump), regions)
            failures.extend(section_failures)

    if size is not None:
        staged_margin = staged_max - size
        app_margin = app_max - size
        print(f"firmware.bin size={size} staged_max={staged_max} staged_margin={staged_margin} app_max={app_max} app_margin={app_margin}")
    else:
        print(f"firmware.bin missing; staged_max={staged_max} app_max={app_max}")

    print(
        "memory regions: "
        f"flash=0x{FLASH_BASE:08X}..0x{FLASH_END:08X} "
        f"eeprom0=0x{EEPROM_PAGE_0_START:08X}..0x{EEPROM_PAGE_0_END:08X} "
        f"eeprom1=0x{EEPROM_PAGE_1_START:08X}..0x{EEPROM_PAGE_1_END:08X} "
        f"appvec=0x{APPVEC_START:08X}..0x{APPVEC_END:08X} "
        f"appbody=0x{APP_BODY_START:08X}..0x{APP_BODY_END:08X} "
        f"bootloader=0x{BOOTLOADER_START:08X}..0x{FLASH_END:08X}"
    )

    if rows:
        print_table(rows)

    eeprom_clean = all(
        row["status"] == "OK" or ("EEPROM" not in row["reason"])
        for row in rows
    )
    bootloader_clean = all(
        row["status"] == "OK" or ("bootloader" not in row["reason"])
        for row in rows
    )
    print(f"EEPROM pages clean: {'yes' if eeprom_clean else 'no'}")
    print(f"Bootloader region clean: {'yes' if bootloader_clean else 'no'}")

    if failures:
        for failure in failures:
            print(f"FAIL: {failure}")
        return 1

    print("flash layout check passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
