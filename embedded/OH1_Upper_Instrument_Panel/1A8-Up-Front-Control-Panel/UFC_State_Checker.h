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
 * @file    UFC_State_Checker.h
 * @author  Sandra Carroll
 * @date    09.05.2026
 * @version 1.0.0
 * @copyright Copyright 2016-2026 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief   UFC overall operational state and power-source detection class.
 *
 * @details Encapsulates the two concerns that determine how the UFC behaves at
 *          any given moment:
 *
 *          **Power source** — whether the board is running from the cockpit 5V bus
 *          (EXTERNAL) or from USB only (USB).  Replaces the standalone
 *          @c powerSource() function and the @c EXT5V / @c USB5V boolean #defines
 *          that previously lived in the main .cpp.
 *
 *          **Operational state** — a higher-level view combining OTA mode, boot
 *          splash activity, power source, interrupt health, and any future states
 *          (DCS connectivity, etc.).  Replaces the incomplete @c getUfcState()
 *          free function.
 *
 *          Usage:
 *          @code
 *          // In main .cpp (global scope, after pin #defines):
 *          UfcState ufcState(VCC_SNS_3_3V);
 *
 *          // In setup():
 *          ufcState.begin();        // configures pin
 *          ufcState.detectPower();  // reads and caches power source
 *
 *          // In loop() — all four user-selected configurations visible at once:
 *          //   otaModeActive   = user held ENC1PB at boot
 *          //   true            = boot splash configured on
 *          //   irqModeEnabled  = USE_IO_INT or USE_ADC_RDY defined
 *          UfcState::Snapshot s = ufcState.getState(otaModeActive, true, irqModeEnabled);
 *          if (s.power   == UfcState::State::EXTPWR)   { }  // cockpit bus power present
 *          if (s.otamode == UfcState::State::OTA)      { WebOtaUpdate::handle(); }
 *          if (s.bootmode== UfcState::State::BOOTMSG)  { }  // show boot splash
 *          if (s.irqmode == UfcState::State::POLLING)  { }  // using polling, not IRQ
 *          @endcode
 */

#pragma once

#ifndef UFC_STATE_CHECKER_H
#define UFC_STATE_CHECKER_H

#include <Arduino.h>
#include <Preferences.h>  // ESP32 NVS key-value store (replaces emulated EEPROM; works identically on S2 and S3)

// ── Logging abstraction ───────────────────────────────────────────────────────
// If ENABLE_ELOG is defined (by the including translation unit before this header),
// route all UFC state messages through Elog at INFO level.  The Elog instance and
// <Elog.h> must already be included; this header does not include them itself.
//
// The caller can override the Elog channel by defining UFC_STATE_LOG_CH before
// including this header.  Default channel is 0 (MYLOG in the main .cpp).
//
// Serial fallback: uses Serial.printf so both plain strings and format specifiers
// work identically with the same macro call.
//
#ifndef UFC_STATE_LOG_CH
  #define UFC_STATE_LOG_CH 0
#endif

#if defined(ENABLE_ELOG)
  #define _UFC_LOG(fmt, ...)  Logger.log(UFC_STATE_LOG_CH, ELOG_LEVEL_INFO, "[UfcState] " fmt, ##__VA_ARGS__)
#else
  #define _UFC_LOG(fmt, ...)  Serial.printf("[UfcState] " fmt "\n", ##__VA_ARGS__)
#endif

/**
 * @brief  UFC operational state, power-source detection, and runtime option persistence.
 *
 * @details Combines three responsibilities:
 *
 *          1. **Power detection** — reads the VCC_SNS sense pin to determine whether
 *             the board is on external 5V (cockpit bus) or USB power.
 *
 *          2. **Runtime option tracking** — holds three user-selectable configuration
 *             flags as @c State enum values in a @c Snapshot struct.  Options are set
 *             via dedicated setters and are independent of each other:
 *             - @c otamode  — OTA firmware update mode (IMPLEMENTED: set at boot via ENC1PB).
 *             - @c bootmode — boot message splash display (PLACEHOLDER: not yet runtime-settable).
 *             - @c irqmode  — interrupt-driven vs polling input mode (PLACEHOLDER: not yet runtime-settable).
 *
 *          3. **NVS persistence** — save() and load() store the runtime options to
 *             ESP32 Non-Volatile Storage (Preferences / NVS flash).  This is the
 *             correct mechanism for both ESP32-S2 and ESP32-S3; emulated EEPROM
 *             is not used.  The power field is never persisted (always re-read from
 *             hardware).
 *
 *          @c getState() refreshes the power field and returns the full @c Snapshot.
 *          Use the setter methods to change runtime options; call save() to persist them.
 */
