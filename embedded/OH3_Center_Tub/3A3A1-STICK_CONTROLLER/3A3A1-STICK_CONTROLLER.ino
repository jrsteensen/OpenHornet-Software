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
 *   Copyright 2016-2025 OpenHornet
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *     http: ///< www.apache.org/licenses/LICENSE-2.0
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
 * @file 3A3A1-STICK_CONTROLLER.ino
 * @author Thibaud Colodié (@Thib-O)
 * @date 12.31.2025
 * @version u.0.1.0
 * @copyright Copyright 2016-2025 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the FLIGHT CONTROL STICK.
 *
 * @details
 *
 *  * **Reference Designator:** 3A3A1
 *  * **Intended Board:** SparkFun Pro Micro (ATmega32U4)
 *  * **RS485 Bus Address:** N/A
 *
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * 6   | CSg Grip
 * 8   | CSy Pitch axis
 * 9   | CSx Roll axis
 *
 */

#include <SPI.h>
#include <Joystick.h>
#include <EEPROM.h>

// =====================================================
// DEBUG
// =====================================================
#define DEBUG_SERIAL 0 ///< Activate to 1 to Enable DEBUG SERIAL. Keep 0 for Normal use
#define DEBUG_PERIOD_MS 100 ///< print every N ms (avoid spamming)

// =====================================================
// PINS
// =====================================================
static const uint8_t PIN_CS_ROLL  = 9; ///< MT6835 Roll CS
static const uint8_t PIN_CS_PITCH = 8; ///< MT6835 Pitch CS
static const uint8_t PIN_CS_GRIP  = 6; ///< Grip CS

// =====================================================
// SPI settings
// =====================================================
// MT6835 - SPI Mode 3 (CPOL=1, CPHA=1)
SPISettings spiMT(1000000, MSBFIRST, SPI_MODE3);

// TM Grip - SPI Mode 3, 24-bit
SPISettings spiGRIP(1000000, MSBFIRST, SPI_MODE3);

// =====================================================
// MT6835 configuration
// =====================================================
#define MT6835_USE_CRC  1 ///< 1 = verify CRC, 0 = ignore CRC
static const uint8_t  MT_CMD_BURST_ANG = 0b1010; ///< Burst read angle starting at 0x003
static const uint32_t MT_ANGLE_FULL_SCALE = 2097152UL; ///< 2^21

// =====================================================
// Axes configuration
// =====================================================
// Roll total mechanical travel (symmetric)
static const float ROLL_TOTAL_DEG = 31.0f;
static const float ROLL_HALF_DEG  = ROLL_TOTAL_DEG * 0.5f;

// Roll sensor-to-mechanical scale factor
static const float ROLL_SENSOR_TO_MECH_SCALE = 15.5f / 110.0f; ///< ~0.140909

// Pitch total mechanical travel (asymmetric): 2/3 pull (aft), 1/3 push (forward)
static const float PITCH_TOTAL_DEG  = 24.0f;
static const float PITCH_CABRER_DEG = PITCH_TOTAL_DEG * (2.0f / 3.0f); ///< ~16° (aft / pull)
static const float PITCH_PIQUER_DEG = PITCH_TOTAL_DEG * (1.0f / 3.0f); ///< ~8°  (fwd / push)

// If your physical axis direction is reversed, flip here
static const bool INVERT_ROLL  = false;
static const bool INVERT_PITCH = false;

// =====================================================
// Deadzone (degrees around neutral)
// Set to 0.0f to disable; increase if you see jitter around center
// =====================================================
static float DEADZONE_ROLL_DEG  = 0.3f;
static float DEADZONE_PITCH_DEG = 0.3f;

// =====================================================
// Recenter (combo hold)
// =====================================================
static const uint32_t RECENTER_HOLD_MS       = 2000; ///< safe recenter hold time
static const uint32_t RECENTER_FORCE_HOLD_MS = 6000; ///< forced recenter hold time (override safety)

// Safety: allow saving to EEPROM only if current position is close to the current center
static const float RECENTER_MAX_ROLL_ERR_DEG  = 1.2f;
static const float RECENTER_MAX_PITCH_ERR_DEG = 1.2f;

