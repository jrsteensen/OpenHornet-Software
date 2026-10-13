# Any extra libraries included by this sketch (space separated)
LIBRARIES = dcs-bios-arduino-library Stepper MultiMap FastLED U8g2 SPI Wire

# Arduino-Makefile defines __PROG_TYPES_COMPAT__ by default (the Arduino IDE does not).
# With it set, U8g2's clib/mui.h crashes avr-gcc 7.3 with an internal compiler error,
# so turn it off for this sketch. This is the opt-out documented in Arduino.mk.
CPPFLAGS += -U__PROG_TYPES_COMPAT__

# Uncomment one of the following to choose the target board
include $(ROOTDIR)/include/mega2560.mk
# include $(ROOTDIR)/include/promicro.mk
# include $(ROOTDIR)/include/promini.mk
# include $(ROOTDIR)/include/s2mini.mk
