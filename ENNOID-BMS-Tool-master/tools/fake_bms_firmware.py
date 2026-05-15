#!/usr/bin/env python3
"""
Fake ENNOID-BMS firmware endpoint for exercising the Qt UI without hardware.

Default mode starts a TCP server that speaks the same packet framing as the UI:
    start byte 0x02 + 1-byte payload length + payload + CRC16 + end byte 0x03

An optional PTY mode is also available on Unix-like systems for serial testing.
"""

from __future__ import annotations

import argparse
import os
import pty
import signal
import socket
import socketserver
import struct
import sys
import threading
import time
from dataclasses import dataclass
from typing import Callable, List, Optional


COMM_FW_VERSION = 0
COMM_PRINT = 21
COMM_FORWARD_CAN = 34
COMM_PING_CAN = 75

COMM_EBMS_STORE_CONF = 150
COMM_EBMS_GET_CELLS = 151
COMM_EBMS_GET_AUX = 152
COMM_EBMS_GET_EXP_TEMP = 153
COMM_EBMS_SET_MCCONF = 154
COMM_EBMS_GET_MCCONF = 155
COMM_EBMS_GET_MCCONF_DEFAULT = 156
COMM_EBMS_GET_VALUES = 157
COMM_EBMS_GET_BMS_STATUS_EXT = 158
COMM_BMS_GET_CAPABILITIES = 159
COMM_BMS_GET_CONFIG_V2 = 160
COMM_BMS_SET_CONFIG_V2 = 161
COMM_BMS_STORE_CONFIG_V2 = 162
COMM_BMS_GET_CONFIG_DEFAULT_V2 = 163
COMM_BMS_VALIDATE_CONFIG_V2 = 164
COMM_BMS_GET_CONFIG_SCHEMA_V2 = 165

OP_STATE_LOAD_ENABLED = 3

PACKET_START_SHORT = 2
PACKET_START_LONG = 3
PACKET_END = 3

TOTAL_CELLS = 75
TOTAL_TEMPS = 75

BMS_CAPABILITIES_MAGIC = 0x424D5332
BMS_CAPABILITIES_VERSION = 1
BMS_CONFIG_V2_MAGIC = 0x43464732
BMS_CONFIG_V2_SCHEMA_VERSION = 1
BMS_CONFIG_V2_MASK_BYTES = 10
BMS_CONFIG_V2_WIRE_SIZE = 112
BMS_CONFIG_V2_RESULT_OK = 0
BMS_CONFIG_V2_RESULT_UNSUPPORTED_VERSION = 1
BMS_CONFIG_V2_RESULT_BAD_MAGIC = 2
BMS_CONFIG_V2_RESULT_BAD_LENGTH = 3
BMS_CONFIG_V2_RESULT_BAD_CRC = 4
BMS_CONFIG_V2_RESULT_WRONG_HARDWARE_PROFILE = 5
BMS_CONFIG_V2_RESULT_INVALID_CELL_COUNT = 6
BMS_CONFIG_V2_RESULT_INVALID_TEMP_COUNT = 7
BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER = 8
BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_RANGE = 9
BMS_CONFIG_V2_RESULT_INVALID_MASK = 10
BMS_CONFIG_V2_RESULT_INVALID_CALIBRATION = 11
BMS_CONFIG_V2_RESULT_STORE_FAILED = 12
BMS_CONFIG_V2_RESULT_READBACK_FAILED = 13
BMS_CONFIG_V2_RESULT_UNSUPPORTED_IN_CURRENT_MODE = 14
BMS_FEATURE_MIGRATED_LTC6812_MODEL = 1 << 0
BMS_FEATURE_DUAL_ISOSPI = 1 << 1
BMS_FEATURE_EXP_TEMP = 1 << 2
BMS_FEATURE_AUX_COUNT_ZERO = 1 << 3
BMS_FEATURE_CONFIG_V2 = 1 << 4
BMS_FEATURE_CONFIG_WRITE = 1 << 5
BMS_FEATURE_BOOTLOADER_UPDATE = 1 << 7