// =====================================================
// EEPROM storage for absolute centers
// =====================================================
// Layout:
// 0..3  : magic
// 4..7  : roll center (u32, 21-bit valid)
// 8..11 : pitch center (u32, 21-bit valid)
static const uint32_t EEPROM_MAGIC = 0x4A53434C; ///< "JSCL"
static const int EEPROM_ADDR_MAGIC = 0;
static const int EEPROM_ADDR_ROLL  = EEPROM_ADDR_MAGIC + 4;
static const int EEPROM_ADDR_PITCH = EEPROM_ADDR_ROLL + 4;

// =====================================================
// HID ranges
// =====================================================
static const int16_t HID_MIN = -32767;
static const int16_t HID_MAX =  32767;

// =====================================================
// HID button indices
// =====================================================
const uint8_t HID_FIRE_TRIGGER_1E_CRAN    = 0; ///< Btn 1
const uint8_t HID_WEAPON_RELEASE          = 1; ///< 2
const uint8_t HID_UNDESIGNATE_NWS         = 2; ///< 3
const uint8_t HID_DISENSAGE_PADDLE        = 3; ///< 4
const uint8_t HID_RECCE_EVENT_MARK        = 4; ///< 5
const uint8_t HID_FIRE_TRIGGER_2E_CRAN    = 5; ///< 6

const uint8_t HID_SENSOR_CONTROL_FORWARD  = 6; ///< 7
const uint8_t HID_SENSOR_CONTROL_RIGHT    = 7; ///< 8
const uint8_t HID_SENSOR_CONTROL_BACKWARD = 8; ///< 9
const uint8_t HID_SENSOR_CONTROL_LEFT     = 9; ///< 10

const uint8_t HID_CASTER_UP               = 10; ///< 11
const uint8_t HID_CASTER_SWITCH           = 11; ///< 12
const uint8_t HID_CASTER_DOWN             = 12; ///< 13

const uint8_t HID_SENSOR_CONTROL_CENTER   = 13; ///< 14

const uint8_t HID_WEAPON_SELECT_FORWARD   = 14; ///< 15
const uint8_t HID_WEAPON_SELECT_RIGHT     = 15; ///< 16
const uint8_t HID_WEAPON_SELECT_BACKWARD  = 16; ///< 17
const uint8_t HID_WEAPON_SELECT_LEFT      = 17; ///< 18
const uint8_t HID_WEAPON_SELECT_CENTER    = 18; ///< 19

// =====================================================
// Grip SPI bit mapping (24 bits)
// =====================================================
const uint8_t BIT_FIRE_TRIGGER_1E_CRAN    = 23;
const uint8_t BIT_WEAPON_RELEASE          = 20;
const uint8_t BIT_UNDESIGNATE_NWS         = 19;
const uint8_t BIT_DISENSAGE_PADDLE        = 18;
const uint8_t BIT_RECCE_EVENT_MARK        = 17;
const uint8_t BIT_FIRE_TRIGGER_2E_CRAN    = 16;

const uint8_t BIT_SENSOR_CONTROL_FORWARD  = 11;
const uint8_t BIT_SENSOR_CONTROL_RIGHT    = 10;
const uint8_t BIT_SENSOR_CONTROL_BACKWARD = 9;
const uint8_t BIT_SENSOR_CONTROL_LEFT     = 8;
const uint8_t BIT_SENSOR_CONTROL_CENTER   = 4;

const uint8_t BIT_CASTER_UP               = 7;
const uint8_t BIT_CASTER_SWITCH           = 6;
const uint8_t BIT_CASTER_DOWN             = 5;

const uint8_t BIT_WEAPON_SELECT_CENTER    = 22;
const uint8_t BIT_WEAPON_SELECT_FORWARD   = 3;
const uint8_t BIT_WEAPON_SELECT_RIGHT     = 2;
const uint8_t BIT_WEAPON_SELECT_BACKWARD  = 1;
const uint8_t BIT_WEAPON_SELECT_LEFT      = 0;

// Trim -> POV hat bits (from your mapping)
const uint8_t BIT_TRIM_NOSE_DOWN          = 15;
const uint8_t BIT_TRIM_NOSE_UP            = 13;
const uint8_t BIT_TRIM_LWD                = 12;
const uint8_t BIT_TRIM_RWD                = 14;

// =====================================================
// Joystick HID descriptor
// =====================================================
Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID,
  JOYSTICK_TYPE_JOYSTICK,
  19, ///< buttons
  1, ///< hat
  true, ///< X (roll)
  true, ///< Y (pitch)
  false, false, false, false,
  false, false, false, false, false
);

