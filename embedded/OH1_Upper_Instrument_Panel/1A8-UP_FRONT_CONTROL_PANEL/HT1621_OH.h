/*******************************************************************************

!!!!! This a modification of the original HT1621 arduino library adopted to Open Hornet needs !!!!


Copyright 2016-2018 anxzhu (github.com/anxzhu)
Copyright 2018-2020 Valerio Nappi (github.com/valerionew) (changes)
Based on segment-lcd-with-ht1621 from anxzhu (2016-2018)
(https://github.com/anxzhu/segment-lcd-with-ht1621)

Partially rewritten and extended by Valerio Nappi (github.com/valerionew) in 2018

This file is part of the HT1621 arduino library, and thus under the MIT license.
More info on the project and the license conditions on :
https://github.com/valerionew/ht1621-7-seg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

/**
 * @file    HT1621_OH.h
 * @brief   OpenHornet-modified HT1621 segment LCD driver for the UFC scratchpad display.
 *
 * @details Extends the original HT1621 Arduino library (anxzhu / Valerio Nappi) with:
 *          - A variable-brightness PWM backlight controlled by set_backlight_brightness().
 *          - A 16-segment (alphanumeric) display buffer (@c _buffer16) for the two
 *            large character positions on the left of the scratchpad.
 *          - A 7-segment numeric buffer (@c _buffer) for the eight digit positions.
 *          - print_str() for writing 4-character alphanumeric strings to the 16-segment
 *            positions using two custom segment-encoding lookup functions.
 *          - print_num() for writing 8-character numeric strings to the 7-segment positions.
 *
 *          Hardware interface: 3-wire bit-bang SPI (CS, WR, DATA) plus an optional
 *          PWM backlight pin.  All timing is managed by direct GPIO toggles with
 *          4 µs half-periods.
 *
 * @note    The stored backlight level (set_backlight_brightness()) is kept apart from whether the
 *          backlight is on. The backlight pin gets that level only while something is lit on the
 *          scratchpad itself or on an option window (setOptionContent()); with everything blank it is
 *          held off, and it returns at the stored level when something lights again.
 */

#ifndef HT1621_OH_H_
#define HT1621_OH_H_

/** @brief LCD bias command: 1/3 duty, 4 COM. */
#define  BIAS     0x52
/** @brief System disable: turn off oscillator and LCD bias generator. */
#define  SYSDIS   0X00
/** @brief System enable: start the internal oscillator. */
#define  SYSEN    0X02
/** @brief LCD off: disable the LCD bias voltage. */
#define  LCDOFF   0X04
/** @brief LCD on: enable the LCD bias voltage. */
#define  LCDON    0X06
/** @brief Use external crystal oscillator clock source. */
#define  XTAL     0x28
/** @brief Use internal RC 256 kHz oscillator (default). */
#define  RC256    0X30
/** @brief Tone output on. */
#define  TONEON   0X12
/** @brief Tone output off. */
#define  TONEOFF  0X10
/** @brief Disable watchdog timer. */
#define  WDTDIS1  0X0A
/** @brief Size of the 7-segment numeric display buffer in bytes. */
#define  BUFFERSIZE 12


/**
 * @brief  OpenHornet HT1621 LCD driver with PWM backlight and dual display buffers.
 *
 * @details Manages a 3-wire bit-bang SPI connection to an HT1621 segment LCD
 *          controller.  Two separate display regions are maintained:
 *          - _buffer16[4]: four 8-bit half-words encoding two 16-segment
 *            (alphanumeric) characters for the scratchpad string display.
 *          - _buffer[BUFFERSIZE]: 8-bit segment data for up to 8 seven-segment
 *            numeric digit positions.
 *
 *          The backlight brightness is stored in _brightness_p and applied
 *          directly to the backlight pin via analogWrite on every call to
 *          backlight() or set_backlight_brightness().
 */
class HT1621_OH
{
public:
  /**
   * @brief  Default constructor — initialises all buffer bytes to zero.
   */
  HT1621_OH();

  /**
   * @brief  Initialise the driver with CS, WR, DATA, and backlight pins.
   *
   * @details Configures all four pins as OUTPUT, stores pin numbers,
   *          sets the initial brightness to 255, and runs the HT1621
   *          startup command sequence (config()).
   *
   * @param cs_p         Arduino pin connected to HT1621 /CS (chip select, active low).
   * @param wr_p         Arduino pin connected to HT1621 WR  (write clock).
   * @param data_p       Arduino pin connected to HT1621 DATA (serial data).
   * @param backlight_p  Arduino pin connected to the backlight PWM driver.
   */
  void begin(int cs_p, int wr_p, int data_p, int backlight_p);

  /**
   * @brief  Initialise the driver without a backlight pin.
   *
   * @details Use when no backlight is fitted.  Backlight functions become no-ops.
   *
   * @param cs_p    Arduino pin connected to HT1621 /CS.
   * @param wr_p    Arduino pin connected to HT1621 WR.
   * @param data_p  Arduino pin connected to HT1621 DATA.
   */
  void begin(int cs_p, int wr_p, int data_p);

  /**
   * @brief  Clear all segment memory on the HT1621 hardware.
   *
   * @details Writes zeros to 16 consecutive HT1621 addresses via wrCLR().
   *          Does not clear the software _buffer or _buffer16 arrays; those
   *          retain their previous values until the next print_num / print_str call.
   */
  void clear();

  /**
   * @brief  Apply the stored brightness level to the backlight PWM pin.
   *
   * @details Writes the stored level to the backlight pin while something is lit on the
   *          scratchpad or on an option window, and 0 otherwise. The level is set by
   *          set_backlight_brightness(). Called internally by print_num() and print_str() after
   *          each display update.
   */
  void backlight();

  /**
   * @brief  Set the backlight PWM brightness and apply it immediately.
   *
   * @details Stores @p brightness in _brightness_p and applies it at once, unless nothing is lit
   *          (see backlight()), in which case the pin stays off and the level is used as soon as
   *          something lights. The level persists across all subsequent print_num() and
   *          print_str() calls until changed again.
   *
   * @param brightness  PWM duty cycle: 0 = off, 255 = full brightness.
   */
  void set_backlight_brightness(int brightness);

  /**
   * @brief  Force the backlight pin LOW (off), ignoring _brightness_p.
   *
   * @details Provided for compatibility; prefer set_backlight_brightness(0)
   *          for consistent behaviour.
   */
  void noBacklight();

  /**
   * @brief  Write an 8-character numeric string to the 7-segment display.
   *
   * @details Each character is encoded via charToSegBits() and stored in
   *          _buffer[0..7].  The first byte of @p str (index 0) is skipped
   *          because DCS-BIOS always pads the scratchpad number string with a
   *          leading space; actual content begins at str[1].  After updating
   *          the buffer, backlight() and update() are called.
   *
   * @param str        Pointer to an 8-character null-terminated string from
   *                   DCS-BIOS (format: space + 7 content characters).
   * @param leftPadded If true, right-aligns the content by padding the left
   *                   with spaces.  Default false (left-aligned).
   */
  void print_num(const char* str, bool leftPadded = false);

  /**
   * @brief  Write a 4-character alphanumeric string to the 16-segment display.
   *
   * @details Encodes each character pair using charTo16SegBits_Dig1() and
   *          charTo16SegBits_Dig2() and stores them in _buffer16[0..3].
   *          Special handling merges the decimal point segment when the next
   *          character is a digit requiring it (4, 5, 6, 8, 9, 0).  After
   *          updating the buffer, backlight() and update() are called.
   *
   * @param str        Pointer to a 4-character null-terminated string.
   * @param leftPadded Reserved for future use; currently unused.
   */
  void print_str(const char* str, bool leftPadded = false);

