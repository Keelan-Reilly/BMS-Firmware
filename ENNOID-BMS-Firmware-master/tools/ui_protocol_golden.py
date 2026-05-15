#!/usr/bin/env python3
"""
Golden packet verification for firmware COMM_EBMS_* handlers against the
existing ENNOID/DieBieMS UI decoder expectations.

This script is intentionally protocol-only. It does not talk to hardware and it
does not mutate firmware behavior.
"""

from __future__ import annotations

import argparse
import importlib.util
import math
import re
import struct
import sys
import xml.etree.ElementTree as ET
from dataclasses import dataclass
from pathlib import Path
from types import ModuleType


FW_ROOT = Path(__file__).resolve().parents[1]
WORKSPACE_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_UI_ROOT = WORKSPACE_ROOT / "ENNOID-BMS-Tool-master"


COMMAND_IDS = {
    "COMM_EBMS_STORE_CONF": 150,
    "COMM_EBMS_GET_CELLS": 151,
    "COMM_EBMS_GET_AUX": 152,
    "COMM_EBMS_GET_EXP_TEMP": 153,
    "COMM_EBMS_SET_MCCONF": 154,
    "COMM_EBMS_GET_MCCONF": 155,
    "COMM_EBMS_GET_MCCONF_DEFAULT": 156,
    "COMM_EBMS_GET_VALUES": 157,
    "COMM_EBMS_GET_BMS_STATUS_EXT": 158,
    "COMM_BMS_GET_CAPABILITIES": 159,
    "COMM_BMS_GET_CONFIG_V2": 160,
    "COMM_BMS_SET_CONFIG_V2": 161,
    "COMM_BMS_STORE_CONFIG_V2": 162,
    "COMM_BMS_GET_CONFIG_DEFAULT_V2": 163,
    "COMM_BMS_VALIDATE_CONFIG_V2": 164,
    "COMM_BMS_GET_CONFIG_SCHEMA_V2": 165,
}

FIRMWARE_CONFIG_ORDER = [
    "noOfCells",
    "batteryCapacity",
    "cellHardUnderVoltage",
    "cellHardOverVoltage",
    "cellLCSoftUnderVoltage",
    "cellHCSoftUnderVoltage",
    "cellSoftOverVoltage",
    "cellBalanceDifferenceThreshold",
    "cellBalanceStart",
    "cellThrottleUpperStart",
    "cellThrottleLowerStart",
    "cellThrottleUpperMargin",
    "cellThrottleLowerMargin",
    "shuntLCFactor",
    "shuntLCOffset",
    "shuntHCFactor",
    "shuntHCOffset",
    "throttleChargeIncreaseRate",
    "throttleDisChargeIncreaseRate",
    "cellBalanceUpdateInterval",
    "maxSimultaneousDischargingCells",
    "timeoutDischargeRetry",
    "hysteresisDischarge",
    "timeoutChargeRetry",
    "hysteresisCharge",
    "timeoutChargeCompleted",
    "timeoutChargingCompletedMinimalMismatch",
    "maxMismatchThreshold",
    "chargerEnabledThreshold",
    "timeoutChargerDisconnected",
    "minimalPrechargePercentage",
    "timeoutLCPreCharge",
    "maxAllowedCurrent",
    "displayTimeoutBatteryDead",
    "displayTimeoutBatteryError",
    "displayTimeoutBatteryErrorPreCharge",
    "displayTimeoutSplashScreen",
    "maxUnderAndOverVoltageErrorCount",
    "notUsedCurrentThreshold",
    "notUsedTimeout",
    "stateOfChargeStoreInterval",
    "CANID",
    "CANIDStyle",
    "emitStatusOverCAN",
    "tempEnableMaskBMS",
    "tempEnableMaskBattery",
    "LCUseDischarge",
    "LCUsePrecharge",
    "allowChargingDuringDischarge",
    "allowForceOn",
    "pulseToggleButton",
    "togglePowerModeDirectHCDelay",
    "useCANSafetyInput",
    "useCANDelayedPowerDown",
    "HCUseRelay",
    "HCUsePrecharge",
    "timeoutHCPreCharge",
    "timeoutHCPreChargeRetryInterval",
    "timeoutHCRelayOverlap",
    "NTCLTCTopResistor",
    "NTCLTC25Deg",
    "NTCLTCBeta",
    "NTCPCBTopResistor",
    "NTCPCB25Deg",
    "NTCPCBBeta",
    "NTCEXPTopResistor",
    "NTCEXP25Deg",
    "NTCEXPBeta",
    "NTCHiAmpPCBTopResistor",
    "NTCHiAmpPCB25Deg",
    "NTCHiAmpPCBBeta",
]

EXPECTED_CONFIG_V2_WIRE_SIZE = 112
EXPECTED_CONFIG_V2_CRC_OFFSET = 18
EXPECTED_CONFIG_V2_CRC_COVERAGE_START = 20
EXPECTED_CONFIG_V2_CRC_COVERAGE_END = 111
EXPECTED_CONFIG_V2_CRC_VALUE = 0x3D58
EXPECTED_CONFIG_V2_MASK_BYTES = 10
EXPECTED_CONFIG_V2_MAGIC = 0x43464732
EXPECTED_CONFIG_V2_SCHEMA_VERSION = 1
EXPECTED_HARDWARE_PROFILE = 1
EXPECTED_CELL_COUNT = 75
EXPECTED_TEMP_COUNT = 75

CANONICAL_CONFIG_V2 = {
    "magic": EXPECTED_CONFIG_V2_MAGIC,
    "schemaVersion": EXPECTED_CONFIG_V2_SCHEMA_VERSION,
    "payloadLength": EXPECTED_CONFIG_V2_WIRE_SIZE,
    "generation": 0x11223344,
    "hardwareProfile": EXPECTED_HARDWARE_PROFILE,
    "cellCount": EXPECTED_CELL_COUNT,
    "tempCount": EXPECTED_TEMP_COUNT,
    "flags": 0x55AA,
    "bodyCrc": EXPECTED_CONFIG_V2_CRC_VALUE,
    "cellOvSoftMv": 4111,
    "cellOvHardMv": 4277,
    "cellUvSoftMv": 2888,
    "cellUvHardMv": 2111,
    "chargeTempLimitDeciC": 123,
    "dischargeTempLimitDeciC": 456,
    "hardTempLimitDeciC": 789,
    "minimalPrechargePermille": 875,
    "lowCurrentPrechargeTimeoutMs": 3210,
    "requiredCellMask": [0xA5, 0x5A, 0xC3, 0x3C, 0x96, 0x69, 0xF0, 0x0F, 0x55, 0x05],
    "requiredTempMask": [0x11, 0x22, 0x44, 0x88, 0x13, 0x37, 0xC0, 0xDE, 0xAA, 0x07],
    "balanceAllowedMask": [0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0xEF, 0x03],
    "vpackGainMicroPerVolt": 1234567,
    "vpackOffsetMicroVolt": -234567,
    "islVbatGainMicroPerVolt": 3456789,
    "islVbatOffsetMicroVolt": -456789,
    "currentGainMicroPerAmp": 567890,
    "currentOffsetMicroAmp": -67890,
    "currentSign": 1,
    "openWirePolicy": 2,
    "balanceStartMv": 3799,
    "balanceDiffMv": 17,
    "tempSettleTimeMs": 250,
    "canTelemetryFlags": 0x1357,
    "featureFlags": 0x2468,
    "reserved": [0] * 8,
}