class UfcState {
public:

  // ── Unified state enum ────────────────────────────────────────────────────

  /**
   * @brief  All possible state token values used across every Snapshot field.
   *
   * @details A single enum provides the vocabulary for all dimensions — power
   *          source, OTA selection, boot message preference, and input mode.
   *          Each Snapshot field uses only the subset of values meaningful for
   *          that dimension.  UNKNOWN is the initial/unconfigured value.
   *
   * @note    EXTERNAL is a macro defined as 0 in Arduino.h; the external-power
   *          token is therefore named EXTPWR to avoid a preprocessor collision.
   */
  enum class State : uint8_t {
    UNKNOWN,   ///< Field not yet evaluated or option not yet configured.
    // ── power field tokens ──
    EXTPWR,    ///< External 5V cockpit bus supply detected on VCC_SNS (HIGH).
    USBPWR,    ///< USB-only power detected on VCC_SNS (LOW).
    // ── otamode field tokens ──
    OTA,       ///< IMPLEMENTED — user held ENC1PB at boot; OTA web server active.
    NOOTA,     ///< IMPLEMENTED — ENC1PB not held; normal firmware operation.
    // ── bootmode field tokens ──
    BOOTMSG,   ///< PLACEHOLDER — user preference: show boot message splash screen.
    NOBOOTMSG, ///< PLACEHOLDER — user preference: skip boot message splash screen.
    // ── irqmode field tokens ──
    IRQ,       ///< PLACEHOLDER — user preference: interrupt-driven keyboard and ADC input.
    POLLING    ///< PLACEHOLDER — user preference: polling-driven keyboard and ADC input.
  };

  // ── Snapshot: all concurrent state dimensions ─────────────────────────────

  /**
   * @brief  Complete snapshot of all concurrent UFC state and configuration dimensions.
   *
   * @details Each field is independently maintained so callers always have the full
   *          picture — no dimension hides another.  Runtime-configurable fields are
   *          set via the setter methods and persisted via save() / load().
   *
   *          | Field      | Type     | Tokens / Range            | Status               |
   *          |------------|----------|---------------------------|----------------------|
   *          | power      | State    | EXTPWR, USBPWR, UNKNOWN   | Hardware + persisted |
   *          | otamode    | State    | OTA, NOOTA                | IMPLEMENTED          |
   *          | bootmode   | State    | BOOTMSG, NOBOOTMSG        | PLACEHOLDER          |
   *          | irqmode    | State    | IRQ, POLLING              | PLACEHOLDER          |
   *          | brightness | uint8_t  | 0–255                     | Set by setBrightness |
   *          | maxCurrent | uint16_t | mA (default 500)          | Set by setMaxCurrent |
   *          | minCurrent | uint16_t | mA (default 38)           | Set by setMinCurrent |
   *
   * @note    @c power is re-read from hardware by detectPower() / getState() on
   *          every call, but the last detected value is also persisted to NVS so
   *          it can be read back before the first detectPower() call on next boot.
   */
  struct Snapshot {
    State   power      = State::UNKNOWN; ///< Detected 5V supply: EXTPWR | USBPWR. Set by detectPower(); persisted.
    State   otamode    = State::NOOTA;   ///< OTA selection (IMPLEMENTED): OTA | NOOTA. Set by setOtaMode(); persisted.
    State   bootmode   = State::BOOTMSG; ///< Boot splash preference (PLACEHOLDER): BOOTMSG | NOBOOTMSG. Set by setBootMode(); persisted.
    State   irqmode    = State::POLLING; ///< Input mode (PLACEHOLDER): IRQ | POLLING. Set by setIrqMode(); persisted.
    uint8_t  brightness  = 128; ///< UFC display brightness 0–255. Set by setBrightness(); persisted.
    uint16_t maxCurrent  = 500; ///< Maximum allowed backlight current in mA. Set by setMaxCurrent(); persisted.
    uint16_t minCurrent  = 38;  ///< Minimum operating current in mA. Set by setMinCurrent(); persisted.
  };

  // ── Construction and initialisation ──────────────────────────────────────

