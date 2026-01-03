from PySide6.QtWidgets import QMainWindow, QApplication

# TODO: read the incoming data from a serial connection
# TODO: save the information in a .csv file

# TODO: display the readings in a GUI
class Dashboard(QMainWindow):
    def __init__(self, app) -> None:
        super().__init__()
        self.app = app
        self.setWindowTitle("Dynamometer Dashboard")

        # init menu bar
        menu_bar = self.menuBar()

        file_menu = menu_bar.addMenu("&File")
        file_menu.addAction("Save")
        file_menu.addAction("Save as ...")
        file_menu.addAction("Quit")

        settings_menu = menu_bar.addMenu("Settings")
        settings_menu.addAction("Select port")


if __name__ == '__main__':
    app = QApplication()
    window = Dashboard(app)
    window.showMaximized()
    app.exec()