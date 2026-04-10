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

You will have to repeat these steps for every Pro Micro and every time you upload a new sketch to that specific microcontroller.
  
For renaming the Arduino MEGAs (Backlight controller, COMM panel, Standby Instruments controller) the “board.txt” file can be found in `~/…/packages/arduino/hardware/avr/<current_version_number>/`

### Table 1
**OpenHornet Suggested PID/Naming**
| **Section** | **Panel**                           | **OH#** | **Suggested PID** |
| ----------- | ----------------------------------- | ------- | ----------------- |
| UIP         | UIP MASTER (Arduino MEGA)           | 1A1A1   | 0x1A11            |
| UIP         | MASTER ARM                          | 1A2A1   | 0x1A21            |
| UIP         | L DDI/EWI                           | 1A3A1   | 0x1A31            |
| UIP         | SPIN RCVY                           | 1A6A1   | 0x1A61            |
| UIP         | HUD                                 | 1A7A1   | 0x1A71            |
| UIP         | R DDI/EWI                           | 1A9A1   | 0x1A91            |
| LIP         | LIP MASTER (Arduino MEGA)           | 2A1A1   | 0x2A11            |
| LIP         | IFEI                                | 2A2A1   | 0x2A21            |
| LIP         | AMPCD                               | 2A3A1   | 0x2A31            |
| LIP         | RWR CONTROL                         | 2A4A1   | 0x2A41            |
| LIP         | ECM/DISP                            | 2A5A1   | 0x2A51            |
| LIP         | STANDBY INST (Arduino MEGA)         | 2A7A1   | 0x2A71            |
| LIP         | BACKLIGHT CONTROLLER (Arduino MEGA) | 2A13    | 0x2A13            |
| CT          | SJU-17 SEAT                         | 3A2A1   | 0x3A21            |
| LC          | LC MASTER (Arduino MEGA)            | 4A2A1   | 0x4A11            |
| LC          | LDG GEAR                            | 4A2A1   | 0x4A21            |
| LC          | SELECT JETT                         | 4A3A1   | 0x4A31            |
| LC          | BRK PRES/BRAKE/ FIRE TEST           | 4A3A3   | 0x4A33            |
| LC          | EXT LIGHTS                          | 4A4A2   | 0x4A42            |
| LC          | APU                                 | 4A5A2   | 0x4A52            |
| LC          | FUEL                                | 4A5A1   | 0x4A51            |
| LC          | FCS                                 | 4A6A1   | 0x4A61            |
| LC          | OBOGS                               | 4A7A2   | 0x4A72            |
| LC          | COMM (Arduino MEGA)                 | 4A7A1   | 0x4A71            |
| RC          | RC MASTER (Arduino MEGA)            | 5A1A1   | 0x5A11            |
| RC          | RADAR ALT                           | 5A2A4   | 0x5A24            |
| RC          | HOOK/WING FD/AV COOL                | 5A2A1   | 0x5A21            |
| RC          | ELEC                                | 5A4A1   | 0x5A41            |
| RC          | ECS                                 | 5A5A1   | 0x5A51            |
| RC          | INTR LT                             | 5A6A1   | 0x5A61            |
| RC          | SNSR                                | 5A7A1   | 0x5A71            |
| RC          | SIM CNTL                            | 5A8A1   | 0x5A81            |
| RC          | KY58                                | 5A9A1   | 0x5A91            |
| RC          | DEFOG/CANOPY                        | 5A10    | 0x5A10            |	
  