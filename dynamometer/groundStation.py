from PySide6.QtWidgets import QMainWindow, QApplication, QMenuBar, QMenu
from PySide6.QtGui import QAction
import serial.tools.list_ports

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


if __name__ == '__main__':
    app: QApplication = QApplication()
    window: Dashboard = Dashboard(app)
    window.showMaximized()
    app.exec()