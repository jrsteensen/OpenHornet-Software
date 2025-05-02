# Tutorials
[TOC]
This document contains various tutorials related to software and programming your microcontrollers.

## PID Assignment of OpenHornet Microcontrollers
All microcontrollers that connect to the PC via USB should have their name, Vendor ID (VID) and Product ID (PID) changed to allow Windows (and you) to distinguish between them.
Spark Fun’s VID is 1B4F. ([Here is a list of all assigned VIDs.](https://www.usb.org/developers))

We only need to change the PID and the board’s name. Both are defined by a file that loads into Arduino IDE and uploaded onto the microcontroller every time we upload a sketch.
- NOTE: We will address the pro-micro here, but the process is similar for all microcontrollers.

1. Find and open `boards.txt`.
  - NOTE: You may have to select “show hidden items” in Windows Explorer to find the folder “AppData”.
  1. For Pro-Micros, this is found in `~users/<username>/AppData/Local/Arduino15/packages/SparkFun/hardware/avr/<current_version_number>/`.
  2. For Arduino Megas, this is found in `~users/<username>/AppData/Local/Arduino15/packages/arduino/hardware/avr/<current_version_number>/`.
  3. For WEMOS S2-MINIS, this is found in `~users/<username>/AppData/Local/Arduino15/packages/<TBD>`.
  
2. Update the board name and PID of the board.
  - For Pro-Micro, it's `Pro Micro`, and similar for the other microcontrollers.
  1. **Board Name:** Change `promicro.build.usb_product="Sparkfun Pro Micro"` to `promicro.build.usb_product="<SKETCH NAME>"`. 
    - Example: `promicro.build.usb_product="OH1A2A1 Master Arm Panel"`
  2. **PID:** Change `promicro.menu.cpu.16MHzatmega32U4.build.pid=0x9207` to `promicro.menu.cpu.16MHzatmega32U4.build.pid=<RECOMMENDED PID>`.
    - See Table 1 for recommended board names and PID values.
	
3. Save the file, close it, open the Arduino IDE, select the COM Port and your new board type (verifying it the 5V/16MHz) and upload your sketch.
  - Windows will show the board's name, while DCS-BIOS Bridge will show the VID/PID.
	
  