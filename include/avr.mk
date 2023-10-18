# Output compiled files to the build directory
OBJDIR             = build
# Set the compiled firmware filename to the directory name (OpenHornet component name)
TARGET             = $(notdir $(CURDIR))
ARDUINO_SKETCHBOOK = $(dir $(realpath $(CURDIR)/..))
ARDMK_DIR          = $(dir $(realpath $(CURDIR)/..))include/Arduino-Makefile

include $(dir $(realpath $(CURDIR)/..))include/openhornet.mk
include $(ARDMK_DIR)/Arduino.mk

release: all
	cp $(TARGET_HEX) $(RELEASE_DIR)/
	cp $(TARGET_EEP) $(RELEASE_DIR)/