CRC16_TABLE = (
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0,
)


def crc16(data: bytes) -> int:
    checksum = 0
    for byte in data:
        checksum = CRC16_TABLE[((checksum >> 8) ^ byte) & 0xFF] ^ ((checksum << 8) & 0xFFFF)
    return checksum & 0xFFFF


def encode_packet(payload: bytes) -> bytes:
    if len(payload) <= 0xFF:
        header = bytes((PACKET_START_SHORT, len(payload)))
    else:
        header = bytes((PACKET_START_LONG, (len(payload) >> 8) & 0xFF, len(payload) & 0xFF))
    crc = crc16(payload)
    return header + payload + struct.pack(">H", crc) + bytes((PACKET_END,))


class PacketDecoder:
    def __init__(self) -> None:
        self._buffer = bytearray()

    def feed(self, data: bytes) -> List[bytes]:
        self._buffer.extend(data)
        packets: List[bytes] = []

        while self._buffer:
            start = self._buffer[0]

            if start == PACKET_START_SHORT:
                if len(self._buffer) < 2:
                    break
                payload_len = self._buffer[1]
                packet_len = 2 + payload_len + 2 + 1
                if len(self._buffer) < packet_len:
                    break
                packet = bytes(self._buffer[:packet_len])
                del self._buffer[:packet_len]
            elif start == PACKET_START_LONG:
                if len(self._buffer) < 3:
                    break
                payload_len = (self._buffer[1] << 8) | self._buffer[2]
                packet_len = 3 + payload_len + 2 + 1
                if len(self._buffer) < packet_len:
                    break
                packet = bytes(self._buffer[:packet_len])
                del self._buffer[:packet_len]
            else:
                del self._buffer[0]
                continue

            if packet[-1] != PACKET_END:
                continue

            payload_start = 2 if packet[0] == PACKET_START_SHORT else 3
            payload = packet[payload_start:-3]
            received_crc = struct.unpack(">H", packet[-3:-1])[0]
            if crc16(payload) != received_crc:
                continue

            packets.append(payload)

        return packets


def pack_u8(value: int) -> bytes:
    return struct.pack(">B", value & 0xFF)


def pack_u16(value: int) -> bytes:
    return struct.pack(">H", value & 0xFFFF)


def pack_u32(value: int) -> bytes:
    return struct.pack(">I", value & 0xFFFFFFFF)


def pack_i16(value: int) -> bytes:
    return struct.pack(">h", int(value))


def pack_i32(value: int) -> bytes:
    return struct.pack(">i", int(value))


def round_scaled(value: float, scale: float) -> int:
    scaled = value * scale
    return int(scaled - 0.5) if scaled < 0 else int(scaled + 0.5)


def pack_f16(value: float, scale: float) -> bytes:
    return pack_i16(round_scaled(value, scale))


def pack_f32(value: float, scale: float) -> bytes:
    return pack_i32(round_scaled(value, scale))


@dataclass
class FakeOptions:
    fault: bool
    invalid_temps: bool
    log_packets: bool