  /**
   * @brief  Write a 2-character alphanumeric string to the scratchpad's second
   *         16-segment cell (DCS-BIOS UFC_SCRATCHPAD_STRING_2_DISPLAY).
   *
   * @details Confirmed against the UFC_Main schematic: the scratchpad glass
   *          (DS1) only breaks out HT1621B SEG0-SEG21 - SEG22-SEG31 are
   *          no-connects on this PCB, so an earlier attempt to give STRING_2
   *          its own address range (24-30) could never produce visible output.
   *          There are only 9 real digit positions on the glass: two 16-segment
   *          alphanumeric cells (digit "9" on SEG0-3, digit "8" on SEG4-7) and
   *          seven 7-segment numeric digits (SEG8-21). STRING_1 only ever
   *          supplies 2 real characters, so it only drives the digit "9" cell
   *          (_buffer16[0..1]); the digit "8" cell was otherwise always blank.
   *          print_str2() now writes STRING_2's pair into that cell
   *          (_buffer16[2..3] / HT1621 addresses 4 and 6), combining the two
   *          characters the same way print_str() does, so they render as the
   *          overlapped composite glyph seen on real hardware (e.g. the "59"
   *          pair in a 10-digit grid coordinate entry).
   *
   * @param str        Pointer to a 2-character (or longer; only first 2 used)
   *                   null-terminated string.
   * @param leftPadded Reserved for future use; currently unused.
   */
  void print_str2(const char* str, bool leftPadded = false);

  /**
   * @brief  Resolve ANY raw character DCS-BIOS can send for a STRING_2 bundle position -- a plain
   *         literal digit '0'-'9', or one of the 20 letter substitutes DCS-BIOS uses instead of a
   *         literal digit in this slot -- to the actual digit it represents (0-9).
   *
   * @details Derived from a complete, hardware-confirmed sweep of all 100 real STRING_2 raw values
   *          (BIOSBuddy, Sep 2026 -- UFC_STRING2_DCS_Tracking.xlsx). Two findings made this a simple
   *          per-character table instead of a 100-entry pair lookup:
   *            - The RIGHT character of a bundled pair is always sent as a plain literal digit, never
   *              substituted, with zero exceptions across all 100 samples.
   *            - Each of the 20 letters always means the same digit no matter which other digit it's
   *              paired with (checked for conflicts across all 100 samples -- none found), even though
   *              several digits use up to 3 different letters depending on context (e.g. '0' can
   *              arrive as 'a', 'b', or 'c').
   *          '1' and '2' are never substituted in either position.
   *
   * @param character  Raw character from DCS-BIOS for one position of the STRING_2 bundle.
   * @return  The digit (0-9) it represents, or -1 if @p character isn't a recognized digit character
   *          (space, an unbundled letter, etc.).
   */
  int8_t resolveDigitIdentity(char character);

  /**
   * @brief  Send the LCDON command to enable the LCD bias voltage.
   */
  void display();

  /**
   * @brief  Send the LCDOFF command to disable the LCD bias voltage (blanks display).
   */
  void noDisplay();

  /**
   * @brief  Tell the driver a UFC key was pressed on this panel (O/S BRG 9th-character recovery only).
   *
   * @details DCS-BIOS exports the BRG string cut to 8 of its 9 characters, so the digit just keyed
   *          is always the lost 9th character. This remembers the last digit key and, while a BRG
   *          string is on the glass, appends it. 'C' (CLR) and 'E' (ENT) forget it. It has no
   *          effect on any other page (grid entries etc.).
   * @param key  '0'-'9' for a digit key, 'C' for CLR, 'E' for ENT.
   */
  void noteKey(char key);

  /**
   * @brief  Light the scratchpad for the power-on built-in test (NATOPS figure 2-48).
   *
   * @details Phase 1 lights the outer segments of the two 16-segment cells, phase 2 the inner
   *          segments; both phases light every segment of the seven numeric digits. Any other value
   *          is ignored. The cells stay claimed by the test, so STRING_1 and STRING_2 do not draw
   *          over it; the next print_num() hands them back.
   * @param phase  1 = outer segments, 2 = inner segments.
   */
  void bitTest(uint8_t phase);

  /**
   * @brief  Tell the driver whether any option window is lit (scratchpad backlight gating).
   *
   * @details The backlight is on only while something is lit on the scratchpad itself or on an
   *          option window; otherwise the backlight pin is held off. The stored brightness level
   *          (set_backlight_brightness()) is not changed, so the backlight returns at that level.
   * @param lit  True when at least one option window shows a visible character.
   */
  void setOptionContent(bool lit);

private:
  /** @brief Arduino pin number for HT1621 /CS (chip select, active low). */
  int _cs_p;
  /** @brief Arduino pin number for HT1621 WR (write clock). */
  int _wr_p;
  /** @brief Arduino pin number for HT1621 DATA (serial data). */
  int _data_p;
  /** @brief Arduino pin number for the PWM backlight driver. */
  int _backlight_p;
  /** @brief Stored backlight PWM level (0-255); applied by backlight() and set_backlight_brightness(). */
  int _brightness_p;
  /** @brief Legacy backlight-enable flag; no longer used to gate the PWM write. */
  bool _backlight_en;
  /** @brief 7-segment display buffer: one byte per digit, up to BUFFERSIZE digits. */
  char _buffer[BUFFERSIZE];
  /** @brief 16-segment display buffer: four bytes encoding two alphanumeric character positions
   *         (_buffer16[0..1] = digit "9" cell, driven by print_str() / STRING_1;
   *          _buffer16[2..3] = digit "8" cell, driven by print_str2() / STRING_2). */
  char _buffer16[4];
  /** @brief Reserved battery-indicator segment bytes (not currently used by UFC). */
  unsigned char _battery[3];

  /** @brief True while print_num() is using the two 16-segment cells as part of a nine-cell number
   *         string (O/S BRG style). While set, print_str()/print_str2() remember their text but do
   *         not draw, so they cannot overwrite the digits the number string put there. */
  bool _numClaimsAlnum;
  /** @brief True while at least one option window is lit (reported by the sketch); keeps the backlight on. */
  bool _optionLit;
  /** @brief Last digit key pressed on this panel (0 = none). Used to rebuild BRG's truncated 9th char. */
  char _lastKey;
  /** @brief True once the most recent print_num() was a BRG string exported at only 8 characters. */
  bool _brgCapped;
  /** @brief Copy of that 8-character BRG string (+ terminator) for the first-press redraw. */
  char _brgLast[9];

  /** @brief True when any segment of the scratchpad itself (16-segment cells or numeric digits) is lit. */
  bool hasContent() const;
  /** @brief Write the backlight pin: the stored level while something is lit (here or on an option
   *         window), otherwise off. */
  void applyBacklight();
  /** @brief Last STRING_1 text received (2 chars + terminator), redrawn when the claim is released. */
  char _str1Last[3];
  /** @brief Last STRING_2 text received (2 chars + terminator), redrawn when the claim is released. */
  char _str2Last[3];

  /**
   * @brief  Draw two characters into the digit "8" cell (_buffer16[2..3]) without touching the
   *         display hardware. This is the drawing half of print_str2(), shared with print_num().
   */
  void drawCell8(const char* str);
  static bool wholeCellGlyph(char a, char b, uint8_t &left, uint8_t &right);

