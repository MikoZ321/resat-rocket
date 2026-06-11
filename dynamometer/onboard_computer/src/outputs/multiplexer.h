// Integrates the MCP23017 multiplexer
#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

namespace multiplexer {
    bool begin(); // Initializes the multiplexer, returns true if successful
    void igniteEngine();
    void releaseMainParachute();
    void releaseDrogueParachute();
    void toggleFuelSolenoid();
    void toggleFuelSolenoid();
}

#endif