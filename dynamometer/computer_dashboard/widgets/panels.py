"""Concrete dashboard section panels, each a self-contained widget.

Panels are grouped by subsystem so that DataContainer's fields (apart from
the live thrust curve, which ThrustPlotPanel owns) each land in one clearly
named place:

  ThrustInfoPanel          -- current_thrust, max_thrust, total_impulse
  ImuPanel                 -- acceleration_main, angular_velocity, acceleration_high_g
  EnvironmentalPanel       -- air_pressure, air_temperature, altitude_fusion
  PropulsionPanel          -- fuel_pressure, oxidizer_pressure, oxidizer_weight, piston_position
  EngineInfoPanel          -- engine_temperature_top, engine_temperature_bottom (+ ignite control)
  PowerPanel               -- pyro_battery_voltage, main_battery_voltage, main_battery_level
  GpsPanel                 -- latitude, longitude, altitude_gps, sattelite_count
  FlightStatusPanel        -- telemetry_frame_index, current_flight_phase, current_arm_state
  CommunicationsInfoPanel  -- data_frequency, elapsed_time, received/invalid/missing frame counts
  HydraulicsInfoPanel      -- valve controls (not telemetry-driven)
"""

from __future__ import annotations

import pyqtgraph as pg
from PySide6.QtWidgets import QGridLayout, QHBoxLayout, QPushButton, QVBoxLayout, QWidget

from widgets.dashboard_panel import DashboardPanel
from widgets.label_value_pair import LabelValuePair
from widgets.valve_control import ValveControlWidget
from widgets.vector3_widget import Vector3Widget


class ThrustPlotPanel(DashboardPanel):
    """Live thrust-vs-time plot."""

    def __init__(self) -> None:
        super().__init__("Thrust plot")

        self.graph_widget = pg.PlotWidget()
        self.graph_widget.setLabel("left", "Thrust (N)")
        self.graph_widget.setLabel("bottom", "Time (s)")
        self.thrust_curve = self.graph_widget.plot([], [])

        layout = QVBoxLayout()
        layout.addWidget(self.graph_widget)
        self.setLayout(layout)


class ThrustInfoPanel(DashboardPanel):
    """Current thrust, max thrust, and total impulse readings."""

    def __init__(self) -> None:
        super().__init__("Thrust info")

        self.current_thrust = LabelValuePair("Current thrust", "0.00", "N")
        self.max_thrust = LabelValuePair("Maximum thrust", "0.00", "N")
        self.total_impulse = LabelValuePair("Total impulse", "0.00", "Ns")

        layout = QVBoxLayout()
        layout.addWidget(self.current_thrust)
        layout.addWidget(self.max_thrust)
        layout.addWidget(self.total_impulse)
        self.setLayout(layout)


class ImuPanel(DashboardPanel):
    """Main/high-g accelerometers and gyro readings."""

    def __init__(self) -> None:
        super().__init__("IMU")

        self.acceleration_main = Vector3Widget("Acceleration (main)", "m/s\u00b2")
        self.angular_velocity = Vector3Widget("Angular velocity", "rad/s")
        self.acceleration_high_g = Vector3Widget("Acceleration (high-g)", "m/s\u00b2")

        layout = QVBoxLayout()
        layout.addWidget(self.acceleration_main)
        layout.addWidget(self.angular_velocity)
        layout.addWidget(self.acceleration_high_g)
        self.setLayout(layout)


class EnvironmentalPanel(DashboardPanel):
    """Ambient air pressure/temperature and fused altitude."""

    def __init__(self) -> None:
        super().__init__("Environmental")

        self.air_pressure = LabelValuePair("Air pressure", "0.00", "Pa")
        self.air_temperature = LabelValuePair("Air temperature", "0.00", "\u00b0C")
        self.altitude_fusion = LabelValuePair("Altitude (fused)", "0.00", "m")

        layout = QVBoxLayout()
        layout.addWidget(self.air_pressure)
        layout.addWidget(self.air_temperature)
        layout.addWidget(self.altitude_fusion)
        self.setLayout(layout)


class PropulsionPanel(DashboardPanel):
    """Tank pressures, oxidizer weight, and piston position."""

    def __init__(self) -> None:
        super().__init__("Propulsion")

        self.fuel_pressure = LabelValuePair("Fuel pressure", "0.00", "bar")
        self.oxidizer_pressure = LabelValuePair("Oxidizer pressure", "0.00", "bar")
        self.oxidizer_weight = LabelValuePair("Oxidizer weight", "0.00", "kg")
        self.piston_position = LabelValuePair("Piston position", "0.00", "mm")

        layout = QVBoxLayout()
        layout.addWidget(self.fuel_pressure)
        layout.addWidget(self.oxidizer_pressure)
        layout.addWidget(self.oxidizer_weight)
        layout.addWidget(self.piston_position)
        self.setLayout(layout)