  /**
   * @brief  Construct a UfcState monitor bound to the VCC_SNS sense pin.
   *
   * @details Snapshot fields are initialised to their default values (NOOTA,
   *          BOOTMSG, POLLING, brightness=128).  Call load() after begin() to
   *          restore any previously saved user preferences from NVS.
   *
   * @param vccSnsPin  Arduino GPIO number of the VCC_SNS sense pin.
   */
  explicit UfcState(uint8_t vccSnsPin)
    : _vccSnsPin(vccSnsPin), _snapshot() {}

  /**
   * @brief  Configure the VCC_SNS pin as INPUT_PULLUP.
   *
   * @details Call once in setup() before detectPower() or getState().
   *          Kept separate from the constructor so the Arduino framework is
   *          fully initialised before pinMode() is called.
   */
  void begin() {
    pinMode(_vccSnsPin, INPUT_PULLUP);
  }

  // ── Power source detection ────────────────────────────────────────────────

  /**
   * @brief  Read the VCC_SNS pin, update the snapshot power field, and return it.
   *
   * @details HIGH = external 5V cockpit bus (@c EXTPWR); LOW = USB only (@c USBPWR).
   *          The power field is NOT persisted — it reflects live hardware state.
   *
   * @return @c State::EXTPWR or @c State::USBPWR.
   */
  State detectPower() {
    if (digitalRead(_vccSnsPin) == HIGH) {
      _UFC_LOG("External power detected (VCC_SNS HIGH)");
      _snapshot.power = State::EXTPWR;
    } else {
      _UFC_LOG("USB power only (VCC_SNS LOW)");
      _snapshot.power = State::USBPWR;
    }
    return _snapshot.power;
  }

  // ── Runtime option setters ────────────────────────────────────────────────

  /**
   * @brief  Set the OTA mode option.  (IMPLEMENTED)
   *
   * @details Typically called once in setup() after reading ENC1PB:
   *          @code
   *          ufcState.setOtaMode(digitalRead(ENC1PB) == LOW);
   *          @endcode
   *          When enabled, the caller is responsible for starting WebOtaUpdate::begin()
   *          and calling WebOtaUpdate::handle() in loop().  This option is persisted
   *          by save() / loaded by load().
   *
   * @param enabled  true = OTA mode active (@c State::OTA);
   *                 false = normal operation (@c State::NOOTA).
   */
  void setOtaMode(bool enabled) {
    _snapshot.otamode = enabled ? State::OTA : State::NOOTA;
  }

  /**
   * @brief  Set the boot message splash preference.  (PLACEHOLDER — not yet implemented)
   *
   * @details When implemented, this will allow the user to enable or disable the
   *          boot splash screen at runtime (e.g. via a long-press or menu option)
   *          rather than it being hardcoded.  Persisted by save() / loaded by load().
   *
   * @todo    Wire this to a runtime trigger (button combo, serial command, etc.)
   *          and call bootMessage() accordingly in the main loop.
   *
   * @param enabled  true = show splash (@c State::BOOTMSG);
   *                 false = skip splash (@c State::NOBOOTMSG).
   */
  void setBootMode(bool enabled) {
    // @todo: act on this change once runtime boot-message toggle is implemented.
    _snapshot.bootmode = enabled ? State::BOOTMSG : State::NOBOOTMSG;
  }

  /**
   * @brief  Set the keyboard/ADC input processing mode.  (PLACEHOLDER — not yet implemented)
   *
   * @details When implemented, this will allow runtime switching between
   *          interrupt-driven and polling input modes without recompiling.
   *          Persisted by save() / loaded by load().
   *
   * @todo    Wire to interrupt attach/detach logic for TCA8418 and ADS1115 when
   *          runtime switching is implemented.
   *
   * @param enabled  true = interrupt-driven (@c State::IRQ);
   *                 false = polling (@c State::POLLING).
   */
  void setIrqMode(bool enabled) {
    // @todo: attach/detach interrupts here once runtime IRQ switching is implemented.
    _snapshot.irqmode = enabled ? State::IRQ : State::POLLING;
  }

  /**
   * @brief  Set the UFC display brightness level.
   *
   * @details Stores the brightness value in the Snapshot so it is included in
   *          save() / load() persistence.  The caller is responsible for applying
   *          the value to the actual display hardware (TM1640 setupDisplay(),
   *          scratchpad set_backlight_brightness(), FastLED setBrightness(), etc.)
   *          — this setter only records the intent.
   *
   *          Typical call site: inside @c UFC_BRTChange() after mapping the
   *          DCS-BIOS value to 0–255:
   *          @code
   *          ufcState.setBrightness(mappedBrightness);
   *          @endcode
   *
   * @param value  Brightness level 0–255 (0 = off, 255 = maximum).
   */
  void setBrightness(uint8_t value) {
    _snapshot.brightness = value;
  }

