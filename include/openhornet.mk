OBJDIR             = build
TARGET             = $(notdir $(CURDIR))
ARDUINO_SKETCHBOOK = $(dir $(realpath $(CURDIR)/..))
ARDMK_DIR          = $(dir $(realpath $(CURDIR)/..))/include/Arduino-Makefile
RELEASE_DIR        = $(dir $(realpath $(CURDIR)/..))/release

include $(ARDMK_DIR)/Arduino.mk

release:
	cp $(TARGET_HEX) $(RELEASE_DIR)/
	cp $(TARGET_EEP) $(RELEASE_DIR)/
