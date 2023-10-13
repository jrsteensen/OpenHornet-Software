OBJDIR             = build
TARGET             = $(notdir $(CURDIR))
ARDUINO_SKETCHBOOK = $(dir $(realpath $(CURDIR)/..))
ARDMK_DIR          = $(dir $(realpath $(CURDIR)/..))/include/Arduino-Makefile

include $(ARDMK_DIR)/Arduino.mk