  /**
   * @brief  Draw a number string as a nine-cell readout (2 alphanumeric cells + 7 numeric cells).
   *
   * @details str[i] goes to cell i, left to right: str[0] = digit "9" cell, str[1] = digit "8" cell,
   *          str[2..8] = the seven numeric cells. This is how DCS lays out O/S BRG entry
   *          ("999@99'99" = 3 digits, degree, 2 digits, minute mark, 2 digits).
   */
  void printNumNineCell(const char* str, int chars);

  /**
   * @brief  Write 8 bits of segment data to one HT1621 RAM address.
   *
   * @param addr   6-bit RAM address within the HT1621 (will be left-shifted by 2).
   * @param sdata  8-bit segment pattern to write.
   */
  void wrone(unsigned char addr, unsigned char sdata);

  /**
   * @brief  Write 8 bits to an HT1621 address with a clear-first operation.
   *
   * @param addr   6-bit RAM address (left-shifted by 2 internally).
   * @param sdata  8-bit segment data to write.
   */
  void wrclrdata(unsigned char addr, unsigned char sdata);

  /**
   * @brief  Clear @p len consecutive HT1621 RAM addresses starting at address 0.
   *
   * @param len  Number of addresses to zero.
   */
  void wrCLR(unsigned char len);

  /**
   * @brief  Bit-bang @p cnt bits of @p data MSB-first on the SPI DATA/WR lines.
   *
   * @param data  Byte to transmit (MSB first).
   * @param cnt   Number of bits to clock out (1-8).
   */
  void wrDATA(unsigned char data, unsigned char cnt);

  /**
   * @brief  Send a command byte to the HT1621 (100b prefix + 8-bit command).
   *
   * @param CMD  One of the HT1621 command defines (BIAS, SYSEN, LCDON, etc.).
   */
  void wrCMD(unsigned char CMD);

  /**
   * @brief  Run the HT1621 power-on initialisation sequence.
   *
   * @details Sends BIAS, RC256, SYSDIS, WDTDIS1, SYSEN, LCDON in order.
   *          Called from begin() after pin setup.
   */
  void config();

  /**
   * @brief  Push all software buffer contents to HT1621 hardware RAM.
   *
   * @details Writes _buffer16[0..3] to addresses 0-6 and _buffer[0..6]
   *          to addresses 8-20.  Called at the end of print_num() and
   *          print_str() so the display reflects the latest buffer state.
   */
  void update();

  /**
   * @brief  Encode a character for the upper half of a 16-segment digit (Dig1).
   *
   * @details Returns an 8-bit segment pattern for the high-byte portion of a
   *          16-segment display cell.  Characters not in the lookup table
   *          return 0 (blank).
   *
   * @param character  ASCII character to encode.
   * @return           8-bit segment pattern for the Dig1 (upper) half of the cell.
   */
  char charTo16SegBits_Dig1(char character);

  /**
   * @brief  Encode a character for the lower half of a 16-segment digit (Dig2).
   *
   * @details Returns an 8-bit segment pattern for the low-byte portion of a
   *          16-segment display cell.  Characters not in the lookup table
   *          return 0 (blank).
   *
   * @param character  ASCII character to encode.
   * @return           8-bit segment pattern for the Dig2 (lower) half of the cell.
   */
  char charTo16SegBits_Dig2(char character);

  /**
   * @brief  Encode a character as a standard 7-segment pattern.
   *
   * @details Returns the segment byte for digits 0-9, letters A-Z (partial set),
   *          and a small number of special symbols ('@', '|', '-', '_', '.').
   *          Characters without a defined pattern return 0 (blank).
   *
   * @param character  ASCII character to encode.
   * @return           7-segment byte in the format 0b?EFADCGB.
   */
  char charToSegBits(char character);
};


