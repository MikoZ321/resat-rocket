from PySide6.QtWidgets import QMainWindow, QApplication, QMenuBar, QMenu, QGridLayout, QWidget, QPushButton, QSizePolicy, QVBoxLayout, QHBoxLayout, QLabel
from PySide6.QtGui import QAction
from PySide6.QtCore import QObject, Signal, Slot, QThread, QTimer
import serial
import serial.tools.list_ports
import time
import pyqtgraph as pg
from collections import deque

# TODO: save the information in a .csv file

# TODO: display the readings in a GUI
class Dashboard(QMainWindow):
    '''Serves as the class container for the entire GUI'''
    def __init__(self, app: QApplication) -> None:
        super().__init__()
        self.app: QApplication = app
        self.setWindowTitle("Dynamometer Dashboard")

        # init menu bar
        menu_bar: QMenuBar = self.menuBar()

        # init file menu
        file_menu: QMenu = menu_bar.addMenu("&File")
        file_menu.addAction("Save")
        file_menu.addAction("Save as ...")
        quit_action: QAction = file_menu.addAction("Quit")
        quit_action.triggered.connect(self.quit)

        # init settings menu
        settings_menu: QMenu = menu_bar.addMenu("Settings")
        self.select_port_menu: QMenu = settings_menu.addMenu("Select port")
        self.select_port_menu.aboutToShow.connect(self.listPorts)

        # init serial port to None, TODO: save recent config
        self.current_port: str | None = None

        # start central widget section
        central_widget: QWidget = QWidget(self)
        self.setCentralWidget(central_widget)

        # init container widgets
        thrust_plot = DashboardPanel("Thrust plot")
        thrust_plot_container = thrust_plot.createThrustPlot()

        self.thrust_curve = thrust_plot.thrust_curve

        # init thrust info panel
        thrust_panel = DashboardPanel("Thrust info")
        thrust_info_container = thrust_panel.createThrustInfo()

        self.current_thrust_widget = thrust_panel.current_thrust
        self.max_thrust_widget = thrust_panel.max_thrust
        self.total_impulse_widget = thrust_panel.total_impulse

        engine_info_container = DashboardPanel("Engine info").createEngineInfo()
        communication_info_container = DashboardPanel("Communications info").createCommunicationsInfo()
        hydraulics_info_container = DashboardPanel("Hydraulics info").createHydraulicsInfo()
        tank_info_container = DashboardPanel("Tank info").createTankInfo()

        # init grid to organize widgets
        main_grid_layout: QGridLayout = QGridLayout()
        main_grid_layout.addWidget(thrust_plot_container, 0,0,1,4)
        main_grid_layout.addWidget(thrust_info_container, 0,4,1,2)
        main_grid_layout.addWidget(engine_info_container, 0,6,2,3)
        main_grid_layout.addWidget(communication_info_container, 1,0,1,2)
        main_grid_layout.addWidget(hydraulics_info_container, 1,2,1,2)
        main_grid_layout.addWidget(tank_info_container, 1,4,1,2)

        main_grid_layout.setSpacing(8)
        main_grid_layout.setContentsMargins(8, 8, 8, 8)

        central_widget.setLayout(main_grid_layout)

        # init status bar TODO: style status bar
        self.status_bar = self.statusBar()

        # init serial
        self.serial_thread: QThread | None = None
        self.serial_worker: SerialWorker | None = None
        self._serial_buffer = b""
        
        # data buffers for thrust plot
        self.time_buffer: deque[float] = deque(maxlen=2000)
        self.thrust_buffer: deque[float] = deque(maxlen=2000)

        self._t0: float | None = None

        # plot refresh timer
        self.plot_timer: QTimer = QTimer(self)
        self.plot_timer.setInterval(33)  # ~30 Hz
        self.plot_timer.timeout.connect(self.update_thrust_plot)


    def handle_packet(self, timestamp: float, raw_data: bytes) -> None:
        """
        Receives raw serial data from SerialWorker.
        Runs in GUI thread.
        """
        # TODO: add thrust plot integration
        self._serial_buffer += raw_data

        while b"\n" in self._serial_buffer:
            line, self._serial_buffer = self._serial_buffer.split(b"\n", 1)

            # TODO: protect against corrupted or incomplete packets
            # TODO: consider changing parsed into class, make it an attribute of Dashboard
            parsed: dict[str, str] = parsePacket(line)
            thrust: str = parsed["thrust"]

            # TODO: change from timestamp to elapsed time
            # log values for thrust plot
            self.thrust_buffer.append(float(thrust))
            self.time_buffer.append(timestamp)

            self.current_thrust_widget.setValue(f"{thrust}")


    def listPorts(self) -> None:
        '''Method used to list all of the available COM ports'''
        self.select_port_menu.clear()

        port_names: list[str] = [port.device for port in serial.tools.list_ports.comports()]
        
        for port_name in port_names:
            port_name_action: QAction = self.select_port_menu.addAction(port_name)
            port_name_action.setCheckable(True)
            port_name_action.setChecked(port_name == self.current_port)
            port_name_action.triggered.connect(lambda checked=False, p=port_name : self.setCurrentPort(p))


    def quit(self) -> None:
        '''Method used to close the window and kill the application'''
        self.app.quit()


    def setCurrentPort(self, port_name: str) -> None:
        '''Method used to change the serial port from which data are read'''
        self.current_port = port_name
        self.status_bar.showMessage(f"Connecting to {port_name}...")

        self._stop_serial_worker()
        self._start_serial_worker(port_name)


    def update_thrust_plot(self) -> None:
        if not self.time_buffer:
            return

        self.thrust_curve.setData(
            list(self.time_buffer),
            list(self.thrust_buffer)
        )


    def _start_serial_worker(self, port_name: str):

        self._stop_serial_worker()

        self.serial_thread = QThread()
        self.serial_worker = SerialWorker()

        self.serial_worker.moveToThread(self.serial_thread)

        # Proper Qt-safe startup
        self.serial_worker.set_port(port_name)
        self.serial_thread.started.connect(self.serial_worker.start)

        # Signals
        self.serial_worker.packet_received.connect(self.handle_packet)
        self.serial_worker.status_changed.connect(self.status_bar.showMessage)
        self.serial_worker.error.connect(self.status_bar.showMessage)

        self.serial_thread.start()
        self.plot_timer.start()


    def _stop_serial_worker(self):

        if self.serial_worker:
            self.serial_worker.stop()

        if self.serial_thread:
            self.serial_thread.quit()
            self.serial_thread.wait()

        self.serial_worker = None
        self.serial_thread = None
        self.plot_timer.stop()

