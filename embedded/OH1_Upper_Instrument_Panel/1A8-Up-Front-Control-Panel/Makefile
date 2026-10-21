# Dependencies for the UFC ESP32-S3 firmware.
# Names must match the libraries/ submodule directory names.
LIBRARIES = dcs-bios-arduino-library Wire \
            Adafruit_TCA8418 Adafruit_BusIO Adafruit_ADS1X15 \
            PCA95x5 FastLED TM16xx

# makeEspArduino scans library subdirectories without evaluating preprocessor
# guards. Keep FastLED's ESP32 sources and omit other platforms/features.
EXCLUDE_DIRS = /FastLED/(examples|tests|extras)/|/TM16xx/examples/|/FastLED/src/platforms/(adafruit|apollo3|arm|avr|neopixelbus|posix|stub|wasm|win|esp/8266)/|/FastLED/src/platforms/(shared/ui|esp/32/audio)/|/FastLED/src/(fonts|fx|sensors)/

include $(ROOTDIR)/include/s3mini.mk
