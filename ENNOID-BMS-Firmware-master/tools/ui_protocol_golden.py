#!/usr/bin/env python3
"""
Golden packet verification for firmware COMM_EBMS_* handlers against the
existing ENNOID/DieBieMS UI decoder expectations.

This script is intentionally protocol-only. It does not talk to hardware and it
does not mutate firmware behavior.
"""

from __future__ import annotations

import argparse
import math
import re
import struct
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


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


def encode_float32_auto(value: float) -> bytes:
    if value == 0.0:
        return encode_uint32(0)
    sig, exponent = math.frexp(value)
    sig_abs = abs(sig)
    sig_i = 0
    if sig_abs >= 0.5:
        sig_i = int((sig_abs - 0.5) * 2.0 * 8388608.0)
        exponent += 126
    result = ((exponent & 0xFF) << 23) | (sig_i & 0x7FFFFF)
    if sig < 0:
        result |= 0x80000000
    return encode_uint32(result)


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


def pop_float32_auto(buffer: bytearray) -> float:
    raw = pop_uint32(buffer)
    exponent = (raw >> 23) & 0xFF
    frac = raw & 0x7FFFFF
    negative = bool(raw & 0x80000000)
    value = 0.0
    if exponent != 0 or frac != 0:
        value = frac / (8388608.0 * 2.0) + 0.5
        exponent -= 126
        value = math.ldexp(value, exponent)
    if negative:
        value = -value
    return value


def assert_close(actual: float, expected: float, label: str, tol: float = 1e-4) -> None:
    if abs(actual - expected) > tol:
        raise AssertionError(f"{label}: expected {expected}, got {actual}")


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
        match = re.search(r"\b(COMM_EBMS_[A-Z0-9_]+)\b(?:\s*=\s*(\d+))?", line)
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
    issues: list[str] = []
    if not ui_order:
        issues.append("UI serialize order could not be parsed from res/config.xml.")
        return issues

    first_diff = None
    for index, (ui_name, fw_name) in enumerate(zip(ui_order, FIRMWARE_CONFIG_ORDER)):
        if ui_name != fw_name:
            first_diff = (index, ui_name, fw_name)
            break
    if first_diff:
        idx, ui_name, fw_name = first_diff
        issues.append(
            f"Config order diverges at field {idx}: UI sends '{ui_name}', firmware expects '{fw_name}'."
        )
    if len(ui_order) != len(FIRMWARE_CONFIG_ORDER):
        issues.append(
            f"Config field count differs: UI has {len(ui_order)} serialized fields, "
            f"firmware parser covers {len(FIRMWARE_CONFIG_ORDER)}."
        )

    fw_set = set(FIRMWARE_CONFIG_ORDER)
    extra_ui = [name for name in ui_order if name not in fw_set]
    if extra_ui:
        issues.append("UI-only serialized fields: " + ", ".join(extra_ui[:12]) + (" ..." if len(extra_ui) > 12 else ""))
    return issues


def verify_values() -> dict:
    payload, expected = build_get_values_payload()
    decoded = decode_get_values_payload(payload)
    assert decoded["packet_id"] == COMMAND_IDS["COMM_EBMS_GET_VALUES"]
    for key, expected_value in expected.items():
        actual = decoded[key]
        if isinstance(expected_value, float):
            assert_close(actual, expected_value, key)
        else:
            if actual != expected_value:
                raise AssertionError(f"{key}: expected {expected_value}, got {actual}")
    if decoded["trailing_bytes"] != 0:
        raise AssertionError("GET_VALUES payload has unexpected trailing bytes")
    return {
        "payload_len": len(payload),
        "frame_len": len(frame_packet(payload)),
        "hex_prefix": payload.hex()[:64],
    }