# TODO: remove the names
class DashboardPanel(QWidget):
    '''Containers used to house different dashboard sections'''
    def __init__(self, name: str) -> None:
        super().__init__()
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.setObjectName(name)


    def createCommunicationsInfo(self) -> DashboardPanel:
        '''Creates the communications info panel and returns it'''
        result: DashboardPanel = DashboardPanel("Communications info")

        rssi: LabelValuePair = LabelValuePair("RSSI", "-100", "dBm")
        data_frequency: LabelValuePair = LabelValuePair("Data frequency", "80", "Hz")
        elapsed_time: LabelValuePair = LabelValuePair("Elapsed time", "1000", "s")

        # layout all of the readings vertically
        main_layout: QVBoxLayout = QVBoxLayout()
        main_layout.addWidget(rssi)
        main_layout.addWidget(data_frequency)
        main_layout.addWidget(elapsed_time)
        result.setLayout(main_layout)

        return result
    

    def createEngineInfo(self) -> DashboardPanel:
        '''Creates the engine info panel and returns it'''
        result: DashboardPanel = DashboardPanel("Engine info")

        # TODO: create graphic to display open/closed state of solenoid valves
        solenoid_valve_container: QWidget = QWidget()
        # TODO: create graphic to display engine temperature and flame being lit
        engine_container: QWidget = QWidget()
        burn_info_container: QWidget = QWidget()
        ignite_button: QPushButton = QPushButton("Ignite")

        # create burn_info_container subcomponents
        burn_time: LabelValuePair = LabelValuePair("Burn time", "3", "s")
        brightness: LabelValuePair = LabelValuePair("Brightness", "37", "%")

        # layout burn_info_container
        burn_info_layout: QHBoxLayout = QHBoxLayout()
        burn_info_layout.addWidget(burn_time)
        burn_info_layout.addWidget(brightness)
        burn_info_container.setLayout(burn_info_layout)


        # layout all components vertically
        main_layout: QVBoxLayout = QVBoxLayout()
        main_layout.addWidget(solenoid_valve_container)
        main_layout.addWidget(engine_container)
        main_layout.addWidget(burn_info_container)
        main_layout.addWidget(ignite_button)
        result.setLayout(main_layout)

        return result


    def createHydraulicsInfo(self) -> DashboardPanel:
        '''Creates the hydraulics info panel and returns it'''
        result: DashboardPanel = DashboardPanel("Hydraulics info")

        fuel_screw_plug: ValveControlWidget = ValveControlWidget("Fuel screw plug")
        oxidizer_tanking_valve: ValveControlWidget = ValveControlWidget("Oxidizer tanking valve")
        vessel_vent_valve: ValveControlWidget = ValveControlWidget("Vessel vent valve")
        hose_vent_valve: ValveControlWidget = ValveControlWidget("Hose vent valve")

        grid_layout: QGridLayout = QGridLayout()
        grid_layout.addWidget(fuel_screw_plug, 0, 0)
        grid_layout.addWidget(oxidizer_tanking_valve, 0, 1)
        grid_layout.addWidget(vessel_vent_valve, 1, 0)
        grid_layout.addWidget(hose_vent_valve, 1, 1)
        result.setLayout(grid_layout)

        return result


    def createTankInfo(self) -> DashboardPanel:
        '''Creates the tank info panel and returns it'''
        result: DashboardPanel = DashboardPanel("Tank info")

        fuel_pressure: LabelValuePair = LabelValuePair("Fuel pressure", "20", "bar")
        # TODO: add tank graphic to track piston position
        tank_container: QWidget = QWidget()
        oxidizer_pressure: LabelValuePair = LabelValuePair("Oxidizer pressure", "20", "bar")

        # layout all components vertically
        main_layout: QVBoxLayout = QVBoxLayout()
        main_layout.addWidget(fuel_pressure)
        main_layout.addWidget(tank_container)
        main_layout.addWidget(oxidizer_pressure)
        result.setLayout(main_layout)

        return result


    def createThrustInfo(self) -> DashboardPanel:
        '''Creates the thrust info panel and returns it'''
        result: DashboardPanel = DashboardPanel("Thrust info")

        self.current_thrust: LabelValuePair = LabelValuePair("Current thrust", "50", "N")
        self.max_thrust: LabelValuePair = LabelValuePair("Maximum thrust", "2000", "N")
        self.total_impulse: LabelValuePair = LabelValuePair("Total impulse", "3000", "Ns")

        # layout all of the readings vertically
        main_layout: QVBoxLayout = QVBoxLayout()
        main_layout.addWidget(self.current_thrust)
        main_layout.addWidget(self.max_thrust)
        main_layout.addWidget(self.total_impulse)
        result.setLayout(main_layout)

        return result
    
    # TODO: style graph, connect serial worker
    def createThrustPlot(self) -> DashboardPanel:
        '''Creates the thrust plot panel and returns it'''
        result: DashboardPanel = DashboardPanel("Thrust plot")

        self.graphWidget: pg.PlotWidget = pg.PlotWidget()

        self.graphWidget = pg.PlotWidget()
        self.thrust_curve = self.graphWidget.plot([], [])

        self.graphWidget.setLabel("left", "Thrust (N)")
        self.graphWidget.setLabel("bottom", "Time (s)")

        layout: QVBoxLayout = QVBoxLayout()
        layout.addWidget(self.graphWidget)
        result.setLayout(layout)

        return result
    

