// Integrates the W25Q64JWSSIQ SPI flash memory module
#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#include <cstdint>

#include "shared/types.h"

namespace spi_flash {
    bool begin(); // Initializes the flash memory, returns true if successful
    void eraseFullMemory(); // Erases the full chip memory, do not call midair
    void writeFullTelemetryFrame(const full_telemetry_frame_t& full_frame); // Writes a full telemetry frame to the flash memory
    void writeMiniTelemetryFrame(const mini_telemetry_frame_t& mini_frame); // Writes a mini telemetry frame to the flash memory
    void periodicMaintenance(std::uint32_t slow_tick_number); // Performs the appropriate periodic maintenance based on the current slow tick number
    void dumpToSerial(); // Dumps the entire chip memory to serial, do not call midair
}

#endif
