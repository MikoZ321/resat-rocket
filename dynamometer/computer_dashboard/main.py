"""Entry point for the Dynamometer Dashboard application."""

import sys

from PySide6.QtWidgets import QApplication

from dashboard import Dashboard


def main() -> None:
    app = QApplication(sys.argv)
    window = Dashboard(app)
    window.showMaximized()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
