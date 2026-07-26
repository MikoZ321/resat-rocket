"""A small labeled reading widget: a title label above a value/unit pair."""

from __future__ import annotations

from PySide6.QtWidgets import QHBoxLayout, QLabel, QVBoxLayout, QWidget


class LabelValuePair(QWidget):
    """Container for the commonly used label and value combination."""

    def __init__(self, label: str, value: str, unit: str, parent: QWidget | None = None) -> None:
        super().__init__(parent)

        self.label = QLabel(label)
        self.value = QLabel(value)
        self.unit = QLabel(unit)

        value_unit_layout = QHBoxLayout()
        value_unit_layout.setContentsMargins(0, 0, 0, 0)
        value_unit_layout.addWidget(self.value)
        value_unit_layout.addWidget(self.unit)

        value_unit_container = QWidget()
        value_unit_container.setLayout(value_unit_layout)

        main_layout = QVBoxLayout()
        main_layout.addWidget(self.label)
        main_layout.addWidget(value_unit_container)
        self.setLayout(main_layout)

    def setValue(self, value: str) -> None:
        """Set the displayed value."""
        self.value.setText(value)
