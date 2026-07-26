"""Telemetry data model and packet parsing.

`parse_packet` used to reach into a module-level `window` global to get the
previous sample for computing derived quantities (elapsed time, data
frequency, total impulse). That made the function untestable and coupled it
tightly to the GUI. It now takes the previous `DataContainer` explicitly as
an argument.
"""

from __future__ import annotations

from dataclasses import dataclass, field

from constants import (
    TelemetryField
)


@dataclass
class DataContainer:
    """Stores both the incoming telemetry and derived quantities."""

    # incoming telemetry values
    timestamp: float = 0.0          # seconds
    telemetry_frame_index: int = 0
    acceleration_main: list[float] = field(default_factory=lambda: [0.0, 0.0, 0.0])    # m/s^2
    angular_velocity: list[float] = field(default_factory=lambda: [0.0, 0.0, 0.0])     # rad/s
    acceleration_high_g: list[float] = field(default_factory=lambda: [0.0, 0.0, 0.0])  # m/s^2
    air_pressure: float = 0.0
    air_temperature: float = 0.0
    altitude_fusion: float = 0.0
    current_thrust: float = 0.0     # Newtons
    oxidizer_weight: float = 0.0
    fuel_pressure: float = 0.0      # Pascals
    oxidizer_pressure: float = 0.0  # Pascals
    pyro_battery_voltage: float = 0.0
    main_battery_voltage: float = 0.0
    engine_temperature_top: float = 0.0
    engine_temperature_bottom: float = 0.0
    piston_position: float = 0.0
    latitude: float = 0.0
    longitude: float = 0.0
    altitude_gps: float = 0.0
    sattelite_count: int = 0
    main_battery_level: float = 0.0
    is_valid_bit_mask: int = 0
    current_flight_phase: int = 0
    current_arm_state: int = 0
    last_command_frame_index: int = 0
    last_command_result: int = 0
    received_frame_count: int = 0
    invalid_frame_count: int = 0
    missing_frame_count: int = 0

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


def _parse_int(fields: list[str], index: int, default: int = 0) -> int:
    """Best-effort int parser; falls back to `default` on any bad/missing field."""
    try: 
        return int(fields[index])
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
    raw_timestamp_ms = _parse_float(fields, TelemetryField.TIMESTAMP_MS)
    result.timestamp = raw_timestamp_ms / 1000

    previous_time_sent = previous.timestamp if previous.timestamp != 0 else result.timestamp
    time_since_last_packet = result.timestamp - previous_time_sent

    if time_since_last_packet:
        result.data_frequency = 1 / time_since_last_packet

    result.elapsed_time = previous.elapsed_time + time_since_last_packet

    result.telemetry_frame_index = _parse_int(fields, TelemetryField.TELEMETRY_FRAME_INDEX)

    # parse imu data
    result.acceleration_main[0] = _parse_float(fields, TelemetryField.ACCELERATION_MAIN_X)
    result.acceleration_main[1] = _parse_float(fields, TelemetryField.ACCELERATION_MAIN_Y)
    result.acceleration_main[2] = _parse_float(fields, TelemetryField.ACCELERATION_MAIN_Z)
    result.angular_velocity[0] = _parse_float(fields, TelemetryField.ANGULAR_VELOCITY_X)
    result.angular_velocity[1] = _parse_float(fields, TelemetryField.ANGULAR_VELOCITY_Y)
    result.angular_velocity[2] = _parse_float(fields, TelemetryField.ANGULAR_VELOCITY_Z)
    result.acceleration_high_g[0] = _parse_float(fields, TelemetryField.ACCELERATION_HIGH_G_X)
    result.acceleration_high_g[1] = _parse_float(fields, TelemetryField.ACCELERATION_HIGH_G_Y)
    result.acceleration_high_g[2] = _parse_float(fields, TelemetryField.ACCELERATION_HIGH_G_Z)

    # parse atmospheric data
    result.air_pressure = _parse_float(fields, TelemetryField.AIR_PRESSURE)
    result.air_temperature = _parse_float(fields, TelemetryField.AIR_TEMPERATURE)

    # parse fused values
    result.altitude_fusion = _parse_float(fields, TelemetryField.ALTITUDE_FUSION)

    # parse thrust
    result.current_thrust = _parse_float(fields, TelemetryField.ENGINE_THRUST)
    result.max_thrust = max(result.current_thrust, previous.max_thrust)
    result.total_impulse = previous.total_impulse + (
        (result.current_thrust + previous.current_thrust) * 0.5 * time_since_last_packet
    )

    # parse oxidizer weight
    result.oxidizer_weight = _parse_float(fields, TelemetryField.OXIDIZER_WEIGHT)

    # parse pressure transducers
    result.fuel_pressure = _parse_float(fields, TelemetryField.FUEL_PRESSURE)
    result.oxidizer_pressure = _parse_float(fields, TelemetryField.OXIDIZER_PRESSURE)

    # parse voltage
    result.pyro_battery_voltage = _parse_float(fields, TelemetryField.PYRO_BATTERY_VOLTAGE)
    result.main_battery_voltage = _parse_float(fields, TelemetryField.MAIN_BATTERY_VOLTAGE)

    # parse engine temperatures
    result.engine_temperature_top = _parse_float(fields, TelemetryField.ENGINE_TEMPERATURE_TOP)
    result.engine_temperature_bottom = _parse_float(fields, TelemetryField.ENGINE_TEMPERATURE_BOTTOM)

    # parse piston position
    result.piston_position = _parse_int(fields, TelemetryField.PISTON_POSITION)

    # parse GPS data
    result.latitude = _parse_float(fields, TelemetryField.LATITUDE)
    result.longitude = _parse_float(fields, TelemetryField.LONGITUDE)
    result.altitude_gps = _parse_float(fields, TelemetryField.ALTITUDE_GPS)
    result.sattelite_count = _parse_int(fields, TelemetryField.SATTELITE_COUNT)

    # parse main battery level
    result.main_battery_level = _parse_float(fields, TelemetryField.MAIN_BATTERY_LEVEL)

    # parse meta data
    result.is_valid_bit_mask = _parse_int(fields, TelemetryField.IS_VALID_BIT_MASK)
    result.current_flight_phase = _parse_int(fields, TelemetryField.CURRENT_FLIGHT_PHASE)
    result.current_arm_state = _parse_int(fields, TelemetryField.CURRENT_ARM_STATE)
    result.last_command_frame_index = _parse_int(fields, TelemetryField.LAST_COMMAND_FRAME_INDEX)
    result.last_command_result = _parse_int(fields, TelemetryField.LAST_COMMAND_RESULT)

    # parse communication info
    result.received_frame_count = _parse_int(fields, TelemetryField.RECEIVED_FRAME_COUNT)
    result.invalid_frame_count = _parse_int(fields, TelemetryField.INVALID_FRAME_COUNT)
    result.missing_frame_count = _parse_int(fields, TelemetryField.MISSING_FRAME_COUNT)

    return result
