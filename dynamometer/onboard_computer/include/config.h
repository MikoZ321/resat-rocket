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
#define BMP581_I2C_ADDRESS 0x46 // May actually be 0x47 depending on the state of the SDO pin

#define ADS1232_DOUT_PIN 21
#define ADS1232_SCLK_PIN 15
#define ADS1232_PWDN_PIN 47
#define THRUST_LOADCELL_SCALE 1000.0 // Placeholder value, to be determined experimentally
#define THRUST_LOADCELL_OFFSET 0.0 // Placeholder value, to be determined experimentally

#endif
