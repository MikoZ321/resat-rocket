"""Main application window."""

from __future__ import annotations

import re
import struct
from collections import deque
from functools import partial

import serial.tools.list_ports
from PySide6.QtCore import QThread, QTimer, Signal, Slot
from PySide6.QtGui import QAction
from PySide6.QtWidgets import (
    QApplication,
    QGridLayout,
    QInputDialog,
    QMainWindow,
    QMenu,
    QMenuBar,
    QMessageBox,
    QWidget,
)

from constants import (
    CommandType,
    DEFAULT_PAYLOAD,
    PACKET_END_BYTE,
    PACKET_PAYLOAD_LENGTH,
    PACKET_START_BYTE_1,
    PACKET_START_BYTE_2,
    PLOT_REFRESH_INTERVAL_MS,
    THRUST_BUFFER_MAXLEN,
)
from data_container import DataContainer, parse_packet
from serial_worker import SerialWorker
from widgets.panels import (
    CommunicationsInfoPanel,
    EngineInfoPanel,
    EnvironmentalPanel,
    FlightStatusPanel,
    GpsPanel,
    HydraulicsInfoPanel,
    ImuPanel,
    PowerPanel,
    PropulsionPanel,
    ThrustInfoPanel,
    ThrustPlotPanel,
)

# Matches a float with an optional C++-style suffix (f/F/l/L), e.g. 12.5, -3.2e3, 1.0f
CPP_FLOAT_PATTERN = re.compile(r"^[+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?[fFlL]?$")

GRID_COLUMNS = 10
GRID_ROWS = 5 


