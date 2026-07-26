"""Telemetry data model and packet parsing.

`parse_packet` used to reach into a module-level `window` global to get the
previous sample for computing derived quantities (elapsed time, data
frequency, total impulse). That made the function untestable and coupled it
tightly to the GUI. It now takes the previous `DataContainer` explicitly as
an argument.
"""

from __future__ import annotations

from dataclasses import dataclass

from constants import (
    TELEMETRY_FIELD_CURRENT_THRUST,
    TELEMETRY_FIELD_FUEL_PRESSURE,
    TELEMETRY_FIELD_OXIDIZER_PRESSURE,
    TELEMETRY_FIELD_TIMESTAMP_MS,
)


@dataclass
class DataContainer:
    """Stores both the incoming telemetry and derived quantities."""

    # incoming telemetry values
    timestamp: float = 0.0          # seconds
    current_thrust: float = 0.0     # Newtons
    oxidizer_pressure: float = 0.0  # Pascals
    fuel_pressure: float = 0.0      # Pascals

    # historical telemetry values
    max_thrust: float = 0.0         # Newtons

    # derived values
    data_frequency: float = 0.0     # Hertz
    elapsed_time: float = 0.0       # seconds
    total_impulse: float = 0.0      # Newton seconds


def _parse_float(fields: list[str], index: int, default: float = 0.0) -> float:
    """Best-effort float parse; falls back to `default` on any bad/missing field."""
    try:
        return float(fields[index])
    except (IndexError, ValueError):
        return default


def parse_packet(raw_data: bytes, previous: DataContainer) -> DataContainer:
    """Parses one semicolon-delimited telemetry line into a new DataContainer.

    `previous` is the last successfully parsed sample, used to compute
    elapsed time, instantaneous data frequency, and total impulse
    (trapezoidal integration of thrust over time).
    """
    try:
        decoded = raw_data.decode("utf-8")
    except UnicodeDecodeError:
        decoded = ""

    fields = decoded.split(";")
    result = DataContainer()

    # timestamp arrives in milliseconds
    raw_timestamp_ms = _parse_float(fields, TELEMETRY_FIELD_TIMESTAMP_MS)
    result.timestamp = raw_timestamp_ms / 1000

    previous_time_sent = previous.timestamp if previous.timestamp != 0 else result.timestamp
    time_since_last_packet = result.timestamp - previous_time_sent

    if time_since_last_packet:
        result.data_frequency = 1 / time_since_last_packet

    result.elapsed_time = previous.elapsed_time + time_since_last_packet

    result.current_thrust = _parse_float(fields, TELEMETRY_FIELD_CURRENT_THRUST)
    result.max_thrust = max(result.current_thrust, previous.max_thrust)

    # TODO: check whether trapezoidal integration is the right model here
    result.total_impulse = previous.total_impulse + (
        (result.current_thrust + previous.current_thrust) * 0.5 * time_since_last_packet
    )

    result.fuel_pressure = _parse_float(fields, TELEMETRY_FIELD_FUEL_PRESSURE)
    result.oxidizer_pressure = _parse_float(fields, TELEMETRY_FIELD_OXIDIZER_PRESSURE)

    return result