// =====================================================
// Types
// =====================================================
/**
 * @brief Parsed result from an MT6835 burst angle read.
 */
struct MT6835_Result
{
  uint32_t angle21; ///< /< 21-bit absolute angle
  uint8_t  status3; ///< /< 3 status bits
  bool     crc_ok; ///< /< CRC check result (or true if CRC disabled)
};

// =====================================================
// State
// =====================================================
static uint32_t lastGrip24 = 0xFFFFFFFF;
static int16_t  lastX = 0x7FFF;
static int16_t  lastY = 0x7FFF;
static int16_t  lastHat = -2;

static uint32_t rollCenter  = 0; ///< 21-bit center reference
static uint32_t pitchCenter = 0; ///< 21-bit center reference

static bool     centersValid = false; ///< true if EEPROM calibration is valid

// Recenter hold detection
static bool     recenterArmed = false;
static uint32_t recenterStartMs = 0;
static bool     recenterDoneThisHold = false;
static bool     recenterSafeTriedThisHold = false;

// =====================================================
// Utility helpers
// =====================================================
/**
 * @brief Return the state of a single bit within a 24-bit value.
 */
static inline bool getBit24(uint32_t v, uint8_t bitIndex)
{
  return (v >> bitIndex) & 0x01;
}

// Compute minimal signed difference of angles in degrees (wrap-around 0..360)
/**
 * @brief Wrap the difference between two angles (degrees) into a continuous range.
 */
static float wrapDiffDeg(float aDeg, float bDeg)
{
  float d = aDeg - bDeg;
  while (d > 180.0f) d -= 360.0f;
  while (d < -180.0f) d += 360.0f;
  return d;
}

// Convert 21-bit MT6835 angle to degrees
/**
 * @brief Convert a 21-bit MT6835 angle value into degrees.
 */
static float angle21ToDeg(uint32_t angle21)
{
  angle21 &= 0x1FFFFF;
  return (360.0f * (float)angle21) / (float)MT_ANGLE_FULL_SCALE;
}

// Symmetric axis mapping with deadzone and post-deadzone renormalization
/**
 * @brief Map a symmetric angle delta (degrees) to a signed 16-bit HID axis value.
 */
static int16_t mapDegToHID_Symmetric(float deltaDeg, float halfRangeDeg, float deadzoneDeg)
{
  // Clamp
  if (deltaDeg >  halfRangeDeg) deltaDeg =  halfRangeDeg;
  if (deltaDeg < -halfRangeDeg) deltaDeg = -halfRangeDeg;

  // Deadzone
  float a = fabs(deltaDeg);
  if (deadzoneDeg < 0) deadzoneDeg = 0;
  if (deadzoneDeg > halfRangeDeg) deadzoneDeg = halfRangeDeg;

  if (a <= deadzoneDeg) return 0;

  // Renormalize to keep full output travel after deadzone
  float scaled = (a - deadzoneDeg) / (halfRangeDeg - deadzoneDeg); ///< 0..1
  float signedNorm = (deltaDeg < 0 ? -scaled : scaled);

  int32_t v = (int32_t)(signedNorm * 32767.0f);
  if (v > HID_MAX) v = HID_MAX;
  if (v < HID_MIN) v = HID_MIN;
  return (int16_t)v;
}

// Asymmetric axis mapping with deadzone and post-deadzone renormalization
// deltaDeg >= 0 uses rangePosDeg (push/forward), deltaDeg < 0 uses rangeNegDeg (pull/aft)
/**
 * @brief Map an asymmetric angle delta (degrees) to a signed 16-bit HID axis value.
 */
static int16_t mapDegToHID_Asymmetric(float deltaDeg, float rangePosDeg, float rangeNegDeg, float deadzoneDeg)
{
  float range = (deltaDeg >= 0) ? rangePosDeg : rangeNegDeg;
  if (range < 0.001f) return 0;

  // Clamp using side-specific range
  if (deltaDeg >  rangePosDeg) deltaDeg =  rangePosDeg;
  if (deltaDeg < -rangeNegDeg) deltaDeg = -rangeNegDeg;

  // Deadzone
  float a = fabs(deltaDeg);
  if (deadzoneDeg < 0) deadzoneDeg = 0;
  if (deadzoneDeg > range) deadzoneDeg = range;

  if (a <= deadzoneDeg) return 0;

  // Renormalize to keep full output travel after deadzone
  float scaled = (a - deadzoneDeg) / (range - deadzoneDeg); ///< 0..1
  float signedNorm = (deltaDeg < 0 ? -scaled : scaled);

  int32_t v = (int32_t)(signedNorm * 32767.0f);
  if (v > HID_MAX) v = HID_MAX;
  if (v < HID_MIN) v = HID_MIN;
  return (int16_t)v;
}

