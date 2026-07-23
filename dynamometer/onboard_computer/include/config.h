// Includes all of the configured values for the onboard computer program
#ifndef CONFIG_H
#define CONFIG_H

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

#define ADS1232_DOUT_PIN 47
#define ADS1232_SCLK_PIN 15
#define ADS1232_PWDN_PIN 21

#define THRUST_LOADCELL_SCALE 1000.0 // Placeholder value, to be determined experimentally
#define THRUST_LOADCELL_OFFSET 0.0 // Placeholder value, to be determined experimentally

#define OXIDIZER_LOADCELL_SCALE 1000.0 // Placeholder value, to be determined experimentally
#define OXIDIZER_LOADCELL_OFFSET 0.0 // Placeholder value, to be determined

#define MAIN_ADS1115_I2C_ADDRESS 0x48

#define MAIN_BATTERY_VOLTAGE_ADC_CHANNEL 3
#define FUEL_PRESSURE_ADC_CHANNEL 1
#define OXIDIZER_PRESSURE_ADC_CHANNEL 0
#define PYRO_BATTERY_VOLTAGE_ADC_CHANNEL 2

#define MAIN_BATTERY_VOLTAGE_SCALE 4.703704 // Placeholder value, to be determined experimentally
#define MAIN_BATTERY_VOLTAGE_OFFSET 0.0 // Placeholder value, to be determined experimentally
#define FUEL_PRESSURE_SCALE 3.2 // Placeholder value, to be determined experimentally
#define FUEL_PRESSURE_OFFSET 0.0 // Placeholder value, to be determined experimentally
#define OXIDIZER_PRESSURE_SCALE 3.2 // Placeholder value, to be determined experimentally
#define OXIDIZER_PRESSURE_OFFSET 0.0 // Placeholder value, to be determined
#define PYRO_BATTERY_VOLTAGE_SCALE 4.030303 // Placeholder value, to be determined experimentally
#define PYRO_BATTERY_VOLTAGE_OFFSET 0.0 // Placeholder value, to be determined experimentally

#define THERMOCOUPLE_TOP_CS_PIN 39
#define THERMOCOUPLE_BOTTOM_CS_PIN 40

#define EXTERNAL_ADS1115_0_I2C_ADDRESS 0x49
#define EXTERNAL_ADS1115_1_I2C_ADDRESS 0x4A
#define EXTERNAL_ADS1115_2_I2C_ADDRESS 0x4B

#define LSM6DSOX_VALID_MASK_BIT 0
#define H3LIS331_VALID_MASK_BIT 1
#define BMP581_VALID_MASK_BIT 2
#define THRUST_LOADCELL_VALID_MASK_BIT 3
#define OXIDIZER_LOADCELL_VALID_MASK_BIT 4
#define FUEL_PRESSURE_VALID_MASK_BIT 5
#define OXIDIZER_PRESSURE_VALID_MASK_BIT 6
#define PYRO_BATTERY_VOLTAGE_VALID_MASK_BIT 7
#define MAIN_BATTERY_VOLTAGE_VALID_MASK_BIT 8
#define THERMOCOUPLE_TOP_VALID_MASK_BIT 9
#define THERMOCOUPLE_BOTTOM_VALID_MASK_BIT 10
#define PISTON_POSITION_VALID_MASK_BIT 11
#define MAIN_GPS_LOCATION_VALID_MASK_BIT 12
#define MAIN_GPS_ALTITUDE_VALID_MASK_BIT 13
#define MAIN_GPS_SATTELITE_COUNT_VALID_MASK_BIT 14
#define MAIN_BATTERY_LEVEL_VALID_MASK_BIT 15

#define MAIN_GPS_RX_PIN 4
#define MAIN_GPS_TX_PIN 5
#define MAIN_GPS_BAUD_RATE 38400

#define MAIN_BATTERY_LEVEL_SENSOR_I2C_ADDRESS 0xC8

#define SPI_FLASH_CS_PIN 38
#define WRITE_POINTER_FLUSH_TICKS 10
#define FLASH_MIRROR_TICKS 50

#define SD_CARD_CS_PIN 10
#define SD_CARD_BUFFER_SIZE 512

#define RADIO_RX_PIN 18
#define RADIO_TX_PIN 17

#define LED_PIN 48
#define LED_COUNT 2

#define CAMERA_TRIGGER_PIN 42

#define BUZZER_PIN 16

#define MCP23017_I2C_ADDRESS 0x27
#define IGNITER_MAIN_PIN 8
#define IGNITER_DROGUE_PIN 9
#define IGNITER_ENGINE_PIN 10
#define SOLENOID_FUEL_PIN 0
#define SOLENOID_OXIDIZER_PIN 1

#endif
