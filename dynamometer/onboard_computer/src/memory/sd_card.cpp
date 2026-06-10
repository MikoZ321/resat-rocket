#include "sd_card.h"

#include <cstdint>
#include <cstddef>
#include <SdFat.h>

#include "config.h"

static SdFat s_sd;
static FatFile s_file;
static bool s_is_ok = false;
static std::uint32_t s_written_frame_count = 0;

static std::uint8_t s_buffer[SD_CARD_BUFFER_SIZE];
static std::uint16_t s_buffer_position = 0;

// Pre-allocate 50 MB to avoid FAT fragmentation mid-flight
static const std::uint32_t PREALLOCATE_SIZE = 50UL * 1024UL * 1024UL;

static bool flushSector() {
    if (s_buffer_position == 0) return true;

    std::size_t written_byte_count = s_file.write(s_buffer, s_buffer_position);
    if (written_byte_count != s_buffer_position) {
        s_is_ok = false;
        return false;
    }

    s_buffer_position = 0;
    return true;
}

namespace sd_card {
    bool begin() {
        s_is_ok = false;

        if (!s_sd.begin(SD_CARD_CS_PIN, SD_SCK_MHZ(25))) return false;

        // Build filename from compile timestamp to avoid overwriting
        char filename[32];
        snprintf(filename, sizeof(filename), "FLIGHT_%lu.bin", (unsigned long)millis());

        if (!s_file.open(filename, O_RDWR | O_CREAT | O_TRUNC)) return false;

        // pre-allocate to avoid fat updates mid-flight
        s_file.preAllocate(PREALLOCATE_SIZE);

        // Write session header (16 bytes)
        std::uint8_t header[16] = {};
        std::uint32_t magic = 0xDEADF1A5UL;
        std::uint32_t timestamp = (std::uint32_t)millis();
        memcpy(header, &magic, 4);
        memcpy(header + 4, &timestamp, 4);
        header[8] = 1; // fw_version
        header[9] = 1; // frame_fmt_version
        s_file.write(header, 16);

        s_is_ok = true;
        return true;
    }

    void writeFullTelemetryFrame(const full_telemetry_frame_t& full_frame) {
        if (!s_is_ok) return;

        const std::uint8_t* src  = (const std::uint8_t*)&full_frame;
        std::size_t remaining_byte_count = sizeof(full_telemetry_frame_t);

        while (remaining_byte_count > 0) {
            std::size_t available_buffer_space = SD_CARD_BUFFER_SIZE - s_buffer_position;
            std::size_t chunk_size = remaining_byte_count < available_buffer_space ? remaining_byte_count : available_buffer_space;
            memcpy(s_buffer + s_buffer_position, src, chunk_size);
            s_buffer_position += (std::uint16_t) chunk_size;
            src += chunk_size;
            remaining_byte_count -= chunk_size;

            if (s_buffer_position == SD_CARD_BUFFER_SIZE && !flushSector()) return;
        }
        s_written_frame_count++;
    }

    void shutdown() {
        if (!s_is_ok) return;

        flushSector();
        s_file.truncate();   // release unused pre-allocated space
        s_file.sync();
        s_file.close();
        s_is_ok = false;
    }
}