  /**
   * @brief  Return the last brightness value set by setBrightness().
   * @return Brightness level 0–255; 128 if setBrightness() has not been called.
   */
  uint8_t brightness() const { return _snapshot.brightness; }

  /**
   * @brief  Set the maximum allowed backlight/LED current limit.
   *
   * @details Stored in the Snapshot and persisted by save() / load().  The caller
   *          is responsible for enforcing this limit in hardware (e.g. by capping
   *          the FastLED global brightness or the scratchpad PWM duty cycle so that
   *          the total LED current does not exceed this value).
   *
   *          Default: 500 mA (suitable for external 5V cockpit bus supply).
   *
   * @param mA  Maximum current in milliamps.
   */
  void setMaxCurrent(uint16_t mA) { _snapshot.maxCurrent = mA; }

  /**
   * @brief  Return the stored maximum current limit.
   * @return Maximum current in mA; 500 if setMaxCurrent() has not been called.
   */
  uint16_t maxCurrent() const { return _snapshot.maxCurrent; }

  /**
   * @brief  Set the minimum operating current floor.
   *
   * @details Stored in the Snapshot and persisted by save() / load().  Can be used
   *          to ensure displays remain at least dimly lit on USB power where the
   *          total available current is constrained.
   *
   *          Default: 38 mA (lowest practical brightness for all displays combined).
   *
   * @param mA  Minimum current in milliamps.
   */
  void setMinCurrent(uint16_t mA) { _snapshot.minCurrent = mA; }

  /**
   * @brief  Return the stored minimum current floor.
   * @return Minimum current in mA; 38 if setMinCurrent() has not been called.
   */
  uint16_t minCurrent() const { return _snapshot.minCurrent; }

  // ── Bool convenience accessors ────────────────────────────────────────────

  /**
   * @brief  Return true if interrupt-driven input mode is active.
   * @details Equivalent to @c snapshot().irqmode == State::IRQ.
   *          Use this in loop() instead of @c #if defined(USE_ADC_RDY).
   * @return @c true = IRQ mode; @c false = polling mode.
   */
  bool getIrqMode()  const { return _snapshot.irqmode  == State::IRQ; }

  /**
   * @brief  Return true if the boot splash screen is configured to show.
   * @details Equivalent to @c snapshot().bootmode == State::BOOTMSG.
   * @return @c true = show boot splash; @c false = skip it.
   */
  bool getBootMode() const { return _snapshot.bootmode == State::BOOTMSG; }

  /**
   * @brief  Return true if OTA firmware update mode is active.
   * @details Equivalent to @c snapshot().otamode == State::OTA.
   * @return @c true = OTA active; @c false = normal operation.
   */
  bool getOtaMode()  const { return _snapshot.otamode  == State::OTA; }

  // ── Full state query ──────────────────────────────────────────────────────

  /**
   * @brief  Refresh the power field from hardware and return the complete Snapshot.
   *
   * @details Re-reads the VCC_SNS pin via detectPower() and returns the full
   *          internal Snapshot.  Runtime option fields (otamode, bootmode, irqmode)
   *          are not changed by this call — update them via the setter methods.
   *
   * @return The current @c Snapshot with a freshly read power field.
   */
  Snapshot getState() {
    detectPower();
    // @todo: populate dcsmode field here once DCS-BIOS state tracking is added.
    return _snapshot;
  }

  /**
   * @brief  Return the cached Snapshot without re-reading any hardware.
   * @return The last @c Snapshot returned by getState() or set by the setters.
   */
  Snapshot snapshot() const { return _snapshot; }

  // ── NVS persistence ───────────────────────────────────────────────────────

