# LOLIN S3 Mini with native USB OTG/TinyUSB CDC and HID.
BOARD     = lolin_s3_mini
# Preserve UFC CDC/HID transport instead of selecting hardware USB-JTAG.
BUILD_EXTRA_FLAGS = -DARDUINO_USB_MODE=0 -DARDUINO_USB_CDC_ON_BOOT=1

include $(ROOTDIR)/include/esp.mk
