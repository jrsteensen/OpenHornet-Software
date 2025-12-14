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

/**
* @file openhornet-mt6835-library.zip
* @author Sandra
* @date 04.21.2024
* @version u.0.6.3
* @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
* @warning This sketch is based on a wiring diagram, and was not yet tested on hardware. The throttle solenoids weren't working. (Remove this line once tested on hardware and in system.)
* @brief Controls the THROTTLE QUADRANT.
*
* @details
* 
* @attention contains parts of Simple FOC libraries with license avialable here: https://github.com/simplefoc/Arduino-FOC/blob/master/LICENSE
* 
*/
#include "OpenhornetMT6835.h"

OpenhornetMT6835::OpenhornetMT6835(int nCS, SPISettings settings) : Sensor(), MT6835(settings, nCS) {
    // nix
};

OpenhornetMT6835::~OpenhornetMT6835() {
    // nix
};

float OpenhornetMT6835::getSensorAngle() {
    return getCurrentAngle();
};

void OpenhornetMT6835::init(SPIClass* _spi) {
    this->MT6835::init(_spi);
    this->Sensor::init();
};

