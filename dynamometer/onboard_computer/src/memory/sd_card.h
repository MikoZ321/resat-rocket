// Integrates the onboard micro-SD card
#ifndef SD_CARD_H
#define SD_CARD_H

#include "shared/types.h"

namespace sd_card {
    bool begin(); // Initializes the SD card, call after SPI init
    void writeFullTelemetryFrame(const full_telemetry_frame_t& full_frame); // Writes a full telemetry frame into the buffer
    void shutdown(); // Writes any partial buffers to memory, do not call midair
}

#endif