class SerialWorker(QObject):
    packet_received = Signal(float, bytes)
    status_changed = Signal(str)
    error = Signal(str)

    def __init__(self, baudrate: int = 115200):
        super().__init__()
        self._baudrate = baudrate
        self._serial = None
        self._port_name = None
        self._poll_timer = None

    def set_port(self, port_name: str):
        self._port_name = port_name

    @Slot()
    def start(self):
        try:
            self._serial = serial.Serial(
                self._port_name,
                self._baudrate,
                timeout=0  # non-blocking
            )

            self.status_changed.emit(f"Connected to {self._port_name}")

            # Create polling timer INSIDE worker thread
            self._poll_timer = QTimer(self)
            self._poll_timer.timeout.connect(self._poll_serial)
            self._poll_timer.start(5)  # 5 ms polling interval

        except Exception as e:
            self.error.emit(str(e))

    @Slot()
    def stop(self):
        if self._poll_timer:
            self._poll_timer.stop()

        if self._serial and self._serial.is_open:
            self._serial.close()

        self.status_changed.emit("Disconnected")

    @Slot()
    def _poll_serial(self):
        if not self._serial:
            return

        try:
            bytes_waiting = self._serial.in_waiting
            if bytes_waiting > 0:
                raw = self._serial.read(bytes_waiting)
                timestamp = time.monotonic()
                self.packet_received.emit(timestamp, raw)

        except Exception as e:
            self.error.emit(str(e))