//#include "esp32-hal.h"
/*******************************************************************************

!!!!! This a modification of the original HT1621 arduino library adopted to Open Hornet needs !!!!



Copyright 2016-2018 anxzhu (github.com/anxzhu)
Copyright 2018-2020 Valerio Nappi (github.com/5N44P) (changes)
Based on segment-lcd-with-ht1621 from anxzhu (2016-2018)
(https://github.com/anxzhu/segment-lcd-with-ht1621)

Partially rewritten and extended by Valerio Nappi (github.com/5N44P) in 2018

This file is part of the HT1621 arduino library, and thus under the MIT license.
More info on the project and the license conditions on :
https://github.com/5N44P/ht1621-7-seg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include <Arduino.h>
#include <HT1621_OH.h>

HT1621_OH::HT1621_OH(){
	_buffer[0] = 0x00;
	_buffer[1] = 0x00;
	_buffer[2] = 0x00;
	_buffer[3] = 0x00;
	_buffer[4] = 0x00;
	_buffer[5] = 0x00;
	_buffer[6] = 0x00;
	_buffer16[0] = 0x0000;
	_buffer16[1] = 0x0000;
	_buffer16[2] = 0x0000;
	_buffer16[3] = 0x0000;
	_buffer[7] = 0x00;
	_numClaimsAlnum = false;
	_optionLit = false;
	_lastKey = 0;
	_brgCapped = false;
	_brgLast[0] = '\0';
	_str1Last[0] = ' '; _str1Last[1] = ' '; _str1Last[2] = '\0';
	_str2Last[0] = ' '; _str2Last[1] = ' '; _str2Last[2] = '\0';
}

void HT1621_OH::begin(int cs_p, int wr_p, int data_p, int backlight_p)
{
	pinMode(cs_p, OUTPUT);
	pinMode(wr_p, OUTPUT);
	pinMode(data_p, OUTPUT);
	pinMode(backlight_p, OUTPUT);
	_cs_p=cs_p;
	_wr_p=wr_p;
	_data_p=data_p;
	_backlight_p=backlight_p;
	_backlight_en=false;
  _brightness_p = 255;
	config();
}

void HT1621_OH::begin(int cs_p, int wr_p, int data_p)
{
	pinMode(cs_p, OUTPUT);
	pinMode(wr_p, OUTPUT);
	pinMode(data_p, OUTPUT);
	_cs_p=cs_p;
	_wr_p=wr_p;
	_data_p=data_p;
	_backlight_en = false;
	config();
}

void HT1621_OH::wrDATA(unsigned char data, unsigned char cnt) {
	unsigned char i;
	for (i = 0; i < cnt; i++) {
		digitalWrite(_wr_p, LOW);
		delayMicroseconds(4);
		if (data & 0x80) {
			digitalWrite(_data_p, HIGH);
		}
		else {
			digitalWrite(_data_p, LOW);
		}
		digitalWrite(_wr_p, HIGH);
		delayMicroseconds(4);
		data <<= 1;
	}
}
void HT1621_OH::wrclrdata(unsigned char addr, unsigned char sdata)
{
	addr <<= 2;
	digitalWrite(_cs_p, LOW);
	wrDATA(0xa0, 3);
	wrDATA(addr, 6);
	wrDATA(sdata, 8);
	digitalWrite(_cs_p, HIGH);
}

void HT1621_OH::display()
{
	wrCMD(LCDON);
}

void HT1621_OH::noDisplay()
{
	wrCMD(LCDOFF);
}

void HT1621_OH::wrone(unsigned char addr, unsigned char sdata)
{
	addr <<= 2;
	digitalWrite(_cs_p, LOW);
	wrDATA(0xa0, 3);
	wrDATA(addr, 6);
	wrDATA(sdata, 8);
	digitalWrite(_cs_p, HIGH);
}

void HT1621_OH::set_backlight_brightness(int brightness){
  // Stores the requested level and applies it (applyBacklight() holds the pin off while nothing is
  // lit). Applies directly, independent of display writes, so the level always reflects the request.
  _brightness_p = brightness;
  applyBacklight();
}
void HT1621_OH::backlight()
{
  // Re-applies the stored level (or off, while nothing is lit) on every display write.
  applyBacklight();
  delay(1);
}
bool HT1621_OH::hasContent() const
{
  for (int i = 0; i < 7; i++) if (_buffer[i]) return true;
  for (int i = 0; i < 4; i++) if (_buffer16[i]) return true;
  return false;
}
void HT1621_OH::applyBacklight()
{
  analogWrite(_backlight_p, (_optionLit || hasContent()) ? _brightness_p : 0);
}
void HT1621_OH::setOptionContent(bool lit)
{
  if (lit == _optionLit) return;
  _optionLit = lit;
  applyBacklight();
}

void HT1621_OH::noBacklight()
{
	//if(_backlight_en)
		digitalWrite(_backlight_p, LOW);
	delay(1);
}

void HT1621_OH::wrCMD(unsigned char CMD) {  //100
	digitalWrite(_cs_p, LOW);
	wrDATA(0x80, 4);
	wrDATA(CMD, 8);
	digitalWrite(_cs_p, HIGH);
}

void HT1621_OH::config()
{
	wrCMD(BIAS);
	wrCMD(RC256);
	wrCMD(SYSDIS);
	wrCMD(WDTDIS1);
	wrCMD(SYSEN);
	wrCMD(LCDON);
}

void HT1621_OH::wrCLR(unsigned char len) {
	unsigned char addr = 0;
	unsigned char i;
	for (i = 0; i < len; i++) {
		wrclrdata(addr, 0x00);
		addr = addr + 2;
	}
}


void HT1621_OH::clear(){
	wrCLR(16);
}



// takes the buffer and puts it straight into the driver
void HT1621_OH::update(){
	// Fixed: this is a 3-wire bit-banged interface with no hardware SPI, so each wrone() call
	// depends on precise back-to-back GPIO timing. With three independent DCS-BIOS fields
	// (String 1, String 2, Number) each able to trigger their own print_*() -> update() call in
	// quick succession while typing, and this call also being reachable from ISR-driven DCS-BIOS
	// processing, a write here could previously be interrupted mid-transfer and corrupt whichever
	// register was in flight (observed as a dropped/blank digit on real hardware). noInterrupts()/
	// interrupts() are the portable Arduino calls (unlike AVR-only cli()/sei()) and work correctly
	// on this ESP32-S3 target. The whole sequence below is ~30 short GPIO writes at 4us half-periods
	// (roughly 2-3ms total) - short enough to be safe against the watchdog, but flagging it here in
	// case WiFi/OTA timing or the watchdog ever need to be revisited around this.
	noInterrupts();
	// the buffer is backwards with respect to the lcd. could be improved
	wrone(0,  _buffer16[0]); // writing 16 bits to memory
	wrone(2,  _buffer16[1]); // writing 16 bits to memory
    wrone(4,  _buffer16[2]); // writing 16 bits to memory
    wrone(6,  _buffer16[3]); // writing 16 bits to memory
	wrone(8,  _buffer[0]);   // writing 8 bits to memory
	wrone(10, _buffer[1]);   // writing 8 bits to memory
	wrone(12, _buffer[2]);   // writing 8 bits to memory
	wrone(14, _buffer[3]);   // writing 8 bits to memory
	wrone(16, _buffer[4]);   // writing 8 bits to memory
	wrone(18, _buffer[5]);   // writing 8 bits to memory
	wrone(20, _buffer[6]);   // writing 8 bits to memory
	// NOTE: per the UFC_Main schematic, HT1621B pin SEG22 (address 22, used for
	// the 8th number-display digit below) is a no-connect on the DS1 scratchpad
	// connector - the real glass only has 7 numeric digit positions (SEG8-21).
	// This write is harmless (goes nowhere) but will never produce visible
	// output; left in case a future glass revision wires it up. Addresses
	// 24-30 (formerly used for STRING_2 here) are no-connects too - that data
	// now goes to _buffer16[2..3] / addresses 4 and 6 instead, see print_str2().
	wrone(22, _buffer[7]);   // writing 8 bits to memory (8th digit - unwired on this PCB, see note above)
	interrupts();
}



// Fixed: per the UFC_Main schematic, DS1 (the scratchpad glass connector) only
// breaks out HT1621B pins SEG0-SEG21 - SEG22 through SEG31 are unconnected (no-
// connect) on this PCB. The scratchpad has only 9 real digit positions: two
// 16-segment alphanumeric cells (digit "9" on SEG0-3 / _buffer16[0..1], and
// digit "8" on SEG4-7 / _buffer16[2..3]) plus seven 7-segment numeric digits
// (SEG8-21). There is no separate hardware slot for STRING_2 - it has to share
// one of the two existing alphanumeric cells. STRING_1 only ever supplies 2
// real characters (confirmed against DCS-BIOS: Max Length 2), so this now only
// touches _buffer16[0..1] (the digit "9" cell) and leaves _buffer16[2..3]
// (digit "8") alone for print_str2() to own, instead of overwriting it with its
// own out-of-range str[2]/str[3] on every call.
// Added (Sep 2026): a single standalone digit here (str[0] blank, str[1] the digit -- DCS-BIOS
// pads STRING_1 this way, same as confirmed for STRING_2) is drawn via Dig2 alone, same as
// print_str2()'s matching case. Dig2 physically has no upper-left segment (see
// charTo16SegBits_Dig2's note), which reads as "top-left never lights up" on 0/4/5/6/8/9 -- but
// that corner turns out to be the SAME shared bridge segment used to close it for bundled
// STRING_2 pairs (bit0 of the Dig1 byte, "Rule 2" in print_str2()). This cell shares that same
// physical hardware, so setting just that one bit in Dig1 -- nothing else in it -- lights the
// shared segment and completes the digit's top-left corner, without drawing any of Dig1's other
// (mismatched) segments the way the old full-digit "both slots" behavior used to.
void HT1621_OH::print_str(const char* str, bool leftPadded){
  // Remember the text, and don't draw while a nine-cell number string (O/S BRG) is using this cell.
  // It is redrawn from the remembered text when that number string ends (see print_num()).
  _str1Last[0] = str[0]; _str1Last[1] = (str[0] != '\0') ? str[1] : ' '; _str1Last[2] = '\0';
  if (_numClaimsAlnum) return;
  if (str[0] == ' ' && isDigit(str[1])) {
    const int digit = str[1] - '0';
    const bool needsTopLeft = (digit == 0 || digit == 4 || digit == 5 || digit == 6 || digit == 8 || digit == 9);
    _buffer16[0] = needsTopLeft ? 0b00000001 : 0;
    _buffer16[1] = charTo16SegBits_Dig2(str[1]);
    backlight();
    update();
    return;
  }
  // A lone E or W (magnetic variation East/West, the other character blank) is drawn across the whole
  // cell; the Dig1/Dig2 half-cell tables below have no glyph for either letter.
  if (str[0] != '\0') {
    uint8_t l, r;
    if (wholeCellGlyph(str[0], str[1], l, r)) {
      _buffer16[0] = l;
      _buffer16[1] = r;
      backlight();
      update();
      return;
    }
  }
  for(int i = 0; i < 2; i++){
      if ( i == 0 ){
        if (str[i] == ' ' && !isDigit(str[i+1]) ){
          _buffer16[i] =charTo16SegBits_Dig1(str[i+1]);
        }else{
          // Removed: a "tmp_seg |= 0x01" force-on merge used to fire here whenever the second
          // character was one of 4/5/6/8/9/0, meant to tidy the shape of a genuine digit-pair
          // overlap back when Dig1 only had placeholder values for most digits. Now that Dig1
          // carries real, hardware-confirmed per-digit shapes (see charTo16SegBits_Dig1) -- where
          // bit0/upper-right is already correctly included or omitted per digit -- that forced OR
          // would incorrectly light upper-right on digits like '5'/'6' that were just confirmed on
          // real hardware to NOT have it, any time they're paired with one of those second digits.
          _buffer16[i] = charTo16SegBits_Dig1(str[i]);
        }
      }else{
        _buffer16[i] = charTo16SegBits_Dig2(str[i]);
      }
  }
  backlight();
  update();
}

// Fixed: previously wrote to _buffer16_2 / HT1621 addresses 24-30, which are
// not wired to the scratchpad glass on this PCB (see UFC_Main schematic - those
// HT1621B pins are marked no-connect), so STRING_2 content could never appear.
// Now writes into _buffer16[2..3], the digit "8" alphanumeric cell (SEG4-7,
// addresses 4 and 6), which IS wired but was otherwise always left blank by
// print_str() (STRING_1 only ever supplies 2 real characters).
//
// The Dig1/Dig2 split below always renders str[0] and str[1] independently in
// the same cell (that's the overlap itself, and it happens for whatever two
// characters this field is given - letters, blanks, digits, doesn't matter).
//
// Removed: this used to force bit0 on via "tmp_seg |= 0x01" whenever both characters were digits
// and the second was one of 4/5/6/8/9/0 -- a hack to tidy the overlap shape from back when Dig1
// only had placeholder values for most digits. Dig1 now carries real, hardware-confirmed per-digit
// shapes (see charTo16SegBits_Dig1) where bit0/upper-right is already correctly included or omitted
// per digit, so that forced OR is now redundant at best and actively wrong at worst -- it would
// light upper-right on '5'/'6' (confirmed to NOT have it) whenever paired with one of those second
// digits.
//
// Added: when exactly ONE of the two characters is a real digit and the other is blank -- i.e.
// this cell is showing a single standalone digit, not a genuine two-digit bundle -- both slots now
// render THAT SAME digit instead of leaving the unused slot dark. Dig1 and Dig2 are two physically
// separate, independently-confirmed segment sets sharing this one cell: Dig1 can light upper-left/
// lower-left/upper-right but never bottom-right, Dig2 can light top-right/bottom-right/bottom-left
// but never upper-left. Drawing the same digit through both at once gets the union of what each can
// show, closing whichever corner that particular slot could never reach alone. This only works when
// there's one real digit to key off of -- a true bundled pair (both characters real, different
// digits) still needs each slot to carry its own digit's meaning, so that case is unchanged.
//
// Rewritten (Sep 2026) to fix two bugs surfaced by a complete 100-pair BIOSBuddy sweep
// (UFC_STRING2_DCS_Tracking.xlsx) of the real raw values DCS-BIOS sends here:
//   1. This used to detect a bundled pair with isDigit() on the raw characters directly. But
//      DCS-BIOS only ever sends str[0] as a literal digit for SOME pairings -- most of the time
//      it substitutes one of 20 letters instead (e.g. "k0" really means digit pair 5,0). isDigit()
//      fails on those letters, so every letter-substituted pair used to fall through to the
//      single-standalone-digit branch and render the SAME digit in both slots instead of the true
//      pair. resolveDigitIdentity() decodes the full 20-letter substitution table (confirmed
//      conflict-free across all 100 real samples) so both characters resolve to their true digit
//      identity regardless of whether DCS sent a literal digit or one of its letter stand-ins.
//   2. Once a genuine two-different-digit bundle is correctly detected, the selective segment
//      bridge confirmed clean on real hardware (1A8_UFC_BRIDGE_FULLSWEEP, all 100 digit-identity
//      pairs) is now applied: left slot's own upper-right helps close the right digit's missing
//      upper-left, right slot's own bottom-left helps close the left digit's missing bottom-right --
//      only where the neighboring digit actually needs that corner closed and doesn't already show
//      it natively. Previously the two slots were rendered fully independently with no bridging at
//      all, leaving a permanently-open corner on nearly every pair.
void HT1621_OH::print_str2(const char* str, bool leftPadded){
  // Same remember-and-skip rule as print_str() while a nine-cell number string owns this cell.
  _str2Last[0] = str[0]; _str2Last[1] = (str[0] != '\0') ? str[1] : ' '; _str2Last[2] = '\0';
  if (_numClaimsAlnum) return;
  drawCell8(str);
  backlight();
  update();
}

// Draws two characters into the digit "8" cell (_buffer16[2..3]). Shared by print_str2() and the
// nine-cell number string (O/S BRG), so both render their pair the same way.
void HT1621_OH::drawCell8(const char* str){
  const int8_t leftId = resolveDigitIdentity(str[0]);
  const int8_t rightId = resolveDigitIdentity(str[1]);

  if (leftId >= 0 && rightId < 0) {
    // Single standalone digit in str[0] (str[1] blank/non-digit). Confirmed on real hardware
    // (Sep 2026): a single digit should show via the RIGHT slot (Dig2) only -- the earlier
    // "combine both slots" behavior mirrored the digit across both halves, which doesn't match
    // real gameplay.
    //
    // Added: Dig2 physically has no upper-left segment (see charTo16SegBits_Dig2's note) -- but
    // that corner is the SAME shared bridge segment used to close it for bundled pairs (Rule 2
    // below: bit0 of the left/Dig1 byte). Setting just that one bit here -- nothing else in the
    // left byte -- lights the shared segment and completes the digit's top-left corner without
    // drawing any of Dig1's other (mismatched) segments, which is what caused the old "mirrored"
    // full-Dig1-shape look.
    const char c = '0' + leftId;
    const bool needsTopLeft = (leftId == 0 || leftId == 4 || leftId == 5 || leftId == 6 || leftId == 8 || leftId == 9);
    _buffer16[2] = needsTopLeft ? 0b00000001 : 0;
    _buffer16[3] = charTo16SegBits_Dig2(c);
  } else if (leftId < 0 && rightId >= 0) {
    // Single standalone digit in str[1] (str[0] blank/non-digit) -- same fix as above, right
    // slot (Dig2) only plus the shared bridge bit when this digit needs its top-left corner.
    const char c = '0' + rightId;
    const bool needsTopLeft = (rightId == 0 || rightId == 4 || rightId == 5 || rightId == 6 || rightId == 8 || rightId == 9);
    _buffer16[2] = needsTopLeft ? 0b00000001 : 0;
    _buffer16[3] = charTo16SegBits_Dig2(c);
  } else if (leftId >= 0 && rightId >= 0) {
    // True bundled pair -- two digits (possibly the same digit twice, e.g. "11"), each slot gets
    // its own base shape, then the confirmed selective bridge closes whichever corner is needed.
    const char lc = '0' + leftId;
    const char rc = '0' + rightId;
    uint8_t lb = (uint8_t)charTo16SegBits_Dig1(lc);
    uint8_t rb = (uint8_t)charTo16SegBits_Dig2(rc);

    // Rule 1: left digit needs bottom-right, right digit doesn't already have bottom-left
    // natively -> amend the RIGHT byte (right digit donates its own bottom-left stroke).
    const bool leftNeedsBR = (leftId != 1) && (leftId != 2);
    const bool rightLacksBL = !(rb & 0x80);
    if (leftNeedsBR && rightLacksBL) {
      rb |= 0x80;
    }

    // Rule 2: right digit needs top-left, left digit doesn't already have upper-right
    // natively -> amend the LEFT byte (left digit donates its own upper-right stroke).
    const bool rightNeedsTL = (rightId == 0 || rightId == 4 || rightId == 5 || rightId == 6 || rightId == 8 || rightId == 9);
    const bool leftLacksUR = !(lb & 0x01);
    if (rightNeedsTL && leftLacksUR) {
      lb |= 0x01;
    }

    _buffer16[2] = lb;
    _buffer16[3] = rb;
  } else {
    uint8_t wl, wr;
    if (wholeCellGlyph(str[0], str[1], wl, wr)) {   // lone E / W
      _buffer16[2] = wl;
      _buffer16[3] = wr;
      return;
    }
    // Neither resolves to a digit (blank / general alphanumeric content) -- unchanged fallback:
    // prefer str[1] over str[0] specifically when str[0] is blank, so the cell isn't wasted on
    // nothing.
    _buffer16[2] = (str[0] == ' ') ? charTo16SegBits_Dig1(str[1]) : charTo16SegBits_Dig1(str[0]);
    _buffer16[3] = charTo16SegBits_Dig2(str[1]);
  }
}

// Whole-cell glyphs for a lone 'E' or 'W' (the other character of the pair blank or absent). Byte
// layout as in the 16-segment map: left = D1 E F A1 N G1 H J, right = M L G2 K D2 C B A2.
//   E = A1 A2 F E D1 D2 G1 G2;  W = F E B C + N L (two sides and the lower diagonals meeting in the centre).
bool HT1621_OH::wholeCellGlyph(char a, char b, uint8_t &left, uint8_t &right) {
  char c;
  if (b == ' ' || b == '\0') c = a;
  else if (a == ' ') c = b;
  else return false;
  switch (c) {
    case 'E': left = 0b11110100; right = 0b00101001; return true;
    case 'W': left = 0b01101000; right = 0b01000110; return true;
    default:  return false;
  }
}

// See the declaration in the class body (HT1621_OH.h public section) for the full derivation.
// Complete 20-letter substitution table, confirmed conflict-free across all 100 real STRING_2
// samples (UFC_STRING2_DCS_Tracking.xlsx, Sep 2026): the right character of a bundled pair is
// always sent literally, and each letter always means the same digit regardless of context.
int8_t HT1621_OH::resolveDigitIdentity(char character) {
  switch (character) {
    case '0': case 'a': case 'b': case 'c': return 0;
    case '1':                                return 1;
    case '2':                                return 2;
    case '3': case 'e': case 'f': case 'g':  return 3;
    case '4': case 'h': case 'i': case 'j':  return 4;
    case '5': case 'k':                      return 5;
    case '6': case 'l':                      return 6;
    case '7': case 'm': case 'n': case 'o':  return 7;
    case '8': case 'p': case 'q': case 's':  return 8;
    case '9': case 't': case 'v': case '@':  return 9;
    default:                                 return -1;
  }
}

void HT1621_OH::print_num(const char* str, bool leftPadded){
	int chars = strlen(str);
	int padding = 8 - chars;
  int k=0;

	// O/S BRG (and any page laid out the same way) uses the whole nine-cell scratchpad: DCS builds a
	// 9-character string such as "999@99'99" (3 digits, degree, 2 digits, minute mark, 2 digits) and
	// str[i] belongs in cell i from the left -- cells 0 and 1 are the two 16-segment cells, cells 2-8
	// are the seven numeric digits. DCS-BIOS exports only the first 8 of those 9 characters
	// (UFC_SCRATCHPAD_NUMBER_DISPLAY max length 8); the missing 9th character is the digit just keyed,
	// which is rebuilt from the panel's own key press (see noteKey()). A real 9-character string is
	// used as is.
	// Recognised by a minute mark ('), or an 8+ character string whose first character is not a space
	// (the normal pages always start with the padding space).
	bool nineCell = false;
	if (!leftPadded) {
		if (chars >= 8 && str[0] != ' ') nineCell = true;
		for (int i = 0; i < chars; i++) {
			if (str[i] == '\'') nineCell = true;
		}
		// DCS-BIOS patched so the first two characters travel in STRING_2 and the number field holds the
		// other seven (padded to 8 with one leading space): the degree mark is then at index 2, never at
		// index 3 as in the stock 9-character string. That string is already complete and is drawn like a
		// normal page (STRING_2 supplies the two 16-segment cells), so the key-press rebuild is not used.
		if (chars == 8 && str[0] == ' ' && str[2] == '@') nineCell = false;
	}
	if (nineCell) {
		if (!_numClaimsAlnum) _lastKey = 0;  // just arrived on a BRG page: forget any older key
		if (chars == 8) {
			// Exported at the 8-character cap, so the 9th character is missing. It is always the digit
			// just keyed (the newest digit enters at the 9th position), so rebuild it from the key.
			char full[10];
			memcpy(full, str, 8);
			full[8] = _lastKey ? _lastKey : ' ';
			full[9] = '\0';
			memcpy(_brgLast, str, 8);
			_brgLast[8] = '\0';
			_brgCapped = true;
			printNumNineCell(full, 9);
		} else {
			_brgCapped = false;  // a real 9-character string (fixed upstream): use it as is
			printNumNineCell(str, chars);
		}
		return;
	}
	_brgCapped = false;
	_lastKey = 0;

	// Normal page. If a nine-cell string had taken over the two 16-segment cells, give them back to
	// STRING_1 / STRING_2 first by redrawing their last received text.
	if (_numClaimsAlnum) {
		_numClaimsAlnum = false;
		print_str(_str1Last, false);
		print_str2(_str2Last, false);
	}

	for(int i = 0; i < 8; i++){ // We're getting 8 chars with the first beeing "space" ... so chop of the first one
		//_buffer[i] &= 0x80; // mask the first bit, used by batter and decimal point
		char character = leftPadded
				 		 ? i < padding ? ' ' : str[i - padding]
				 		 : i >= chars ? ' ' : str[i+1];
		  _buffer[i] = charToSegBits(character);
	}
  backlight();
	update();
}

void HT1621_OH::bitTest(uint8_t phase){
	if (phase != 1 && phase != 2) return;
	// 16-segment cell, left byte: D1 E F A1 N G1 H J (bit 7..0); right byte: M L G2 K D2 C B A2.
	// Outer ring = A1 A2 B C D1 D2 E F; inner = H J K G1 G2 N M L.
	const uint8_t left  = (phase == 1) ? 0xF0 : 0x0F;
	const uint8_t right = (phase == 1) ? 0x0F : 0xF0;
	_numClaimsAlnum = true;
	_brgCapped = false;
	_lastKey = 0;
	for (int cell = 0; cell < 2; cell++) {
		_buffer16[cell * 2]     = left;
		_buffer16[cell * 2 + 1] = right;
	}
	for (int i = 0; i < 7; i++) _buffer[i] = charToSegBits('8');  // every numeric segment
	_buffer[7] = 0;
	backlight();
	update();
}

void HT1621_OH::noteKey(char key){
	const bool isDigitKey = (key >= '0' && key <= '9');
	_lastKey = isDigitKey ? key : 0;  // CLR / ENT forget the digit
	if (!_numClaimsAlnum || !_brgCapped) return;

	// Only the first digit of an entry needs an immediate redraw: the exported 8 characters do not
	// change when it is keyed (or when CLR/ENT removes it), so DCS-BIOS sends no update. Later digits
	// redraw from the next DCS-BIOS update instead.
	for (int i = 0; i < 8; i++) if (isDigit(_brgLast[i])) return;
	char full[10];
	memcpy(full, _brgLast, 8);
	full[8] = isDigitKey ? key : ' ';
	full[9] = '\0';
	printNumNineCell(full, 9);
}

// The first two characters of a nine-cell string (O/S BRG) are drawn together in the digit "8"
// 16-segment cell, exactly as a STRING_2 pair is drawn; the digit "9" cell stays blank.
void HT1621_OH::printNumNineCell(const char* str, int chars){
	_numClaimsAlnum = true;
	const int n = (chars > 9) ? 9 : chars;
	_buffer16[0] = 0;
	_buffer16[1] = 0;
	char pair[3] = { (n > 0 ? str[0] : ' '), (n > 1 ? str[1] : ' '), '\0' };
	drawCell8(pair);
	for (int i = 0; i < 7; i++) {
		const int idx = i + 2;
		_buffer[i] = charToSegBits(idx < n ? str[idx] : ' ');
	}
	_buffer[7] = 0;
	backlight();
	update();
}




/*
  |-A1-A2-|
   |\  |  /|
   F H J K B
   |  \|/  |
   |-G1-G2-|
   |  /|\  |
   E N M L C 
   |/  |  \|
   |-D1-D2-| DP
        
                 1  1 1
   High Byte:  0bDEFANGHJ  
                   2 2  2
   Low  Byte:  0bMLGKDCBA  
   */
  