  /**
   * @brief  Save all persistent state fields to ESP32 NVS flash.
   *
   * @details Uses the Arduino @c Preferences library (NVS key-value store), which
   *          works identically on ESP32-S2 and ESP32-S3.
   *
   *          Written to namespace @c "ufc":
   *          | Key        | Type     | Content                                             |
   *          |------------|----------|-----------------------------------------------------|
   *          | @c "magic" | uint32_t | Eyecatcher: @c NVS_MAGIC (0x4F485546 "OHUF")        |
   *          | @c "ver"   | uint8_t  | Schema version: @c NVS_VERSION                      |
   *          | @c "otam"  | uint8_t  | otamode enum value                                  |
   *          | @c "bootm" | uint8_t  | bootmode enum value                                 |
   *          | @c "irqm"  | uint8_t  | irqmode enum value                                  |
   *          | @c "pwr"    | uint8_t  | last detected power enum value                           |
   *          | @c "brt"    | uint8_t  | brightness 0–255                                         |
   *          | @c "maxcur" | uint16_t | maximum current limit in mA                              |
   *          | @c "mincur" | uint16_t | minimum current floor in mA                              |
   *          | @c "crc"    | uint8_t  | CRC-8/SMBUS over (ver,otam,bootm,irqm,pwr,brt,maxcur×2,mincur×2) |
   *
   * @return @c true if the namespace opened and all keys were written.
   * @return @c false if NVS is unavailable.
   */
  bool save() {
    // uint16_t fields are split into high/low bytes for CRC coverage
    const uint8_t data[10] = {
      NVS_VERSION,
      static_cast<uint8_t>(_snapshot.otamode),
      static_cast<uint8_t>(_snapshot.bootmode),
      static_cast<uint8_t>(_snapshot.irqmode),
      static_cast<uint8_t>(_snapshot.power),
      _snapshot.brightness,
      static_cast<uint8_t>(_snapshot.maxCurrent >> 8),
      static_cast<uint8_t>(_snapshot.maxCurrent & 0xFF),
      static_cast<uint8_t>(_snapshot.minCurrent >> 8),
      static_cast<uint8_t>(_snapshot.minCurrent & 0xFF)
    };

    Preferences prefs;
    if (!prefs.begin("ufc", /*readOnly=*/false)) {
      _UFC_LOG("NVS save: failed to open namespace");
      return false;
    }
    prefs.putUInt("magic",  NVS_MAGIC);
    prefs.putUChar("ver",   data[0]);
    prefs.putUChar("otam",  data[1]);
    prefs.putUChar("bootm", data[2]);
    prefs.putUChar("irqm",  data[3]);
    prefs.putUChar("pwr",   data[4]);
    prefs.putUChar("brt",   data[5]);
    prefs.putUShort("maxcur", _snapshot.maxCurrent);
    prefs.putUShort("mincur", _snapshot.minCurrent);
    prefs.putUChar("crc",   _crc8(data, sizeof(data)));
    prefs.end();

    _UFC_LOG("NVS save: OK");
    return true;
  }

  /**
   * @brief  Load and validate all persistent state fields from ESP32 NVS flash.
   *
   * @details Performs three integrity checks before applying any data:
   *          1. **Eyecatcher** — @c "magic" must equal @c NVS_MAGIC.
   *          2. **Schema version** — @c "ver" must equal @c NVS_VERSION; a version
   *             mismatch means the saved data has a different field layout and must
   *             be discarded (call clearSaved() then save() to reinitialise).
   *          3. **CRC-8** — recomputed over (ver, otam, bootm, irqm, pwr, brt,
   *             maxcur×2 bytes, mincur×2 bytes) and compared against @c "crc";
   *             detects bit-level flash corruption.
   *
   *          If any check fails the Snapshot is left at its defaults and @c false
   *          is returned.  detectPower() still overwrites the power field on the
   *          next call regardless of what was loaded.
   *
   *          First-boot defaults (no valid NVS data):
   *          - @c otamode    → @c NOOTA   — OTA disabled
   *          - @c bootmode   → @c BOOTMSG — show boot splash
   *          - @c irqmode    → @c POLLING — polling mode
   *          - @c power      → @c UNKNOWN — not yet detected
   *          - @c brightness → 128        — mid-range
   *          - @c maxCurrent → 500 mA     — external bus supply limit
   *          - @c minCurrent → 38 mA      — minimum operating current
   *
   * @return @c true  — all checks passed; Snapshot updated from NVS.
   * @return @c false — namespace absent, eyecatcher mismatch, version mismatch,
   *                    or CRC failure; defaults remain in effect.
   */
  bool load() {
    Preferences prefs;
    if (!prefs.begin("ufc", /*readOnly=*/true)) {
      _UFC_LOG("NVS load: namespace not found, using defaults");
      return false;
    }

    // ── Eyecatcher ────────────────────────────────────────────────────────
    const uint32_t magic = prefs.getUInt("magic", 0);
    if (magic != NVS_MAGIC) {
      prefs.end();
      _UFC_LOG("NVS load: eyecatcher mismatch (0x%08X), using defaults", magic);
      return false;
    }

    // ── Schema version ────────────────────────────────────────────────────
    const uint8_t ver = prefs.getUChar("ver", 0);
    if (ver != NVS_VERSION) {
      prefs.end();
      _UFC_LOG("NVS load: version mismatch (got %u, want %u), using defaults", ver, NVS_VERSION);
      return false;
    }

    // ── Read all data bytes (uint16_t fields split to high/low for CRC) ──────
    const uint16_t maxCur = prefs.getUShort("maxcur", 500);
    const uint16_t minCur = prefs.getUShort("mincur", 38);
    const uint8_t data[10] = {
      ver,
      prefs.getUChar("otam",  static_cast<uint8_t>(State::NOOTA)),
      prefs.getUChar("bootm", static_cast<uint8_t>(State::BOOTMSG)),
      prefs.getUChar("irqm",  static_cast<uint8_t>(State::POLLING)),
      prefs.getUChar("pwr",   static_cast<uint8_t>(State::UNKNOWN)),
      prefs.getUChar("brt",   128),
      static_cast<uint8_t>(maxCur >> 8),
      static_cast<uint8_t>(maxCur & 0xFF),
      static_cast<uint8_t>(minCur >> 8),
      static_cast<uint8_t>(minCur & 0xFF)
    };
    const uint8_t storedCrc = prefs.getUChar("crc", 0);
    prefs.end();

    // ── CRC integrity check ───────────────────────────────────────────────
    if (_crc8(data, sizeof(data)) != storedCrc) {
      _UFC_LOG("NVS load: CRC mismatch, using defaults");
      return false;
    }

    // All checks passed — apply all loaded values
    _snapshot.otamode    = static_cast<State>(data[1]);
    _snapshot.bootmode   = static_cast<State>(data[2]);
    _snapshot.irqmode    = static_cast<State>(data[3]);
    _snapshot.power      = static_cast<State>(data[4]);
    _snapshot.brightness = data[5];
    _snapshot.maxCurrent = maxCur;
    _snapshot.minCurrent = minCur;

    _UFC_LOG("NVS load: OK");
    return true;
  }