class EngineInfoPanel(DashboardPanel):
    """Engine temperatures and ignition control."""

    def __init__(self) -> None:
        super().__init__("Engine info")

        # TODO: create graphic to display open/closed state of solenoid valves
        solenoid_valve_container = QWidget()
        # TODO: create graphic to display engine temperature and flame being lit
        engine_container = QWidget()

        self.engine_temperature_top = LabelValuePair("Engine temp (top)", "0.00", "\u00b0C")
        self.engine_temperature_bottom = LabelValuePair("Engine temp (bottom)", "0.00", "\u00b0C")

        temperature_layout = QHBoxLayout()
        temperature_layout.addWidget(self.engine_temperature_top)
        temperature_layout.addWidget(self.engine_temperature_bottom)
        temperature_container = QWidget()
        temperature_container.setLayout(temperature_layout)

        self.ignite_button = QPushButton("Ignite")

        layout = QVBoxLayout()
        layout.addWidget(solenoid_valve_container)
        layout.addWidget(engine_container)
        layout.addWidget(temperature_container)
        layout.addWidget(self.ignite_button)
        self.setLayout(layout)


class PowerPanel(DashboardPanel):
    """Battery voltages and charge level."""

    def __init__(self) -> None:
        super().__init__("Power")

        self.pyro_battery_voltage = LabelValuePair("Pyro battery", "0.00", "V")
        self.main_battery_voltage = LabelValuePair("Main battery", "0.00", "V")
        self.main_battery_level = LabelValuePair("Main battery level", "0.00", "%")

        layout = QVBoxLayout()
        layout.addWidget(self.pyro_battery_voltage)
        layout.addWidget(self.main_battery_voltage)
        layout.addWidget(self.main_battery_level)
        self.setLayout(layout)


class GpsPanel(DashboardPanel):
    """GPS fix: position, altitude, satellite count."""

    def __init__(self) -> None:
        super().__init__("GPS")

        self.latitude = LabelValuePair("Latitude", "0.000000", "\u00b0")
        self.longitude = LabelValuePair("Longitude", "0.000000", "\u00b0")
        self.altitude_gps = LabelValuePair("Altitude (GPS)", "0.00", "m")
        self.sattelite_count = LabelValuePair("Satellites", "0", "")

        layout = QVBoxLayout()
        layout.addWidget(self.latitude)
        layout.addWidget(self.longitude)
        layout.addWidget(self.altitude_gps)
        layout.addWidget(self.sattelite_count)
        self.setLayout(layout)


class FlightStatusPanel(DashboardPanel):
    """Current flight phase, arm state, and frame index.

    TODO: map current_flight_phase / current_arm_state int codes to their
    human-readable names once the corresponding enums are defined; for now
    the raw integer values are shown.
    """

    def __init__(self) -> None:
        super().__init__("Flight status")

        self.telemetry_frame_index = LabelValuePair("Frame index", "0", "")
        self.current_flight_phase = LabelValuePair("Flight phase", "0", "")
        self.current_arm_state = LabelValuePair("Arm state", "0", "")

        layout = QVBoxLayout()
        layout.addWidget(self.telemetry_frame_index)
        layout.addWidget(self.current_flight_phase)
        layout.addWidget(self.current_arm_state)
        self.setLayout(layout)


class CommunicationsInfoPanel(DashboardPanel):
    """Link timing/rate and frame accounting."""

    def __init__(self) -> None:
        super().__init__("Communications info")

        self.data_frequency = LabelValuePair("Data frequency", "0.00", "Hz")
        self.elapsed_time = LabelValuePair("Elapsed time", "0.00", "s")
        self.received_frame_count = LabelValuePair("Received frames", "0", "")
        self.invalid_frame_count = LabelValuePair("Invalid frames", "0", "")
        self.missing_frame_count = LabelValuePair("Missing frames", "0", "")

        layout = QVBoxLayout()
        layout.addWidget(self.data_frequency)
        layout.addWidget(self.elapsed_time)
        layout.addWidget(self.received_frame_count)
        layout.addWidget(self.invalid_frame_count)
        layout.addWidget(self.missing_frame_count)
        self.setLayout(layout)


class HydraulicsInfoPanel(DashboardPanel):
    """Valve status/controls for the hydraulics system."""

    def __init__(self) -> None:
        super().__init__("Hydraulics info")

        self.fuel_screw_plug = ValveControlWidget("Fuel screw plug")
        self.oxidizer_tanking_valve = ValveControlWidget("Oxidizer tanking valve")
        self.vessel_vent_valve = ValveControlWidget("Vessel vent valve")
        self.hose_vent_valve = ValveControlWidget("Hose vent valve")

        layout = QGridLayout()
        layout.addWidget(self.fuel_screw_plug, 0, 0)
        layout.addWidget(self.oxidizer_tanking_valve, 0, 1)
        layout.addWidget(self.vessel_vent_valve, 1, 0)
        layout.addWidget(self.hose_vent_valve, 1, 1)
        self.setLayout(layout)
