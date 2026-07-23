#ifndef COMMAND_RELAY_H
#define COMMAND_RELAY_H

#include <cstdint>

namespace command_relay {
    bool begin(); // Initializes the command relay, returns true if successful
    void process(); // Call in loop() to process incoming commands and forward them
}

#endif
