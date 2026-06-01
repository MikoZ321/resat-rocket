#include "crc.h"

namespace crc {
    // CRC-16/CCITT-FALSE  poly=0x1021  init=0xFFFF
    std::uint16_t compute(const std::uint8_t* data, std::size_t len) {
        std::uint16_t crc = 0xFFFF;
        for (std::size_t i = 0; i < len; i++) {
            crc ^= (std::uint16_t)data[i] << 8;
            for (int b = 0; b < 8; b++)
                crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
        }
        return crc; 
    }

    bool verify(const std::uint8_t* data, std::size_t len, std::uint16_t expected) {
        return compute(data, len) == expected;
    }
}