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

#define SPI_MISO_PIN 12
#define SPI_MOSI_PIN 11
#define SPI_SCLK_PIN 13

#define LSM6DSOX_I2C_ADDRESS 0x6B
#define H3LIS331_I2C_ADDRESS 0x18
#define BMP581_I2C_ADDRESS 0x47

#define ADS1232_DOUT_PIN 21
#define ADS1232_SCLK_PIN 15
#define ADS1232_PWDN_PIN 47

#define THRUST_LOADCELL_SCALE 1000.0 // Placeholder value, to be determined experimentally
#define THRUST_LOADCELL_OFFSET 0.0 // Placeholder value, to be determined experimentally

#define OXIDIZER_LOADCELL_SCALE 1000.0 // Placeholder value, to be determined experimentally
#define OXIDIZER_LOADCELL_OFFSET 0.0 // Placeholder value, to be determined

#define MAIN_ADS1115_I2C_ADDRESS 0x48 // May actually be 0x49, 0x4A, or 0x4B depending on the state of the address pins

#define MAIN_BATTERY_VOLTAGE_ADC_CHANNEL 0
#define FUEL_PRESSURE_ADC_CHANNEL 1
#define OXIDIZER_PRESSURE_ADC_CHANNEL 2
#define PYRO_BATTERY_VOLTAGE_ADC_CHANNEL 3

#define MAIN_BATTERY_VOLTAGE_SCALE 0.01 // Placeholder value, to be determined experimentally
#define MAIN_BATTERY_VOLTAGE_OFFSET 0.0 // Placeholder value, to be determined experimentally
#define FUEL_PRESSURE_SCALE 1.0 // Placeholder value, to be determined experimentally
#define FUEL_PRESSURE_OFFSET 0.0 // Placeholder value, to be determined experimentally
#define OXIDIZER_PRESSURE_SCALE 1.0 // Placeholder value, to be determined experimentally
#define OXIDIZER_PRESSURE_OFFSET 0.0 // Placeholder value, to be determined
#define PYRO_BATTERY_VOLTAGE_SCALE 0.01 // Placeholder value, to be determined experimentally
#define PYRO_BATTERY_VOLTAGE_OFFSET 0.0 // Placeholder value, to be determined experimentally

#define THERMOCOUPLE_TOP_CS_PIN 39
#define THERMOCOUPLE_BOTTOM_CS_PIN 40

#define MAIN_GPS_RX_PIN 4
#define MAIN_GPS_TX_PIN 5
#define MAIN_GPS_BAUD_RATE 38400

#define SPI_FLASH_CS_PIN 38
#define WRITE_POINTER_FLUSH_TICKS 10
#define FLASH_MIRROR_TICKS 50

#define SD_CARD_CS_PIN 10
#define SD_CARD_BUFFER_SIZE 512

#define RADIO_RX_PIN 17
#define RADIO_TX_PIN 18

#define LED_PIN 48
#define LED_COUNT 2

#define CAMERA_TRIGGER_PIN 42

#define BUZZER_PIN 16

#endif
