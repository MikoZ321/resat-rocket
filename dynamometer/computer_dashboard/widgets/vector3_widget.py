"""Compact XYZ vector display, e.g. for accelerometer/gyro readings."""

from __future__ import annotations

from PySide6.QtWidgets import QHBoxLayout, QLabel, QVBoxLayout, QWidget

from widgets.label_value_pair import LabelValuePair


class Vector3Widget(QWidget):
    """Displays a labeled 3-axis vector (x, y, z) sharing one unit."""

    def __init__(self, title: str, unit: str, parent: QWidget | None = None) -> None:
        super().__init__(parent)

        title_label = QLabel(title)

        self.x: LabelValuePair = LabelValuePair("X", "0.00", unit)
        self.y: LabelValuePair = LabelValuePair("Y", "0.00", unit)
        self.z: LabelValuePair = LabelValuePair("Z", "0.00", unit)

        axes_layout = QHBoxLayout()
        axes_layout.setContentsMargins(0, 0, 0, 0)
        axes_layout.addWidget(self.x)
        axes_layout.addWidget(self.y)
        axes_layout.addWidget(self.z)

        axes_container = QWidget()
        axes_container.setLayout(axes_layout)

        main_layout = QVBoxLayout()
        main_layout.addWidget(title_label)
        main_layout.addWidget(axes_container)
        self.setLayout(main_layout)

    def setValue(self, vector: list[float] | tuple[float, float, float]) -> None:
        """Updates all three axes from a length-3 sequence [x, y, z]."""
        self.x.setValue(f"{vector[0]:.2f}")
        self.y.setValue(f"{vector[1]:.2f}")
        self.z.setValue(f"{vector[2]:.2f}")