class FakeFirmwareProtocol:
    def __init__(self, options: FakeOptions) -> None:
        self.options = options
        self.cell_values = [3.700 + 0.001 * index for index in range(TOTAL_CELLS)]
        self.exp_temps = [20.0 + 0.1 * index for index in range(TOTAL_TEMPS)]
        self.hw_name = b"FAKE-BMS"
        self.uuid = bytes.fromhex("00112233445566778899AABB")
        self.config_v2 = self._default_config_v2()

    def _measurement_flags(self) -> int:
        flags = 0
        valid_bit_positions = [0, 2, 3, 4, 5, 6, 7, 8, 9]
        for bit in valid_bit_positions:
            flags |= 1 << bit
        if not self.options.invalid_temps:
            flags |= 1 << 1
        return flags

    def _fault_code(self) -> int:
        return 1 if self.options.fault else 0

    def _fault_mask(self) -> int:
        return 0x00000001 if self.options.fault else 0

    def _print_packet(self, text: str) -> bytes:
        payload = bytearray()
        payload.extend(pack_u8(COMM_PRINT))
        payload.extend(text.encode("latin1", errors="replace"))
        return bytes(payload)

    def _fw_version_packet(self) -> bytes:
        # COMM_FW_VERSION payload layout as parsed by Commands::processPacket:
        #   uint8  packet_id (0)
        #   int8   fw_major
        #   int8   fw_minor
        #   char[] hw_name, NUL-terminated
        #   uint8[12] uuid (optional, but expected by the real firmware path)
        #
        # The desktop UI currently loads its supported firmware list from
        # res/info.xml, where the accepted version is 6.00. Returning 1.1 keeps
        # the handshake structurally valid, but it lands in the "too old
        # firmware / limited mode" path. Return 6.0 here so the UI enters normal
        # monitoring mode and starts polling 157/151/152/153.
        payload = bytearray()
        payload.extend(pack_u8(COMM_FW_VERSION))
        payload.extend(pack_u8(6))
        payload.extend(pack_u8(0))
        payload.extend(self.hw_name)
        payload.extend(b"\x00")
        payload.extend(self.uuid)
        return bytes(payload)

    def _values_packet(self) -> bytes:
        payload = bytearray()
        payload.extend(pack_u8(COMM_EBMS_GET_VALUES))
        payload.extend(pack_f32(300.0, 1e3))
        payload.extend(pack_f32(-12.3, 1e3))
        payload.extend(pack_u8(56))
        payload.extend(pack_f32(4.10, 1e3))
        payload.extend(pack_f32(4.00, 1e3))
        payload.extend(pack_f32(3.90, 1e3))
        payload.extend(pack_f32(0.20, 1e3))
        payload.extend(pack_f16(295.0, 1e1))
        payload.extend(pack_f16(-12.3, 1e1))
        payload.extend(pack_f16(0.0, 1e1))
        payload.extend(pack_f16(28.0, 1e1))
        payload.extend(pack_f16(24.0, 1e1))
        payload.extend(pack_f16(20.0, 1e1))
        payload.extend(pack_f16(35.0, 1e1))
        payload.extend(pack_f16(30.0, 1e1))
        payload.extend(pack_f16(25.0, 1e1))
        payload.extend(pack_f16(50.0, 1e1))
        payload.extend(pack_u8(OP_STATE_LOAD_ENABLED))
        payload.extend(pack_u8(0))
        payload.extend(pack_u8(self._fault_code()))
        for _ in range(6):
            payload.extend(pack_f32(0.0, 1e3))
        return bytes(payload)

    def _cells_packet(self) -> bytes:
        payload = bytearray()
        payload.extend(pack_u8(COMM_EBMS_GET_CELLS))
        payload.extend(pack_u8(len(self.cell_values)))
        for cell in self.cell_values:
            payload.extend(pack_f16(cell, 1e3))
        return bytes(payload)

    def _aux_packet(self) -> bytes:
        return bytes((COMM_EBMS_GET_AUX, 0))

    def _exp_temp_packet(self) -> bytes:
        payload = bytearray()
        payload.extend(pack_u8(COMM_EBMS_GET_EXP_TEMP))
        if self.options.invalid_temps:
            payload.extend(pack_u8(0))
            return bytes(payload)

        payload.extend(pack_u8(len(self.exp_temps)))
        for temperature in self.exp_temps:
            payload.extend(pack_f16(temperature, 1e1))
        return bytes(payload)

    def _status_ext_packet(self) -> bytes:
        payload = bytearray()
        payload.extend(pack_u8(COMM_EBMS_GET_BMS_STATUS_EXT))
        payload.extend(pack_u8(1))
        payload.extend(pack_u8(1))
        payload.extend(pack_u8(TOTAL_CELLS))
        payload.extend(pack_u8(0 if self.options.invalid_temps else TOTAL_TEMPS))
        payload.extend(pack_u32(self._fault_mask()))
        payload.extend(pack_u32(self._fault_mask()))
        payload.extend(pack_u16(self._measurement_flags()))
        payload.extend(pack_u8(0))
        payload.extend(pack_u8(0))
        payload.extend(pack_u8(self._fault_code()))
        payload.extend(pack_u8(OP_STATE_LOAD_ENABLED))
        return bytes(payload)

    def _capabilities_packet(self) -> bytes:
        payload = bytearray()
        payload.extend(pack_u8(COMM_BMS_GET_CAPABILITIES))
        payload.extend(pack_u32(BMS_CAPABILITIES_MAGIC))
        payload.extend(pack_u8(BMS_CAPABILITIES_VERSION))
        payload.extend(pack_u8(1))
        payload.extend(pack_u8(6))
        payload.extend(pack_u8(0))
        payload.extend(pack_u16(0))
        payload.extend(pack_u16(1))
        payload.extend(pack_u8(1))
        payload.extend(pack_u8(1))
        payload.extend(pack_u8(TOTAL_CELLS))
        payload.extend(pack_u8(TOTAL_TEMPS))
        payload.extend(pack_u8(5))
        payload.extend(pack_u8(5))
        payload.extend(pack_u32(
            BMS_FEATURE_MIGRATED_LTC6812_MODEL |
            BMS_FEATURE_DUAL_ISOSPI |
            BMS_FEATURE_EXP_TEMP |
            BMS_FEATURE_AUX_COUNT_ZERO |
            BMS_FEATURE_CONFIG_V2 |
            BMS_FEATURE_CONFIG_WRITE |
            BMS_FEATURE_BOOTLOADER_UPDATE
        ))
        payload.extend(pack_u32(0x08000000))
        payload.extend(pack_u32(0x08001800))
        payload.extend(pack_u32(0x08000800))
        payload.extend(pack_u32(0x08001000))
        payload.extend(pack_u32(0x08019000))
        payload.extend(pack_u32(0x08032000))
        payload.extend(pack_u32(0x19000))
        payload.extend(pack_u32(0x32000))
        payload.extend(pack_u8(1))
        payload.extend(b"\x00\x00\x00")
        return bytes(payload)

    def _default_config_v2(self) -> dict:
        return {
            "magic": BMS_CONFIG_V2_MAGIC,
            "schemaVersion": BMS_CONFIG_V2_SCHEMA_VERSION,
            "payloadLength": BMS_CONFIG_V2_WIRE_SIZE,
            "generation": 0x11223344,
            "hardwareProfile": 1,
            "cellCount": TOTAL_CELLS,
            "tempCount": TOTAL_TEMPS,
            "flags": 0x55AA,
            "bodyCrc": 0,
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

    def _serialize_config_v2(self, config: dict) -> bytes:
        payload = bytearray()
        payload.extend(struct.pack("<IHHIHBBHH", config["magic"], config["schemaVersion"],
                                   BMS_CONFIG_V2_WIRE_SIZE, config["generation"],
                                   config["hardwareProfile"], config["cellCount"],
                                   config["tempCount"], config["flags"], 0))
        payload.extend(struct.pack("<HHHHhhhHH",
                                   config["cellOvSoftMv"], config["cellOvHardMv"],
                                   config["cellUvSoftMv"], config["cellUvHardMv"],
                                   config["chargeTempLimitDeciC"], config["dischargeTempLimitDeciC"],
                                   config["hardTempLimitDeciC"], config["minimalPrechargePermille"],
                                   config["lowCurrentPrechargeTimeoutMs"]))
        payload.extend(bytes(config["requiredCellMask"]))
        payload.extend(bytes(config["requiredTempMask"]))
        payload.extend(bytes(config["balanceAllowedMask"]))
        payload.extend(struct.pack("<iiiiiiBBHHHHH",
                                   config["vpackGainMicroPerVolt"], config["vpackOffsetMicroVolt"],
                                   config["islVbatGainMicroPerVolt"], config["islVbatOffsetMicroVolt"],
                                   config["currentGainMicroPerAmp"], config["currentOffsetMicroAmp"],
                                   config["currentSign"], config["openWirePolicy"],
                                   config["balanceStartMv"], config["balanceDiffMv"],
                                   config["tempSettleTimeMs"], config["canTelemetryFlags"],
                                   config["featureFlags"]))
        payload.extend(bytes(config["reserved"]))
        body_crc = crc16(bytes(payload[20:]))
        payload[18:20] = struct.pack("<H", body_crc)
        return bytes(payload)

    def _deserialize_config_v2(self, payload: bytes) -> Optional[dict]:
        if len(payload) != BMS_CONFIG_V2_WIRE_SIZE:
            return None

        header_fields = struct.unpack("<IHHIHBBHH", payload[:20])
        threshold_fields = struct.unpack("<HHHHhhhHH", payload[20:38])
        config = {
            "magic": header_fields[0],
            "schemaVersion": header_fields[1],
            "payloadLength": header_fields[2],
            "generation": header_fields[3],
            "hardwareProfile": header_fields[4],
            "cellCount": header_fields[5],
            "tempCount": header_fields[6],
            "flags": header_fields[7],
            "bodyCrc": header_fields[8],
            "cellOvSoftMv": threshold_fields[0],
            "cellOvHardMv": threshold_fields[1],
            "cellUvSoftMv": threshold_fields[2],
            "cellUvHardMv": threshold_fields[3],
            "chargeTempLimitDeciC": threshold_fields[4],
            "dischargeTempLimitDeciC": threshold_fields[5],
            "hardTempLimitDeciC": threshold_fields[6],
            "minimalPrechargePermille": threshold_fields[7],
            "lowCurrentPrechargeTimeoutMs": threshold_fields[8],
        }
        offset = 38
        config["requiredCellMask"] = list(payload[offset:offset + BMS_CONFIG_V2_MASK_BYTES])
        offset += BMS_CONFIG_V2_MASK_BYTES
        config["requiredTempMask"] = list(payload[offset:offset + BMS_CONFIG_V2_MASK_BYTES])
        offset += BMS_CONFIG_V2_MASK_BYTES
        config["balanceAllowedMask"] = list(payload[offset:offset + BMS_CONFIG_V2_MASK_BYTES])
        offset += BMS_CONFIG_V2_MASK_BYTES
        tail = struct.unpack("<iiiiiiBBHHHHH", payload[offset:offset + 36])
        config["vpackGainMicroPerVolt"] = tail[0]
        config["vpackOffsetMicroVolt"] = tail[1]
        config["islVbatGainMicroPerVolt"] = tail[2]
        config["islVbatOffsetMicroVolt"] = tail[3]
        config["currentGainMicroPerAmp"] = tail[4]
        config["currentOffsetMicroAmp"] = tail[5]
        config["currentSign"] = tail[6]
        config["openWirePolicy"] = tail[7]
        config["balanceStartMv"] = tail[8]
        config["balanceDiffMv"] = tail[9]
        config["tempSettleTimeMs"] = tail[10]
        config["canTelemetryFlags"] = tail[11]
        config["featureFlags"] = tail[12]
        offset += 36
        config["reserved"] = list(payload[offset:offset + 8])
        return config

    def _validate_config_v2(self, config: dict) -> int:
        if config["magic"] != BMS_CONFIG_V2_MAGIC:
            return BMS_CONFIG_V2_RESULT_BAD_MAGIC
        if config["schemaVersion"] != BMS_CONFIG_V2_SCHEMA_VERSION:
            return BMS_CONFIG_V2_RESULT_UNSUPPORTED_VERSION
        if config["payloadLength"] != BMS_CONFIG_V2_WIRE_SIZE:
            return BMS_CONFIG_V2_RESULT_BAD_LENGTH
        if config["hardwareProfile"] != 1:
            return BMS_CONFIG_V2_RESULT_WRONG_HARDWARE_PROFILE
        if config["cellCount"] != TOTAL_CELLS:
            return BMS_CONFIG_V2_RESULT_INVALID_CELL_COUNT
        if config["tempCount"] != TOTAL_TEMPS:
            return BMS_CONFIG_V2_RESULT_INVALID_TEMP_COUNT
        serialized = self._serialize_config_v2(config)
        expected_crc = struct.unpack("<H", serialized[18:20])[0]
        if expected_crc != config["bodyCrc"]:
            return BMS_CONFIG_V2_RESULT_BAD_CRC
        if not (config["cellOvHardMv"] > config["cellOvSoftMv"] and
                config["cellUvHardMv"] < config["cellUvSoftMv"] and
                config["cellUvSoftMv"] < config["cellOvSoftMv"]):
            return BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER
        if config["currentGainMicroPerAmp"] == 0 or config["currentSign"] > 1:
            return BMS_CONFIG_V2_RESULT_INVALID_CALIBRATION
        if any((mask[9] & 0xF8) != 0 for mask in (
                config["requiredCellMask"], config["requiredTempMask"], config["balanceAllowedMask"])):
            return BMS_CONFIG_V2_RESULT_INVALID_MASK
        return BMS_CONFIG_V2_RESULT_OK

    def _config_v2_packet(self, packet_id: int, config: dict) -> bytes:
        return bytes((packet_id,)) + self._serialize_config_v2(config)

    def _config_v2_result_packet(self, packet_id: int, result: int) -> bytes:
        return bytes((packet_id, result))

    def handle_payload(self, payload: bytes) -> List[bytes]:
        if not payload:
            return []

        packet_id = payload[0]
        payload_body = payload[1:]

        if packet_id == COMM_FORWARD_CAN and payload_body:
            packet_id = payload_body[1] if len(payload_body) >= 2 else None
            payload_body = payload_body[2:] if len(payload_body) >= 2 else b""

        if packet_id is None:
            return []

        if self.options.log_packets:
            print(f"rx packet id={packet_id}", flush=True)

        if packet_id == COMM_FW_VERSION:
            return [self._fw_version_packet()]
        if packet_id == COMM_EBMS_GET_VALUES:
            return [self._values_packet()]
        if packet_id == COMM_EBMS_GET_CELLS:
            return [self._cells_packet()]
        if packet_id == COMM_EBMS_GET_AUX:
            return [self._aux_packet()]
        if packet_id == COMM_EBMS_GET_EXP_TEMP:
            return [self._exp_temp_packet()]
        if packet_id == COMM_EBMS_GET_BMS_STATUS_EXT:
            return [self._status_ext_packet()]
        if packet_id == COMM_BMS_GET_CAPABILITIES:
            return [self._capabilities_packet()]
        if packet_id == COMM_PING_CAN:
            return [bytes((COMM_PING_CAN,))]
        if packet_id in (
            COMM_EBMS_STORE_CONF,
            COMM_EBMS_SET_MCCONF,
            COMM_EBMS_GET_MCCONF,
            COMM_EBMS_GET_MCCONF_DEFAULT,
        ):
            return [self._print_packet(
                "Fake firmware: legacy config is blocked. Use capabilities and Config V2 only."
            )]
        if packet_id in (
            COMM_BMS_GET_CONFIG_V2,
        ):
            return [self._config_v2_packet(packet_id, self.config_v2)]
        if packet_id == COMM_BMS_GET_CONFIG_DEFAULT_V2:
            return [self._config_v2_packet(packet_id, self._default_config_v2())]
        if packet_id == COMM_BMS_GET_CONFIG_SCHEMA_V2:
            return [bytes((packet_id,)) + struct.pack("<IHHHBB",
                                                     BMS_CONFIG_V2_MAGIC,
                                                     BMS_CONFIG_V2_SCHEMA_VERSION,
                                                     BMS_CONFIG_V2_WIRE_SIZE,
                                                     1, TOTAL_CELLS, TOTAL_TEMPS)]
        if packet_id in (COMM_BMS_VALIDATE_CONFIG_V2, COMM_BMS_SET_CONFIG_V2):
            incoming = self._deserialize_config_v2(payload_body)
            if incoming is None:
                return [self._config_v2_result_packet(packet_id, BMS_CONFIG_V2_RESULT_BAD_LENGTH)]
            result = self._validate_config_v2(incoming)
            if result == BMS_CONFIG_V2_RESULT_OK and packet_id == COMM_BMS_SET_CONFIG_V2:
                self.config_v2 = incoming
            return [self._config_v2_result_packet(packet_id, result)]
        if packet_id == COMM_BMS_STORE_CONFIG_V2:
            return [self._config_v2_result_packet(packet_id, BMS_CONFIG_V2_RESULT_UNSUPPORTED_IN_CURRENT_MODE)]

        return [self._print_packet(f"Fake firmware: unsupported command {packet_id}.")]


class FakeTcpHandler(socketserver.BaseRequestHandler):
    def handle(self) -> None:
        decoder = PacketDecoder()
        protocol: FakeFirmwareProtocol = self.server.protocol  # type: ignore[attr-defined]

        while True:
            data = self.request.recv(4096)
            if not data:
                return

            for payload in decoder.feed(data):
                for reply in protocol.handle_payload(payload):
                    self.request.sendall(encode_packet(reply))


class ThreadedTcpServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    allow_reuse_address = True
    daemon_threads = True

    def __init__(self, server_address, handler_class, protocol: FakeFirmwareProtocol):
        super().__init__(server_address, handler_class)
        self.protocol = protocol


class PtyServer:
    def __init__(self, protocol: FakeFirmwareProtocol):
        self.protocol = protocol
        self.master_fd: Optional[int] = None
        self.slave_name: Optional[str] = None
        self._thread: Optional[threading.Thread] = None
        self._stop = threading.Event()

    def start(self) -> str:
        master_fd, slave_fd = pty.openpty()
        self.master_fd = master_fd
        self.slave_name = os.ttyname(slave_fd)
        os.close(slave_fd)
        self._thread = threading.Thread(target=self._run, name="fake-bms-pty", daemon=True)
        self._thread.start()
        return self.slave_name

    def stop(self) -> None:
        self._stop.set()
        if self.master_fd is not None:
            try:
                os.close(self.master_fd)
            except OSError:
                pass
            self.master_fd = None

    def _run(self) -> None:
        assert self.master_fd is not None
        decoder = PacketDecoder()
        while not self._stop.is_set():
            try:
                data = os.read(self.master_fd, 4096)
            except OSError:
                break
            if not data:
                time.sleep(0.05)
                continue
            for payload in decoder.feed(data):
                for reply in self.protocol.handle_payload(payload):
                    try:
                        os.write(self.master_fd, encode_packet(reply))
                    except OSError:
                        return


def run_self_test(options: FakeOptions) -> int:
    protocol = FakeFirmwareProtocol(options)
    failures: List[str] = []

    for request_id in (
        COMM_FW_VERSION,
        COMM_EBMS_GET_VALUES,
        COMM_EBMS_GET_CELLS,
        COMM_EBMS_GET_AUX,
        COMM_EBMS_GET_EXP_TEMP,
        COMM_EBMS_GET_BMS_STATUS_EXT,
        COMM_BMS_GET_CAPABILITIES,
    ):
        request_decoder = PacketDecoder()
        request_frames = request_decoder.feed(encode_packet(bytes((request_id,))))

        if len(request_frames) != 1 or request_frames[0][0] != request_id:
            failures.append(f"request framing failed for packet id {request_id}")
            continue

        replies = protocol.handle_payload(request_frames[0])
        if not replies:
            failures.append(f"no reply for packet id {request_id}")
            continue

        reply_decoder = PacketDecoder()
        reply_frames: List[bytes] = []
        for reply in replies:
            reply_frames.extend(reply_decoder.feed(encode_packet(reply)))

        if not reply_frames:
            failures.append(f"reply framing failed for packet id {request_id}")
            continue

        reply_id = reply_frames[0][0]
        if reply_id != request_id:
            failures.append(f"reply id mismatch for {request_id}: got {reply_id}")

    aux_replies = protocol.handle_payload(bytes((COMM_EBMS_GET_AUX,)))
    if not aux_replies or aux_replies[0][:2] != bytes((COMM_EBMS_GET_AUX, 0)):
        failures.append("AUX reply did not return count=0")

    capabilities_replies = protocol.handle_payload(bytes((COMM_BMS_GET_CAPABILITIES,)))
    if not capabilities_replies or capabilities_replies[0][0] != COMM_BMS_GET_CAPABILITIES:
        failures.append("capabilities reply missing")

    config_replies = protocol.handle_payload(bytes((COMM_BMS_GET_CONFIG_V2,)))
    if not config_replies or config_replies[0][0] != COMM_BMS_GET_CONFIG_V2:
        failures.append("config v2 reply missing")

    valid_config = protocol._serialize_config_v2(protocol.config_v2)
    validate_replies = protocol.handle_payload(bytes((COMM_BMS_VALIDATE_CONFIG_V2,)) + valid_config)
    if not validate_replies or validate_replies[0] != bytes((COMM_BMS_VALIDATE_CONFIG_V2, BMS_CONFIG_V2_RESULT_OK)):
        failures.append("config v2 validate success path failed")

    bad_config_dict = protocol._deserialize_config_v2(valid_config)
    assert bad_config_dict is not None
    bad_config_dict["cellOvHardMv"] = 4100
    bad_config = protocol._serialize_config_v2(bad_config_dict)
    bad_validate = protocol.handle_payload(bytes((COMM_BMS_VALIDATE_CONFIG_V2,)) + bad_config)
    if not bad_validate or bad_validate[0] != bytes((COMM_BMS_VALIDATE_CONFIG_V2, BMS_CONFIG_V2_RESULT_INVALID_THRESHOLD_ORDER)):
        failures.append("config v2 validate failure path failed")

    if failures:
        for failure in failures:
            print(f"self-test failed: {failure}", file=sys.stderr)
        return 1

    print("self-test passed")
    return 0


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Fake ENNOID-BMS firmware endpoint for UI testing.")
    parser.add_argument("--host", default="127.0.0.1", help="TCP bind host. Default: 127.0.0.1")
    parser.add_argument("--port", type=int, default=65102, help="TCP bind port. Default: 65102")
    parser.add_argument("--serial-pty", action="store_true", help="Also create a pseudo-terminal serial endpoint.")
    parser.add_argument("--fault", action="store_true", help="Send a non-zero UI fault code and fault masks.")
    parser.add_argument(
        "--invalid-temps",
        action="store_true",
        help="Make COMM_EBMS_GET_EXP_TEMP return count=0 to exercise unavailable-temperature UI state.",
    )
    parser.add_argument("--quiet", action="store_true", help="Reduce packet logging.")
    parser.add_argument("--self-test", action="store_true", help="Run a local framing/protocol self-test and exit.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    options = FakeOptions(
        fault=args.fault,
        invalid_temps=args.invalid_temps,
        log_packets=not args.quiet,
    )

    if args.self_test:
        return run_self_test(options)

    protocol = FakeFirmwareProtocol(options)
    server = ThreadedTcpServer((args.host, args.port), FakeTcpHandler, protocol)
    pty_server: Optional[PtyServer] = None

    def shutdown(*_args) -> None:
        server.shutdown()
        server.server_close()
        if pty_server is not None:
            pty_server.stop()

    signal.signal(signal.SIGINT, shutdown)
    signal.signal(signal.SIGTERM, shutdown)

    if args.serial_pty:
        pty_server = PtyServer(protocol)
        slave_name = pty_server.start()
        print(f"serial PTY: {slave_name}", flush=True)

    print(f"fake BMS TCP server listening on {args.host}:{args.port}", flush=True)
    if args.fault:
        print("fault mode enabled", flush=True)
    if args.invalid_temps:
        print("invalid temp mode enabled: EXP_TEMP will return count=0", flush=True)

    try:
        server.serve_forever()
    finally:
        shutdown()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
