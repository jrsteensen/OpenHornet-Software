# Software Manual
[TOC]
This manual covers writing software for the OpenHornet project, including how to write, document, and test your code. It also provides guidelines for creating clear, maintainable sketches that are approachable for beginners while still meeting the needs of advanced users. Some items in this manual are mandatory project rules, while others are style guidelines - you **SHOULD** follow the guidelines by default unless you have an articulable reason to deviate. Because guidelines can occasionally conflict, use good judgment; if you’re unsure, ask a project maintainer or another contributor.

## Supported Hardware

- [Arduino MEGA 2560](https://store-usa.arduino.cc/products/arduino-mega-2560-rev3)
- [Sparkfun Pro-Micro](https://www.sparkfun.com/products/12640)
- [Arduino Pro-Mini](https://www.adafruit.com/product/2378)
- [WEMOS S2 mini (ESP32-S2FN4R2)](https://www.wemos.cc/en/latest/s2/s2_mini.html)

## Prerequisites

- Arduino IDE (with libraries listed below)
- GNU Make
- Local Doxygen installation

## Supported Software

- Arduino IDE 1.8.10+ (Arduino IDE 2.0.0+ preferred)
- GNU Make 3.81+
- Flightpanels DCS-BIOS v4.11.16+
- Doxygen 1.8.13+

### Arduino Libraries

- Flightpanels dcs-bios-arduino-library-0.3.11+
- FastLED (Not Adafruit_NeoPixel. It will be removed in the future.)
- Servo
- ESP32-Servo
- TCA9534
- Wire
- ArduinoJoystickLibrary
- AccelStepper

### Suggested Debug Tools

- Bort v0.2.4+
- Dcs-Insight v1.8.3+

## Normative Language

This document uses the keywords **SHALL**, **SHOULD**, and **MAY** to indicate requirement levels. These terms are to be interpreted as follows:

- SHALL: A mandatory requirement. You must follow this rule for OpenHornet contributions unless a project maintainer explicitly grants an exception. Noncompliance is considered a defect, policy violation, or grounds for requested changes before acceptance.

- SHOULD: A strong recommendation and the default expectation. You are expected to follow this guidance in most cases, but you **MAY** deviate when you have a clear, articulable reason (for example, hardware constraints, performance requirements, safety concerns, or compatibility). When deviating, document the rationale in code comments, commit notes, or the pull request description as appropriate.

- MAY: An optional practice. This is permitted but not required. Use it when it improves clarity, maintainability, testing, or consistency, and ignore it when it does not add value.

If a SHALL statement conflicts with a SHOULD or MAY statement, the SHALL statement takes precedence. If two SHOULD statements conflict, use engineering judgment and favor the option that improves readability, safety, maintainability, and project consistency.

---

## Getting Started

### Preparation

The starting point of every new software sketch is the `OHSketchTemplate` folder.

1. Copy the entire `OHSketchTemplate` folder.
2. Delete the sample function from `OHSketchTemplate.ino`.
3. Update all `@tags` with your sketch's information.
4. Ensure the correct board line in the `Makefile` is uncommented (see [Makefile](#makefile)).

### Sketch Naming

Sketches are named according to the reference designator found in the OH-INTERCONNECT document.

- The first part of the name is the OH No. found in the drawing (the number beneath the Board type).
  - Example: `1A2` for the Master Arm Panel.
- Follow the OH No. with a minus sign `-`, then the component name in uppercase with underscores.
  - Example: `1A2-MASTER_ARM_PANEL` for the Master Arm Panel.

If the sketch controls more than one component, use the name of the first component controlled by the board.

---

## Writing Example Code

This section focuses on writing code examples and sketches that can be read by beginners and advanced users alike.

- Efficiency is not paramount; readability is.
- Assume many readers are relative beginners who care about getting projects done, not about code for its own sake.
- When forced to choose between technically simple and technically efficient, choose the former.
- Introduce concepts only when they are useful, and minimize the number of new concepts introduced in each example.
- Use `setup()` and `loop()` at the beginning of the program so readers can get an overview quickly.

### Sketch Structure and Forward Declarations

Arduino sketches are ultimately compiled as C++. In standard C++, a function must be **declared** before it is used. The Arduino build system often auto-generates function prototypes, which can hide this requirement - but that behavior is not guaranteed in every build environment and can break in more complex code.

- You **SHOULD** place `setup()` and `loop()` near the top of the file so readers can quickly understand the sketch flow.
- You **SHOULD** add **forward declarations (function prototypes)** above `setup()` and `loop()` for any helper functions they call.
- You **MAY** place full helper function definitions above `setup()` if it improves readability for a specific sketch.

A recommended sketch layout:

```cpp
// Includes
#include <Arduino.h>

// Constants, pin definitions, and globals
#define SOME_PIN 2

// Forward declarations (prototypes)
void functionA();
int  functionB(int arg);

// Entry points (overview first)
void setup() {
  functionA();
}

void loop() {
  // main logic
}

// Helper function definitions (implementation later)
void functionA() {
  // ...
}

int functionB(int arg) {
  return arg + 1;
}
```

---

## Documenting Software with Doxygen

All code **SHALL** be documented using Doxygen-compatible comments. We use Doxygen as the API documentation generator, so comments must be Doxygen compatible in order to show up in generated documentation.

We use Javadoc-style comment markup because it is easy to read.

### Standard File Header

This is the standard file header of OpenHornet. It must be used as-is in every sketch written for the OpenHornet-Software repository.

```
/**************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *   ----------------------------------------------------------------------------------
 *   Copyright 2016-2026 OpenHornet
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you **MAY** not use this file except in compliance with the License.
 *   You **MAY** obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *   ----------------------------------------------------------------------------------
 *   Note: All other portions of OpenHornet not within the 'OpenHornet-Software'
 *   GitHub repository is released under the Creative Commons Attribution -
 *   Non-Commercial - Share Alike License. (CC BY-NC-SA 4.0)
 *   ----------------------------------------------------------------------------------
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxygen capable comments.
 **************************************************************************************/
```

### Sketch Summary Block

The sketch summary comment block goes at the top of the file, right after the standard header.

A template looks like this:

```
/**
 * @file OHSketchTemplate.ino
 * @author Balz Reber
 * @date 02.26.2026
 * @version u.0.0.1 (untested)
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware
 * @brief This is the OpenHornet Sketch Template.
 *
 * @details This is the OpenHornet Sketch Template. It **SHOULD** be used as a starting point for every new sketch.
 * Please copy the whole OHSketchTemplate folder to start, and ensure the correct line in the Makefile is uncommented.
 *
 *  * **Reference Designator:**
 * <Replace with the reference designator, such as 4A3A1>
 *
 * **Intended Board:**
 * <Replace with hardware type such as ABSIS ALE, or ABSIS ALE w/ABSIS Relay Module, etc.>
 *
 * **RS485 Bus Address:**
 * <Replace with RS485 bus address, if applicable>
 *
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 1   | function 1
 * 2   | function 2
 * 3   | function 3
 *
 */
```

#### Summary Elements

- `@file`: Must exactly match the filename, including extension.
- `@author`: List authors, comma-separated, with the original author first.
- `@date`: Date last modified.
- `@version`: See [Versioning](#versioning).
- `@warning`: Untested sketches must include the untested warning line shown above.
- `@brief`: One-line description.
- `@details`: A fuller description, plus metadata like reference designator, intended board, bus address, and wiring diagram.

### Verbose Title Block Example

Use a verbose title block at the beginning of every sketch. This is an example format (adapt as needed):

```
/**
 * @file 4A3A1-SELECT_JETT_PANEL.ino
 * @author Arribe
 * @date 03.01.2024
 * @version 0.0.1
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the SELECT JETT panel.
 *
 * @details
 *
 *  * **Reference Designator:** 4A3A1
 *  * **Intended Board:** ABSIS ALE w/ Relay Module
 *  * **RS485 Bus Address:** 2
 *
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A0  | Hook Bypass Switch
 * A1  | Launch Bar Switch
 * A2  | Flaps Switch Position 2
 * A3  | Flaps Switch Position 1
 * 2   | Launch Bar Mag Switch
 * 3   | Hook Bypass Mag Switch
 * 4   | Landing Taxi Light
 * 6   | Selector Jettison Position 1
 * 7   | Selector Jettison Position 3
 * 8   | Selector Jettison Position 8
 * 10  | Antiskid Switch
 * 14  | Selector Jettison Position 2
 * 15  | Selector Jettison Push
 * 16  | Selector Jettison Position 4
 */
```

### Functions

Function documentation **SHOULD** follow this pattern:

```
/**
 * A brief description on a single line, ended by a period or blank line.
 *
 * A longer comment, which **MAY** stretch over several lines and **MAY** include other things like:
 * - a list like this
 * - special markup like below
 *
 * @param myParam1 Description of 1st parameter.
 * @param myParam2 Description of 2nd parameter.
 * @returns Description of returned value.
 */
int sampleFunction(int myParam1, int myParam2) {
  return myReturn;
}
```

#### Commenting Inside Functions

Doxygen does not parse the insides of functions into API output, but humans still have to read the code.
Comment internal logic with normal `//` comments.

Example:

```
// Takes the input var and duplicates it. Writes the result in a newly created result var of type int.
int result = 2 * input;
```

---

## Commenting Your Code

All comments **SHOULD** use Doxygen-compatible comments and tags.

### Doxygen Tags

We use the `@` prefix (not the `\` prefix).

- `@note` is the most generic tag, and is the one to use in most cases.
- `@attention` highlights a particularly important note you do not want the reader to overlook.
- `@warning` is used when there **MAY** be negative consequences if the reader is not careful.
- `@remark` / `@remarks` are for lower-importance notes.
- `@todo` indicates unfinished work and is collected into a Doxygen TODO list.
- `@bug` indicates a known bug and is collected into a Doxygen bug list.

Reference: https://www.doxygen.nl/manual/commands.html

### Variables, Constants, and Macros/Defines

Comment every variable or constant declaration with a description of what it does. Use trailing Doxygen-compatible comments unless the description is complex enough to warrant a full block.

Example:

```
/**
 * @brief Pilots **MAY** want the launch bar to automatically release when the throttles advance to MIL power.
 * The sim's version of the F/A-18 does *NOT* have this feature. The define allows setting it as desired, default is *false*.
 */
#define LBAR_SW_AUTORETRACT false

// Define pins for DCS-BIOS per interconnect diagram.
#define HOOKB_SW A0 ///< Hook Bypass Switch
#define LBAR_SW A1 ///< Launch Bar Switch
#define FLAPS_SW2 A2 ///< Flaps Switch Position 2
#define FLAPS_SW1 A3 ///< Flaps Switch Position 1
#define LBAR_RET 2    ///< Launch Bar Switch Mag

// Declare variables for custom non-DCSBios logic for mag-switches.
bool hookLeverState = LOW;        ///< Initializing for correct cold/ground start position as switch releases on power off.
unsigned long hookLeverTime = 0;  ///< Initializing variable to hold time of last hook change.
bool wowLeft = true;              ///< Initializing weight-on-wheel value for cold/ground start.
```

### Code Blocks

Comment every code block, preferably before the block so the reader knows what is coming.

Example:

```
/**
 * @brief Need to save the hook lever state to test turning off the hook bypass mag-switch in the main loop.
 */
void onHookLeverChange(unsigned int newValue) {
  hookLeverState = newValue;
  hookLeverTime = millis();
}
DcsBios::IntegerBuffer hookLeverBuffer(0x74a0, 0x0200, 9, onHookLeverChange);
```

- Comment every `for` loop.
- Avoid one-line `if` statements in example code. Use block format for clarity.

Instead of this:

`if (somethingIsTrue) doSomething;`

Use this:

```
if (somethingIsTrue == TRUE) {
  doSomething;
}
```

### Avoid Pointers in Examples

Pointers are confusing to novice programmers. Avoid pointers in example code unless absolutely necessary, and explain them if you must use them.

### Example: Explain Logic Before Code

```
/**
 * ### Launch Bar Auto Retract Logic
 * If the launch bar mag-switch is held in extend position, then: \n
 * -# if no weight on wheels retract the launch bar. \n
 * -# if the launch bar auto-retract is defined as true and both engines' RPM is >85 then retract launch bar. \n
 * @note If launch bar auto-retract is true, when connecting to the catapult it **MAY** be easier to keep one engine under 80% while advancing the other with enough power get over the shuttle.
 */
// Named switch positions (avoid bare numbers in control flow)
#define LBAR_SWITCH_RETRACT LOW
#define LBAR_SWITCH_EXTEND  HIGH

if (launchBarMagState == HIGH) {
  switch (launchBarState) {
    case LBAR_SWITCH_RETRACT:  // launch bar switch in retract
      // Do nothing. The state change method will retract the launch bar.
      break;

    case LBAR_SWITCH_EXTEND:  // launch bar switch in extend
      if (!wowLeft && !wowRight && !wowNose) {  // no weight on wheels retract launch bar
        digitalWrite(LBAR_RET, LOW);
        launchBarMagState = LOW;  // mag is off
        break;
      }

      if ((LBAR_SW_AUTORETRACT == true) && (rpmL >= 85 && rpmR >= 85)) {  // If auto retract and both engines over 85% RPM, turn off mag.
        digitalWrite(LBAR_RET, LOW);
        launchBarMagState = LOW;  // mag is off
      }
      break;
  }
}
```

---

## Variables and Constants

### Variables

- Use camelCase naming convention for variables.
- Avoid single-letter variable names. Make them descriptive.
- Avoid vague names like `val` or `pin`. Prefer names like `buttonState` or `switchPin`.
- Prefer simple Arduino/C++ types in examples (`bool`, `int`, `unsigned long`, `float`).
 - You **SHOULD** avoid fixed-width integer types (`uint8_t`, etc.) in beginner-facing examples unless needed - and explain them if used.
 - You **SHOULD** avoid Arduino `String` in most examples unless there is a clear reason (heap fragmentation and memory use on small MCUs).
- Avoid confusing numbering schemes like `pin1`, `pin2`, etc.

If you need to renumber pins, consider an array:

`int myPins[] = { 2, 7, 6, 5, 4, 3 };`

This allows:

- `digitalWrite(myPins[1], HIGH);  // turns on pin 7`

And iteration:

```
for (int thisPin = 0; thisPin < 6; thisPin++) {
  digitalWrite(myPins[thisPin], HIGH);
  delay(500);
  digitalWrite(myPins[thisPin], LOW);
  delay(500);
}
```

### Constants and Macros

- Use all caps for constants and macro values.
- Use `#define` or `const int` for values that do not change.
  - Pins requiring digital pin functionality **SHOULD** use numeric digital pin notation.
  - Pins requiring analog functionality **SHOULD** use `A<numeric>` notation.

`#define` is preferred for simple pin assignments. `const` is acceptable where required. Avoid `#define` when it could result in a hard-to-diagnose unfavorable outcome.

Reference: https://forum.arduino.cc/t/when-to-use-const-int-int-or-define/668071

Example:

```
// Define pins for DCS-BIOS per interconnect diagram.
#define HOOKB_SW A0 ///< Hook Bypass Switch
#define LBAR_SW A1 ///< Launch Bar Switch
#define FLAPS_SW2 A2 ///< Flaps Switch Position 2
#define FLAPS_SW1 A3 ///< Flaps Switch Position 1
#define LBAR_RET 2    ///< Launch Bar Switch Mag
#define HOOK_FIELD 3  ///< Hook Bypass Switch Mag
#define LADG_SW 4     ///< Landing Taxi Light
#define SJET_SW1 6    ///< Selector Jettison Position 1
#define SJET_SW3 7    ///< Selector Jettison Position 3
#define SJET_SW5 8    ///< Selector Jettison Position 8
#define ASKID_SW 10   ///< Anti-Skid Switch
#define SJET_SW2 14   ///< Selector Jettison Position 2
#define SJET_PUSH 15  ///< Selector Jettison Push
#define SJET_SW4 16   ///< Selector Jettison Position 4
```

#### Avoid Magic Numbers in Control Flow

Standalone numeric values in logic (especially `switch/case`) are hard to read and easy to misunderstand.

- You **SHOULD NOT** use unexplained numeric literals like `case 1:` or `case 2:` in example code.
- You **SHOULD** use self-describing names via `enum`, `const`, or `#define` so the code explains itself.
- You **SHOULD** prefer names that describe *meaning* (for example `GEAR_RETRACTED`) rather than raw values.
- You **MUST NOT** redefine Arduino core constants like `HIGH` and `LOW`. If you need named states, define your own names that map to `HIGH`/`LOW` or to numeric values.

Examples:

```
// Good: named states map to Arduino HIGH/LOW (do not redefine HIGH/LOW)
#define GEAR_RETRACTED_STATE HIGH
#define GEAR_EXTENDED_STATE  LOW

switch (gearState) {
  case GEAR_RETRACTED_STATE:
    // gear is retracted
    break;

  case GEAR_EXTENDED_STATE:
    // gear is extended
    break;
}
// Good: enum makes intent obvious

enum GearState : uint8_t {
  GEAR_EXTENDED  = 0,
  GEAR_RETRACTED = 1
};

switch (gearState) {
  case GEAR_EXTENDED:
    // ...
    break;

  case GEAR_RETRACTED:
    // ...
    break;
}
```

### Addressing

Whenever referencing an address, mask, or shift, reference them using `addresses.h` of DCS-BIOS:

https://github.com/DCS-Skunkworks/dcs-bios/wiki/Addresses.h

---

## Writing Tutorials or Instructions

- Write in the active voice.
- Write clearly and conversationally, as if the person following your instructions were in the room with you.
- Give instructions in the second person: "Next, you'll read the sensor..." "Make a variable called `thisPin`..."
- Prefer short, simple, declarative sentences and commands.
- Avoid filler phrases that add no information. Do not say "you want to set the pins"; say "set the pins."
- Use pictures and schematics where possible.
- Check your assumptions. Define or link to concepts you use.
- Explain the big picture first, then give step-by-step instructions.
- Spell out acronyms the first time you use them.
- Be consistent with terms.
- Make examples do one thing well.

---

## RS485 Slave ID

All sketches that run as a slave on the RS485 bus must have a slave ID (`DCSBIOS_RS485_SLAVE`).

- The slave ID must be unique on the RS485 bus the sketch runs on.
- Slave IDs can repeat across different buses (each RS485 master has its own bus).
- The slave ID is assigned according to the BUS ADDR in the interconnect.

Example: `1A2A` (Master Arm Panel) **SHALL** be Bus Address 1.

---

## Versioning

The version number consists of three digits:

`release.feature.change`

Example: `1.4.2`

Rules:

- Increment `change` every time there is any change to the sketch, no matter how small.
- Increment `feature` every time a new complete feature is added.
- Increment `release` only for a new released version.
- If any number changes, all subsequent numbers reset to 0.

Untested sketches must have a leading `u` and `(untested)` at the end:

`u.1.4.2 (untested)`

---

## Makefile

Because the project targets multiple Arduino boards and libraries, each sketch directory includes a `Makefile` to define the target board and libraries.

- `LIBRARIES` is a space-separated list of libraries to include in your sketch.
  - Libraries must be added as a git submodule in `/libraries` first (see [Git Submodules](#git-submodules)).
  - **Libraries referenced in the Makefile **SHALL** be ONLY the libraries required by the sketch.** This is important because it reduces runtime of the GitHub Workflow and cycle time for checks.

At the end of the Makefile, include the appropriate board makefile:

- `../../include/mega2560.mk` (Mega 2560)
- `../../include/promicro.mk` (Sparkfun Pro Micro)
- `../../include/promini.mk` (Sparkfun Pro Mini)
- `../../include/s2mini.mk` (Wemos S2 Mini)

GitHub Actions uses the Makefile to verify that code compiles. When successful, a downloadable zip file is created with compiled firmware which can be flashed to the desired board.

---

## Testing Your Software

Before you upload anything:

1. Verify your sketch compiles in the Arduino IDE.
2. Verify Doxygen compiles locally.

Then:

1. Open a PR to the OpenHornet-Software repo's `develop` branch.
2. Confirm CI and Doxygen checks pass.
3. Request review.

---

## GitHub Actions

GitHub Actions is the project's continuous integration engine. When a pull request is opened or merged, it checks out the repository and attempts to compile the code. If compilation succeeds and there are no errors detected, it updates the Doxygen documentation and uploads it back to the repository.

---

## Git Submodules

The OpenHornet software projects use git submodules to include external Arduino libraries.

- Git for Windows: select the option to recursively clone submodules when you clone the repository.
- GitHub Desktop: clones submodules automatically if they exist in the branch being cloned.

Reference: https://github.blog/2016-02-01-working-with-submodules/

---

## Arduino Libraries

Libraries used by sketches must be added as a git submodule under `/libraries`, then referenced in the sketch Makefile under `LIBRARIES`.

If you need a new library, add it under `/libraries`:

`git submodule add https://github.com/<organization>/<project>.git`

Libraries are downloaded during each GitHub Actions run and made available during compilation. Ensure the library name in `LIBRARIES` matches the folder name under `/libraries`.

---

## Resources

- http://www.doxygen.org
- https://github.com/DCS-Skunkworks/dcs-bios
- https://github.com/DCS-Skunkworks/dcs-bios-arduino-library/tree/327bf2233603c28989de63dc96e17abecf91ed31
- https://github.blog/2016-02-01-working-with-submodules/
- https://www.arduino.cc/en/Reference/StyleGuide