// =====================================================
// EEPROM helpers
// =====================================================
/**
 * @brief Write a 32-bit unsigned value to EEPROM (little-endian).
 */
static void eepromWriteU32(int addr, uint32_t v)
{
  EEPROM.update(addr + 0, (uint8_t)(v >> 0));
  EEPROM.update(addr + 1, (uint8_t)(v >> 8));
  EEPROM.update(addr + 2, (uint8_t)(v >> 16));
  EEPROM.update(addr + 3, (uint8_t)(v >> 24));
}
/**
 * @brief Read a 32-bit unsigned value from EEPROM (little-endian).
 */

static uint32_t eepromReadU32(int addr)
{
  uint32_t v = 0;
  v |= (uint32_t)EEPROM.read(addr + 0) << 0;
  v |= (uint32_t)EEPROM.read(addr + 1) << 8;
  v |= (uint32_t)EEPROM.read(addr + 2) << 16;
  v |= (uint32_t)EEPROM.read(addr + 3) << 24;
  return v;
}

// Returns true if EEPROM contains a valid calibration
/**
 * @brief Load stored roll/pitch center values from EEPROM.
 */
static bool loadCentersFromEEPROM(uint32_t &r, uint32_t &p)
{
  uint32_t magic = eepromReadU32(EEPROM_ADDR_MAGIC);
  if (magic != EEPROM_MAGIC) return false;

  r = eepromReadU32(EEPROM_ADDR_ROLL);
  p = eepromReadU32(EEPROM_ADDR_PITCH);

  // Sanity check for 21-bit values
  if ((r & 0x1FFFFF) != r) return false;
  if ((p & 0x1FFFFF) != p) return false;

  return true;
}

// Save calibration to EEPROM (writes only when you recenter)
/**
 * @brief Save roll/pitch center values to EEPROM.
 */
static void saveCentersToEEPROM(uint32_t r, uint32_t p)
{
  eepromWriteU32(EEPROM_ADDR_MAGIC, EEPROM_MAGIC);
  eepromWriteU32(EEPROM_ADDR_ROLL,  r & 0x1FFFFF);
  eepromWriteU32(EEPROM_ADDR_PITCH, p & 0x1FFFFF);
}
/**
 * @brief Print the stored EEPROM center values to Serial (debug only).
 */

static void debugDumpEEPROMCenters()
{
#if DEBUG_SERIAL
  uint32_t magic = eepromReadU32(EEPROM_ADDR_MAGIC);
  uint32_t r     = eepromReadU32(EEPROM_ADDR_ROLL);
  uint32_t p     = eepromReadU32(EEPROM_ADDR_PITCH);

  Serial.print(F("EEPROM magic=0x"));
  Serial.print(magic, HEX);
  Serial.print(F(" roll="));
  Serial.print(r);
  Serial.print(F(" ("));
  Serial.print(angle21ToDeg(r), 3);
  Serial.print(F(" deg) pitch="));
  Serial.print(p);
  Serial.print(F(" ("));
  Serial.print(angle21ToDeg(p), 3);
  Serial.println(F(" deg)"));
#endif
}

// =====================================================
// MT6835 CRC (CRC-8 poly 0x07, MSB-first over 24 bits)
// The CRC covers 24 bits: ANGLE[20:0] + STATUS[2:0]
// =====================================================
/**
 * @brief Compute CRC-8 over a 24-bit frame (MSB first) for MT6835.
 */
static uint8_t mt_crc8_24bits_msb(uint32_t data24)
{
  uint8_t crc = 0x00;
  for (int i = 23; i >= 0; --i)
  {
    uint8_t bit = (data24 >> i) & 0x01;
    uint8_t c7  = (crc >> 7) & 0x01;
    crc <<= 1;
    if (c7 ^ bit) crc ^= 0x07;
  }
  return crc;
}

