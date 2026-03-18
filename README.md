# resat-rocket
All of the code used by the ReSat team during the development of a liquid-propellant rocket.

# Dynamometer

The *dynamometer* directory contains the code used during the engine test campaign. This includes C++ programs meant to be flashed onto the onboard and ground station microcontrollers, a C++ program mean to emulate a properly working ground station for testing purposes, and a Python script, which is to be executed on a seperate computer connected via USB to the ground station.

## Aim 

The aim of the onboard microntroller (onboardDynamometer.ino) code is to:
* control the fuel and oxidizer supply
* ignite the engine to begin combustion
* collect data from several peripherals and sensors described in the next section
* save all of the information on a SD-card
* send the data to the ground station via telemetry

The aim of the ground station microcontroller (groundStationDynamometer.ino) code is to:
* receive the telemetry data from the onboard microcontroller
* parse the telemetry data
* check for and deal with corrupt data packets or errors in transmission
* save the information on a SD-card for redundancy
* send the data to the computer via a USB connection

The aim of the computer dashboard (computerDashboard.py) code is to:
* read the incoming data from a serial connection
* display the readings in a GUI

## Hardware

The .ino files will run on custom made PCBs operating on the ESP32-S3-WROOM-1(N8R8) microcontroller. The board will receive data from the following peripherals:
* loadcell (measuring the thrust) through HX711 ADC 1
* MH-series loadcell (measuring the weight of the oxidizer tank) through HX711 ADC 2
* MAX31855 temperature sensor through SPI
* MAX31855 temperature sensor through SPI
* RFD868 radio interface through UART
* M10Q-5883 GPS through I2C and UART
* a photoresistor through the internal ADC
* LTC2944IDD#PBF main battery level sensor through I2C
* BMP581 atmospheric pressure sensor through I2C
* LSM6DSOXTR accelerometer and gyroscope through I2C
* H3LIS331DLTR high-G accelerometer through I2C
* ADS1115IDGSR high-accuracy ADC, containing:
    * the main battery level through a 100k/27k voltage divider
    * fuel pressure through a 22k/10k voltage divider
    * oxidizer pressure through a 22k/10k voltage divider
    * the pyro battery level through a 100k/33k voltage divider
* three external ADS1115IDGSR with four SS39ET Hall-Effect sensors each, used to measure the position of the piston within the tank

It will output data to the following peripherals:
* MG996R servo motor for fuel valve
* MG996R servo motor for oxidizer valve
* SD-card interface through SPI
* W25Q64JWSSIQ flash memory through SPI
* RFD868 radio interface through UART
* Hawkeye 4K Split V5 video camera trigger
* buzzer
* WS2812 with four interfacable LEDs through I2C
* MCP23017-E/SS serial interface through I2C, containing:
    * the main igniter
    * the drogue igniter
    * the engine ignition
    * the fuel solenoid valve
    * the oxidizer solenoid valve

The data handling is displayed in the diagram below.

![Data handling diagram](./media/dynamometerDataHandling.png "Data handling diagram")

## Software Architecture

Each data packet contains the following sensor readings, all saved as C++ floats and seperated by a semicolon (;):
* time
* thrust
* oxidizer pressure
* fuel pressure
* temperature top
* temperature middle
* temperature bottom
* photoresistor
* hall effect sensors (not implemented yet)

Additonally, at the ground station the following values will be attached, before being sent to the computer dashboard:
* received signal strength indicator

The data will be transfered between components as displayed in the diagram below.

![Data transfer diagram](./media/dynamometerDataTransfers.png "Data transfer diagram")

## Dependencies

For dummySerialCode.ino, onboardDynamometer.ino, and groundStationDynamometer.ino:
* This project was coded in Arduino IDE using the esp32 board library by Espressif (for a step-by-step setup process consult the [link](https://dronebotworkshop.com/esp32-intro/))

For computerDashboard.py:
* PySide6
* pyqtgraph
* pySerial