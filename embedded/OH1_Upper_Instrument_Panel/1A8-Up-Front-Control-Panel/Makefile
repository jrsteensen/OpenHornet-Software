# Dependencies for the UFC ESP32-S3 firmware.
# Names must match the libraries/ submodule directory names.
LIBRARIES = dcs-bios-arduino-library Wire \
            Adafruit_TCA8418 Adafruit_BusIO Adafruit_ADS1X15 \
            PCA95x5 FastLED TM16xx Adafruit_TinyUSB_Arduino

# makeEspArduino scans library subdirectories recursively. Exclude
# examples/tests to avoid compiling unrelated demo sources (FastLED xypaths).
EXCLUDE_DIRS = /FastLED/(examples|tests|extras)/|/TM16xx/examples/|/Adafruit_TinyUSB_Arduino/(examples|test)/

include $(ROOTDIR)/include/s3mini.mk
