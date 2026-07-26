"""Toggle-style control representing a single hydraulics valve."""

from __future__ import annotations

from PySide6.QtWidgets import QLabel, QPushButton, QVBoxLayout, QWidget

# TODO: actually open and close when pressed (wire up to a command), add cooldown


class ValveControlWidget(QWidget):
    """Container for the valve controls used for the hydraulics."""

    def __init__(self, name: str, is_open_initial: bool = False, parent: QWidget | None = None) -> None:
        super().__init__(parent)

        label = QLabel(name)
        self.status = QLabel()
        self.button = QPushButton()

        self.is_open = is_open_initial
        self._updateWidgetVisuals()
        self.button.clicked.connect(self._changeState)

        main_layout = QVBoxLayout()
        main_layout.addWidget(label)
        main_layout.addWidget(self.status)
        main_layout.addWidget(self.button)
        self.setLayout(main_layout)

    def _changeState(self) -> None:
        """Toggles the valve state and refreshes the widget."""
        self.is_open = not self.is_open
        self._updateWidgetVisuals()

    def _updateWidgetVisuals(self) -> None:
        """Updates the visuals of the widget to reflect the state of the valve."""
        if self.is_open:
            self.status.setText("Currently open")
            self.button.setText("Click to close")
        else:
            self.status.setText("Currently closed")
            self.button.setText("Click to open")
