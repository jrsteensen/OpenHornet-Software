# Flashing Firmware from CI Artifacts
[TOC]

This guide explains how to download pre-built firmware from the OpenHornet CI pipeline and flash it to your microcontrollers using free, open-source command-line tools — no Arduino IDE required.

---

## Overview

Every time code is merged into the `main` or `develop` branch (and on every pull request), the GitHub Actions Continuous Integration (CI) pipeline automatically compiles all OpenHornet sketches. The resulting firmware files are bundled into a downloadable artifact called **`firmware`**. This artifact is available for **7 days** after each workflow run.

There are two kinds of compiled firmware files depending on which microcontroller the sketch targets:

| File extension | Board type | Flashing tool |
|---|---|---|
| `.hex` + `.eep` | Arduino MEGA 2560, SparkFun Pro Micro, Arduino Pro Mini | `avrdude` |
| `.bin` | WEMOS S2 Mini (ESP32-S2) | `esptool.py` |

---

## Step 1 — Download the Firmware Artifact

1. Open the [OpenHornet-Software repository](https://github.com/jrsteensen/OpenHornet-Software) in your browser.
2. Click the **Actions** tab at the top.
3. In the left sidebar, click **Continuous Integration**.
4. Click the most recent **successful** run (green checkmark) on the branch you want (typically `main`).
5. Scroll to the bottom of the run page to find the **Artifacts** section.
6. Click **firmware** to download the zip file.
7. Extract the zip — you will find a flat folder of `.hex`, `.eep`, and `.bin` files, one set per sketch.

Each file is named after the sketch it belongs to (e.g., `1A2-MASTER_ARM_PANEL.hex`). Use the table in [Sketch-to-Board Reference](#sketch-to-board-reference) to find the right file for your panel.

---

## Step 2 — Install the Flashing Tools

### avrdude (for AVR boards — MEGA 2560, Pro Micro, Pro Mini)

- **Windows**: Download and install the [Arduino IDE](https://www.arduino.cc/en/software). `avrdude` is bundled inside it, or install it standalone via the [WinAVR](https://winavr.sourceforge.net/) package. After installation, `avrdude` is typically found at `C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\avrdude.exe`.
- **macOS**: `brew install avrdude`
- **Linux**: `sudo apt install avrdude` (Debian/Ubuntu) or your distro's equivalent.

Verify: `avrdude -v` should print version information.

### esptool.py (for WEMOS S2 Mini — ESP32-S2)

Requires Python 3.7+.

```
pip install esptool
```

Verify: `esptool.py version` should print version information.

---

## Step 3 — Find Your COM Port

You need to know which serial port your microcontroller is on before flashing.

**Windows:**
Open Device Manager (press `Win + X` → Device Manager) and expand **Ports (COM & LPT)**. Your board will appear as something like `COM3` or `COM7`. Note down the port name.

**macOS:**
```bash
ls /dev/cu.*
```
Look for something like `/dev/cu.usbmodem14201` or `/dev/cu.usbserial-XXXX`.

**Linux:**
```bash
ls /dev/ttyACM* /dev/ttyUSB*
```
Look for `/dev/ttyACM0`, `/dev/ttyACM1`, `/dev/ttyUSB0`, etc. If you have multiple USB devices, plug the board in and out while running the command to see which entry appears and disappears.

---

## Step 4 — Flash the Firmware

Replace `<PORT>` with your actual port (e.g., `COM3` on Windows or `/dev/ttyACM0` on Linux/macOS) and `<SKETCH>` with the sketch name from the artifact folder.

### Arduino MEGA 2560

Used by: bus master panels (`1A1`, `2A1A1`, `4A1A1`, `5A1A1`), backlight controller (`2A13`), COMM panel (`4A7A1`), and standby instruments (`2A7A1`).

```bash
avrdude -p m2560 -c wiring -P <PORT> -b 115200 -D \
  -U flash:w:<SKETCH>.hex:i \
  -U eeprom:w:<SKETCH>.eep:i
```

**Example (Windows):**
```
avrdude -p m2560 -c wiring -P COM4 -b 115200 -D -U flash:w:1A1-UIP_ABSIS_BUS_MASTER.hex:i -U eeprom:w:1A1-UIP_ABSIS_BUS_MASTER.eep:i
```

**Example (Linux/macOS):**
```
avrdude -p m2560 -c wiring -P /dev/ttyACM0 -b 115200 -D -U flash:w:1A1-UIP_ABSIS_BUS_MASTER.hex:i -U eeprom:w:1A1-UIP_ABSIS_BUS_MASTER.eep:i
```

---

### SparkFun Pro Micro (ATmega32U4, 16 MHz)

Used by: the majority of OpenHornet panels. See [Sketch-to-Board Reference](#sketch-to-board-reference) for the full list.

The Pro Micro uses the **Caterina** USB bootloader. Because the board presents itself as a USB device, flashing requires a brief reset window:

1. **Trigger the bootloader**: Quickly double-tap the reset button on the Pro Micro. The board will enter the bootloader for about 8 seconds, during which a *new* COM port briefly appears. You must run the `avrdude` command **while the bootloader port is active**.
2. Run the flash command immediately after double-tapping reset, targeting the **bootloader port** (which may be a different COM number than the normal operating port).

```bash
avrdude -p atmega32u4 -c avr109 -P <BOOTLOADER_PORT> -b 57600 -D \
  -U flash:w:<SKETCH>.hex:i \
  -U eeprom:w:<SKETCH>.eep:i
```

**Example (Windows):**
```
avrdude -p atmega32u4 -c avr109 -P COM5 -b 57600 -D -U flash:w:1A2-MASTER_ARM_PANEL.hex:i -U eeprom:w:1A2-MASTER_ARM_PANEL.eep:i
```

**Example (Linux/macOS):**
```
avrdude -p atmega32u4 -c avr109 -P /dev/ttyACM1 -b 57600 -D -U flash:w:1A2-MASTER_ARM_PANEL.hex:i -U eeprom:w:1A2-MASTER_ARM_PANEL.eep:i
```

> **Tip:** On Linux you may need to add your user to the `dialout` group to access the serial port without `sudo`: `sudo usermod -aG dialout $USER` (log out and back in after).

---

### Arduino Pro Mini (ATmega328P, 8 MHz)

Used by: `4A9A1A1-THROTTLE_INNER_GRIP`.

The Pro Mini does not have a USB port of its own — you need a USB-to-serial adapter (e.g., FTDI FT232 or CP2102) connected to its TX/RX/GND/DTR pins.

```bash
avrdude -p atmega328p -c arduino -P <PORT> -b 57600 \
  -U flash:w:<SKETCH>.hex:i \
  -U eeprom:w:<SKETCH>.eep:i
```

**Example (Windows):**
```
avrdude -p atmega328p -c arduino -P COM6 -b 57600 -U flash:w:4A9A1A1-THROTTLE_INNER_GRIP.hex:i -U eeprom:w:4A9A1A1-THROTTLE_INNER_GRIP.eep:i
```

**Example (Linux/macOS):**
```
avrdude -p atmega328p -c arduino -P /dev/ttyUSB0 -b 57600 -U flash:w:4A9A1A1-THROTTLE_INNER_GRIP.hex:i -U eeprom:w:4A9A1A1-THROTTLE_INNER_GRIP.eep:i
```

---

### WEMOS S2 Mini (ESP32-S2)

Used by: `4A9A1-THROTTLE_CONTROLLER`.

The artifact contains a single merged binary (`.bin`) that includes the bootloader, partition table, and application in one file.

Put the board into download mode first:
1. Hold the **BOOT** (IO0) button.
2. While holding BOOT, press and release **RESET**.
3. Release BOOT. The board is now in download mode.

```bash
esptool.py --chip esp32s2 --port <PORT> --baud 921600 \
  write_flash 0x0 <SKETCH>.bin
```

**Example (Windows):**
```
esptool.py --chip esp32s2 --port COM7 --baud 921600 write_flash 0x0 4A9A1-THROTTLE_CONTROLLER.bin
```

**Example (Linux/macOS):**
```
esptool.py --chip esp32s2 --port /dev/ttyUSB0 --baud 921600 write_flash 0x0 4A9A1-THROTTLE_CONTROLLER.bin
```

After flashing completes, press **RESET** once to start running the new firmware.

---

## Sketch-to-Board Reference

Use this table to find which firmware file belongs on which panel and which flash command to use.

| Sketch (firmware filename prefix) | Panel Name | Board | Flash Command |
|---|---|---|---|
| `1A1-UIP_ABSIS_BUS_MASTER` | UIP Bus Master | Arduino MEGA 2560 | [MEGA 2560](#arduino-mega-2560) |
| `1A2-MASTER_ARM_PANEL` | Master Arm Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `1A3-L_DDI_AND_EWI` | L DDI / EWI | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `1A6-SPIN_RCVY_PANEL` | Spin Recovery Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `1A7-HUD_PANEL` | HUD Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `1A9-R_DDI_AND_EWI` | R DDI / EWI | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `2A13-BACKLIGHT_CONTROLLER` | Backlight Controller | Arduino MEGA 2560 | [MEGA 2560](#arduino-mega-2560) |
| `2A1A1-LIP_ABSIS_BUS_MASTER` | LIP Bus Master | Arduino MEGA 2560 | [MEGA 2560](#arduino-mega-2560) |
| `2A2A1-LEFT_LIP_MODULE` | Left LIP Module (IFEI) | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `2A3A1-AMPCD` | AMPCD | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `2A4A1-RWR_CONTROL_PANEL` | RWR Control Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `2A7A1-STANDBY_INSTR_MODULE` | Standby Instruments | Arduino MEGA 2560 | [MEGA 2560](#arduino-mega-2560) |
| `3A2A1-SEAT_CONTROLS` | Seat Controls | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `3A3A1-STICK_CONTROLLER` | Stick Controller | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A1A1-LC_ABSIS_BUS_MASTER` | LC Bus Master | Arduino MEGA 2560 | [MEGA 2560](#arduino-mega-2560) |
| `4A2A1-LDG_GEAR_PANEL` | Landing Gear Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A3A1-SELECT_JETT_PANEL` | Select Jettison Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A3A3-FIRE_TEST_PANEL` | Fire Test Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A4A2-EXT_LIGHTS_PANEL` | Exterior Lights Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A5A1-FUEL_PANEL` | Fuel Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A5A2-APU_PANEL` | APU Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A6A1-FCS_PANEL` | FCS Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A7A1-COMM_PANEL` | COMM Panel | Arduino MEGA 2560 | [MEGA 2560](#arduino-mega-2560) |
| `4A7A2-OBOGS_PANEL` | OBOGS Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `4A9A1-THROTTLE_CONTROLLER` | Throttle Controller | WEMOS S2 Mini | [S2 Mini](#wemos-s2-mini-esp32-s2) |
| `4A9A1A1-THROTTLE_INNER_GRIP` | Throttle Inner Grip | Arduino Pro Mini | [Pro Mini](#arduino-pro-mini-atmega328p-8-mhz) |
| `5A10-DEFOG_PANEL` | Defog Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `5A1A1-RC_ABSIS_BUS_MASTER` | RC Bus Master | Arduino MEGA 2560 | [MEGA 2560](#arduino-mega-2560) |
| `5A5A1-ECS_PANEL` | ECS Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `5A6A1-INTR_LT_PANEL` | Interior Lights Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `5A7A1-SNSR_PANEL` | Sensor Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `5A8A1-SIM_CNTL_PANEL` | Sim Control Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |
| `5A9A1-KY58_PANEL` | KY-58 Panel | SparkFun Pro Micro | [Pro Micro](#sparkfun-pro-micro-atmega32u4-16-mhz) |

---

## Troubleshooting

**avrdude: stk500v2_ReceiveMessage(): timeout**
The MEGA 2560 is not responding. Make sure the correct COM port is selected and the board is powered. Try pressing the reset button once and immediately running the command.

**avrdude: butterfly_recv(): programmer is not responding**
The Pro Micro bootloader window has closed. Double-tap reset again and re-run the `avrdude` command faster.

**avrdude: ser_open(): can't open device**
Wrong COM port, or your user lacks permission (Linux). Check Device Manager / `ls /dev/tty*` again. On Linux, run `sudo usermod -aG dialout $USER` and re-login.

**esptool: A fatal error occurred: Failed to connect to ESP32-S2**
The board is not in download mode. Repeat the BOOT + RESET button sequence and try again. On some setups you may need to lower the baud rate: replace `921600` with `460800` or `115200`.

**Artifact has expired / not found**
CI artifacts are kept for 7 days. If the artifact is gone, trigger a new CI run by pushing a commit or asking a maintainer. Alternatively, build the firmware locally — see the [Software Manual](SoftwareManual.md).

**Wrong firmware / board bricked**
Flash the correct `.hex`/`.bin` file for that panel using the table above. If a board appears completely unresponsive, it may need a bootloader re-burn — see the OpenHornet Discord for help.
