"""Concrete dashboard section panels, each a self-contained widget."""

from __future__ import annotations

import pyqtgraph as pg
from PySide6.QtWidgets import QGridLayout, QHBoxLayout, QPushButton, QVBoxLayout, QWidget

from widgets.dashboard_panel import DashboardPanel
from widgets.label_value_pair import LabelValuePair
from widgets.valve_control import ValveControlWidget


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

        self.current_thrust = LabelValuePair("Current thrust", "50", "N")
        self.max_thrust = LabelValuePair("Maximum thrust", "2000", "N")
        self.total_impulse = LabelValuePair("Total impulse", "3000", "Ns")

        layout = QVBoxLayout()
        layout.addWidget(self.current_thrust)
        layout.addWidget(self.max_thrust)
        layout.addWidget(self.total_impulse)
        self.setLayout(layout)


class EngineInfoPanel(DashboardPanel):
    """Engine solenoid/flame state and burn-time info."""

    def __init__(self) -> None:
        super().__init__("Engine info")

        # TODO: create graphic to display open/closed state of solenoid valves
        solenoid_valve_container = QWidget()
        # TODO: create graphic to display engine temperature and flame being lit
        engine_container = QWidget()

        # TODO: add burn_time calculations
        burn_time = LabelValuePair("Burn time", "3", "s")
        self.brightness = LabelValuePair("Brightness", "37", "%")

        burn_info_layout = QHBoxLayout()
        burn_info_layout.addWidget(burn_time)
        burn_info_layout.addWidget(self.brightness)
        burn_info_container = QWidget()
        burn_info_container.setLayout(burn_info_layout)

        self.ignite_button = QPushButton("Ignite")

        layout = QVBoxLayout()
        layout.addWidget(solenoid_valve_container)
        layout.addWidget(engine_container)
        layout.addWidget(burn_info_container)
        layout.addWidget(self.ignite_button)
        self.setLayout(layout)


class CommunicationsInfoPanel(DashboardPanel):
    """Link quality (RSSI), data rate, and elapsed time."""

    def __init__(self) -> None:
        super().__init__("Communications info")

        self.rssi = LabelValuePair("RSSI", "-100", "dBm")
        self.data_frequency = LabelValuePair("Data frequency", "80", "Hz")
        self.elapsed_time = LabelValuePair("Elapsed time", "1000", "s")

        layout = QVBoxLayout()
        layout.addWidget(self.rssi)
        layout.addWidget(self.data_frequency)
        layout.addWidget(self.elapsed_time)
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


class TankInfoPanel(DashboardPanel):
    """Propellant tank pressures."""

    def __init__(self) -> None:
        super().__init__("Tank info")

        self.fuel_pressure = LabelValuePair("Fuel pressure", "20", "Pa")
        # TODO: add tank graphic to track piston position
        tank_container = QWidget()
        self.oxidizer_pressure = LabelValuePair("Oxidizer pressure", "20", "Pa")

        layout = QVBoxLayout()
        layout.addWidget(self.fuel_pressure)
        layout.addWidget(tank_container)
        layout.addWidget(self.oxidizer_pressure)
        self.setLayout(layout)