CANONICAL_CONFIG_V2_HEX = (
    "32474643010070004433221101004b4baa55583d0f10b510480b3f087b00c80115036b038a0c"
    "a55ac33c9669f00f5505112244881337c0deaa07fedcba9876543210ef0387d61200b96bfcff"
    "15bf3400ab07f9ff52aa0800cef6feff0102d70e1100fa00571368240000000000000000"
)

EXPECTED_RESULT_CODES = {
    "BMS_CONFIG_V2_RESULT_OK": 0,
    "BMS_CONFIG_V2_RESULT_UNSUPPORTED_VERSION": 1,
    "BMS_CONFIG_V2_RESULT_BAD_MAGIC": 2,
    "BMS_CONFIG_V2_RESULT_BAD_LENGTH": 3,
    "BMS_CONFIG_V2_RESULT_BAD_CRC": 4,
    "BMS_CONFIG_V2_RESULT_WRONG_HARDWARE_PROFILE": 5,
    "BMS_CONFIG_V2_RESULT_INVALID_CELL_COUNT": 6,
    "BMS_CONFIG_V2_RESULT_INVALID_TEMP_COUNT": 7,
    "BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER": 8,
    "BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_RANGE": 9,
    "BMS_CONFIG_V2_RESULT_INVALID_MASK": 10,
    "BMS_CONFIG_V2_RESULT_INVALID_CALIBRATION": 11,
    "BMS_CONFIG_V2_RESULT_STORE_FAILED": 12,
    "BMS_CONFIG_V2_RESULT_READBACK_FAILED": 13,
    "BMS_CONFIG_V2_RESULT_UNSUPPORTED_IN_CURRENT_MODE": 14,
}

EXPECTED_CONFIG_V2_LAYOUT = [
    ("magic", 0, 4, "uint32", "protocol magic"),
    ("schemaVersion", 4, 2, "uint16", "schema version"),
    ("payloadLength", 6, 2, "uint16", "fixed wire size"),
    ("generation", 8, 4, "uint32", "config generation counter"),
    ("hardwareProfile", 12, 2, "uint16", "target hardware profile"),
    ("cellCount", 14, 1, "uint8", "must be 75"),
    ("tempCount", 15, 1, "uint8", "must be 75"),
    ("flags", 16, 2, "uint16", "reserved config flags"),
    ("bodyCrc", 18, 2, "uint16", "CRC16 over bytes 20..111"),
    ("cellOvSoftMv", 20, 2, "uint16", "soft over-voltage"),
    ("cellOvHardMv", 22, 2, "uint16", "hard over-voltage"),
    ("cellUvSoftMv", 24, 2, "uint16", "soft under-voltage"),
    ("cellUvHardMv", 26, 2, "uint16", "hard under-voltage"),
    ("chargeTempLimitDeciC", 28, 2, "int16", "charge temperature limit"),
    ("dischargeTempLimitDeciC", 30, 2, "int16", "discharge temperature limit"),
    ("hardTempLimitDeciC", 32, 2, "int16", "hard temperature limit"),
    ("minimalPrechargePermille", 34, 2, "uint16", "minimum precharge percentage x1000"),
    ("lowCurrentPrechargeTimeoutMs", 36, 2, "uint16", "precharge timeout ms"),
    ("requiredCellMask", 38, 10, "bytes[10]", "required cell mask, 75 bits"),
    ("requiredTempMask", 48, 10, "bytes[10]", "required temp mask, 75 bits"),
    ("balanceAllowedMask", 58, 10, "bytes[10]", "balance-allowed mask, 75 bits"),
    ("vpackGainMicroPerVolt", 68, 4, "int32", "Vpack gain"),
    ("vpackOffsetMicroVolt", 72, 4, "int32", "Vpack offset"),
    ("islVbatGainMicroPerVolt", 76, 4, "int32", "ISL Vbat gain"),
    ("islVbatOffsetMicroVolt", 80, 4, "int32", "ISL Vbat offset"),
    ("currentGainMicroPerAmp", 84, 4, "int32", "current gain"),
    ("currentOffsetMicroAmp", 88, 4, "int32", "current offset"),
    ("currentSign", 92, 1, "uint8", "current sign enum"),
    ("openWirePolicy", 93, 1, "uint8", "open-wire policy enum"),
    ("balanceStartMv", 94, 2, "uint16", "balance start threshold"),
    ("balanceDiffMv", 96, 2, "uint16", "balance hysteresis"),
    ("tempSettleTimeMs", 98, 2, "uint16", "temp settle time"),
    ("canTelemetryFlags", 100, 2, "uint16", "CAN telemetry flags"),
    ("featureFlags", 102, 2, "uint16", "config feature flags"),
    ("reserved", 104, 8, "bytes[8]", "must be zero"),
]


@dataclass(frozen=True)
class ConfigVectorCase:
    name: str
    mutate: callable
    expected_result: str


def crc16(data: bytes) -> int:
    crc = 0
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            if crc & 0x8000:
                crc = ((crc << 1) ^ 0x1021) & 0xFFFF
            else:
                crc = (crc << 1) & 0xFFFF
    return crc


def frame_packet(payload: bytes) -> bytes:
    if len(payload) <= 256:
        header = bytes([0x02, len(payload)])
    else:
        header = bytes([0x03, (len(payload) >> 8) & 0xFF, len(payload) & 0xFF])
    crc = crc16(payload)
    return header + payload + bytes([(crc >> 8) & 0xFF, crc & 0xFF, 0x03])


def encode_uint8(value: int) -> bytes:
    return struct.pack(">B", value & 0xFF)


def encode_uint16(value: int) -> bytes:
    return struct.pack(">H", value & 0xFFFF)