class LabelValuePair(QWidget):
    '''Container for the commonly used label and value combination'''
    def __init__(self, label: str, value: str, unit: str):
        super().__init__()

        self.label: QLabel = QLabel(label)
        # container to organise the value and unit
        value_unit_container: QWidget = QWidget()
        value_unit_layout: QHBoxLayout = QHBoxLayout()

        self.value: QLabel = QLabel(value)
        self.unit: QLabel = QLabel(unit)

        # layout value and unit horizontally
        value_unit_layout.addWidget(self.value)
        value_unit_layout.addWidget(self.unit)
        value_unit_container.setLayout(value_unit_layout)

        # layout label and value-unit pair vertically
        main_layout: QVBoxLayout = QVBoxLayout()
        main_layout.addWidget(self.label)
        main_layout.addWidget(value_unit_container)
        self.setLayout(main_layout)


    def setValue(self, value: str) -> None:
        '''Set value of a specific LabelValuePair'''
        self.value.setText(value)


# TODO: actually open and close when pressed, add cooldown
class ValveControlWidget(QWidget):
    '''Container for the valve controls used for the hydraulics'''
    def __init__(self, name: str, isOpenInitial: bool = False):
        super().__init__()

        label: QLabel = QLabel(name)
        self.status: QLabel = QLabel()
        self.button: QPushButton = QPushButton()

        self.isOpen: bool = isOpenInitial
        self.updateWidgetVisuals()
        self.button.clicked.connect(self.changeState)

        main_layout: QVBoxLayout = QVBoxLayout()
        main_layout.addWidget(label)
        main_layout.addWidget(self.status)
        main_layout.addWidget(self.button)
        self.setLayout(main_layout)


    def changeState(self) -> None:
        '''Changes the state of the valve and widget'''
        if self.isOpen: self.isOpen = False
        else: self.isOpen = True

        self.updateWidgetVisuals()
        return None

    
    def updateWidgetVisuals(self) -> None:
        '''Updates the visuals of the widget to reflect the state of the valve'''
        if self.isOpen:
            self.status.setText("Currently open")
            self.button.setText("Click to close")
            return None
        
        self.status.setText("Currently closed")
        self.button.setText("Click to open")
        return None


def parsePacket(rawData: bytes) -> dict[str, str]:
    decoded_data: str = rawData.decode('utf-8')

    value_list: list[str] = list(decoded_data.split(';'))
    
    result: dict[str, str] = dict()

    # parse values according to predetermined order
    result["thrust"] = value_list[0]

    return result

if __name__ == '__main__':
    app: QApplication = QApplication()
    window: Dashboard = Dashboard(app)
    window.showMaximized()
    app.exec()