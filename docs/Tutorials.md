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
  
## Using OpenHornet's Included Arduino Libraries

The OpenHornet-Software repository includes specific, version-pinned Arduino libraries as git submodules inside the `libraries/` folder. Using these instead of whatever the Arduino IDE installs automatically ensures every contributor is compiling against the same tested library versions, avoiding subtle bugs caused by version mismatches.

### Included Libraries

| Library | Purpose |
|---|---|
| `dcs-bios-arduino-library` | DCS-BIOS serial communication |
| `FastLED` | LED control |
| `Adafruit_NeoPixel` | NeoPixel LED control |
| `Servo` | Servo motor control |
| `ArduinoJoystickLibrary` | USB HID joystick emulation (AVR) |
| `Joystick_ESP32S2` | USB HID joystick emulation (ESP32-S2) |
| `AccelStepper` | Stepper motor control with acceleration |
| `Stepper` | Basic stepper motor control |
| `RotaryEncoder` | Rotary encoder reading |
| `MultiMap` | Non-linear value mapping |
| `TCA9534` | I2C I/O expander driver |
| `U8g2` | Display driver (OLED/LCD) |
| `arduino-esp32` | Espressif ESP32 Arduino core |
| `Arduino_Boards` | SparkFun board definitions |

### Step 1 — Clone the Repository with Submodules

When you first clone the repository, you must initialise the submodules so the `libraries/` folders are actually populated. If you cloned without `--recurse-submodules`, the library folders will be empty.

**If you haven't cloned yet:**
```bash
git clone --recurse-submodules https://github.com/jrsteensen/OpenHornet-Software.git
```

**If you already cloned and the library folders are empty:**
```bash
git submodule update --init --recursive
```

After this step, each subfolder under `libraries/` will contain the correct library source files.

### Step 2 — Locate Your Arduino IDE Libraries Folder

The Arduino IDE looks for libraries in a specific sketchbook location on your machine:

| OS | Default path |
|---|---|
| Windows | `C:\Users\<username>\Documents\Arduino\libraries\` |
| macOS | `~/Documents/Arduino/libraries/` |
| Linux | `~/Arduino/libraries/` |

You can confirm (or change) this path from inside the Arduino IDE:
1. Open Arduino IDE.
2. Go to **File → Preferences** (Windows/Linux) or **Arduino → Preferences** (macOS).
3. Note the value in the **Sketchbook location** field — the `libraries` folder lives inside it.

### Step 3 — Link or Copy the Libraries

You have two options: **symbolic links** (recommended — lets you `git pull` updates automatically) or **copying** (simpler, but requires you to re-copy when the submodule is updated).

#### Option A: Symbolic Links (Recommended)

Symbolic links let the Arduino IDE find the libraries inside the repository without duplicating files. After a `git submodule update`, the IDE automatically picks up any changes.

**Windows (run as Administrator in Command Prompt):**
```cmd
mklink /D "C:\Users\<username>\Documents\Arduino\libraries\dcs-bios-arduino-library" "C:\path\to\OpenHornet-Software\libraries\dcs-bios-arduino-library"
```
Repeat for each library you need. Replace the paths with your actual clone location and username.

**macOS / Linux (Terminal):**
```bash
ln -s /path/to/OpenHornet-Software/libraries/dcs-bios-arduino-library ~/Documents/Arduino/libraries/dcs-bios-arduino-library
```
Repeat for each library you need.

#### Option B: Copy the Folders

Copy each subfolder from `libraries/` directly into your Arduino sketchbook `libraries/` folder. For example:

```
OpenHornet-Software/libraries/dcs-bios-arduino-library/  →  ~/Documents/Arduino/libraries/dcs-bios-arduino-library/
OpenHornet-Software/libraries/FastLED/                   →  ~/Documents/Arduino/libraries/FastLED/
```
(and so on for each library you need)

> **Warning:** If you copy instead of linking, you must re-copy after every `git submodule update` to pick up upstream changes.

### Step 4 — Verify in the Arduino IDE

1. Open (or restart) the Arduino IDE.
2. Open the **Library Manager** (**Sketch → Include Library → Manage Libraries…**) and confirm the libraries appear — or simply open one of the sketches from the `embedded/` folder and try **Sketch → Verify/Compile**.
   - If the IDE cannot find a library, it will show a red error like `fatal error: <LibraryName.h>: No such file or directory`.
3. If a library is missing, double-check that its subfolder is non-empty (run `git submodule update --init --recursive` again if needed) and that the link or copy target path is correct.

### Step 5 — Keeping Libraries Up to Date

When the OpenHornet-Software repository updates a submodule (e.g. to a newer version of `dcs-bios-arduino-library`), you need to update your local copy:

```bash
git pull
git submodule update --recursive
```

If you used symbolic links (Option A), the Arduino IDE will automatically use the updated version the next time you open it. If you copied the folders (Option B), repeat the copy step for any updated library.

### Notes

- **Do not install these libraries through the Arduino IDE Library Manager.** Doing so may install a different version than the one pinned in the repository, which can cause compile errors or unexpected behaviour.
- If the Arduino IDE has already auto-installed a conflicting version of a library, remove it from the sketchbook `libraries/` folder before linking/copying the repo version.
- The `arduino-esp32` submodule is the full Espressif Arduino core. It is used for build system purposes; for the Arduino IDE you normally install the ESP32 board support package separately via **File → Preferences → Additional Boards Manager URLs** using `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`.
