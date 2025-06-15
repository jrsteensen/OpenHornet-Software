# Software Manual
[TOC]
This manual is about writing software for the OpenHornet project. It shows how to write, document and test your software.

## Supported Hardware

- [Arduino MEGA 2560](https://store-usa.arduino.cc/products/arduino-mega-2560-rev3)
- [Sparkfun Pro-Micro](https://www.sparkfun.com/products/12640)
- [Arduino Pro-Mini](https://www.adafruit.com/product/2378)
- [WEMOS S2 mini (ESP32-S2FN4R2)](https://www.wemos.cc/en/latest/s2/s2_mini.html)

## Prerequisites

- Arduino IDE (with Libraries listed below)
- GNU Make
- Local Doxygen installation

## Supported Software

- Arduino IDE 1.8.10+ (Arduino IDE 2.0.0+ preferred)
- GNU Make 3.81+
- DCS-BIOS v2.8.7+
- Doxygen 1.8.13

### Arduino Libraries

- dcs-bios-arduino-library-0.3.9+
- Adafruit_NeoPixel
- Servo
- TCA9534
- Wire
- ArduinoJoystickLibrary
- Stepper

### Suggested debug tools

- Bort v0.2.4+
- Dcs-Insight v1.8.3+

## Preparation

The starting point of every new software sketch is the OHSketchTemplate folder. Please copy the whole folder. There is a Makefile included which defines the target board and libraries needed for your sketch. Once copied delete the sample function from the OHSketchTemplate.ino. Then change all the \@tags with your own information.

## Sketch naming

The Sketches are named according to the reference designator found in the OH-INTERCONNECT document.
The first part of the name is the OH No. found in the drawing. It is the number beneath the Board type.

e.g: 1A2 for the Master Arm Panel.

The No. is followed by a minus sign "-" (without space). Then the name of the component the sketch is for.
The name is written in uppercase and with underscores "\_" instead of spaces.

e.g: 1A2-MASTER_ARM_PANEL for the Master Arm Panel.

If the Sketch is for more than one component. Meaning that the board controls more than one component,
use the name of the first component the sketch is for.


## Documenting Software with Doxygen

It is imperative that all the code you write is documented. We use doxygen as API documentation generator. So all the comments have to be doxygen compatible. Otherwise they will not show up in the documentation.

We use Javadoc style comment markup. Since it is the easiest to read for humans.

The following is an example of how to document code:

### File Header


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
 *   Copyright 2016-2024 OpenHornet
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
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
This is the standard file header of OH. It has to be used as is in every sketch written for the OH Software.


### Sketch Summary

The Sketch summary comments should be on top of the file, right after the Header and looks like this:
```
/**
 * @file OHSketchTemplate.ino
 * @author Balz Reber
 * @date 02.26.2024
 * @version u.0.0.1 (untested)
 * @warning This sketch is based on a wiring diagram, and was not yet tested on hardware
 * @brief This is the OpenHornet Sketch Template.
 *
 * @details This is the OpenHornet Sketch Template. It should be used as a starting point for every new sketch.
 * Please copy the whole OHSketchTemplate folder to start, and ensure the correct line in the Makefile is uncommented.
 * 
 *  * **Intended Board:**
 * <Replace with hardware type such as ABSIS ALE, or ABSIS ALE w/ABSIS Relay Module, etc.>
 * 
 * **Wiring diagram:**
 * PIN | Function
 * --- | ---
 * 1   | function 1
 * 2   | function 2
 * 3   | function 3
 * 
 */
```
The elements in the summary are the following:

#### File
The \@file has to have the exact same name as the filename. Including the file extension.

#### Author
The author of the Sketch. If more than one person was working on the sketch, the authors shall be listed
in a comma separated list. With the original author at first place.

#### Date
The date the sketch was last modified.

#### Version
The version of the sketch. To see how the versioning of sketches works, see "Versioning" in this document.

#### Warning
Every untested sketch has to have a "This sketch is based on a wiring diagram, and was not yet tested on hardware" warning.

#### Brief
A brief description of the sketch.

#### Detail
A more detailed description of the sketch

#### Intended board
This is the hardware that this sketch is intended to run on.

#### Wiring diagram
A quick reference showing connections to this PCB, for ease of reference when reading this code.

### Functions

Function documentations should look like this:

```
/**
* A brief description on a single line, ended by a period or blank line.
*
* A longer comment, which may stretch over several lines and may include other things like:
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
Doxygen then takes this and generates a Function documentation looking like this:

![IMAGE](https://raw.githubusercontent.com/jrsteensen/OpenHornet/master/images/Logo/open_hornet_avatar.png)

Functions should be commented as seen above. It is important that every function you write is commented like this. Otherwise it will not show up in the API documentation.
The comment structure is as followed:

#### Brief

```
/**
* A brief description on a single line, ended by a period or blank line.
```
The comment has to start with `/**` and then a new line with a single `*`.
Then you write a very short description of what the function will do. The short description has to end with a dot `.` or a blank like.

#### Description

```
* A longer comment, which may stretch over several lines and may include other things like:
* - a list like this
* - special markup like below
```
This is the main description of your function. Write in detail what your function does.

#### Parameters

```
* @param myParam1 Description of 1st parameter.
* @param myParam2 Description of 2nd parameter.
```
Each parameter the function uses is documented with a `@param` tag. After the tag there is a space and then the name of the parameter. Then another space and then the description of the parameter.

#### Return
```
* @returns Description of returned value.
```
The return parameter, if there is any, is documented with a `@returns` parameter. The return parameter needs no name. So after a space you can write the description of the parameter.

#### Other Comments
```
// Takes the input var and duplicates it. Writes the result in a newly created result var of type int.
int result = 2 * input
```
The insides of a function are a black box to doxygen. It is important that you comment the code inside a function nevertheless. This has to be done for other coders who might have to work with your code. Comments inside a function are done with a simple `//` before the comment.

## Slave ID
All sketches who run as slave on the RS485 bus have to have a slave ID (DCSBIOS_RS485_SLAVE). That slave ID has to be unique on the RS485 bus the sketch runs on. But they can be the same for different buses (Every RS485 master has it's own bus).
The slave ID is given according to the BUS ADDR in the interconnect.

e.g 1A2A (Master Arm Panel) shall be Bus Address 1

## Versioning
The version number consists of three digits.

eg: 1.4.2

Those three numbers are release.feature.change

- The last number has to be changed every time there was a change to the sketch. No matter how miniscule.
- The middle number has to be changed every time there is a new complete feature inside the sketch.
- The leading number is only for releases. So only changes if there was a new release of the sketch.
- If any number changes, all subsequent numbers are reset to 0.

Sketches who are untested have to have a leading "u" before the version number and "(untested)" behind the version no.

eg: u.1.4.2 (untested)

## Makefile
Since the OpenHornet project targets multiple Arduino boards and libraries, the `Makefile` located in each sketch directory is necessary to determine which specific Arduino board and libraries are needed for each component.

`LIBRARIES` is a space-separated list of libraries to include in your sketch.  These must first be added as a [git sub-module](#Git-Submodule) in the `/libraries` folder of the project.  See [Arduino Libraries](#Arduino-Libraries)

The appropriate board Makefile must be included at the end of the file.  Valid options are:
- ../../include/mega2560.mk (Mega 2560)
- ../../include/promicro.mk (Sparkfun Pro Micro)
- ../../include/promini.mk (Sparkfun Pro Mini)
- ../../include/s2mini.mk (Wemos S2 Mini)

The Makefile will be used by Github Actions to verify that code will compile without errors. Once successful, a downloadable zip file will be created with the compiled firmware which can be flashed to the desired board.

## Testing your Software

Before you upload anything, please check if your sketch compiles in your Arduino editor. If it does, check if doxygen compiles with your local doxygen installation.

Once those local tests are successfully, open a PR to the OpenHornet-Software repo's develop branch and see if the CI/Doxygen tests are successful. If they are, then request your PR to be reviewed.

## Github Actions
Github Actions is a continuous integration testing engine. It is connected with the OpenHornet Github repository. When a Pull request is opened or merged, Github Actions will checkout the git repo and attempt to compile the code. If the code compiles successfully and there where no errors detected, it automatically updates the Doxygen documentation and uploads it back to the repo.

## Git Submodules
The OpenHornet software projects makes use of [git submodules](https://github.blog/2016-02-01-working-with-submodules/) to include and link to other software libraries which are used in Arduino sketches to provide additional functionality.  Most git frontends and tools support git submodules although options differ between each.

- Git for Windows: Ensure that you select the option to "recursively clone submodules" when you clone the repository.
- Github Desktop: Github Desktop will clone submodules automatically if they exist in the main branch.

## Arduino Libraries
Arduino Libraries which are included in your sketch need to be added as a (git submodule)[#Git-Submodules] to the repository and then referenced in the Makefile for the sketch. You can find a list of already included libraries in the "Supported Software" section of this manual. If you need another library, please add it using `git submodule add https://github.com/<organization>/<project>.git` under the `/libraries` directory.

Libraries are downloaded during each Github Actions run and made available to the sketch when compiling.  Ensure that the libraries are referenced in the Makefile using the same name that they exist in the `/libraries` folder.

## Resources

- http://www.doxygen.org
- https://github.com/DCS-Skunkworks/dcs-bios
- https://github.com/DCS-Skunkworks/dcs-bios-arduino-library/tree/327bf2233603c28989de63dc96e17abecf91ed31
- https://github.blog/2016-02-01-working-with-submodules/
