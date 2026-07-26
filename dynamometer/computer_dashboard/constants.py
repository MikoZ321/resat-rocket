"""Shared constants: command codes, serial packet framing, and telemetry field layout."""

from enum import IntEnum


class CommandType(IntEnum):
    ENTER_CONFIG = 0
    EXIT_CONFIG = 1
    SET_THRUST_SCALE = 2
    SET_THRUST_OFFSET = 3
    TARE_THRUST = 4
    CALIBRATE_THRUST = 5
    DUMP_FLASH = 6


# --- Outgoing command packet framing ---
PACKET_START_BYTE_1 = 0xCC
PACKET_START_BYTE_2 = 0x77
PACKET_END_BYTE = 0x88
PACKET_PAYLOAD_LENGTH = 4
DEFAULT_PAYLOAD = b"\x00\x00\x00\x00"

# --- Incoming telemetry packet field indices (semicolon-delimited line) ---
TELEMETRY_FIELD_TIMESTAMP_MS = 0
TELEMETRY_FIELD_CURRENT_THRUST = 14
TELEMETRY_FIELD_FUEL_PRESSURE = 16
TELEMETRY_FIELD_OXIDIZER_PRESSURE = 17

# --- Serial / UI timing ---
DEFAULT_BAUDRATE = 115200
SERIAL_POLL_INTERVAL_MS = 5
PLOT_REFRESH_INTERVAL_MS = 33  # ~30 Hz
THRUST_BUFFER_MAXLEN = 2000
