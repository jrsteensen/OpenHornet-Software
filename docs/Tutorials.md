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

## Setting Up VS Code and PlatformIO for Sketch Development

> **Important:** The Arduino IDE (Integrated Development Environment) is the supported way for end users to load sketches onto their OpenHornet hardware. If you only want to put firmware on your panels, stop here: use the Arduino IDE, or flash the pre-built release firmware as described in [Flashing Firmware from Release Packages](Flashing.md).
>
> This tutorial is for contributors who write and change sketches and want a full code editor while they work.

### Why Use VS Code and PlatformIO?

[Visual Studio Code](https://code.visualstudio.com/) (VS Code) is a free code editor. [PlatformIO](https://platformio.org/) is an extension for VS Code that compiles and uploads Arduino sketches. Together they give you:

- Code completion and "go to definition" across your sketch, DCS-BIOS, and the other libraries.
- Errors underlined in the editor as you type.
- Git, a serial monitor, and a terminal in one window.

This setup is a development convenience only. The project's official build is the `Makefile` in each sketch folder, which GitHub Actions runs on every pull request (PR). PlatformIO does not replace it, and you do not commit any PlatformIO files to the repository.

### How It Works

Each sketch folder under `embedded/` contains one `.ino` file and a `Makefile`. The `Makefile` names the target board and the libraries the sketch needs. The libraries live in `libraries/` as git submodules.

You will create one `platformio.ini` file at the root of your local clone. It tells PlatformIO three things:

1. Which sketch folder to compile (`src_dir`).
2. Where the libraries are (`libraries/`, the same folder the `Makefile` build uses).
3. Which board to compile for (one environment per board type).

To work on a different sketch, you change one line.

### Step 1 - Install the Tools

1. Install [VS Code](https://code.visualstudio.com/).
2. Open VS Code, click the **Extensions** icon in the left sidebar, search for `PlatformIO IDE`, and click **Install**.
3. Wait for PlatformIO to finish installing its core tools. A PlatformIO (alien head) icon appears in the left sidebar when it is ready. Restart VS Code if it asks you to.

### Step 2 - Clone the Repository with Its Submodules

The libraries in `libraries/` are git submodules. If they are empty, nothing compiles.

- **New clone:** follow the GitHub process in [CONTRIBUTING](https://github.com/jrsteensen/OpenHornet-Software/blob/main/CONTRIBUTING.md) to fork and clone the repository. GitHub Desktop clones submodules for you. On the command line, use:

  ```
  git clone --recurse-submodules https://github.com/<your-username>/OpenHornet-Software.git
  ```

- **Existing clone:** open a terminal in the repository root and run:

  ```
  git submodule update --init --recursive
  ```

Check that a library folder, for example `libraries/dcs-bios-arduino-library`, contains files.

### Step 3 - Create platformio.ini

1. In VS Code, click **File** > **Open Folder** and open the root of your OpenHornet-Software clone (the folder that contains `embedded/` and `libraries/`).
2. Create a new file named `platformio.ini` in that root folder.
3. Paste in the following:

```ini
; Local PlatformIO setup for OpenHornet sketch development.
; This file is ignored by git. Do not commit it.
; See docs/Tutorials.md - "Setting Up VS Code and PlatformIO for Sketch Development".

[platformio]
; The sketch folder to build. Change this line to work on another sketch.
src_dir = embedded/OH1_Upper_Instrument_Panel/1A2-MASTER_ARM_PANEL

[env]
framework = arduino
; Use the repository's library submodules, like the Makefile build does.
lib_extra_dirs = libraries
; Honor #define/#ifdef in sketches when picking libraries.
lib_ldf_mode = chain+
; DCS-BIOS serial speed.
monitor_speed = 250000

; SparkFun Pro Micro 5V/16MHz (promicro.mk)
[env:promicro]
platform = atmelavr
board = sparkfun_promicro16

; Arduino MEGA 2560 (mega2560.mk)
[env:mega2560]
platform = atmelavr
board = megaatmega2560

; Arduino Pro Mini 3.3V/8MHz (promini.mk)
[env:promini]
platform = atmelavr
board = pro8MHzatmega328

; WEMOS S2 Mini, ESP32-S2 (s2mini.mk)
[env:s2mini]
; The pioarduino platform provides Arduino-ESP32 core 3.x, the same major
; version as the libraries/arduino-esp32 submodule used by the Makefile build.
platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
board = lolin_s2_mini
; The core's USB library needs FS.h, which PlatformIO does not find on its own.
build_flags = -I${platformio.packages_dir}/framework-arduinoespressif32/libraries/FS/src
```

4. Save the file. PlatformIO detects the project and downloads the board toolchains the first time you build. The first build of each board type takes a few minutes; later builds take seconds.

> **Note:** `platformio.ini` and the `.pio/` build folder are listed in `.gitignore`, so git does not offer to commit them.

### Step 4 - Select the Sketch and Board

1. Set `src_dir` to the folder of the sketch you want to work on.
   - Example: `src_dir = embedded/OH2_Lower_Instrument_Panel/2A13-BACKLIGHT_CONTROLLER`
2. Open the sketch's `Makefile` and find the `include` line that is not commented out. Use the matching environment:

| **Makefile line**                         | **PlatformIO environment** | **Board**               |
| ----------------------------------------- | -------------------------- | ----------------------- |
| `include $(ROOTDIR)/include/promicro.mk`  | `promicro`                 | SparkFun Pro Micro      |
| `include $(ROOTDIR)/include/mega2560.mk`  | `mega2560`                 | Arduino MEGA 2560       |
| `include $(ROOTDIR)/include/promini.mk`   | `promini`                  | Arduino Pro Mini        |
| `include $(ROOTDIR)/include/s2mini.mk`    | `s2mini`                   | WEMOS S2 Mini (ESP32-S2)|

3. Select the environment in VS Code: click the environment name in the blue status bar at the bottom of the window (it shows `Default` at first), then pick, for example, `env:promicro`.

You can also look up the board for each sketch in the [Sketch-to-Board Reference](Flashing.md#sketch-to-board-reference).

### Step 5 - Build the Sketch

- Click the **check mark** icon in the status bar, or
- Open a terminal in VS Code (**Terminal** > **New Terminal**) and run:

  ```
  pio run -e promicro
  ```

A successful build ends with `[SUCCESS]` and prints how much RAM (Random Access Memory) and flash memory the sketch uses.

Do not add PlatformIO-specific code or files to sketch folders. Sketches must keep compiling in the Arduino IDE and with the `Makefile`.

### Step 6 - Upload and Monitor (Your Own Test Hardware Only)

You can upload to a board on your bench while you develop:

- Click the **right arrow** icon in the status bar, or run:

  ```
  pio run -e promicro -t upload
  ```

- To watch serial output, click the **plug** icon, or run:

  ```
  pio device monitor
  ```

PlatformIO finds the serial port by itself. If you have several boards connected, add `upload_port = COM5` (Windows) or `upload_port = /dev/ttyACM0` (Linux) to the environment. If an upload cannot connect, put the board into its bootloader or download mode as described in [Flashing Firmware from Release Packages](Flashing.md), then upload again.

> **Note:** PlatformIO uses the stock USB product ID (PID) for each board. The custom names and PIDs from [PID Assignment of OpenHornet Microcontrollers](#pid-assignment-of-openhornet-microcontrollers) are set up through the Arduino IDE.

### Keep VS Code Files Out of Your Commits

When PlatformIO opens the project it rewrites `.vscode/c_cpp_properties.json` (a tracked file) and creates `.vscode/launch.json` and `.vscode/extensions.json`. Do not commit these changes. Before you commit, discard them:

```
git checkout -- .vscode/c_cpp_properties.json
```

In GitHub Desktop, uncheck these files in the **Changes** list (or right-click > **Discard changes**). Commit only the sketch and documentation files you meant to change.

### Before You Open a Pull Request

PlatformIO uses different toolchain and core versions than the official build, so a PlatformIO build is not proof that CI (continuous integration) will pass. Before you open a PR:

1. Compile the sketch in the Arduino IDE, as required by [Testing Your Software](SoftwareManual.md#testing-your-software).
2. Confirm the GitHub Actions checks pass on your PR. CI is the authority.

### Troubleshooting

**`MissingPackageManifestError: Could not find one of 'library.json, library.properties, module.json'`**
You listed a repository library in `lib_deps` with `symlink://`. Some libraries, such as `TCA9534`, have no manifest file. Use `lib_extra_dirs = libraries` as shown above instead.

**`'ledcSetup' was not declared in this scope` (in `libraries/Servo`) when building for `s2mini`**
PlatformIO pulled in the AVR Servo library because DCS-BIOS includes it unless `DCSBIOS_DISABLE_SERVO` is defined. Make sure `lib_ldf_mode = chain+` is set, so PlatformIO honors the `#define DCSBIOS_DISABLE_SERVO` in the sketch.

**`USBMSCFS.h: fatal error: FS.h: No such file or directory` when building for `s2mini`**
The `build_flags` line is missing from `[env:s2mini]`. Add it as shown above.

**`fatal error: DcsBios.h: No such file or directory`** (or any other library header)
The library submodules are empty. Run `git submodule update --init --recursive`.

**The build compiles the wrong sketch**
Check `src_dir`. It must point to the folder that contains the `.ino` file, not to the `.ino` file itself.

### Remember

VS Code and PlatformIO are tools for writing and testing code. End users load sketches onto their hardware with the Arduino IDE, or flash the release firmware as described in [Flashing Firmware from Release Packages](Flashing.md). Do not point end users to this tutorial for loading their panels.