def encode_int16(value: int) -> bytes:
    return struct.pack(">h", int(value))


def encode_uint32(value: int) -> bytes:
    return struct.pack(">I", value & 0xFFFFFFFF)


def encode_int32(value: int) -> bytes:
    return struct.pack(">i", int(value))


def encode_float16(value: float, scale: float) -> bytes:
    return encode_int16(round(value * scale))


def encode_float32(value: float, scale: float) -> bytes:
    return encode_int32(round(value * scale))


def pop_uint8(buffer: bytearray) -> int:
    return buffer.pop(0)


def pop_int16(buffer: bytearray) -> int:
    value = struct.unpack(">h", bytes(buffer[:2]))[0]
    del buffer[:2]
    return value


def pop_uint16(buffer: bytearray) -> int:
    value = struct.unpack(">H", bytes(buffer[:2]))[0]
    del buffer[:2]
    return value


def pop_int32(buffer: bytearray) -> int:
    value = struct.unpack(">i", bytes(buffer[:4]))[0]
    del buffer[:4]
    return value


def pop_uint32(buffer: bytearray) -> int:
    value = struct.unpack(">I", bytes(buffer[:4]))[0]
    del buffer[:4]
    return value


def pop_float16(buffer: bytearray, scale: float) -> float:
    return pop_int16(buffer) / scale


def pop_float32(buffer: bytearray, scale: float) -> float:
    return pop_int32(buffer) / scale


def assert_close(actual: float, expected: float, label: str, tol: float = 1e-4) -> None:
    if abs(actual - expected) > tol:
        raise AssertionError(f"{label}: expected {expected}, got {actual}")


def assert_equal(actual, expected, label: str) -> None:
    if actual != expected:
        raise AssertionError(f"{label}: expected {expected!r}, got {actual!r}")


def pack_config_v2(config: dict) -> bytes:
    payload = bytearray()
    payload.extend(struct.pack(
        "<IHHIHBBHH",
        config["magic"],
        config["schemaVersion"],
        config["payloadLength"],
        config["generation"],
        config["hardwareProfile"],
        config["cellCount"],
        config["tempCount"],
        config["flags"],
        0,
    ))
    payload.extend(struct.pack(
        "<HHHHhhhHH",
        config["cellOvSoftMv"],
        config["cellOvHardMv"],
        config["cellUvSoftMv"],
        config["cellUvHardMv"],
        config["chargeTempLimitDeciC"],
        config["dischargeTempLimitDeciC"],
        config["hardTempLimitDeciC"],
        config["minimalPrechargePermille"],
        config["lowCurrentPrechargeTimeoutMs"],
    ))
    payload.extend(bytes(config["requiredCellMask"]))
    payload.extend(bytes(config["requiredTempMask"]))
    payload.extend(bytes(config["balanceAllowedMask"]))
    payload.extend(struct.pack(
        "<iiiiiiBBHHHHH",
        config["vpackGainMicroPerVolt"],
        config["vpackOffsetMicroVolt"],
        config["islVbatGainMicroPerVolt"],
        config["islVbatOffsetMicroVolt"],
        config["currentGainMicroPerAmp"],
        config["currentOffsetMicroAmp"],
        config["currentSign"],
        config["openWirePolicy"],
        config["balanceStartMv"],
        config["balanceDiffMv"],
        config["tempSettleTimeMs"],
        config["canTelemetryFlags"],
        config["featureFlags"],
    ))
    payload.extend(bytes(config["reserved"]))
    if len(payload) != EXPECTED_CONFIG_V2_WIRE_SIZE:
        raise AssertionError(f"packed canonical config size drifted: {len(payload)}")
    body_crc = crc16(payload[EXPECTED_CONFIG_V2_CRC_COVERAGE_START:])
    payload[EXPECTED_CONFIG_V2_CRC_OFFSET:EXPECTED_CONFIG_V2_CRC_OFFSET + 2] = struct.pack("<H", body_crc)
    return bytes(payload)


def unpack_config_v2(payload: bytes) -> dict:
    if len(payload) != EXPECTED_CONFIG_V2_WIRE_SIZE:
        raise AssertionError(f"expected {EXPECTED_CONFIG_V2_WIRE_SIZE} bytes, got {len(payload)}")

    header = struct.unpack("<IHHIHBBHH", payload[:20])
    thresholds = struct.unpack("<HHHHhhhHH", payload[20:38])
    offset = 38
    required_cell_mask = list(payload[offset:offset + EXPECTED_CONFIG_V2_MASK_BYTES])
    offset += EXPECTED_CONFIG_V2_MASK_BYTES
    required_temp_mask = list(payload[offset:offset + EXPECTED_CONFIG_V2_MASK_BYTES])
    offset += EXPECTED_CONFIG_V2_MASK_BYTES
    balance_allowed_mask = list(payload[offset:offset + EXPECTED_CONFIG_V2_MASK_BYTES])
    offset += EXPECTED_CONFIG_V2_MASK_BYTES
    tail = struct.unpack("<iiiiiiBBHHHHH", payload[offset:offset + 36])
    offset += 36
    reserved = list(payload[offset:offset + 8])

    return {
        "magic": header[0],
        "schemaVersion": header[1],
        "payloadLength": header[2],
        "generation": header[3],
        "hardwareProfile": header[4],
        "cellCount": header[5],
        "tempCount": header[6],
        "flags": header[7],
        "bodyCrc": header[8],
        "cellOvSoftMv": thresholds[0],
        "cellOvHardMv": thresholds[1],
        "cellUvSoftMv": thresholds[2],
        "cellUvHardMv": thresholds[3],
        "chargeTempLimitDeciC": thresholds[4],
        "dischargeTempLimitDeciC": thresholds[5],
        "hardTempLimitDeciC": thresholds[6],
        "minimalPrechargePermille": thresholds[7],
        "lowCurrentPrechargeTimeoutMs": thresholds[8],
        "requiredCellMask": required_cell_mask,
        "requiredTempMask": required_temp_mask,
        "balanceAllowedMask": balance_allowed_mask,
        "vpackGainMicroPerVolt": tail[0],
        "vpackOffsetMicroVolt": tail[1],
        "islVbatGainMicroPerVolt": tail[2],
        "islVbatOffsetMicroVolt": tail[3],
        "currentGainMicroPerAmp": tail[4],
        "currentOffsetMicroAmp": tail[5],
        "currentSign": tail[6],
        "openWirePolicy": tail[7],
        "balanceStartMv": tail[8],
        "balanceDiffMv": tail[9],
        "tempSettleTimeMs": tail[10],
        "canTelemetryFlags": tail[11],
        "featureFlags": tail[12],
        "reserved": reserved,
    }


