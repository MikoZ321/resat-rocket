// Handles the command parsing, validation, and execution logic
#ifndef COMMAND_H
#define COMMAND_H

#include <cstdint>

namespace command {
    void begin();
    void executeOne(); // Executes one command
    std::uint16_t getCommandFrameIndex(); // Returns the current command frame index
    void onByteReceived(std::uint8_t byte);
}

#endif
