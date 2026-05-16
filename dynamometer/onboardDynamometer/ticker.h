// Implements the master tick using a hardware timer
#ifndef TICKER_H
#define TICKER_H

#include <cstdint>

namespace ticker {
    void begin(); // Initializes the hardware ticker
    bool consume(); // Returns true once per tick, clears flag
    std::uint32_t getTickCount(); // Returns the current tick count
    std::uint32_t getMissedTickCount(); // Returns the current missed tick count
}
#endif