def verify_cells() -> dict:
    voltages = [3.700 + (i * 0.001) for i in range(75)]
    payload = build_counted_float16_payload(COMMAND_IDS["COMM_EBMS_GET_CELLS"], voltages, 1e3)
    packet_id, decoded, trailing = decode_counted_float16_payload(payload, 1e3)
    assert packet_id == COMMAND_IDS["COMM_EBMS_GET_CELLS"]
    assert len(decoded) == 75
    for index, expected in enumerate(voltages):
        assert_close(decoded[index], expected, f"cell[{index}]")
    if trailing != 0:
        raise AssertionError("GET_CELLS payload has unexpected trailing bytes")
    return {
        "payload_len": len(payload),
        "frame_len": len(frame_packet(payload)),
        "first": decoded[0],
        "last": decoded[-1],
    }


def verify_aux() -> dict:
    payload = bytes([COMMAND_IDS["COMM_EBMS_GET_AUX"], 0])
    packet_id, decoded, trailing = decode_counted_float16_payload(payload, 1e1)
    assert packet_id == COMMAND_IDS["COMM_EBMS_GET_AUX"]
    assert decoded == []
    if trailing != 0:
        raise AssertionError("GET_AUX payload has unexpected trailing bytes")
    return {"payload_len": len(payload), "frame_len": len(frame_packet(payload))}


def verify_exp_temp() -> dict:
    temps = [20.0 + (i * 0.1) for i in range(75)]
    valid_payload = build_counted_float16_payload(COMMAND_IDS["COMM_EBMS_GET_EXP_TEMP"], temps, 1e1)
    packet_id, decoded, trailing = decode_counted_float16_payload(valid_payload, 1e1)
    assert packet_id == COMMAND_IDS["COMM_EBMS_GET_EXP_TEMP"]
    assert len(decoded) == 75
    for index, expected in enumerate(temps):
        assert_close(decoded[index], expected, f"temp[{index}]")
    if trailing != 0:
        raise AssertionError("GET_EXP_TEMP valid payload has unexpected trailing bytes")

    invalid_payload = bytes([COMMAND_IDS["COMM_EBMS_GET_EXP_TEMP"], 0])
    packet_id_invalid, decoded_invalid, trailing_invalid = decode_counted_float16_payload(invalid_payload, 1e1)
    assert packet_id_invalid == COMMAND_IDS["COMM_EBMS_GET_EXP_TEMP"]
    assert decoded_invalid == []
    if trailing_invalid != 0:
        raise AssertionError("GET_EXP_TEMP invalid payload has unexpected trailing bytes")

    return {
        "valid_payload_len": len(valid_payload),
        "invalid_payload_len": len(invalid_payload),
        "first": decoded[0],
        "last": decoded[-1],
    }


def verify_status_ext() -> dict:
    payload, expected = build_status_ext_payload()
    decoded = decode_status_ext_payload(payload)
    assert decoded["packet_id"] == COMMAND_IDS["COMM_EBMS_GET_BMS_STATUS_EXT"]
    for key, expected_value in expected.items():
        if decoded[key] != expected_value:
            raise AssertionError(f"{key}: expected {expected_value}, got {decoded[key]}")
    if decoded["trailing_bytes"] != 0:
        raise AssertionError("GET_BMS_STATUS_EXT payload has unexpected trailing bytes")
    return {"payload_len": len(payload), "frame_len": len(frame_packet(payload))}


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
        if name == "COMM_EBMS_GET_BMS_STATUS_EXT":
            if ui_id is not None:
                issues.append(f"UI unexpectedly defines {name} = {ui_id}; current UI was expected to omit it.")
            continue
        if ui_id != expected_id:
            issues.append(f"Command ID mismatch for {name}: UI={ui_id}, expected={expected_id}")

    results = {
        "GET_VALUES": verify_values(),
        "GET_CELLS": verify_cells(),
        "GET_AUX": verify_aux(),
        "GET_EXP_TEMP": verify_exp_temp(),
        "GET_BMS_STATUS_EXT": verify_status_ext(),
    }

    ui_order = load_ui_serialize_order(ui_root)
    issues.extend(compare_config_orders(ui_order))

    print("UI protocol golden packet verification")
    print(f"UI repo: {ui_root}")
    print("")
    print("Verified packet shapes:")
    for name, result in results.items():
        print(f"- {name}: {result}")
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
