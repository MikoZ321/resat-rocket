from PySide6.QtWidgets import QMainWindow, QApplication

# TODO: read the incoming data from a serial connection
# TODO: save the information in a .csv file

# TODO: display the readings in a GUI
class Dashboard(QMainWindow):
    '''Serves as the class container for the entire GUI'''
    def __init__(self, app) -> None:
        super().__init__()
        self.app = app
        self.setWindowTitle("Dynamometer Dashboard")

        # init menu bar
        menu_bar = self.menuBar()

        file_menu = menu_bar.addMenu("&File")
        file_menu.addAction("Save")
        file_menu.addAction("Save as ...")
        quit_action = file_menu.addAction("Quit")
        quit_action.triggered.connect(self.quit)

        settings_menu = menu_bar.addMenu("Settings")
        settings_menu.addAction("Select port")

    def quit(self) -> None:
        '''Method used to close the window and kill the application'''
        self.app.quit()


if __name__ == '__main__':
    app = QApplication()
    window = Dashboard(app)
    window.showMaximized()
    app.exec()