def config_v2_mask_valid(mask: list[int]) -> bool:
    return len(mask) == EXPECTED_CONFIG_V2_MASK_BYTES and (mask[9] & 0xF8) == 0


def validate_config_v2_contract(config: dict) -> str:
    expected_crc = crc16(pack_config_v2({**config, "bodyCrc": 0})[EXPECTED_CONFIG_V2_CRC_COVERAGE_START:])

    if config["magic"] != EXPECTED_CONFIG_V2_MAGIC:
        return "BMS_CONFIG_V2_RESULT_BAD_MAGIC"
    if config["schemaVersion"] != EXPECTED_CONFIG_V2_SCHEMA_VERSION:
        return "BMS_CONFIG_V2_RESULT_UNSUPPORTED_VERSION"
    if config["payloadLength"] != EXPECTED_CONFIG_V2_WIRE_SIZE:
        return "BMS_CONFIG_V2_RESULT_BAD_LENGTH"
    if config["hardwareProfile"] != EXPECTED_HARDWARE_PROFILE:
        return "BMS_CONFIG_V2_RESULT_WRONG_HARDWARE_PROFILE"
    if config["cellCount"] != EXPECTED_CELL_COUNT:
        return "BMS_CONFIG_V2_RESULT_INVALID_CELL_COUNT"
    if config["tempCount"] != EXPECTED_TEMP_COUNT:
        return "BMS_CONFIG_V2_RESULT_INVALID_TEMP_COUNT"
    if config["bodyCrc"] != expected_crc:
        return "BMS_CONFIG_V2_RESULT_BAD_CRC"
    if (
        config["cellOvHardMv"] <= config["cellOvSoftMv"]
        or config["cellUvHardMv"] >= config["cellUvSoftMv"]
        or config["cellUvSoftMv"] >= config["cellOvSoftMv"]
    ):
        return "BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER"
    if (
        config["cellUvHardMv"] < 1500
        or config["cellUvSoftMv"] > 3600
        or config["cellOvSoftMv"] < 3500
        or config["cellOvHardMv"] > 5000
        or config["minimalPrechargePermille"] == 0
        or config["minimalPrechargePermille"] > 1000
        or config["lowCurrentPrechargeTimeoutMs"] < 50
        or config["lowCurrentPrechargeTimeoutMs"] > 10000
        or config["chargeTempLimitDeciC"] < -400
        or config["chargeTempLimitDeciC"] > 900
        or config["dischargeTempLimitDeciC"] < -400
        or config["dischargeTempLimitDeciC"] > 1100
        or config["hardTempLimitDeciC"] < -400
        or config["hardTempLimitDeciC"] > 1200
        or config["hardTempLimitDeciC"] < config["chargeTempLimitDeciC"]
        or config["hardTempLimitDeciC"] < config["dischargeTempLimitDeciC"]
    ):
        return "BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_RANGE"
    if not (
        config_v2_mask_valid(config["requiredCellMask"])
        and config_v2_mask_valid(config["requiredTempMask"])
        and config_v2_mask_valid(config["balanceAllowedMask"])
    ):
        return "BMS_CONFIG_V2_RESULT_INVALID_MASK"
    if (
        config["vpackGainMicroPerVolt"] <= 0
        or config["islVbatGainMicroPerVolt"] <= 0
        or config["currentGainMicroPerAmp"] == 0
        or config["currentSign"] > 1
    ):
        return "BMS_CONFIG_V2_RESULT_INVALID_CALIBRATION"
    if any(value != 0 for value in config["reserved"]):
        return "BMS_CONFIG_V2_RESULT_BAD_LENGTH"
    return "BMS_CONFIG_V2_RESULT_OK"


def parse_define(text: str, name: str) -> int:
    match = re.search(rf"#define\s+{re.escape(name)}\s+(0x[0-9A-Fa-f]+|\d+)u?", text)
    if not match:
        raise AssertionError(f"missing define {name}")
    return int(match.group(1), 0)


def parse_python_constant(text: str, name: str) -> int:
    match = re.search(rf"^{re.escape(name)}\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*$", text, re.MULTILINE)
    if not match:
        raise AssertionError(f"missing python constant {name}")
    return int(match.group(1), 0)


def parse_enum_mapping(text: str, enum_names: list[str]) -> dict[str, int]:
    found: dict[str, int] = {}
    for name in enum_names:
        match = re.search(rf"\b{re.escape(name)}\s*=\s*(\d+)", text)
        if not match:
            raise AssertionError(f"missing enum mapping for {name}")
        found[name] = int(match.group(1))
    return found


