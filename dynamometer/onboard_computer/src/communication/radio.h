// Integrates radio communication via the RFD868
#ifndef RADIO_H
#define RADIO_H

#include "shared/types.h"

namespace radio {
    bool begin(); // Initializes the transparent serial connection, returns true if successful
    void enqueueFullTelemetryFrame(const full_telemetry_frame_t& full_frame); // Pushes a full telemetry frame to the UART buffer
}

#endif