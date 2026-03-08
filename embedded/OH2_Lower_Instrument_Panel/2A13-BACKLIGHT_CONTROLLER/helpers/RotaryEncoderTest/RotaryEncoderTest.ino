* Rotary Encoder Test Sketch
* Outputs "Turning" when rotated, "PUSH" when pressed
*/

#include "RotaryEncoder.h"

// Pin definitions (from Board.h)
const int ENC_SW = 24;  // Switch/push button
const int ENC_A  = 22;  // Encoder A
const int ENC_B  = 23;  // Encoder B

RotaryEncoder encoder(ENC_A, ENC_B, RotaryEncoder::LatchMode::TWO03);
int lastPos = 0;

void setup() {
   Serial.begin(9600);
   pinMode(ENC_SW, INPUT_PULLUP);
   Serial.println("Rotary Encoder Test Ready");
}

void loop() {
   encoder.tick();
   
   // Check rotation
   int newPos = encoder.getPosition();
   if (newPos != lastPos) {
       Serial.println("Turning");
       lastPos = newPos;
   }
   
   // Check button press
   static bool lastButtonState = HIGH;
   bool buttonState = digitalRead(ENC_SW);
   if (buttonState == LOW && lastButtonState == HIGH) {
       Serial.println("PUSH");
       delay(50);  // Simple debounce
   }
   lastButtonState = buttonState;
}
