#include "main_battery_level_sensor.h"

#include "config.h"
#include "core/state.h"

#include <Arduino.h>
#include <cstdint>
#include <Wire.h>

// ── Constants ─────────────────────────────────────────────────────────────────

// Control register value:
// ADC automatic mode (bits 7:6 = 11)
// Prescaler M=1    (bits 5:3 = 000)
// ALCC disabled    (bits 2:1 = 00)
// Not shutdown     (bit 0    = 0)
static constexpr std::uint8_t CONTROL_VALUE   = 0xC0;

// Sense resistor in ohms — match your PCB
static constexpr float SENSE_RESISTOR_OHM = 0.050f;

// Prescaler M value — must match CONTROL_VALUE bits 5:3
static constexpr float PRESCALER_M = 1.0f;

// Full battery charge count — written to 0x02:0x03 on first init
// 0xFFFF means full, 0x0000 means empty
// Start at 0xFFFF assuming a fully charged battery before flight
static constexpr std::uint16_t FULL_CHARGE_COUNT = 0xFFFF;

// Charge per LSB in mAh
// Q_LSB = (0.085 * M) / R_SENSE
static constexpr float Q_LSB_MAH = (0.085f * PRESCALER_M) / SENSE_RESISTOR_OHM;

// ── State ─────────────────────────────────────────────────────────────────────

static float   s_charge_pct  = 100.0f;
static float   s_charge_mah  = 0.0f;
static bool    s_overflow    = false;
static bool    s_initialised = false;

// ── Internal helpers ──────────────────────────────────────────────────────────

// Write a single byte to a register
// Returns true on success
static bool writeRegister(std::uint8_t reg, std::uint8_t value) {
    Wire.beginTransmission(MAIN_BATTERY_LEVEL_SENSOR_I2C_ADDRESS);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
    // endTransmission returns 0 on success, non-zero on failure
    // 1 = data too long for buffer
    // 2 = NACK on address
    // 3 = NACK on data
    // 4 = other error
}

// Read a single byte from a register
// Returns true on success, writes value into out
static bool readRegister(std::uint8_t reg, std::uint8_t& out) {
    Wire.beginTransmission(MAIN_BATTERY_LEVEL_SENSOR_I2C_ADDRESS);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;
    // false = repeated start — keeps bus active for the read
    // This is required for register read operations on LTC2944

    std::uint8_t received = Wire.requestFrom(MAIN_BATTERY_LEVEL_SENSOR_I2C_ADDRESS, 1);
    if (received != 1) return false;
    out = Wire.read();
    return true;
}

// Read two consecutive bytes as a big-endian uint16_t
// Returns true on success
static bool readRegister16(std::uint8_t reg, std::uint16_t& out) {
    Wire.beginTransmission(MAIN_BATTERY_LEVEL_SENSOR_I2C_ADDRESS);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;

    std::uint8_t received = Wire.requestFrom(MAIN_BATTERY_LEVEL_SENSOR_I2C_ADDRESS, 2);
    if (received != 2) return false;

    std::uint8_t msb = Wire.read();
    std::uint8_t lsb = Wire.read();
    out = ((std::uint16_t)msb << 8) | lsb;
    return true;
}

// Write a 16-bit value to two consecutive registers (MSB first)
static bool writeRegister16(std::uint8_t reg, std::uint16_t value) {
    Wire.beginTransmission(MAIN_BATTERY_LEVEL_SENSOR_I2C_ADDRESS);
    Wire.write(reg);
    Wire.write((std::uint8_t)(value >> 8));    // MSB
    Wire.write((std::uint8_t)(value & 0xFF));  // LSB
    return Wire.endTransmission() == 0;
}

// ── Public interface ──────────────────────────────────────────────────────────