// Build MT6835 24-bit frame: [cmd(4) | addr(12) | data(8)]
/**
 * @brief Build a 24-bit MT6835 SPI command frame (with optional CRC).
 */
static inline uint32_t mt_frame(uint8_t cmd4, uint16_t addr12, uint8_t data8)
{
  cmd4  &= 0x0F;
  addr12 &= 0x0FFF;
  return ((uint32_t)cmd4 << 20) | ((uint32_t)addr12 << 8) | (uint32_t)data8;
}

// Burst read angle: reads registers 0x003..0x006 in one CS low
static MT6835_Result readMT6835_burstAngle(uint8_t csPin)
{
  MT6835_Result res{};
  res.angle21 = 0;
  res.status3 = 0;
  res.crc_ok  = true;

  SPI.beginTransaction(spiMT);
  digitalWrite(csPin, LOW);

  // Send burst command starting at 0x003
  uint32_t frame = mt_frame(MT_CMD_BURST_ANG, 0x003, 0x00);
  uint8_t tx0 = (frame >> 16) & 0xFF;
  uint8_t tx1 = (frame >> 8)  & 0xFF;
  uint8_t tx2 = (frame >> 0)  & 0xFF;

  (void)SPI.transfer(tx0);
  (void)SPI.transfer(tx1);
  uint8_t r003 = SPI.transfer(tx2); ///< 0x003
  uint8_t r004 = SPI.transfer(0x00); ///< 0x004
  uint8_t r005 = SPI.transfer(0x00); ///< 0x005
  uint8_t r006 = SPI.transfer(0x00); ///< 0x006 (CRC)

  digitalWrite(csPin, HIGH);
  SPI.endTransaction();

  // Reconstruct ANGLE[20:0] and STATUS[2:0]
  uint32_t angle21 =
      ((uint32_t)r003 << 13) |
      ((uint32_t)r004 << 5)  |
      ((uint32_t)(r005 >> 3) & 0x1F);

  uint8_t status3 = r005 & 0x07;

  res.angle21 = angle21 & 0x1FFFFF;
  res.status3 = status3;

#if MT6835_USE_CRC
  uint32_t data24 = ((res.angle21 & 0x1FFFFF) << 3) | (uint32_t)(res.status3 & 0x07);
  uint8_t crc = mt_crc8_24bits_msb(data24);
  res.crc_ok = (crc == r006);
#else
  (void)r006;
  res.crc_ok = true;
#endif

  return res;
}

// =====================================================
// Grip read (24-bit) - Mode 3
// =====================================================
/**
 * @brief Read the 24-bit Thrustmaster grip frame over SPI.
 */
static uint32_t readGrip24()
{
  SPI.beginTransaction(spiGRIP);
  digitalWrite(PIN_CS_GRIP, LOW);
  delayMicroseconds(1); ///< small settling time

  uint8_t b2 = SPI.transfer(0x00);
  uint8_t b1 = SPI.transfer(0x00);
  uint8_t b0 = SPI.transfer(0x00);

  digitalWrite(PIN_CS_GRIP, HIGH);
  SPI.endTransaction();

  return ((uint32_t)b2 << 16) | ((uint32_t)b1 << 8) | (uint32_t)b0;
}

// =====================================================
// POV hat from Trim bits
// Note: Up/Down were inverted, so we swap those here.
// =====================================================
/**
 * @brief Compute HID hat-switch value from trim bit states.
 */
static int16_t computeHatFromTrim(uint32_t grip24)
{
  bool up    = getBit24(grip24, BIT_TRIM_NOSE_DOWN); ///< swapped
  bool down  = getBit24(grip24, BIT_TRIM_NOSE_UP); ///< swapped
  bool left  = getBit24(grip24, BIT_TRIM_LWD);
  bool right = getBit24(grip24, BIT_TRIM_RWD);

  // Joystick library: -1 = released, otherwise 0..315 in 45° steps
  if (!up && !down && !left && !right) return -1;

  if (up && right)   return 45;
  if (up && left)    return 315;
  if (down && right) return 135;
  if (down && left)  return 225;

  if (up)    return 0;
  if (right) return 90;
  if (down)  return 180;
  if (left)  return 270;

  return -1;
}

// =====================================================
// Apply grip bits to HID buttons
// =====================================================
/**
 * @brief Update joystick button states based on the grip bitfield.
 */
