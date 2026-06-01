// Implements a basic cyclic redundancy check (CRC-16/CCITT-FALSE)
#ifndef CRC_H
#define CRC_H

#include <cstddef>
#include <cstdint>

namespace crc {
    std::uint16_t compute(const std::uint8_t* data, std::size_t len); // Computes the crc value for the given data buffer
    bool verify(const std::uint8_t* data, std::size_t len, std::uint16_t expected); // Verifies the crc value for the given data buffer
}

#endif
