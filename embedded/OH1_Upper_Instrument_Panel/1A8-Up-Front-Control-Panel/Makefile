# Dependencies for the UFC ESP32-S3 firmware.
# Names must match the libraries/ submodule directory names.
LIBRARIES = dcs-bios-arduino-library Wire \
            Adafruit_TCA8418 Adafruit_BusIO Adafruit_ADS1X15 \
            PCA95x5 FastLED TM16xx

# makeEspArduino scans library subdirectories recursively. Exclude
# examples/tests to avoid compiling unrelated demo sources (FastLED xypaths).
EXCLUDE_DIRS = /FastLED/(examples|tests|extras)/|/TM16xx/examples/

# FastLED's implementation lives in unity-build translation units, which
# makeEspArduino's header-driven source discovery does not find on its own.
CUSTOM_LIBS += $(wildcard $(ROOTDIR)/libraries/FastLED/src/fl/build/*.cpp)
INCLUDE_DIRS += $(ROOTDIR)/libraries/FastLED/src

include $(ROOTDIR)/include/s3mini.mk
