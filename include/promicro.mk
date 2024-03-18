ifndef PROMICRO_DIR
PROMICRO_DIR = $(ROOTDIR)/libraries/Arduino_Boards/sparkfun
endif

ALTERNATE_CORE      = promicro
ALTERNATE_CORE_PATH = $(PROMICRO_DIR)/avr
AVRDUDE_OPTS        = -v
BOARDS_TXT          = $(PROMICRO_DIR)/avr/boards.txt
BOARD_TAG           = promicro
BOARD_SUB           = 16MHzatmega32U4
BOOTLOADER_PARENT   = $(PROMICRO_DIR)/avr/bootloaders
BOOTLOADER_PATH     = caterina
BOOTLOADER_FILE     = Caterina-promicro16.hex
ISP_PROG            = usbasp
USB_PID             = 0x9206

include $(ROOTDIR)/include/avr.mk
