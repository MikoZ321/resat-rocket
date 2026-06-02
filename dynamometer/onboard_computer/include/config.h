// Includes all of the configured values for the onboard computer program
#ifndef CONFIG_H
#define CONFIG_H

#define TICK_FAST_HZ 50
#define TICK_FAST_INTERVAL_MS 20
#define TICK_SLOW_DIVISOR 2 // The ratio of the frequency of the fast tick to that of the slow tick
#define TICK_HOUSE_DIVISOR 10 // The ratio of the frequency of the fast tick to that of the house tick

#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 9
#define I2C_FREQUENCY_HZ 400000
#define I2C_TIMEOUT_MS 3

#define LSM6DSOX_I2C_ADDRESS 0x6A // May actually be 107 if SDO pin is high
#define H3LIS331_I2C_ADDRESS 0x18 // May actually be 0x19 if SA0 pin is high

#endif
