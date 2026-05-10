ifndef ESP_ROOT
# Path to the ESP32 Arduino core.
ESP_ROOT = $(ROOTDIR)/libraries/arduino-esp32
endif

LIBRARY_DIR        = $(ROOTDIR)/libraries
ESPMK_DIR          = $(ROOTDIR)/include/makeEspArduino
# Include only the repo libraries named by each sketch. ESP core libraries are
# found separately by makeEspArduino.
LIBS               = $(wildcard $(addprefix $(LIBRARY_DIR)/,$(LIBRARIES)))
CHIP               = esp32
CPU                ?= esp32
BUILD_ROOT         = build
BUILD_DIR          = build
# Set the compiled firmware filename to the directory name (OpenHornet component name)
MAIN_NAME          = $(notdir $(CURDIR))
MERGED_BIN         = $(BUILD_DIR)/$(MAIN_NAME).merged.bin
BOOTLOADER_BIN     = $(BUILD_DIR)/$(MAIN_NAME).bootloader.bin
PARTITIONS_BIN     = $(BUILD_DIR)/$(MAIN_NAME).partitions.bin
BOOT_APP_BIN       = $(ESP_ROOT)/tools/partitions/boot_app0.bin
BOOTLOADER_ADDR    = 0x1000
PARTITIONS_ADDR    = 0x8000
BOOT_APP_ADDR      = 0xe000
APP_ADDR           = 0x10000
MERGE_BIN_SIZE     = $(if $(FLASH_DEF),$(FLASH_DEF),4MB)
# These flags are set automatically to blank strings.
# Override them so that the debug build output doesn't fail.
BUILD_EXTRA_FLAGS  = -DARDUINO_HOST_OS=\"linux\" -DARDUINO_FQBN=\"esp32:esp32:esp32s2\"

include $(ROOTDIR)/include/openhornet.mk
include $(ESPMK_DIR)/makeEspArduino.mk


$(MERGED_BIN): all
	test -f "$(BOOTLOADER_BIN)"
	test -f "$(PARTITIONS_BIN)"
	test -f "$(BOOT_APP_BIN)"
	test -f "$(MAIN_EXE)"
	$(ESPTOOL) --chip $(MCU) merge-bin -o $@ --pad-to-size $(MERGE_BIN_SIZE) --flash-mode keep --flash-freq keep --flash-size keep $(BOOTLOADER_ADDR) "$(BOOTLOADER_BIN)" $(PARTITIONS_ADDR) "$(PARTITIONS_BIN)" $(BOOT_APP_ADDR) "$(BOOT_APP_BIN)" $(APP_ADDR) "$(MAIN_EXE)"

release: $(MERGED_BIN)
	cp $(MERGED_BIN) $(RELEASE_DIR)/$(MAIN_NAME).bin