def import_fake_module(ui_root: Path) -> ModuleType:
    fake_path = ui_root / "tools" / "fake_bms_firmware.py"
    module_name = "fake_bms_firmware_contract"
    spec = importlib.util.spec_from_file_location(module_name, fake_path)
    if spec is None or spec.loader is None:
        raise AssertionError(f"unable to import fake firmware module from {fake_path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    return module


def build_get_values_payload() -> tuple[bytes, dict]:
    packet_id = COMMAND_IDS["COMM_EBMS_GET_VALUES"]
    expected = {
        "packVoltage": 300.0,
        "packCurrent": -12.3,
        "soC": 55,
        "cVHigh": 4.10,
        "cVAverage": 4.00,
        "cVLow": 3.90,
        "cVMisMatch": 0.20,
        "loadLCVoltage": 299.5,
        "loadLCCurrent": -12.3,
        "chargerVoltage": 0.0,
        "tempBattHigh": 27.4,
        "tempBattAverage": 23.7,
        "tempBattLow": 20.0,
        "tempBMSHigh": 31.2,
        "tempBMSAverage": 29.1,
        "tempBMSLow": 27.0,
        "humidity": 0.0,
        "opState": 4,
        "balanceActive": 3,
        "faultState": 1,
        "AhCnt": 0.0,
        "WhCnt": 0.0,
        "AhCntChg": 0.0,
        "WhCntChg": 0.0,
        "AhCntDis": 0.0,
        "WhCntDis": 0.0,
    }
    payload = bytearray()
    payload += encode_uint8(packet_id)
    payload += encode_float32(expected["packVoltage"], 1e3)
    payload += encode_float32(expected["packCurrent"], 1e3)
    payload += encode_uint8(expected["soC"])
    for key in ("cVHigh", "cVAverage", "cVLow", "cVMisMatch"):
        payload += encode_float32(expected[key], 1e3)
    for key in (
        "loadLCVoltage",
        "loadLCCurrent",
        "chargerVoltage",
        "tempBattHigh",
        "tempBattAverage",
        "tempBattLow",
        "tempBMSHigh",
        "tempBMSAverage",
        "tempBMSLow",
        "humidity",
    ):
        payload += encode_float16(expected[key], 1e1)
    payload += encode_uint8(expected["opState"])
    payload += encode_uint8(expected["balanceActive"])
    payload += encode_uint8(expected["faultState"])
    for key in ("AhCnt", "WhCnt", "AhCntChg", "WhCntChg", "AhCntDis", "WhCntDis"):
        payload += encode_float32(expected[key], 1e3)
    return bytes(payload), expected


def decode_get_values_payload(payload: bytes) -> dict:
    data = bytearray(payload)
    decoded = {"packet_id": pop_uint8(data)}
    decoded["packVoltage"] = pop_float32(data, 1e3)
    decoded["packCurrent"] = pop_float32(data, 1e3)
    decoded["soC"] = pop_uint8(data)
    decoded["cVHigh"] = pop_float32(data, 1e3)
    decoded["cVAverage"] = pop_float32(data, 1e3)
    decoded["cVLow"] = pop_float32(data, 1e3)
    decoded["cVMisMatch"] = pop_float32(data, 1e3)
    decoded["loadLCVoltage"] = pop_float16(data, 1e1)
    decoded["loadLCCurrent"] = pop_float16(data, 1e1)
    decoded["chargerVoltage"] = pop_float16(data, 1e1)
    decoded["tempBattHigh"] = pop_float16(data, 1e1)
    decoded["tempBattAverage"] = pop_float16(data, 1e1)
    decoded["tempBattLow"] = pop_float16(data, 1e1)
    decoded["tempBMSHigh"] = pop_float16(data, 1e1)
    decoded["tempBMSAverage"] = pop_float16(data, 1e1)
    decoded["tempBMSLow"] = pop_float16(data, 1e1)
    decoded["humidity"] = pop_float16(data, 1e1)
    decoded["opState"] = pop_uint8(data)
    decoded["balanceActive"] = pop_uint8(data)
    decoded["faultState"] = pop_uint8(data)
    decoded["AhCnt"] = pop_float32(data, 1e3)
    decoded["WhCnt"] = pop_float32(data, 1e3)
    decoded["AhCntChg"] = pop_float32(data, 1e3)
    decoded["WhCntChg"] = pop_float32(data, 1e3)
    decoded["AhCntDis"] = pop_float32(data, 1e3)
    decoded["WhCntDis"] = pop_float32(data, 1e3)
    decoded["trailing_bytes"] = len(data)
    return decoded


def build_counted_float16_payload(packet_id: int, scale_values: list[float], scale: float) -> bytes:
    payload = bytearray()
    payload += encode_uint8(packet_id)
    payload += encode_uint8(len(scale_values))
    for value in scale_values:
        payload += encode_float16(value, scale)
    return bytes(payload)


def decode_counted_float16_payload(payload: bytes, scale: float) -> tuple[int, list[float], int]:
    data = bytearray(payload)
    packet_id = pop_uint8(data)
    count = pop_uint8(data)
    values = [pop_float16(data, scale) for _ in range(count)]
    return packet_id, values, len(data)


def build_status_ext_payload() -> tuple[bytes, dict]:
    expected = {
        "fwMajor": 1,
        "fwMinor": 0,
        "cellCount": 75,
        "tempCount": 75,
        "activeFaultMask": 0x12345678,
        "latchedFaultMask": 0x9ABCDEF0,
        "measurementFlags": 0x0137,
        "balancingActiveCount": 4,
        "openWireFaultCount": 2,
        "uiFaultCode": 1,
        "operationalState": 5,
    }
    payload = bytearray()
    payload += encode_uint8(COMMAND_IDS["COMM_EBMS_GET_BMS_STATUS_EXT"])
    payload += encode_uint8(expected["fwMajor"])
    payload += encode_uint8(expected["fwMinor"])
    payload += encode_uint8(expected["cellCount"])
    payload += encode_uint8(expected["tempCount"])
    payload += encode_uint32(expected["activeFaultMask"])
    payload += encode_uint32(expected["latchedFaultMask"])
    payload += encode_uint16(expected["measurementFlags"])
    payload += encode_uint8(expected["balancingActiveCount"])
    payload += encode_uint8(expected["openWireFaultCount"])
    payload += encode_uint8(expected["uiFaultCode"])
    payload += encode_uint8(expected["operationalState"])
    return bytes(payload), expected


def decode_status_ext_payload(payload: bytes) -> dict:
    data = bytearray(payload)
    decoded = {
        "packet_id": pop_uint8(data),
        "fwMajor": pop_uint8(data),
        "fwMinor": pop_uint8(data),
        "cellCount": pop_uint8(data),
        "tempCount": pop_uint8(data),
        "activeFaultMask": pop_uint32(data),
        "latchedFaultMask": pop_uint32(data),
        "measurementFlags": pop_uint16(data),
        "balancingActiveCount": pop_uint8(data),
        "openWireFaultCount": pop_uint8(data),
        "uiFaultCode": pop_uint8(data),
        "operationalState": pop_uint8(data),
        "trailing_bytes": len(data),
    }
    return decoded


def load_ui_command_ids(ui_root: Path) -> dict[str, int]:
    text = (ui_root / "datatypes.h").read_text().splitlines()
    found: dict[str, int] = {}
    current_value: int | None = None
    for line in text:
        match = re.search(r"\b(COMM_[A-Z0-9_]+)\b(?:\s*=\s*(\d+))?", line)
        if not match:
            continue
        name = match.group(1)
        explicit_value = match.group(2)
        if explicit_value is not None:
            current_value = int(explicit_value)
        elif current_value is not None:
            current_value += 1
        else:
            continue
        if name in COMMAND_IDS:
            found[name] = current_value
    return found


def load_ui_serialize_order(ui_root: Path) -> list[str]:
    tree = ET.parse(ui_root / "res" / "config.xml")
    ser_order = []
    for ser in tree.findall(".//SerOrder/ser"):
        if ser.text:
            ser_order.append(ser.text.strip())
    return ser_order


def compare_config_orders(ui_order: list[str]) -> list[str]:
    notes: list[str] = []
    if not ui_order:
        notes.append("UI serialize order could not be parsed from res/config.xml.")
        return notes

    first_diff = None
    for index, (ui_name, fw_name) in enumerate(zip(ui_order, FIRMWARE_CONFIG_ORDER)):
        if ui_name != fw_name:
            first_diff = (index, ui_name, fw_name)
            break
    if first_diff:
        idx, ui_name, fw_name = first_diff
        notes.append(
            f"Legacy config remains quarantined: UI field {idx} is '{ui_name}' while legacy firmware parser expects '{fw_name}'."
        )
    if len(ui_order) != len(FIRMWARE_CONFIG_ORDER):
        notes.append(
            f"Config field count differs: UI has {len(ui_order)} serialized fields, "
            f"firmware parser covers {len(FIRMWARE_CONFIG_ORDER)}."
        )

    fw_set = set(FIRMWARE_CONFIG_ORDER)
    extra_ui = [name for name in ui_order if name not in fw_set]
    if extra_ui:
        notes.append("UI-only serialized fields: " + ", ".join(extra_ui[:12]) + (" ..." if len(extra_ui) > 12 else ""))
    return notes


def verify_values() -> dict:
    payload, expected = build_get_values_payload()
    decoded = decode_get_values_payload(payload)
    assert decoded["packet_id"] == COMMAND_IDS["COMM_EBMS_GET_VALUES"]
    for key, expected_value in expected.items():
        actual = decoded[key]
        if isinstance(expected_value, float):
            assert_close(actual, expected_value, key)
        else:
            assert_equal(actual, expected_value, key)
    assert_equal(decoded["trailing_bytes"], 0, "GET_VALUES trailing bytes")
    return {
        "payload_len": len(payload),
        "frame_len": len(frame_packet(payload)),
        "hex_prefix": payload.hex()[:64],
    }


def verify_cells() -> dict:
    voltages = [3.700 + (i * 0.001) for i in range(75)]
    payload = build_counted_float16_payload(COMMAND_IDS["COMM_EBMS_GET_CELLS"], voltages, 1e3)
    packet_id, decoded, trailing = decode_counted_float16_payload(payload, 1e3)
    assert_equal(packet_id, COMMAND_IDS["COMM_EBMS_GET_CELLS"], "GET_CELLS packet id")
    assert_equal(len(decoded), 75, "GET_CELLS count")
    for index, expected in enumerate(voltages):
        assert_close(decoded[index], expected, f"cell[{index}]")
    assert_equal(trailing, 0, "GET_CELLS trailing bytes")
    return {
        "payload_len": len(payload),
        "frame_len": len(frame_packet(payload)),
        "first": decoded[0],
        "last": decoded[-1],
    }


def verify_aux() -> dict:
    payload = bytes([COMMAND_IDS["COMM_EBMS_GET_AUX"], 0])
    packet_id, decoded, trailing = decode_counted_float16_payload(payload, 1e1)
    assert_equal(packet_id, COMMAND_IDS["COMM_EBMS_GET_AUX"], "GET_AUX packet id")
    assert_equal(decoded, [], "GET_AUX count")
    assert_equal(trailing, 0, "GET_AUX trailing bytes")
    return {"payload_len": len(payload), "frame_len": len(frame_packet(payload))}


def verify_exp_temp() -> dict:
    temps = [20.0 + (i * 0.1) for i in range(75)]
    valid_payload = build_counted_float16_payload(COMMAND_IDS["COMM_EBMS_GET_EXP_TEMP"], temps, 1e1)
    packet_id, decoded, trailing = decode_counted_float16_payload(valid_payload, 1e1)
    assert_equal(packet_id, COMMAND_IDS["COMM_EBMS_GET_EXP_TEMP"], "GET_EXP_TEMP packet id")
    assert_equal(len(decoded), 75, "GET_EXP_TEMP count")
    for index, expected in enumerate(temps):
        assert_close(decoded[index], expected, f"temp[{index}]")
    assert_equal(trailing, 0, "GET_EXP_TEMP trailing bytes")

    invalid_payload = bytes([COMMAND_IDS["COMM_EBMS_GET_EXP_TEMP"], 0])
    packet_id_invalid, decoded_invalid, trailing_invalid = decode_counted_float16_payload(invalid_payload, 1e1)
    assert_equal(packet_id_invalid, COMMAND_IDS["COMM_EBMS_GET_EXP_TEMP"], "GET_EXP_TEMP invalid packet id")
    assert_equal(decoded_invalid, [], "GET_EXP_TEMP invalid count")
    assert_equal(trailing_invalid, 0, "GET_EXP_TEMP invalid trailing bytes")

    return {
        "valid_payload_len": len(valid_payload),
        "invalid_payload_len": len(invalid_payload),
        "first": decoded[0],
        "last": decoded[-1],
    }


def verify_status_ext() -> dict:
    payload, expected = build_status_ext_payload()
    decoded = decode_status_ext_payload(payload)
    assert_equal(decoded["packet_id"], COMMAND_IDS["COMM_EBMS_GET_BMS_STATUS_EXT"], "GET_BMS_STATUS_EXT packet id")
    for key, expected_value in expected.items():
        assert_equal(decoded[key], expected_value, key)
    assert_equal(decoded["trailing_bytes"], 0, "GET_BMS_STATUS_EXT trailing bytes")
    return {"payload_len": len(payload), "frame_len": len(frame_packet(payload))}


def verify_capabilities() -> dict:
    payload = bytearray()
    payload += encode_uint8(COMMAND_IDS["COMM_BMS_GET_CAPABILITIES"])
    payload += encode_uint32(0x424D5332)
    payload += encode_uint8(1)
    payload += encode_uint8(1)
    payload += encode_uint8(0)
    payload += encode_uint8(21)
    payload += encode_uint16(0)
    payload += encode_uint16(1)
    payload += encode_uint8(1)
    payload += encode_uint8(1)
    payload += encode_uint8(75)
    payload += encode_uint8(75)
    payload += encode_uint8(5)
    payload += encode_uint8(5)
    payload += encode_uint32(0xBF)
    payload += encode_uint32(0x08000000)
    payload += encode_uint32(0x08001800)
    payload += encode_uint32(0x08000800)
    payload += encode_uint32(0x08001000)
    payload += encode_uint32(0x08019000)
    payload += encode_uint32(0x08032000)
    payload += encode_uint32(0x19000)
    payload += encode_uint32(0x32000)
    payload += encode_uint8(1)
    payload += b"\x00\x00\x00"
    assert_equal(len(payload), 59, "GET_CAPABILITIES payload length")
    return {"payload_len": len(payload), "frame_len": len(frame_packet(bytes(payload)))}


def verify_config_v2(ui_root: Path) -> dict:
    firmware_header = (FW_ROOT / "Main" / "mainDataTypes.h").read_text()
    firmware_commands = (FW_ROOT / "Modules" / "Src" / "modCommands.c").read_text()
    tool_header = (ui_root / "datatypes.h").read_text()
    tool_commands = (ui_root / "commands.cpp").read_text()
    fake_text = (ui_root / "tools" / "fake_bms_firmware.py").read_text()
    fake_module = import_fake_module(ui_root)

    wire_sizes = {
        "firmware": parse_define(firmware_header, "BMS_CONFIG_V2_WIRE_SIZE"),
        "tool": parse_define(tool_header, "BMS_CONFIG_V2_WIRE_SIZE"),
        "fake": parse_python_constant(fake_text, "BMS_CONFIG_V2_WIRE_SIZE"),
    }
    for repo_name, wire_size in wire_sizes.items():
        assert_equal(wire_size, EXPECTED_CONFIG_V2_WIRE_SIZE, f"{repo_name} config v2 wire size")

    firmware_body_offset = parse_define(firmware_commands, "MOD_COMMANDS_CONFIG_V2_BODY_OFFSET")
    assert_equal(firmware_body_offset, EXPECTED_CONFIG_V2_CRC_COVERAGE_START, "firmware Config V2 body offset")
    if "stream.setByteOrder(QDataStream::LittleEndian);" not in tool_commands:
        raise AssertionError("tool commands.cpp no longer forces little-endian Config V2 serialization")
    if "QByteArray body = data.mid(20);" not in tool_commands:
        raise AssertionError("tool commands.cpp CRC coverage no longer starts at byte 20")
    if "data[18]" not in tool_commands or "data[19]" not in tool_commands:
        raise AssertionError("tool commands.cpp no longer writes Config V2 CRC at bytes 18..19")

    canonical_payload = pack_config_v2(CANONICAL_CONFIG_V2)
    assert_equal(canonical_payload.hex(), CANONICAL_CONFIG_V2_HEX, "canonical Config V2 hex vector")
    assert_equal(len(canonical_payload), EXPECTED_CONFIG_V2_WIRE_SIZE, "canonical Config V2 length")
    crc_bytes = canonical_payload[EXPECTED_CONFIG_V2_CRC_OFFSET:EXPECTED_CONFIG_V2_CRC_OFFSET + 2]
    crc_value = struct.unpack("<H", crc_bytes)[0]
    assert_equal(crc_value, EXPECTED_CONFIG_V2_CRC_VALUE, "canonical Config V2 CRC value")
    assert_equal(
        crc16(canonical_payload[EXPECTED_CONFIG_V2_CRC_COVERAGE_START:]),
        EXPECTED_CONFIG_V2_CRC_VALUE,
        "canonical Config V2 body CRC recomputation",
    )

    decoded = unpack_config_v2(canonical_payload)
    for field_name, expected_value in CANONICAL_CONFIG_V2.items():
        assert_equal(decoded[field_name], expected_value, f"decoded canonical field {field_name}")

    for field_name, offset, size, _, _ in EXPECTED_CONFIG_V2_LAYOUT:
        segment = canonical_payload[offset:offset + size]
        if len(segment) != size:
            raise AssertionError(f"field {field_name} expected {size} bytes at offset {offset}, got {len(segment)}")
    for mask_name in ("requiredCellMask", "requiredTempMask", "balanceAllowedMask"):
        mask = decoded[mask_name]
        assert_equal(len(mask), EXPECTED_CONFIG_V2_MASK_BYTES, f"{mask_name} length")
        if mask[9] & 0xF8:
            raise AssertionError(f"{mask_name} sets bits above channel 75")

    mutated_crc_cases = {}
    for label, index in (
        ("byte20", 20),
        ("byte111", 111),
        ("crc_low", 18),
        ("crc_high", 19),
    ):
        mutated = bytearray(canonical_payload)
        mutated[index] ^= 0x01
        mutated_crc_cases[label] = crc16(mutated[EXPECTED_CONFIG_V2_CRC_COVERAGE_START:]) == struct.unpack("<H", mutated[18:20])[0]

    if mutated_crc_cases["byte20"]:
        raise AssertionError("mutating Config V2 byte 20 did not break the internal CRC")
    if mutated_crc_cases["byte111"]:
        raise AssertionError("mutating Config V2 byte 111 did not break the internal CRC")
    if mutated_crc_cases["crc_low"] or mutated_crc_cases["crc_high"]:
        raise AssertionError("mutating Config V2 CRC bytes did not break CRC verification")

    header_mutation = bytearray(canonical_payload)
    header_mutation[0] ^= 0x01
    header_crc_still_matches = crc16(header_mutation[EXPECTED_CONFIG_V2_CRC_COVERAGE_START:]) == struct.unpack("<H", header_mutation[18:20])[0]
    if not header_crc_still_matches:
        raise AssertionError("mutating bytes 0..17 unexpectedly changed the internal CRC contract")

    repaired = bytearray(canonical_payload)
    repaired[20] ^= 0x01
    repaired_crc = crc16(repaired[EXPECTED_CONFIG_V2_CRC_COVERAGE_START:])
    repaired[18:20] = struct.pack("<H", repaired_crc)
    if crc16(repaired[EXPECTED_CONFIG_V2_CRC_COVERAGE_START:]) != struct.unpack("<H", repaired[18:20])[0]:
        raise AssertionError("recomputing Config V2 CRC after a body mutation did not restore validity")

    firmware_results = parse_enum_mapping(firmware_header, list(EXPECTED_RESULT_CODES))
    tool_results = parse_enum_mapping(tool_header, list(EXPECTED_RESULT_CODES))
    fake_results = {name: parse_python_constant(fake_text, name) for name in EXPECTED_RESULT_CODES}
    assert_equal(firmware_results, EXPECTED_RESULT_CODES, "firmware Config V2 result code mapping")
    assert_equal(tool_results, EXPECTED_RESULT_CODES, "tool Config V2 result code mapping")
    assert_equal(fake_results, EXPECTED_RESULT_CODES, "fake Config V2 result code mapping")

    fake_protocol = fake_module.FakeFirmwareProtocol(fake_module.FakeOptions(False, False, False))
    fake_default_payload = fake_protocol._serialize_config_v2(fake_protocol._default_config_v2())
    assert_equal(fake_default_payload.hex(), CANONICAL_CONFIG_V2_HEX, "fake Config V2 canonical payload")

    def fake_validate(config: dict) -> str:
        result_code = fake_protocol._validate_config_v2(config)
        for result_name, result_value in EXPECTED_RESULT_CODES.items():
            if result_value == result_code:
                return result_name
        raise AssertionError(f"unknown fake validation result code {result_code}")

    negative_vectors = [
        ConfigVectorCase(
            "bad magic",
            lambda config: {**config, "magic": 0x43464731},
            "BMS_CONFIG_V2_RESULT_BAD_MAGIC",
        ),
        ConfigVectorCase(
            "bad schema version",
            lambda config: {**config, "schemaVersion": 2},
            "BMS_CONFIG_V2_RESULT_UNSUPPORTED_VERSION",
        ),
        ConfigVectorCase(
            "bad payload length",
            lambda config: {**config, "payloadLength": 111},
            "BMS_CONFIG_V2_RESULT_BAD_LENGTH",
        ),
        ConfigVectorCase(
            "bad CRC",
            lambda config: {**config, "bodyCrc": config["bodyCrc"] ^ 0x0001},
            "BMS_CONFIG_V2_RESULT_BAD_CRC",
        ),
        ConfigVectorCase(
            "wrong hardware profile",
            lambda config: {**config, "hardwareProfile": 2},
            "BMS_CONFIG_V2_RESULT_WRONG_HARDWARE_PROFILE",
        ),
        ConfigVectorCase(
            "invalid cell count",
            lambda config: {**config, "cellCount": 74},
            "BMS_CONFIG_V2_RESULT_INVALID_CELL_COUNT",
        ),
        ConfigVectorCase(
            "invalid temp count",
            lambda config: {**config, "tempCount": 74},
            "BMS_CONFIG_V2_RESULT_INVALID_TEMP_COUNT",
        ),
        ConfigVectorCase(
            "OV hard <= OV soft",
            lambda config: {**config, "cellOvHardMv": config["cellOvSoftMv"]},
            "BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER",
        ),
        ConfigVectorCase(
            "UV hard >= UV soft",
            lambda config: {**config, "cellUvHardMv": config["cellUvSoftMv"]},
            "BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER",
        ),
        ConfigVectorCase(
            "mask bit above channel 75 set",
            lambda config: {
                **config,
                "requiredCellMask": config["requiredCellMask"][:-1] + [config["requiredCellMask"][-1] | 0x08],
            },
            "BMS_CONFIG_V2_RESULT_INVALID_MASK",
        ),
        ConfigVectorCase(
            "balance mask invalid channel set",
            lambda config: {
                **config,
                "balanceAllowedMask": config["balanceAllowedMask"][:-1] + [config["balanceAllowedMask"][-1] | 0x08],
            },
            "BMS_CONFIG_V2_RESULT_INVALID_MASK",
        ),
        ConfigVectorCase(
            "invalid current sign enum",
            lambda config: {**config, "currentSign": 2},
            "BMS_CONFIG_V2_RESULT_INVALID_CALIBRATION",
        ),
        ConfigVectorCase(
            "zero current gain",
            lambda config: {**config, "currentGainMicroPerAmp": 0},
            "BMS_CONFIG_V2_RESULT_INVALID_CALIBRATION",
        ),
    ]

    negative_results = {}
    for case in negative_vectors:
        mutated = case.mutate(dict(CANONICAL_CONFIG_V2))
        if case.name != "bad CRC":
            mutated["bodyCrc"] = struct.unpack("<H", pack_config_v2(mutated)[18:20])[0]
        contract_result = validate_config_v2_contract(mutated)
        fake_result = fake_validate(mutated)
        assert_equal(contract_result, case.expected_result, f"contract result for {case.name}")
        assert_equal(fake_result, case.expected_result, f"fake result for {case.name}")
        negative_results[case.name] = EXPECTED_RESULT_CODES[case.expected_result]

    store_result = fake_protocol.handle_payload(bytes((COMMAND_IDS["COMM_BMS_STORE_CONFIG_V2"],)))[0][1]
    assert_equal(store_result, EXPECTED_RESULT_CODES["BMS_CONFIG_V2_RESULT_UNSUPPORTED_IN_CURRENT_MODE"], "STORE_CONFIG_V2 result code")

    return {
        "wire_size": EXPECTED_CONFIG_V2_WIRE_SIZE,
        "crc_offset": f"{EXPECTED_CONFIG_V2_CRC_OFFSET}..{EXPECTED_CONFIG_V2_CRC_OFFSET + 1}",
        "crc_coverage": f"{EXPECTED_CONFIG_V2_CRC_COVERAGE_START}..{EXPECTED_CONFIG_V2_CRC_COVERAGE_END}",
        "canonical_crc": f"0x{EXPECTED_CONFIG_V2_CRC_VALUE:04x}",
        "header_crc_contract": "bytes 0..17 are outside the internal CRC; packet framing CRC still covers the whole packet payload on the wire",
        "result_codes_locked": True,
        "negative_vectors": negative_results,
        "deferred_validation_gaps": [
            "Non-zero but implausible calibration magnitudes are not range-checked yet.",
            "Balance-allowed mask is only checked for bits above channel 75, not against runtime cell availability/open-wire state.",
        ],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--ui-root",
        type=Path,
        default=DEFAULT_UI_ROOT,
        help="Path to the ENNOID-BMS-Tool-master repo",
    )
    args = parser.parse_args()

    ui_root = args.ui_root.resolve()
    issues: list[str] = []

    if not ui_root.exists():
        print(f"UI repo not found: {ui_root}", file=sys.stderr)
        return 2

    ui_ids = load_ui_command_ids(ui_root)
    for name, expected_id in COMMAND_IDS.items():
        ui_id = ui_ids.get(name)
        if ui_id != expected_id:
            issues.append(f"Command ID mismatch for {name}: UI={ui_id}, expected={expected_id}")

    try:
        results = {
            "GET_VALUES": verify_values(),
            "GET_CELLS": verify_cells(),
            "GET_AUX": verify_aux(),
            "GET_EXP_TEMP": verify_exp_temp(),
            "GET_BMS_STATUS_EXT": verify_status_ext(),
            "GET_CAPABILITIES": verify_capabilities(),
            "CONFIG_V2": verify_config_v2(ui_root),
        }
    except AssertionError as exc:
        issues.append(str(exc))
        results = {}

    ui_order = load_ui_serialize_order(ui_root)
    quarantine_notes = compare_config_orders(ui_order)

    print("UI protocol golden packet verification")
    print(f"UI repo: {ui_root}")
    print("")

    if results:
        print("Verified packet shapes:")
        for name, result in results.items():
            print(f"- {name}: {result}")
        print("")

    if quarantine_notes:
        print("Legacy config quarantine notes:")
        for note in quarantine_notes:
            print(f"- {note}")
        print("")

    if issues:
        print("Mismatches found:")
        for issue in issues:
            print(f"- {issue}")
        return 1

    print("No mismatches found for the covered commands.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