static void updateButtonsFromGrip(uint32_t g)
{
  Joystick.setButton(HID_FIRE_TRIGGER_1E_CRAN,    getBit24(g, BIT_FIRE_TRIGGER_1E_CRAN));
  Joystick.setButton(HID_WEAPON_RELEASE,          getBit24(g, BIT_WEAPON_RELEASE));
  Joystick.setButton(HID_UNDESIGNATE_NWS,         getBit24(g, BIT_UNDESIGNATE_NWS));
  Joystick.setButton(HID_DISENSAGE_PADDLE,        getBit24(g, BIT_DISENSAGE_PADDLE));
  Joystick.setButton(HID_RECCE_EVENT_MARK,        getBit24(g, BIT_RECCE_EVENT_MARK));
  Joystick.setButton(HID_FIRE_TRIGGER_2E_CRAN,    getBit24(g, BIT_FIRE_TRIGGER_2E_CRAN));

  Joystick.setButton(HID_SENSOR_CONTROL_FORWARD,  getBit24(g, BIT_SENSOR_CONTROL_FORWARD));
  Joystick.setButton(HID_SENSOR_CONTROL_RIGHT,    getBit24(g, BIT_SENSOR_CONTROL_RIGHT));
  Joystick.setButton(HID_SENSOR_CONTROL_BACKWARD, getBit24(g, BIT_SENSOR_CONTROL_BACKWARD));
  Joystick.setButton(HID_SENSOR_CONTROL_LEFT,     getBit24(g, BIT_SENSOR_CONTROL_LEFT));
  Joystick.setButton(HID_SENSOR_CONTROL_CENTER,   getBit24(g, BIT_SENSOR_CONTROL_CENTER));

  Joystick.setButton(HID_CASTER_UP,               getBit24(g, BIT_CASTER_UP));
  Joystick.setButton(HID_CASTER_SWITCH,           getBit24(g, BIT_CASTER_SWITCH));
  Joystick.setButton(HID_CASTER_DOWN,             getBit24(g, BIT_CASTER_DOWN));

  Joystick.setButton(HID_WEAPON_SELECT_FORWARD,   getBit24(g, BIT_WEAPON_SELECT_FORWARD));
  Joystick.setButton(HID_WEAPON_SELECT_RIGHT,     getBit24(g, BIT_WEAPON_SELECT_RIGHT));
  Joystick.setButton(HID_WEAPON_SELECT_BACKWARD,  getBit24(g, BIT_WEAPON_SELECT_BACKWARD));
  Joystick.setButton(HID_WEAPON_SELECT_LEFT,      getBit24(g, BIT_WEAPON_SELECT_LEFT));
  Joystick.setButton(HID_WEAPON_SELECT_CENTER,    getBit24(g, BIT_WEAPON_SELECT_CENTER));
}

// =====================================================
// Recenter combo detection (RECCE EVENT + FIRE TRIGGER 1st detent)
/**
 * @brief Check whether the recenter button-combo is currently pressed.
 */
static bool recenterComboPressed(uint32_t grip24)
{
  bool recce = getBit24(grip24, BIT_RECCE_EVENT_MARK);
  bool trig1 = getBit24(grip24, BIT_FIRE_TRIGGER_1E_CRAN);
  return recce && trig1;
}

// =====================================================
// Debug print (throttled)
// =====================================================
/**
 * @brief Print current sensor/joystick state to Serial (debug only).
 */
static void debugPrint(uint32_t grip24,
                       const MT6835_Result& roll, float rollDeg, float rollDelta,
                       const MT6835_Result& pitch, float pitchDeg, float pitchDelta)
{
#if DEBUG_SERIAL
  static uint32_t lastMs = 0;
  uint32_t now = millis();
  if (now - lastMs < DEBUG_PERIOD_MS) return;
  lastMs = now;

  Serial.print(F("[R] deg="));
  Serial.print(rollDeg, 3);
  Serial.print(F(" d(sensor)="));
  Serial.print(rollDelta, 3);
  Serial.print(F(" st="));
  Serial.print(roll.status3);
  Serial.print(F(" crc="));
  Serial.print(roll.crc_ok ? F("OK") : F("BAD"));

  Serial.print(F(" | [P] deg="));
  Serial.print(pitchDeg, 3);
  Serial.print(F(" d="));
  Serial.print(pitchDelta, 3);
  Serial.print(F(" st="));
  Serial.print(pitch.status3);
  Serial.print(F(" crc="));
  Serial.print(pitch.crc_ok ? F("OK") : F("BAD"));

  Serial.print(F(" | GRIP=0x"));
  Serial.println(grip24, HEX);
#endif
}

