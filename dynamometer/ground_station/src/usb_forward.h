// Manages the forwarding of the received data to the PC connected via USB
#ifndef USB_FORWARD_H
#define USB_FORWARD_H

#include <cstdint>

#include "shared/types.h"

namespace usb_forward {
    bool begin(); // Initializes the connection, returns true if successful
    void sendFullFrame(const full_telemetry_frame_t& full_frame, std::uint32_t frames_received_count, std::uint32_t crc_fail_count, std::uint32_t frame_index_gap_count); // Forwards a full frame along with the collected metadata
}

#endif