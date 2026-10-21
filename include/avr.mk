# Output compiled files to the build directory
OBJDIR             = build
# Set the compiled firmware filename to the directory name (OpenHornet component name)
TARGET             = $(notdir $(CURDIR))
ARDUINO_SKETCHBOOK = $(ROOTDIR)
ARDUINO_VERSION    = 10819
ARDMK_DIR          = $(ROOTDIR)/include/Arduino-Makefile
ARDUINO_LIBS       = $(LIBRARIES)

include $(ROOTDIR)/include/openhornet.mk
include $(ARDMK_DIR)/Arduino.mk

release: all
	cp $(TARGET_HEX) $(RELEASE_DIR)/
	cp $(TARGET_EEP) $(RELEASE_DIR)/
