ifndef PROMICRO_DIR
PROMICRO_DIR = $(dir $(realpath $(CURDIR)/..))libraries/Arduino_Boards/sparkfun
endif

ALTERNATE_CORE      = promicro
ALTERNATE_CORE_PATH = $(PROMICRO_DIR)/avr
AVRDUDE_OPTS        = -v
BOARDS_TXT          = $(PROMICRO_DIR)/avr/boards.txt
BOOTLOADER_PARENT   = $(PROMICRO_DIR)/avr/bootloaders
BOOTLOADER_PATH     = caterina
BOOTLOADER_FILE     = Caterina-promicro16.hex
ISP_PROG            = usbasp
USB_PID             = 0x9206
