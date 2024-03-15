ifndef ESP_ROOT
# Path to the ESP32 Arduino core.
ESP_ROOT = $(ROOTDIR)/libraries/arduino-esp32
endif

LIBRARY_DIR        = $(ROOTDIR)/libraries
ESPMK_DIR          = $(ROOTDIR)/include/makeEspArduino
# Include libraries from the libraries directory for linking.
CUSTOM_LIBS        = $(LIBRARY_DIR)
# Exclude platform libraries to avoid compilation and linking errors.
EXCLUDE_DIRS	   = $(LIBRARY_DIR)/Servo|$(LIBRARY_DIR)/Arduino_Boards|$(ESP_ROOT)
CHIP			   = esp32
BUILD_ROOT         = build
BUILD_DIR          = build
# Set the compiled firmware filename to the directory name (OpenHornet component name)
MAIN_NAME          = $(notdir $(CURDIR))
# These flags are set automatically to blank strings.
# Override them so that the debug build output doesn't fail.
BUILD_EXTRA_FLAGS  = -DARDUINO_HOST_OS=\"linux\" -DARDUINO_FQBN=\"esp32:esp32:esp32s2\"

include $(ROOTDIR)/include/openhornet.mk
include $(ESPMK_DIR)/makeEspArduino.mk


release: all
	cp $(MAIN_EXE) $(RELEASE_DIR)/