  /**
   * @brief  Erase all UFC state keys from NVS.
   *
   * @details Clears the entire @c "ufc" namespace so the next load() call will
   *          find no valid data and revert to defaults.  Useful during firmware
   *          upgrades that change the schema version, or during factory reset.
   *
   * @return @c true if the namespace was opened and cleared successfully.
   */
  bool clearSaved() {
    Preferences prefs;
    if (!prefs.begin("ufc", /*readOnly=*/false)) return false;
    prefs.clear();
    prefs.end();
    _UFC_LOG("NVS cleared");
    return true;
  }

private:
  /** @brief Eyecatcher written to NVS to verify data belongs to this firmware. 'O','H','U','F' = OpenHornet UFC. */
  static constexpr uint32_t NVS_MAGIC   = 0x4F485546;
  /**
   * @brief  NVS schema version — increment whenever the set of saved keys or their meaning changes.
   *
   * Version history:
   * - 1: otamode, bootmode, irqmode only; CRC over 4 bytes.
   * - 2: added power and brightness; CRC over 6 bytes.
   * - 3: added maxCurrent and minCurrent (uint16_t each); CRC over 10 bytes.
   */
  static constexpr uint8_t  NVS_VERSION = 3;

  /**
   * @brief  Compute CRC-8/SMBUS over a byte buffer (polynomial 0x07, init 0x00).
   *
   * @details Used to detect bit-level corruption of NVS data.  The CRC is computed
   *          over (version, otamode, bootmode, irqmode, power, brightness) so any
   *          schema-version change also invalidates previously stored CRCs.
   *
   * @param data  Pointer to the byte array to process.
   * @param len   Number of bytes in @p data.
   * @return      8-bit CRC value.
   */
  static uint8_t _crc8(const uint8_t* data, size_t len) {
    uint8_t crc = 0x00;
    for (size_t i = 0; i < len; i++) {
      crc ^= data[i];
      for (uint8_t bit = 0; bit < 8; bit++) {
        crc = (crc & 0x80) ? ((crc << 1) ^ 0x07) : (crc << 1);
      }
    }
    return crc;
  }

  /** @brief Arduino GPIO number of the VCC_SNS sense pin. */
  uint8_t  _vccSnsPin;
  /** @brief All current state dimensions. Runtime options set by setters; power set by detectPower(). */
  Snapshot _snapshot;
};

#endif // UFC_STATE_CHECKER_H
