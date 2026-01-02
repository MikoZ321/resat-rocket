# resat-rocket
All of the code used by the ReSat team during the development of a liquid-propellant rocket.

# Dynamometer

The *dynamometer* directory contains the code used during the engine test campaign. This includes both a C++ program meant to be flashed onto the onboard microcontroller and a Python script, which is to be executed on a seperate computer connected via USB.

## Aim 

The aim of the microntroller (.ino) code is to:
* control the fuel and oxidizer supply
* ignite the engine to begin combustion
* collect data from several peripherals and sensors described in the next section
* save all of the information on a SD-card
* send the data to a computer via a serial connection

The aim of the ground station (.py) code is to:
* read the incoming data from a serial connection
* save the information in a .csv file for redundancy
* display the readings in a GUI