#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "shared/types.h"

namespace telemetry {
    void assembleFullFrame(); // Assemble a full telemetry frame from the current sensor readings
    void assembleMiniFrame(); // Assemble a mini telemetry frame from the current sensor readings
    const full_telemetry_frame_t& getFullFrame(); // Get the current full telemetry frame
    const mini_telemetry_frame_t& getMiniFrame(); // Get the current mini telemetry frame
}

#endif