// =====================================================
// Setup
// =====================================================
/**
 * @brief Arduino setup routine. Initializes peripherals and joystick interface.
 */
void setup()
{
#if DEBUG_SERIAL
  Serial.begin(115200);
  delay(400);
#endif

  // CS pins idle high
  pinMode(PIN_CS_ROLL, OUTPUT);
  pinMode(PIN_CS_PITCH, OUTPUT);
  pinMode(PIN_CS_GRIP, OUTPUT);

  digitalWrite(PIN_CS_ROLL, HIGH);
  digitalWrite(PIN_CS_PITCH, HIGH);
  digitalWrite(PIN_CS_GRIP, HIGH);

  SPI.begin();

  // HID init
  Joystick.setXAxisRange(HID_MIN, HID_MAX);
  Joystick.setYAxisRange(HID_MIN, HID_MAX);
  Joystick.begin();

  // Load absolute centers from EEPROM if valid; otherwise initialize once from current position
  centersValid = loadCentersFromEEPROM(rollCenter, pitchCenter);

#if DEBUG_SERIAL
  if (centersValid) Serial.println(F("EEPROM centers loaded (valid)."));
  else              Serial.println(F("EEPROM centers not valid (will init from boot position)."));
#endif

  if (!centersValid)
  {
    delay(50);
    for (int i = 0; i < 8; i++)
    {
      MT6835_Result r = readMT6835_burstAngle(PIN_CS_ROLL);
      if (r.crc_ok) rollCenter = r.angle21;

      MT6835_Result p = readMT6835_burstAngle(PIN_CS_PITCH);
      if (p.crc_ok) pitchCenter = p.angle21;

      delay(2);
    }

    saveCentersToEEPROM(rollCenter, pitchCenter);
    centersValid = true;

#if DEBUG_SERIAL
    Serial.println(F("EEPROM centers initialized from boot position (now valid)."));
#endif
  }

#if DEBUG_SERIAL
  debugDumpEEPROMCenters();
#endif
}

// =====================================================
// Main loop
// =====================================================
/**
 * @brief Arduino main loop. Reads sensors/grip and updates the HID report.
 */
