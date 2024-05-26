#define NUM_LEDS1 LC1_LED_COUNT                 ///< Number of LC Channel 1 LEDs
#define NUM_LEDS2 LC2_LED_COUNT                 ///< Number of LC Channel 2 LEDs

CRGB leds1[NUM_LEDS1];                          ///< LED array for LC Channel 1
CRGB leds2[NUM_LEDS2];                          ///< LED array for LC Channel 2

void sequentialBootUpTest() {
  // Sequentially light up each LED
  for (int i = 0; i < NUM_LEDS1; i++) {
    leds1[i] = CRGB::Green;
    FastLED.show();
    delay(50); // Adjust delay for desired speed
  }
  for (int i = 0; i < NUM_LEDS2; i++) {
    leds2[i] = CRGB::Green;
    FastLED.show();
    delay(5); // Adjust delay for desired speed
  }
  delay(50); // Hold for a moment

  // Turn off all LEDs
  fill_solid(leds1, NUM_LEDS1, CRGB::Black);
  fill_solid(leds2, NUM_LEDS2, CRGB::Black);
  FastLED.show();
}

void pulseWaveTest() {
  for (int j = 0; j < 3; j++) { // Repeat the pulse three times
    for (int brightness = 0; brightness <= 255; brightness += 5) {
      fill_solid(leds1, NUM_LEDS1, CRGB(brightness, brightness, brightness));
      fill_solid(leds2, NUM_LEDS2, CRGB(brightness, brightness, brightness));
      FastLED.show();
      delay(30);
    }
    for (int brightness = 255; brightness >= 0; brightness -= 5) {
      fill_solid(leds1, NUM_LEDS1, CRGB(brightness, brightness, brightness));
      fill_solid(leds2, NUM_LEDS2, CRGB(brightness, brightness, brightness));
      FastLED.show();
      delay(3);
    }
  }
}

void rapidFlashTest() {
  for (int i = 0; i < 5; i++) {
    fill_solid(leds1, NUM_LEDS1, CRGB::White);
    fill_solid(leds2, NUM_LEDS2, CRGB::White);
    FastLED.show();
    delay(100);
    fill_solid(leds1, NUM_LEDS1, CRGB::Black);
    fill_solid(leds2, NUM_LEDS2, CRGB::Black);
    FastLED.show();
    delay(100);
  }
}

void strobeLightEffect() {
  const int strobeDuration = 100; // Total duration of the strobe effect in milliseconds
  const int strobeInterval = 50;  // Interval between strobe flashes in milliseconds
  const int strobeCycles = strobeDuration / strobeInterval;
  const int delayBetweenStrobes = 3000; // Delay between strobe repetitions in milliseconds

  for (int repeat = 0; repeat < 3; repeat++) { // Repeat 3 times
    for (int i = 0; i < strobeCycles; i++) {
      // Turn on all LEDs to white
      fill_solid(leds1, NUM_LEDS1, CRGB::White);
      fill_solid(leds2, NUM_LEDS2, CRGB::White);
      FastLED.show();
      delay(strobeInterval);

      // Turn off all LEDs
      fill_solid(leds1, NUM_LEDS1, CRGB::Black);
      fill_solid(leds2, NUM_LEDS2, CRGB::Black);
      FastLED.show();
      delay(strobeInterval);
    }
    delay(delayBetweenStrobes); // Delay between repetitions
  }
  setBacklightBrightness(brtns);
  applyMode(0);
}

void fluorescentEffect() {
  const int flashCount = 4; // Number of random flashes
  const int minFlashDuration = 20; // Minimum duration of a flash in milliseconds
  const int maxFlashDuration = 200; // Maximum duration of a flash in milliseconds
  const int steadyDuration = 3000; // Duration for steady light in milliseconds

  // Random flashes
  for (int i = 0; i < flashCount; i++) {
    int flashDuration = random(minFlashDuration, maxFlashDuration);
    
    // Turn on all LEDs to white
    fill_solid(leds1, NUM_LEDS1, CRGB::Green);
    fill_solid(leds2, NUM_LEDS2, CRGB::Green);
    FastLED.show();
    delay(flashDuration);

    // Turn off all LEDs
    fill_solid(leds1, NUM_LEDS1, CRGB::Black);
    fill_solid(leds2, NUM_LEDS2, CRGB::Black);
    FastLED.show();
    delay(flashDuration);
  }

  // Steady light
  fill_solid(leds1, NUM_LEDS1, CRGB::Green);
  fill_solid(leds2, NUM_LEDS2, CRGB::Green);
  FastLED.show();
  delay(steadyDuration);

  // Turn off all LEDs
  fill_solid(leds1, NUM_LEDS1, CRGB::Black);
  fill_solid(leds2, NUM_LEDS2, CRGB::Black);
  FastLED.show();
}