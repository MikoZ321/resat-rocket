from PySide6.QtWidgets import QMainWindow, QApplication, QMenuBar, QMenu, QGridLayout, QWidget, QPushButton, QSizePolicy, QVBoxLayout, QHBoxLayout, QLabel
from PySide6.QtGui import QAction
import serial.tools.list_ports
import pyqtgraph as pg
from collections import deque

# TODO: read the incoming data from a serial connection
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
        thrust_plot_container = DashboardPanel("Thrust plot").createThrustPlot()
        thrust_info_container = DashboardPanel("Thrust info").createThrustInfo()
        engine_info_container = DashboardPanel("Engine")
        communication_info_container = DashboardPanel("Comms")
        hydraulics_info_container = DashboardPanel("Hydraulics")
        tank_info_container = DashboardPanel("Tank")

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


    def listPorts(self) -> None:
        '''Method used to list all of the available COM ports'''
        self.select_port_menu.clear()

        port_names: list[str] = [port.device for port in serial.tools.list_ports.comports()]

        print([port for port in serial.tools.list_ports.comports()])
        
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
        self.status_bar.showMessage(f"Port set to {port_name}")

# TODO: remove the names
class DashboardPanel(QWidget):
    '''Containers used to house different dashboard sections'''
    def __init__(self, name: str) -> None:
        super().__init__()
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.setObjectName(name)


    def createThrustInfo(self) -> DashboardPanel:
        '''Creates the thrust info panel and returns it'''
        result: DashboardPanel = DashboardPanel("Thrust info")

        current_thrust: LabelValuePair = LabelValuePair("Current thrust", "50", "N")
        max_thrust: LabelValuePair = LabelValuePair("Maximum thrust", "2000", "N")
        total_impulse: LabelValuePair = LabelValuePair("Total impulse", "3000", "Ns")

        # layout all of the readings vertically
        main_layout: QVBoxLayout = QVBoxLayout()
        main_layout.addWidget(current_thrust)
        main_layout.addWidget(max_thrust)
        main_layout.addWidget(total_impulse)
        result.setLayout(main_layout)

        return result
    

    # TODO: style graph, connect serial worker
    def createThrustPlot(self) -> DashboardPanel:
        '''Creates the thrust plot panel and returns it'''
        result: DashboardPanel = DashboardPanel("Thrust plot")

        self.graphWidget: pg.PlotWidget = pg.PlotWidget()

        # mock data used for testing 
        times: deque[float] = deque([1, 2, 3, 4, 5])
        thrusts: deque[float] = deque([4, 50, 2000, 32, 500])

        self.graphWidget.plot(times, thrusts)
        self.graphWidget.setLabel("left", "Thrust (N)")
        self.graphWidget.setLabel("bottom", "Time (s)")

        layout: QVBoxLayout = QVBoxLayout()
        layout.addWidget(self.graphWidget)
        result.setLayout(layout)

        return result
    
    

class LabelValuePair(QWidget):
    '''Container for the commonly used label and value combination'''
    def __init__(self, label: str, value: str, unit: str):
        super().__init__()

        self.label: QLabel = QLabel(label)
        # container to organise the value and unit
        value_unit_container: QWidget = QWidget()
        value_unit_layout: QHBoxLayout = QHBoxLayout()

        self.value: QLabel = QLabel(value)
        self.unit:QLabel = QLabel(unit)

        # layout value and unit horizontally
        value_unit_layout.addWidget(self.value)
        value_unit_layout.addWidget(self.unit)
        value_unit_container.setLayout(value_unit_layout)

        # layout label and value-unit pair vertically
        main_layout: QVBoxLayout = QVBoxLayout()
        main_layout.addWidget(self.label)
        main_layout.addWidget(value_unit_container)
        self.setLayout(main_layout)


if __name__ == '__main__':
    app: QApplication = QApplication()
    window: Dashboard = Dashboard(app)
    window.showMaximized()
    app.exec()