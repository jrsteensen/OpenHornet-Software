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
 * @note    The backlight state is now decoupled from display buffer content.
 *          set_backlight_brightness() applies the PWM level directly; subsequent
 *          print_num() / print_str() calls re-apply the same stored level.
 *          The backlight will only be off if set_backlight_brightness(0) was called.
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

 //#define HT1621_DEBUG


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
   * @details Always calls analogWrite(_backlight_p, _brightness_p), so the
   *          backlight level is determined solely by the last call to
   *          set_backlight_brightness().  Called internally by print_num()
   *          and print_str() after each display update.
   */
  void backlight();

  /**
   * @brief  Set the backlight PWM brightness and apply it immediately.
   *
   * @details Stores @p brightness in _brightness_p then calls
   *          analogWrite(_backlight_p, _brightness_p) directly, bypassing the
   *          old buffer-content check.  The new level persists across all
   *          subsequent print_num() and print_str() calls until changed again.
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
   * @brief  Send the LCDON command to enable the LCD bias voltage.
   */
  void display();

  /**
   * @brief  Send the LCDOFF command to disable the LCD bias voltage (blanks display).
   */
  void noDisplay();

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
  /** @brief 16-segment display buffer: four bytes encoding two alphanumeric character positions. */
  char _buffer16[4];
  /** @brief Reserved battery-indicator segment bytes (not currently used by UFC). */
  unsigned char _battery[3];

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
  _brightness_p = brightness;
  // Fixed: previously called backlight() here, which evaluated buffer content and wrote 0 to the
  // hardware pin if the display happened to be blank at the moment of the call — overriding the
  // stored brightness even though _brightness_p was correctly saved. Now applies directly so the
  // physical PWM always reflects the requested level regardless of display state.
  analogWrite(_backlight_p, _brightness_p);
}

void HT1621_OH::backlight()
{
  // Fixed: previously gated analogWrite on buffer content (all-zero buffer → analogWrite 0),
  // which caused the backlight to turn off whenever print_num/print_str was called with blank
  // content, even if a valid brightness had been set. Brightness state is now owned solely by
  // set_backlight_brightness(); backlight() just re-applies the stored level on every display write.
  analogWrite(_backlight_p, _brightness_p);
  delay(1);
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
}



void HT1621_OH::print_str(const char* str, bool leftPadded){
  for(int i = 0; i < 4; i++){
      if ( i == 0 | i == 2){
        if (str[i] == ' ' && !isDigit(str[i+1]) ){
          _buffer16[i] =charTo16SegBits_Dig1(str[i+1]);
        }else if ( str[i+1] == '4' || str[i+1] == '5' || str[i+1] == '6' || str[i+1] == '8' || str[i+1] == '9' || str[i+1] == '0'  ){
           char tmp_seg = charTo16SegBits_Dig1(str[i]);
           tmp_seg |= 0x01;
          _buffer16[i] = tmp_seg;
        }else{
          _buffer16[i] = charTo16SegBits_Dig1(str[i]);
        }
      }else{
        _buffer16[i] = charTo16SegBits_Dig2(str[i]);
      }
  }
  backlight();
  update();
}

void HT1621_OH::print_num(const char* str, bool leftPadded){
	int chars = strlen(str);
	int padding = 8 - chars;
  int k=0;

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
        return 0b00000001;
      case '1': 
        return 0b01100000;
      case '2': 
        return 0b11010101;
      case '3': 
        return 0b00000000;
      case '4': 
        return 0b00000001;
      case '5': 
        return 0b00000001;
      case '6': 
        return 0b00000001;
      case '7': 
        return 0b00000111;
      case '8': 
        return 0b00000001;
      case '9': 
        return 0b00100111;
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
char HT1621_OH::charTo16SegBits_Dig2(char character) {
  	switch (character) {
      case '-':
        return 0b00100000;
      case '0': 
        return 0b10001111;
      case '1': 
        return 0b00000110;
      case '2': 
        return 0b10101011;
      case '3': 
        return 0b00101111;
      case '4': 
        return 0b00100110;
      case '5': 
        return 0b00101101;
      case '6': 
        return 0b10101100;
      case '7': 
        return 0b00000111;
      case '8': 
        return 0b10101111;
      case '9': 
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
