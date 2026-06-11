// Integrates the buzzer
#ifndef BUZZER_H
#define BUZZER_H

namespace buzzer {
    bool begin(); // Initializes the buzzer, returns true if successful
    void sendPulse(); // Produces a short sound burst
}

#endif
