Import("env")
from SCons.Node.FS import File

# Restrict Adafruit TinyUSB Library compilation to ESP32 port sources only.
def _tinyusb_filter(node):
    if not isinstance(node, File):
        return node
    src = node.get_path().replace("\\", "/")
    if "Adafruit TinyUSB Library/src/arduino/ports/" in src:
        if "/ports/esp32/" not in src:
            return None
    return node

env.AddBuildMiddleware(_tinyusb_filter, "*")
