#include <Arduino_Modulino.h>

const int LED_PIN = 13;
const int irSensorPin = 8;

ModulinoBuzzer buzzer;

const size_t FREQ = 440;
const size_t BEEP_MS = 200;
const size_t GAP_MS = 800;
const unsigned long sleepDuration =20000;

unsigned long sleepStartTime = 0;
bool alarmRinging = false;
bool sawNothingFirst = false;
const int DETECT_SOMETHING = LOW;  
const int DETECT_NOTHING   = HIGH; 

void setup() {
  Serial.begin(9600);

  Modulino.begin();
  buzzer.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(irSensorPin, INPUT);

  sleepStartTime = millis();
  Serial.println("Goodnight! Alarm is set.");
}

void loop() {
  unsigned long now = millis();


  if (!alarmRinging && (now - sleepStartTime >= sleepDuration)) {
    alarmRinging = true;
    sawNothingFirst = false; 
    Serial.println("PILLS, PILLS, PILLS.");
  }

  if (!alarmRinging) {
    digitalWrite(LED_PIN, LOW);
    return;
  }

  int sensorState = digitalRead(irSensorPin);

  
  if (!sawNothingFirst && sensorState == DETECT_NOTHING) {
    sawNothingFirst = true;
    Serial.println("Cap lifted (nothing detected).");
  }

  
  if (sawNothingFirst && sensorState == DETECT_SOMETHING) {
    Serial.println("Hand detected after cap lift. Alarm reset.");
    alarmRinging = false;
    sawNothingFirst = false;
    sleepStartTime = now;
    buzzer.noTone();
    
    digitalWrite(LED_PIN, HIGH);
    delay(10000);
    return;
  }

  Serial.println("TAKE IT.");
  digitalWrite(LED_PIN, HIGH);
  buzzer.tone(FREQ, BEEP_MS);
  delay(BEEP_MS);

  buzzer.noTone();
  digitalWrite(LED_PIN, LOW);
  delay(GAP_MS);
}