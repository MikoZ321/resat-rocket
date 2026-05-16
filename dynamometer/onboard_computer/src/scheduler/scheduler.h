#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cstdint>

namespace scheduler {
    void begin(); // Initializes the scheduler and all the peripherals
    void runTick(std::uint32_t tickNumber); // Runs the appropriate tick (fast/slow/house)
}

#endif
