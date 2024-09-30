/**
* @brief Backlight Brightness
* This function controls the brightness of the panels backlight
*/
void setBacklightBrightness(uint8_t brtns) {
  FastLED.setBrightness(brtns);
  FastLED.show();
}

void dynamicRainbow() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i] = CHSV(hue + (i * 10), 255, 255);
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    leds2[i] = CHSV(hue + (i * 10), 255, 255);
  }
  FastLED.show();
  hue++;
  delay(1); // Adjust to control the speed of the rainbow
}

void confetti() {
  fadeToBlackBy(leds1, NUM_LEDS1, 10);
  int pos = random16(NUM_LEDS1);
  leds1[pos] += CHSV(random8(), 200, 255);

  fadeToBlackBy(leds2, NUM_LEDS2, 10);
  pos = random16(NUM_LEDS2);
  leds2[pos] += CHSV(random8(), 200, 255);

  FastLED.show();
}

void sinelon() {
  fadeToBlackBy(leds1, NUM_LEDS1, 20);
  int pos = beatsin16(13, 0, NUM_LEDS1 - 1);
  leds1[pos] += CHSV(0, 255, 192);

  fadeToBlackBy(leds2, NUM_LEDS2, 20);
  pos = beatsin16(13, 0, NUM_LEDS2 - 1);
  leds2[pos] += CHSV(0, 255, 192);

  FastLED.show();
}


void juggle() {
    fadeToBlackBy(leds1, NUM_LEDS1, 20);
    for (int i = 0; i < 8; i++) {
        leds1[beatsin16(i + 7, 0, NUM_LEDS1 - 1)] |= CHSV(i * 32, 200, 255);
    }

    fadeToBlackBy(leds2, NUM_LEDS2, 20);
    for (int i = 0; i < 8; i++) {
        leds2[beatsin16(i + 7, 0, NUM_LEDS2 - 1)] |= CHSV(i * 32, 200, 255);
    }

    FastLED.show();
}