class Dashboard(QMainWindow):
    """Serves as the class container for the entire GUI."""

    send_serial_command = Signal(bytes)

    def __init__(self, app: QApplication) -> None:
        super().__init__()
        self.app = app
        self.setWindowTitle("Dynamometer Dashboard")

        self._buildMenuBar()
        self._buildCentralWidget()

        self.status_bar = self.statusBar()

        # serial state, TODO: save recent port config
        self.current_port: str | None = None
        self.serial_thread: QThread | None = None
        self.serial_worker: SerialWorker | None = None
        self._serial_buffer = b""

        # data buffers for thrust plot
        self.time_buffer: deque[float] = deque(maxlen=THRUST_BUFFER_MAXLEN)
        self.thrust_buffer: deque[float] = deque(maxlen=THRUST_BUFFER_MAXLEN)

        # plot refresh timer
        self.plot_timer = QTimer(self)
        self.plot_timer.setInterval(PLOT_REFRESH_INTERVAL_MS)
        self.plot_timer.timeout.connect(self.updateThrustPlot)

        self.parsed_data = DataContainer()

    # --- UI construction -------------------------------------------------

    def _buildMenuBar(self) -> None:
        menu_bar: QMenuBar = self.menuBar()

        file_menu: QMenu = menu_bar.addMenu("&File")
        file_menu.addAction("Save")
        file_menu.addAction("Save as ...")
        quit_action: QAction = file_menu.addAction("Quit")
        quit_action.triggered.connect(self.quit)

        settings_menu: QMenu = menu_bar.addMenu("Settings")
        self.select_port_menu: QMenu = settings_menu.addMenu("Select port")
        self.select_port_menu.aboutToShow.connect(self.listPorts)

        command_menu: QMenu = menu_bar.addMenu("Commands")
        dump_flash_action: QAction = command_menu.addAction("Dump flash")
        dump_flash_action.triggered.connect(partial(self.handleCommand, CommandType.DUMP_FLASH))

        config_menu: QMenu = menu_bar.addMenu("Config")
        enter_config_action: QAction = config_menu.addAction("Begin config")
        enter_config_action.triggered.connect(partial(self.handleCommand, CommandType.ENTER_CONFIG))
        exit_config_action: QAction = config_menu.addAction("Finish config")
        exit_config_action.triggered.connect(partial(self.handleCommand, CommandType.EXIT_CONFIG))

        # thrust loadcell config
        config_menu.addSeparator()
        set_thrust_scale_action: QAction = config_menu.addAction("Set thrust scale")
        set_thrust_scale_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.SET_THRUST_SCALE, "Set Thrust Scale", "Enter the desired scale factor")
        )
        set_thrust_offset_action: QAction = config_menu.addAction("Set thrust offset")
        set_thrust_offset_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.SET_THRUST_OFFSET, "Set Thrust Offset", "Enter the desired offset")
        )
        tare_thrust_action: QAction = config_menu.addAction("Tare thrust")
        tare_thrust_action.triggered.connect(partial(self.handleCommand, CommandType.TARE_THRUST))
        calibrate_thrust_action: QAction = config_menu.addAction("Calibrate thrust")
        calibrate_thrust_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.CALIBRATE_THRUST, "Calibrate Thrust", "Enter the mass placed on the loadcell in kilograms")
        )

        # oxidizer loadcell config
        config_menu.addSeparator()
        set_oxidizer_loadcell_scale_action: QAction = config_menu.addAction("Set oxidizer loadcell scale")
        set_oxidizer_loadcell_scale_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.SET_OXIDIZER_LOADCELL_SCALE, "Set Oxidizer Loadcell Scale", "Enter the desired scale factor")
        )
        set_oxidizer_loadcell_offset_action: QAction = config_menu.addAction("Set oxidizer loadcell offset")
        set_oxidizer_loadcell_offset_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.SET_OXIDIZER_LOADCELL_OFFSET, "Set Oxidizer Loadcell Offset", "Enter the desired offset")
        )
        tare_oxidizer_loadcell_action: QAction = config_menu.addAction("Tare oxidizer loadcell")
        tare_oxidizer_loadcell_action.triggered.connect(partial(self.handleCommand, CommandType.TARE_OXIDIZER_LOADCELL))
        calibrate_oxidizer_loadcell_action: QAction = config_menu.addAction("Calibrate oxidizer loadcell")
        calibrate_oxidizer_loadcell_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.CALIBRATE_OXIDIZER_LOADCELL, "Calibrate Oxidizer Loadcell", "Enter the mass placed on the loadcell in kilograms")
        )

        # pressure transducer config
        config_menu.addSeparator()
        set_fuel_pressure_scale_action: QAction = config_menu.addAction("Set fuel pressure scale")
        set_fuel_pressure_scale_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.SET_FUEL_PRESSURE_SCALE, "Set Fuel Pressure Scale", "Enter the desired scale factor")
        )
        set_fuel_pressure_offset_action: QAction = config_menu.addAction("Set fuel pressure offset")
        set_fuel_pressure_offset_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.SET_FUEL_PRESSURE_OFFSET, "Set Fuel Offset Scale", "Enter the desired offset")
        )
        set_oxidizer_pressure_scale_action: QAction = config_menu.addAction("Set oxidizer pressure scale")
        set_oxidizer_pressure_scale_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.SET_OXIDIZER_PRESSURE_SCALE, "Set Oxidizer Pressure Scale", "Enter the desired scale factor")
        )
        set_oxidizer_pressure_offset_action: QAction = config_menu.addAction("Set oxidizer pressure offset")
        set_oxidizer_pressure_offset_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.SET_OXIDIZER_PRESSURE_OFFSET, "Set Oxidizer Offset Scale", "Enter the desired offset")
        )
        tare_pressure_action: QAction = config_menu.addAction("Tare pressure")
        tare_pressure_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.TARE_PRESSURE, "Tare Pressure", "Enter the known atmospheric pressure in Pascals.")
        )
        calibrate_fuel_pressure_action: QAction = config_menu.addAction("Calibrate fuel pressure")
        calibrate_fuel_pressure_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.CALIBRATE_FUEL_PRESSURE, "Calibrate Fuel Pressure", "Enter the known pressure in Pascals")
        )
        calibrate_oxidizer_pressure_action: QAction = config_menu.addAction("Calibrate oxidizer pressure")
        calibrate_oxidizer_pressure_action.triggered.connect(
            partial(self.promptFloatCommand, CommandType.CALIBRATE_OXIDIZER_PRESSURE, "Calibrate Oxidizer Pressure", "Enter the known pressure in Pascals")
        )


    def _buildCentralWidget(self) -> None:
        central_widget = QWidget(self)
        self.setCentralWidget(central_widget)
 
        # thrust plot lives in its own full-width row; everything else is
        # a subsystem panel laid out in a uniform grid below it
        self.thrust_plot_panel = ThrustPlotPanel()
        self.thrust_curve = self.thrust_plot_panel.thrust_curve
 
        self.thrust_info_panel = ThrustInfoPanel()
        self.imu_panel = ImuPanel()
        self.environmental_panel = EnvironmentalPanel()
        self.propulsion_panel = PropulsionPanel()
        self.engine_info_panel = EngineInfoPanel()
        self.power_panel = PowerPanel()
        self.gps_panel = GpsPanel()
        self.flight_status_panel = FlightStatusPanel()
        self.communications_panel = CommunicationsInfoPanel()
        self.hydraulics_panel = HydraulicsInfoPanel()
  
        main_grid_layout = QGridLayout()
        main_grid_layout.addWidget(self.thrust_plot_panel, 0, 0, 3, 6)
        main_grid_layout.addWidget(self.thrust_info_panel, 0, 6, 3, 1)
        main_grid_layout.addWidget(self.propulsion_panel, 0, 7, 3, 1)
        main_grid_layout.addWidget(self.gps_panel, 0, 8, 3, 1)
        main_grid_layout.addWidget(self.communications_panel, 0, 9, 3, 1)
        main_grid_layout.addWidget(self.imu_panel, 3, 0, 2, 2)
        main_grid_layout.addWidget(self.environmental_panel, 3, 2, 2, 1)
        main_grid_layout.addWidget(self.engine_info_panel, 3, 3, 2, 2)
        main_grid_layout.addWidget(self.power_panel, 3, 5, 2, 1)
        main_grid_layout.addWidget(self.hydraulics_panel, 3, 6, 2, 2)
        main_grid_layout.addWidget(self.flight_status_panel, 3, 8, 2, 2)
  
        main_grid_layout.setSpacing(8)
        main_grid_layout.setContentsMargins(8, 8, 8, 8)
 
        central_widget.setLayout(main_grid_layout)

    # --- Commands ----------------------------------------------------------

    @Slot(CommandType)
    def handleCommand(self, command: CommandType, payload: bytes = DEFAULT_PAYLOAD) -> None:
        if len(payload) != PACKET_PAYLOAD_LENGTH:
            payload = (payload + DEFAULT_PAYLOAD)[:PACKET_PAYLOAD_LENGTH]

        packet = (
            bytes([PACKET_START_BYTE_1, PACKET_START_BYTE_2, int(command)])
            + payload
            + bytes([PACKET_END_BYTE])
        )
        self.send_serial_command.emit(packet)

    def promptFloatCommand(self, command: CommandType, dialog_title: str, dialog_text: str = "Enter payload as a float (e.g. 12.5, -3.2e3, 1.0f):") -> None:
        """Prompts the user for a float payload and sends it with the given command.

        Replaces the previous promptThrustScale/promptThrustOffset, which were
        near-identical copies differing only in which CommandType they sent.
        """
        text, ok = QInputDialog.getText(
            self, dialog_title, dialog_text
        )

        if not ok:
            return  # user cancelled

        text = text.strip()

        if not CPP_FLOAT_PATTERN.match(text):
            QMessageBox.warning(
                self, "Invalid payload",
                "Payload must be a valid float, e.g. 12.5, -3.2e3, or 1.0f."
            )
            return

        # strip C++-style float suffix (f/F/l/L); Python's float() rejects it
        numeric_text = text.rstrip("fFlL")

        try:
            value = float(numeric_text)
        except ValueError:
            QMessageBox.warning(
                self, "Invalid payload",
                "Payload must be a valid float, e.g. 12.5, -3.2e3, or 1.0f."
            )
            return

        try:
            payload = struct.pack("<f", value)
        except struct.error:
            QMessageBox.warning(
                self, "Invalid payload",
                "Value is out of range for a 32-bit float."
            )
            return

        self.handleCommand(command, payload)

    # --- Telemetry ----------------------------------------------------------

    def handlePacket(self, raw_data: bytes) -> None:
        """Receives raw serial data from SerialWorker. Runs in the GUI thread."""
        self._serial_buffer += raw_data

        while b"\n" in self._serial_buffer:
            line, self._serial_buffer = self._serial_buffer.split(b"\n", 1)

            # TODO: protect against corrupted or incomplete packets
            self.parsed_data = parse_packet(line, self.parsed_data)
            self._refreshWidgets(self.parsed_data)

    def _refreshWidgets(self, data: DataContainer) -> None:
        """Pushes every field of `data` into its corresponding dashboard widget."""
        self.thrust_buffer.append(data.current_thrust)
        self.time_buffer.append(data.elapsed_time)

        # thrust info
        self.thrust_info_panel.current_thrust.setValue(f"{data.current_thrust:.2f}")
        self.thrust_info_panel.max_thrust.setValue(f"{data.max_thrust:.2f}")
        self.thrust_info_panel.total_impulse.setValue(f"{data.total_impulse:.2f}")

        # IMU
        self.imu_panel.acceleration_main.setValue(data.acceleration_main)
        self.imu_panel.angular_velocity.setValue(data.angular_velocity)
        self.imu_panel.acceleration_high_g.setValue(data.acceleration_high_g)

        # environmental
        self.environmental_panel.air_pressure.setValue(f"{data.air_pressure:.2f}")
        self.environmental_panel.air_temperature.setValue(f"{data.air_temperature:.2f}")
        self.environmental_panel.altitude_fusion.setValue(f"{data.altitude_fusion:.2f}")

        # propulsion
        self.propulsion_panel.fuel_pressure.setValue(f"{data.fuel_pressure:.2f}")
        self.propulsion_panel.oxidizer_pressure.setValue(f"{data.oxidizer_pressure:.2f}")
        self.propulsion_panel.oxidizer_weight.setValue(f"{data.oxidizer_weight:.2f}")
        self.propulsion_panel.piston_position.setValue(f"{data.piston_position:.2f}")

        # engine
        self.engine_info_panel.engine_temperature_top.setValue(f"{data.engine_temperature_top:.2f}")
        self.engine_info_panel.engine_temperature_bottom.setValue(f"{data.engine_temperature_bottom:.2f}")

        # power
        self.power_panel.pyro_battery_voltage.setValue(f"{data.pyro_battery_voltage:.2f}")
        self.power_panel.main_battery_voltage.setValue(f"{data.main_battery_voltage:.2f}")
        self.power_panel.main_battery_level.setValue(f"{data.main_battery_level:.2f}")

        # GPS
        self.gps_panel.latitude.setValue(f"{data.latitude:.6f}")
        self.gps_panel.longitude.setValue(f"{data.longitude:.6f}")
        self.gps_panel.altitude_gps.setValue(f"{data.altitude_gps:.2f}")
        self.gps_panel.sattelite_count.setValue(str(data.sattelite_count))

        # flight status
        self.flight_status_panel.telemetry_frame_index.setValue(str(data.telemetry_frame_index))
        self.flight_status_panel.current_flight_phase.setValue(str(data.current_flight_phase))
        self.flight_status_panel.current_arm_state.setValue(str(data.current_arm_state))

        # communications / link health
        self.communications_panel.data_frequency.setValue(f"{data.data_frequency:.2f}")
        self.communications_panel.elapsed_time.setValue(f"{data.elapsed_time:.2f}")
        self.communications_panel.received_frame_count.setValue(str(data.received_frame_count))
        self.communications_panel.invalid_frame_count.setValue(str(data.invalid_frame_count))
        self.communications_panel.missing_frame_count.setValue(str(data.missing_frame_count))

    def updateThrustPlot(self) -> None:
        if not self.time_buffer:
            return

        self.thrust_curve.setData(list(self.time_buffer), list(self.thrust_buffer))

    # --- Serial port lifecycle ----------------------------------------------

    def listPorts(self) -> None:
        """Lists all of the available COM ports."""
        self.select_port_menu.clear()

        port_names = [port.device for port in serial.tools.list_ports.comports()]

        for port_name in port_names:
            port_name_action = self.select_port_menu.addAction(port_name)
            port_name_action.setCheckable(True)
            port_name_action.setChecked(port_name == self.current_port)
            port_name_action.triggered.connect(partial(self.setCurrentPort, port_name))

    def setCurrentPort(self, port_name: str) -> None:
        """Changes the serial port from which data are read."""
        self.current_port = port_name
        self.status_bar.showMessage(f"Connecting to {port_name}...")

        self._stopSerialWorker()
        self._startSerialWorker(port_name)

    def _startSerialWorker(self, port_name: str) -> None:
        self._stopSerialWorker()

        self.serial_thread = QThread()
        self.serial_worker = SerialWorker()
        self.serial_worker.moveToThread(self.serial_thread)

        self.serial_worker.setPort(port_name)
        self.serial_thread.started.connect(self.serial_worker.start)

        self.serial_worker.packet_received.connect(self.handlePacket)
        self.serial_worker.status_changed.connect(self.status_bar.showMessage)
        self.serial_worker.error.connect(self.status_bar.showMessage)
        self.send_serial_command.connect(self.serial_worker.sendCommand)

        self.serial_thread.start()
        self.plot_timer.start()

    def _stopSerialWorker(self) -> None:
        if self.serial_worker:
            self.serial_worker.stop()

        if self.serial_thread:
            self.serial_thread.quit()
            self.serial_thread.wait()

        self.serial_worker = None
        self.serial_thread = None
        self.plot_timer.stop()

    # --- Misc ----------------------------------------------------------

    def quit(self) -> None:
        """Closes the window and kills the application."""
        self.app.quit()