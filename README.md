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