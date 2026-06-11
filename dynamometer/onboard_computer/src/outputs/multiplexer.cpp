#include "multiplexer.h"

#include <cstdint>
#include <MCP23017.h>

#include "config.h"

MCP23017 mcp(MCP23017_I2C_ADDRESS);

static std::uint8_t s_fuel_solenoid_state, s_oxidizer_solenoid_state;

namespace multiplexer {
    bool begin() {
        if (!mcp.begin()) return false;

        // Set all pins to output and set to low
        mcp.pinMode8(SOLENOID_FUEL_PIN, 0x00);
        mcp.write1(SOLENOID_FUEL_PIN, LOW);
        s_fuel_solenoid_state = 0;
        mcp.pinMode8(SOLENOID_OXIDIZER_PIN, 0x00);
        mcp.write1(SOLENOID_OXIDIZER_PIN, LOW);
        s_oxidizer_solenoid_state = 0;
        mcp.pinMode8(IGNITER_MAIN_PIN, 0x00);
        mcp.write1(IGNITER_MAIN_PIN, LOW);
        mcp.pinMode8(IGNITER_DROGUE_PIN, 0x00);
        mcp.write1(IGNITER_DROGUE_PIN, LOW);
        mcp.pinMode8(IGNITER_ENGINE_PIN, 0x00);
        mcp.write1(IGNITER_ENGINE_PIN, LOW);
        
        return true;
    }

    void igniteEngine() {
        mcp.write1(IGNITER_ENGINE_PIN, HIGH);
    }

    void releaseMainParachute() {
        mcp.write1(IGNITER_MAIN_PIN, HIGH);
    }

    void releaseDrogueParachute() {
        mcp.write1(IGNITER_MAIN_PIN, HIGH);
    }

    void toggleFuelSolenoid() {
        s_fuel_solenoid_state = (s_fuel_solenoid_state + 1) % 2;
        mcp.write1(SOLENOID_FUEL_PIN, s_fuel_solenoid_state);
    }

    void toggleOxididzerSolenoid() {
        s_oxidizer_solenoid_state = (s_oxidizer_solenoid_state + 1) % 2;
        mcp.write1(SOLENOID_OXIDIZER_PIN, s_oxidizer_solenoid_state);
    }
}
