"""Base class shared by all dashboard section panels."""

from __future__ import annotations

from PySide6.QtWidgets import QSizePolicy, QWidget


class DashboardPanel(QWidget):
    """Common base for the different dashboard sections.

    Note: the original implementation had each panel build a *second*,
    throwaway `DashboardPanel` instance inside a `create...()` method and
    return that as the actual widget, while attributes were left dangling
    on the first instance. Every panel is now a single self-contained
    QWidget subclass: it builds its own layout in `__init__` and exposes
    its child widgets directly as attributes, with no duplicate instance.
    """

    def __init__(self, name: str, parent: QWidget | None = None) -> None:
        super().__init__(parent)
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.setObjectName(name)
