#include <Arduino_Modulino.h>

const int LED_PIN = 13;

ModulinoBuzzer buzzer;

const size_t FREQ = 440;     // Hz
const size_t BEEP_MS = 200;  // how long it beeps
const size_t GAP_MS  = 1000;  // silence between beeps

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Modulino.begin();
  buzzer.begin();
}

void loop() {
  digitalWrite(LED_PIN, HIGH);

  buzzer.tone(FREQ, BEEP_MS);
  delay(BEEP_MS);

  buzzer.noTone();
  

  delay(GAP_MS);
}