char HT1621_OH::charTo16SegBits_Dig1(char character) {
  	switch (character) {
      case '-':
        return 0b00000100;
      case '0':
      case 'a': // DCS-BIOS sends 'a' instead of '0' when this digit repeats the one before it (scratchpad entry cursor quirk)
      case 'b': // DCS-BIOS sends 'b' instead of '0' in the String1/String2 bundle cells (confirmed static via
                // BIOSBuddy on live hardware -- not a transient cursor blink, it stays 'b' indefinitely). Same
                // family of cursor-quirk substitute as 'a', just a different letter used in this field/position.
      case 'c': // Added from the full 100-pair STRING_2 raw-value sweep (Sep 2026): DCS-BIOS uses a THIRD distinct
                // letter for a repeated-position '0' depending on which digit it's bundled with in this slot --
                // 'a'/'b'/'c' all mean '0' here, just chosen by context. See resolveDigitIdentity() for the
                // complete, hardware-confirmed 20-letter table this was derived from.
        // Confirmed on real hardware via 1A8_UFC_CANDIDATES2: top+UL+LL+upper-right+bottom. Bottom-right stays
        // open -- this cell has no lower-right segment at all (see '2' below, which also never needed one).
        return 0b11110001;
      case '1':
        return 0b01100000;
      case '2':
        return 0b11010101;
      case '3':
      case 'g': // DCS-BIOS sends 'g' instead of '3' in the String1/String2 bundle cells (empirically catalogued
                // via BIOSBuddy sweep -- 'a'/'i'/'l'/'p'/'v' above are the repeat-adjacent cursor quirk, but this
                // whole family of lowercase letters turns out to be the STANDARD raw glyph the jet sends for a
                // non-'1'/'2' digit in this specific display slot, repeat or not. DCS-BIOS only pre-translates 1
                // and 2 (backtick/tilde) back to literal digits before we ever see them -- everything else (0,
                // 3-9) arrives as one of these letters and was never being decoded at all before this.
      case 'e': // Added from the full 100-pair sweep: '3' also uses 'e' and 'f' depending on context, same
      case 'f': // three-letter-per-digit pattern as '0' above -- see resolveDigitIdentity().
        // Confirmed on real hardware via 1A8_UFC_CANDIDATES2: top+upper-right+middle+bottom, bottom-right open.
        return 0b10010101;
      case '4':
      case 'i': // repeat-adjacent cursor quirk (see 'a' above)
      case 'j': // standard (non-repeat) glyph for '4' in this slot -- see 'g' comment above
      case 'h': // Added from the full 100-pair sweep: third context letter for '4', same pattern as '0'/'3' above.
        // Confirmed on real hardware via 1A8_UFC_CANDIDATES2: UL+upper-right+middle.
        return 0b00100101;
      case '5':
      case 'k': // standard (non-repeat) glyph for '5' in this slot -- see 'g' comment above
        // Confirmed on real hardware via 1A8_UFC_CANDIDATES2: top+UL+middle+bottom, no right side at all.
        return 0b10110100;
      case '6':
      case 'l': // repeat-adjacent cursor quirk (see 'a' above) -- also happens to be the standard glyph for '6'
        // Dean's photo of the FULLSWEEP result showed the 0xF4 candidate (with top bar) actually
        // reading like a "b" on the glass -- dropping the top bar reads cleanly as '6' instead:
        // UL+LL+middle+bottom only, no top, no right side.
        return 0b11100100;
      case '7':
      case 'o': // standard (non-repeat) glyph for '7' in this slot -- see 'g' comment above
      case 'm': // Added from the full 100-pair sweep: third/fourth context letters for '7', same pattern as above.
      case 'n':
        // Confirmed on real hardware via 1A8_UFC_79FINAL: top+upper-right only. There is no diagonal
        // on this slot after all (bit1/bit3 both confirmed to be real diagonals, not a stand-in for
        // the missing bottom-right), and no bit can light bottom-right at all -- same ceiling as
        // '0'/'3'/'8'. A diagonal crossing the middle read worse than just leaving the corner open.
        return 0b00010001;
      case '8':
      case 'p': // repeat-adjacent cursor quirk (see 'a' above)
      case 's': // standard (non-repeat) glyph for '8' in this slot -- see 'g' comment above
      case 'q': // Added from the full 100-pair sweep: third context letter for '8', same pattern as above.
        // Confirmed on real hardware via 1A8_UFC_CANDIDATES2: top+UL+LL+upper-right+middle+bottom, bottom-right
        // open (same hardware ceiling as '0'/'3' above). This is what now distinguishes '8' from '6': '8' lights
        // upper-right, '6' doesn't.
        return 0b11110101;
      case '9':
      case 'v': // repeat-adjacent cursor quirk (see 'a' above)
      case '@': // standard (non-repeat) glyph for '9' in this slot -- see 'g' comment above
      case 't': // Added from the full 100-pair sweep: third context letter for '9', same pattern as above.
        // Confirmed on real hardware via 1A8_UFC_79FINAL: top+upper-left+upper-right+middle, no
        // diagonal, bottom-right open (same ceiling as '7' above).
        return 0b00110101;
      case 'A':
        return 0b01110100;
	  case 'B':
	  	return 0b10010001;
      case 'C': 
        return 0b11110000;
	  case 'D':
	  	return 0b00000000;
	  case 'E':
	  	return 0b00000000;
	  case 'F':
	  	return 0b00000000;
      case 'G': 
        return 0b11110000;
	  case 'H':
	  	return 0b01100100;
      case 'I': 
        return 0b10010001;
      case 'M': 
        return 0b01100010;
      case 'N':    
        return 0b01100010;
      case 'O': 
        return 0b11110000;
      case 'P': 
        return 0b01110100;
      case 'S': 
        return 0b10110100;
      case 'X': 
        return 0b00001010;
      case 'Y': 
        return 0b00000010;
      case ' ':
      default:
		    return 0b00000000;
    }
}
// Right/second-half (_buffer16[3]) segment map, confirmed on real hardware via 1A8_UFC_RIGHTBITMAP
// (Dean's hand-drawn bit map from the raw single-bit sweep):
//   bit0=top  bit1=top-right  bit2=bottom-right  bit3=bottom  bit5=middle  bit7=bottom-left
//   bit4=diagonal top-right -> middle-left   bit6=diagonal middle-left -> bottom-right
// There is NO upper-left segment on this slot at all -- the mirror-image hardware ceiling to the
// LEFT slot's missing lower-right (see charTo16SegBits_Dig1). That's why "top-left never shows" on
// every digit that would normally use it (0/4/5/6/8/9): it isn't a bug, there's nothing to light.
// Every value below already omitted upper-left correctly (matches the standard 7-segment definition
// minus that one segment) except '6', which was also missing its top bar -- fixed here.
char HT1621_OH::charTo16SegBits_Dig2(char character) {
  	switch (character) {
      case '-':
        return 0b00100000;
      case '0':
      case 'a': // DCS-BIOS sends 'a' instead of '0' when this digit repeats the one before it (scratchpad entry cursor quirk)
      case 'b': // DCS-BIOS sends 'b' instead of '0' in the String1/String2 bundle cells (see charTo16SegBits_Dig1 for details)
      case 'c': // Added from the full 100-pair sweep (Sep 2026) -- see charTo16SegBits_Dig1 for the full story.
        return 0b10001111;
      case '1':
        return 0b00000110;
      case '2':
        return 0b10101011;
      case '3':
      case 'g': // standard (non-repeat) glyph for '3' in this slot (see charTo16SegBits_Dig1 for the full story)
      case 'e':
      case 'f': // Added from the full 100-pair sweep -- see charTo16SegBits_Dig1.
        return 0b00101111;
      case '4':
      case 'i': // repeat-adjacent cursor quirk (see 'a' above)
      case 'j': // standard (non-repeat) glyph for '4' in this slot
      case 'h': // Added from the full 100-pair sweep -- see charTo16SegBits_Dig1.
        return 0b00100110;
      case '5':
      case 'k': // standard (non-repeat) glyph for '5' in this slot
        return 0b00101101;
      case '6':
      case 'l': // repeat-adjacent cursor quirk (see 'a' above) -- also the standard glyph for '6'
        // Reverted: tried adding bit0/top back in (0xAD) to match strict standard 7-segment theory,
        // but Dean confirmed that still shows the top bar and it's wrong -- same lesson as the LEFT
        // slot's '6' (the "b"-shaped photo): this font's '6' just doesn't use a top bar on either
        // slot, full stop, not a standard-segment-theory question. Back to no top: bottom-left+
        // middle+bottom-right+bottom only.
        return 0b10101100;
      case '7':
      case 'o': // standard (non-repeat) glyph for '7' in this slot
      case 'm':
      case 'n': // Added from the full 100-pair sweep -- see charTo16SegBits_Dig1.
        return 0b00000111;
      case '8':
      case 'p': // repeat-adjacent cursor quirk (see 'a' above)
      case 's': // standard (non-repeat) glyph for '8' in this slot
      case 'q': // Added from the full 100-pair sweep -- see charTo16SegBits_Dig1.
        return 0b10101111;
      case '9':
      case 'v': // repeat-adjacent cursor quirk (see 'a' above)
      case '@': // standard (non-repeat) glyph for '9' in this slot
      case 't': // Added from the full 100-pair sweep -- see charTo16SegBits_Dig1.
        return 0b00100111;
      case 'A': 
        return 0b00100111;
	  case 'B':
		return 0b10101111;
	  case 'C': 
        return 0b00001001;
	  case 'D':
	  	return 0b00000000;
	  case 'E':
	  	return 0b00000000;
	  case 'F':
	  	return 0b00000000;  
      case 'G': 
        return 0b00101101;
	  case 'H':
	  	return 0b00100110;	
      case 'I': 
        return 0b10001001;
      case 'M': 
        return 0b00010110;
      case 'N': 
        return 0b01000110;
      case 'O': 
        return 0b00001111;
      case 'P': 
        return 0b00100011;
      case 'S': 
        return 0b00101101;
      case 'X': 
        return 0b01010000;
      case 'Y': 
        return 0b10010000;
      
      case ' ':
      default:
		    return 0b00000000;
    }
}
/*
7 Segments
   |--A--|
   F	   B
   |--G--|
   E	   C
   |--D--|
    
  0b?EFADCGB 
  
  */