bool main_battery_level_sensor::begin() {
    s_initialised = false;

    // ── Step 1: Verify the chip responds at its I2C address ───────────────────
    // The simplest check is to read the status register and confirm
    // we get a response — any non-timeout response means the chip is present
    std::uint8_t status = 0;
    if (!readRegister(0x00, status)) {
        Serial.println("[Battery] LTC2944 not found at address 0x64");
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }

    Serial.print("[Battery] LTC2944 found. Status register: 0x");
    Serial.println(status, HEX);

    // ── Step 2: Check for existing overflow condition ─────────────────────────
    if (status & 0x01) {
        // CA_OVFL bit set — charge counter has overflowed
        // This should not happen on a fresh power-on
        // Log it but do not fail init — reset the counter below
        Serial.println("[Battery] WARNING: Charge counter overflow detected. Resetting.");
    }

    // ── Step 3: Write control register ───────────────────────────────────────
    if (!writeRegister(0x01, CONTROL_VALUE)) {
        Serial.println("[Battery] Failed to write control register");
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }

    // ── Step 4: Verify control register was written correctly ─────────────────
    uint8_t controlReadback = 0;
    if (!readRegister(0x01, controlReadback)) {
        Serial.println("[Battery] Failed to read back control register");
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }
    if (controlReadback != CONTROL_VALUE) {
        Serial.print("[Battery] Control register mismatch. Wrote: 0x");
        Serial.print(CONTROL_VALUE, HEX);
        Serial.print(" Read: 0x");
        Serial.println(controlReadback, HEX);
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }

    // ── Step 5: Set accumulated charge register to full ───────────────────────
    // This assumes the battery is fully charged at init time
    // If you want to resume tracking across power cycles, read from
    // flash config instead of writing FULL_CHARGE_COUNT here
    if (!writeRegister16(0x02, FULL_CHARGE_COUNT)) {
        Serial.println("[Battery] Failed to write charge register");
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }

    // ── Step 6: Verify charge register was written ────────────────────────────
    std::uint16_t chargeReadback = 0;
    if (!readRegister16(0x02, chargeReadback)) {
        Serial.println("[Battery] Failed to read back charge register");
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }
    if (chargeReadback != FULL_CHARGE_COUNT) {
        Serial.print("[Battery] Charge register mismatch. Wrote: 0x");
        Serial.print(FULL_CHARGE_COUNT, HEX);
        Serial.print(" Read: 0x");
        Serial.println(chargeReadback, HEX);
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }

    s_initialised = true;
    state::setValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
    Serial.println("[Battery] LTC2944 initialised successfully.");
    return true;
}

bool main_battery_level_sensor::readSensorData() {
    if (!s_initialised) {
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }

    // ── Step 1: Read and check status register ────────────────────────────────
    std::uint8_t status = 0;
    if (!readRegister(0x00, status)) {
        Serial.println("[Battery] Failed to read status register");
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }

    // Check CA_OVFL — counter wrapped around, reading is invalid
    if (status & 0x01) {
        s_overflow = true;
        Serial.println("[Battery] Charge counter overflow — reading invalid");
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }
    s_overflow = false;

    // ── Step 2: Read accumulated charge register ──────────────────────────────
    std::uint16_t rawCharge = 0;
    if (!readRegister16(0x02, rawCharge)) {
        Serial.println("[Battery] Failed to read charge register");
        state::clearValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
        return false;
    }

    // ── Step 3: Convert to mAh and percentage ────────────────────────────────
    // rawCharge counts down from FULL_CHARGE_COUNT as battery discharges
    // Remaining charge in mAh = rawCharge * Q_LSB
    // Full charge in mAh = FULL_CHARGE_COUNT * Q_LSB
    s_charge_mah = (float)rawCharge * Q_LSB_MAH;
    float fullCharge_mah = (float)FULL_CHARGE_COUNT * Q_LSB_MAH;
    s_charge_pct = (s_charge_mah / fullCharge_mah) * 100.0f;

    // Clamp to 0–100 range in case of minor register noise
    if (s_charge_pct > 100.0f) s_charge_pct = 100.0f;
    if (s_charge_pct < 0.0f)   s_charge_pct = 0.0f;

    state::setValidMaskBit(MAIN_BATTERY_LEVEL_VALID_MASK_BIT);
    return true;
}

void main_battery_level_sensor::fill(float& main_battery_level) {
    main_battery_level = s_charge_pct;
}