void loop()
{
  // ----- Read MT6835 sensors -----
  MT6835_Result roll  = readMT6835_burstAngle(PIN_CS_ROLL);
  MT6835_Result pitch = readMT6835_burstAngle(PIN_CS_PITCH);

  // Keep last valid degrees if CRC fails (avoid jumps)
  static float rollDegPrev  = 0.0f;
  static float pitchDegPrev = 0.0f;

  float rollDeg  = rollDegPrev;
  float pitchDeg = pitchDegPrev;

  if (roll.crc_ok)  { rollDeg  = angle21ToDeg(roll.angle21);  rollDegPrev  = rollDeg; }
  if (pitch.crc_ok) { pitchDeg = angle21ToDeg(pitch.angle21); pitchDegPrev = pitchDeg; }

  // Compute deltas versus current centers
  float rollCenterDeg  = angle21ToDeg(rollCenter);
  float pitchCenterDeg = angle21ToDeg(pitchCenter);

  float rollDelta  = wrapDiffDeg(rollDeg,  rollCenterDeg);
  float pitchDelta = wrapDiffDeg(pitchDeg, pitchCenterDeg);

  if (INVERT_ROLL)  rollDelta  = -rollDelta;
  if (INVERT_PITCH) pitchDelta = -pitchDelta;

  // ----- Read grip (24-bit) -----
  uint32_t grip24 = readGrip24();

  // Your grip is active-low: 0 = pressed, 1 = released
  // Invert so that 1 = pressed for our mapping.
  grip24 = ~grip24 & 0xFFFFFF;

  // ----- Recenter (SAFE 2s / FORCED 6s with correct behavior) -----
  bool combo = recenterComboPressed(grip24);

  if (combo)
  {
    if (!recenterArmed)
    {
      recenterArmed = true;
      recenterStartMs = millis();
      recenterDoneThisHold = false;
      recenterSafeTriedThisHold = false;
    }
    else if (!recenterDoneThisHold)
    {
      uint32_t held = millis() - recenterStartMs;

      // Forced recenter has priority once you reach the forced hold time
      if (held >= RECENTER_FORCE_HOLD_MS)
      {
        if (roll.crc_ok && pitch.crc_ok)
        {
          rollCenter  = roll.angle21;
          pitchCenter = pitch.angle21;
          saveCentersToEEPROM(rollCenter, pitchCenter);
          centersValid = true;

#if DEBUG_SERIAL
          Serial.println(F("RECENTER FORCED (saved)."));
          debugDumpEEPROMCenters();
#endif
        }
        recenterDoneThisHold = true;
      }
      // Try safe recenter once at 2s; if refused, keep holding to allow forced at 6s
      else if (held >= RECENTER_HOLD_MS && !recenterSafeTriedThisHold)
      {
        recenterSafeTriedThisHold = true;

        // If EEPROM calibration was not valid, treat SAFE as forced init
        if (!centersValid)
        {
          if (roll.crc_ok && pitch.crc_ok)
          {
            rollCenter  = roll.angle21;
            pitchCenter = pitch.angle21;
            saveCentersToEEPROM(rollCenter, pitchCenter);
            centersValid = true;

#if DEBUG_SERIAL
            Serial.println(F("RECENTER (EEPROM invalid -> forced init) saved."));
            debugDumpEEPROMCenters();
#endif
          }
          recenterDoneThisHold = true;
        }
        else
        {
          // Safe recenter: allow only if near current center
          float rErr = fabs(rollDelta);
          float pErr = fabs(pitchDelta);
          bool okPos = (rErr <= RECENTER_MAX_ROLL_ERR_DEG) && (pErr <= RECENTER_MAX_PITCH_ERR_DEG);

          if (okPos && roll.crc_ok && pitch.crc_ok)
          {
            rollCenter  = roll.angle21;
            pitchCenter = pitch.angle21;
            saveCentersToEEPROM(rollCenter, pitchCenter);

#if DEBUG_SERIAL
            Serial.print(F("RECENTER SAFE OK (saved). rErr="));
            Serial.print(rErr, 3);
            Serial.print(F(" pErr="));
            Serial.println(pErr, 3);
            debugDumpEEPROMCenters();
#endif
            recenterDoneThisHold = true;
          }
          else
          {
#if DEBUG_SERIAL
            Serial.print(F("RECENTER SAFE REFUSED. rErr="));
            Serial.print(rErr, 3);
            Serial.print(F(" pErr="));
            Serial.print(pErr, 3);
            Serial.println(F(" -> Keep holding to 6s for FORCED recenter."));
#endif
            // Do not set recenterDoneThisHold here: allow forced recenter later
          }
        }
      }
    }
  }
  else
  {
    // Combo released: reset hold state
    recenterArmed = false;
    recenterDoneThisHold = false;
    recenterSafeTriedThisHold = false;
  }

  // ----- Map axes to HID -----
  // Roll: apply sensor->mechanical scaling, then symmetric mapping + deadzone
  float rollDeltaMech = rollDelta * ROLL_SENSOR_TO_MECH_SCALE;
  int16_t x = mapDegToHID_Symmetric(rollDeltaMech, ROLL_HALF_DEG, DEADZONE_ROLL_DEG);

  // Pitch: asymmetric mapping + deadzone
  // Convention here: pitchDelta > 0 => forward (push / "piquer"), pitchDelta < 0 => aft (pull / "cabrer")
  int16_t y = mapDegToHID_Asymmetric(
    pitchDelta,
    /*rangePosDeg*/ PITCH_PIQUER_DEG,
    /*rangeNegDeg*/ PITCH_CABRER_DEG,
    DEADZONE_PITCH_DEG
  );

  // ----- Hat + Buttons -----
  int16_t hat = computeHatFromTrim(grip24);

  // Update HID only when changed (reduces USB traffic)
  if (x != lastX) { Joystick.setXAxis(x); lastX = x; }
  if (y != lastY) { Joystick.setYAxis(y); lastY = y; }

  if (grip24 != lastGrip24)
  {
    updateButtonsFromGrip(grip24);
    lastGrip24 = grip24;
  }

  if (hat != lastHat)
  {
    Joystick.setHatSwitch(0, hat);
    lastHat = hat;
  }

  // ----- Debug -----
  debugPrint(grip24, roll, rollDeg, rollDelta, pitch, pitchDeg, pitchDelta);

  delay(1);
}