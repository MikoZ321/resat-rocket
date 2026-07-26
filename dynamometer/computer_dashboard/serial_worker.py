"""Background worker that owns the serial port and runs on its own QThread."""

from __future__ import annotations

import serial
from PySide6.QtCore import QObject, QTimer, Signal, Slot

from constants import DEFAULT_BAUDRATE, SERIAL_POLL_INTERVAL_MS


class SerialWorker(QObject):
    packet_received = Signal(bytes)
    status_changed = Signal(str)
    error = Signal(str)

    def __init__(self, baudrate: int = DEFAULT_BAUDRATE) -> None:
        super().__init__()
        self._baudrate = baudrate
        self._serial: serial.Serial | None = None
        self._port_name: str | None = None
        self._poll_timer: QTimer | None = None

    def setPort(self, port_name: str) -> None:
        self._port_name = port_name

    @Slot()
    def start(self) -> None:
        try:
            self._serial = serial.Serial(
                self._port_name,
                self._baudrate,
                timeout=0,  # non-blocking
            )

            self.status_changed.emit(f"Connected to {self._port_name}")

            # Timer must be created inside the worker thread so it fires there.
            self._poll_timer = QTimer(self)
            self._poll_timer.timeout.connect(self._pollSerial)
            self._poll_timer.start(SERIAL_POLL_INTERVAL_MS)

        except Exception as e:
            self.error.emit(str(e))

    @Slot()
    def stop(self) -> None:
        if self._poll_timer:
            self._poll_timer.stop()

        if self._serial and self._serial.is_open:
            self._serial.close()

        self.status_changed.emit("Disconnected")

    @Slot(bytes)
    def sendCommand(self, packet: bytes) -> None:
        if self._serial and self._serial.is_open:
            try:
                self._serial.write(packet)
                self._serial.flush()
            except Exception as e:
                self.error.emit(str(e))

    @Slot()
    def _pollSerial(self) -> None:
        if not self._serial:
            return

        try:
            bytes_waiting = self._serial.in_waiting
            if bytes_waiting > 0:
                raw = self._serial.read(bytes_waiting)
                self.packet_received.emit(raw)
        except Exception as e:
            self.error.emit(str(e))