char HT1621_OH::charToSegBits(char character) {
	switch (character) {
  case '@': // For ° 
		return 0b00110011;
	case '|':
		return 0b01100000;
	case '-':
		return 0b00000010;
	case '_':
		return 0b00001000;
  case '.':
		return 0b01001110;
	case '\'': // Minute mark (O/S BRG): one small tick at the top right of its cell (segment B).
		return 0b00000001;
	case '0':
		return 0b01111101;
	case '1':
		return 0b00000101;
	case '2':
		return 0b01011011;
	case '3':
		return 0b00011111;
	case '4':
		return 0b00100111;
	case '5':
		return 0b00111110;
	case '6':
		return 0b01101110;
	case '7':
		return 0b00010101;
	case '8':
		return 0b01111111;
	case '9':
		return 0b00110111;
	case 'A':
	case 'a':
		return 0b01110111;
	case 'b':
	case 'B':
		return 0b01111111;
	case 'c':
	case 'C':
		return 0b01111000;
	case 'd':
	case 'D':
		return 0b01111101;
	case 'e':
	case 'E':
		return 0b01111000;
	case 'f':
	case 'F':
		return 0b01110010;
	case 'G':
	case 'g':
		return 0b01111100;
	case 'h':
	case 'H':
		return 0b01100111;
	case 'i':
	case 'I':
		return 0b00000101;
	case 'J':
	case 'j':
		return 0b00011101;
	case 'l':
	case 'L':
		return 0b01101000;
	case 'm':
	case 'M':
		return 0b1010100;
	case 'n':
	case 'N':
		return 0b01000110;
	case 'O': 
	 	return 0b01111101;
	case 'o':
		return 0b01111101;
	case 'P':
	case 'p':
		return 0b01111010; // DCS-Bios sends p for an E (in ERROR)
	case 'q':
	case 'Q':
		return 0b00110111;
	case 'r':
	case 'R':
		return 0b01110101;
	case 'S':
	case 's':
		return 0b00111110;
	case 't':
	case 'T':
		return 0b0001111;
	case 'u':
	//	return 0b1001100;
	case 'U':
		return 0b01101101;
    case 'w':
		return 0b01110111; // DCS-Bios sends w for an R (in ERROR)
	case 'Y':
	case 'y':
		return 0b1101011;
	case 'z':
	case 'Z':
		return 0b0111110;
	case ' ':
	default:
		return 0b0000000;
	}
}
#endif
