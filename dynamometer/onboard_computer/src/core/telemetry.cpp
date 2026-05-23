#include "telemetry.h"

#include <Arduino.h>

#include "types.h"

static full_telemetry_frame_t s_full_frame;
static mini_telemetry_frame_t s_mini_frame;
static std::uint16_t s_current_frame_index = 0;

namespace telemetry {
    void assembleFullFrame() {
        s_full_frame.sync_bytes[0] = 0xAA;
        s_full_frame.sync_bytes[1] = 0x55;

        s_full_frame.telemetry_frame_index = s_current_frame_index;
        s_current_frame_index++;

        s_full_frame.timestamp_ms = millis();

        // TODO: Fill Tier A sensor readings
        // TODO: Fill Tier B sensor readings
        // TODO: Fill Tier C sensor readings

        // TODO: Fill is_valid_reading_mask
        // TODO: Set crc
    }

    void assembleMiniFrame() {
        s_mini_frame.sync_bytes[0] = 0xBB;
        s_mini_frame.sync_bytes[1] = 0x66;

        s_mini_frame.telemetry_frame_index = s_current_frame_index;
        s_current_frame_index++;

        s_mini_frame.timestamp_ms = millis();

        // TODO: Fill Tier A sensor readings

        // TODO: Fill is_valid_reading_mask
        // TODO: Set crc
    }

    const full_telemetry_frame_t& getFullFrame() {
        return s_full_frame;
    }

    const mini_telemetry_frame_t& getMiniFrame() {
        return s_mini_frame;
    }
}
