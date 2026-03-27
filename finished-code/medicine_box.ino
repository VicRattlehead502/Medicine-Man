/*
 * Medicine Man - Smart Medicine Reminder Box
 *
 * Board:  Arduino UNO R4 WiFi
 * Hardware:
 *   - 2x Modulino Buzzer (I2C) — one per compartment
 *   - 4x LEDs (2 per compartment) — pins 2,3 and 4,5
 *   - 1x DFRobot SEN0556 IR Sensor — pin 6 (shared lid detection)
 *
 * The web interface is embedded and served by the on-board HTTP server.
 * Connect to the same WiFi network, then open the IP printed on Serial.
 */

#include <WiFiS3.h>
#include <Modulino.h>
#include "web_page.h"

// ======================== WiFi (WPA2) ========================
// Replace with your network credentials
char ssid[] = "YOUR_NETWORK_SSID";
char pass[] = "YOUR_NETWORK_PASSWORD";

WiFiServer server(80);

// ======================== Pin Definitions ========================
const int LED1_A = 2;
const int LED1_B = 3;
const int LED2_A = 4;
const int LED2_B = 5;
const int IR_PIN = 6;

// ======================== Modulino Buzzers ========================
// Both buzzers sit on the I2C bus. If they share the same default
// address, change one module's address via its DIP switch / solder
// pad, then pass the new address to the second constructor.
ModulinoBuzzer buzzer1;
ModulinoBuzzer buzzer2;

// ======================== Configurable Variables ========================
// "time" — countdown in seconds (set from the web interface)
unsigned long time1 = 0;
unsigned long time2 = 0;

// "pitch" — buzzer frequency in Hz (set from the web interface)
//   High = 1000, Medium = 700, Low = 300
int pitch = 700;

String name1 = "Medication1";
String name2 = "Medication2";

// ======================== State Machine ========================
//  IDLE     → timer counting down, everything off
//  ALARM    → timer expired: buzzer beeps, LEDs shine (solid)
//  BLINKING → box opened: buzzer off, LEDs blink for 10 s
enum State { IDLE, ALARM, BLINKING };

State  state1      = IDLE;
State  state2      = IDLE;

unsigned long timer1Start = 0;
unsigned long timer2Start = 0;
unsigned long blink1Start = 0;
unsigned long blink2Start = 0;

const unsigned long BLINK_DURATION  = 10000;   // 10 s of blinking
const unsigned long BLINK_INTERVAL  = 300;     // LED toggle rate (ms)
const unsigned long BEEP_PERIOD     = 1000;    // buzzer cycle length (ms)
const unsigned long BEEP_ON_TIME    = 500;     // tone duration within each cycle

unsigned long lastBeep1 = 0;
unsigned long lastBeep2 = 0;

// ======================== Setup ========================
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED1_A, OUTPUT);
  pinMode(LED1_B, OUTPUT);
  pinMode(LED2_A, OUTPUT);
  pinMode(LED2_B, OUTPUT);
  pinMode(IR_PIN, INPUT);

  Modulino.begin();
  buzzer1.begin();
  buzzer2.begin();

  Serial.print("Connecting to WiFi (");
  delay(3000);
  
  Serial.print(ssid);
  Serial.print(")");

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("Open in browser: http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

// ======================== Main Loop ========================
void loop() {
  unsigned long now = millis();

  // IR sensor: adjust polarity to match your wiring.
  // LOW = no object detected = lid removed = box opened
  bool boxOpened = (digitalRead(IR_PIN) == LOW);

  // --- State machines ---
  updateState(state1, time1, timer1Start, blink1Start, boxOpened, now);
  updateState(state2, time2, timer2Start, blink2Start, boxOpened, now);

  // --- LEDs ---
  bool blinkPhase = ((now / BLINK_INTERVAL) % 2 == 0);

  driveLeds(state1, LED1_A, LED1_B, blinkPhase);
  driveLeds(state2, LED2_A, LED2_B, blinkPhase);

  // --- Buzzers ---
  driveBuzzer(buzzer1, state1, lastBeep1, now);
  driveBuzzer(buzzer2, state2, lastBeep2, now);

  // --- HTTP ---
  handleClient();
}

// ======================== State Machine ========================
void updateState(State &state, unsigned long timeVal,
                 unsigned long &timerStart, unsigned long &blinkStart,
                 bool boxOpened, unsigned long now) {
  switch (state) {
    case IDLE:
      if (timeVal > 0 && timerStart > 0 &&
          (now - timerStart) >= (timeVal * 1000UL)) {
        state = ALARM;
      }
      break;

    case ALARM:
      if (boxOpened) {
        state = BLINKING;
        blinkStart = now;
      }
      break;

    case BLINKING:
      if ((now - blinkStart) >= BLINK_DURATION) {
        state = IDLE;
        timerStart = now;          // restart countdown
      }
      break;
  }
}

// ======================== LED Driver ========================
void driveLeds(State state, int pinA, int pinB, bool blinkPhase) {
  switch (state) {
    case ALARM:
      digitalWrite(pinA, HIGH);
      digitalWrite(pinB, HIGH);
      break;
    case BLINKING:
      digitalWrite(pinA, blinkPhase);
      digitalWrite(pinB, blinkPhase);
      break;
    default:
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
      break;
  }
}

// ======================== Buzzer Driver ========================
void driveBuzzer(ModulinoBuzzer &buz, State state,
                 unsigned long &lastBeep, unsigned long now) {
  if (state == ALARM) {
    if (now - lastBeep >= BEEP_PERIOD) {
      lastBeep = now;
      buz.tone(pitch, BEEP_ON_TIME);
    }
  }
  // When not in ALARM the last tone expires on its own.
  // For an immediate cut-off when transitioning out of ALARM,
  // uncomment the line below (depends on your Modulino library version):
  // else { buz.tone(0, 0); }
}

// ======================== HTTP Server ========================
void handleClient() {
  WiFiClient client = server.available();
  if (!client) return;

  unsigned long timeout = millis() + 3000;
  while (!client.available() && millis() < timeout) delay(1);
  if (!client.available()) { client.stop(); return; }

  String reqLine = client.readStringUntil('\n');

  // Consume remaining headers
  while (client.available()) {
    String hdr = client.readStringUntil('\n');
    if (hdr.length() <= 1) break;
  }

  int sp1 = reqLine.indexOf(' ');
  int sp2 = reqLine.indexOf(' ', sp1 + 1);
  String path = reqLine.substring(sp1 + 1, sp2);

  if (path.startsWith("/api")) {
    handleApi(client, path);
  } else {
    sendHtml(client);
  }

  delay(2);
  client.stop();
}

// ---- Serve the embedded web page ----
void sendHtml(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();

  const char *ptr = HTML_PAGE;
  size_t len = strlen(HTML_PAGE);
  size_t sent = 0;
  while (sent < len) {
    size_t chunk = min(len - sent, (size_t)512);
    client.write(&ptr[sent], chunk);
    sent += chunk;
  }
}

// ---- API endpoint ----
void handleApi(WiFiClient &client, String path) {
  String action = getParam(path, "action");
  String value  = getParam(path, "value");
  String nameP  = getParam(path, "name");

  String json = "{\"ok\":true}";

  if (action == "setTime1") {
    time1 = value.toInt();
    if (nameP.length() > 0) name1 = urlDecode(nameP);
    timer1Start = millis();
    state1 = IDLE;

  } else if (action == "setTime2") {
    time2 = value.toInt();
    if (nameP.length() > 0) name2 = urlDecode(nameP);
    timer2Start = millis();
    state2 = IDLE;

  } else if (action == "setPitch") {
    pitch = value.toInt();
    if (pitch < 100)  pitch = 300;
    if (pitch > 2000) pitch = 1000;

  } else if (action == "clearTime1") {
    time1 = 0;
    timer1Start = 0;
    state1 = IDLE;
    digitalWrite(LED1_A, LOW);
    digitalWrite(LED1_B, LOW);

  } else if (action == "clearTime2") {
    time2 = 0;
    timer2Start = 0;
    state2 = IDLE;
    digitalWrite(LED2_A, LOW);
    digitalWrite(LED2_B, LOW);

  } else if (action == "status") {
    json  = "{\"time1\":";  json += time1;
    json += ",\"time2\":";  json += time2;
    json += ",\"pitch\":";  json += pitch;
    json += ",\"name1\":\""; json += name1; json += "\"";
    json += ",\"name2\":\""; json += name2; json += "\"";
    json += ",\"state1\":\""; json += stateName(state1); json += "\"";
    json += ",\"state2\":\""; json += stateName(state2); json += "\"";
    json += "}";
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();
  client.println(json);
}

// ======================== Helpers ========================
String stateName(State s) {
  switch (s) {
    case IDLE:     return "idle";
    case ALARM:    return "alarm";
    case BLINKING: return "blinking";
  }
  return "unknown";
}

String getParam(String url, String key) {
  int qIdx = url.indexOf('?');
  if (qIdx < 0) return "";
  String query = url.substring(qIdx + 1);
  String search = key + "=";
  int kIdx = query.indexOf(search);
  if (kIdx < 0) return "";
  int vStart = kIdx + search.length();
  int vEnd   = query.indexOf('&', vStart);
  if (vEnd < 0) vEnd = query.length();
  return query.substring(vStart, vEnd);
}

String urlDecode(String str) {
  String decoded = "";
  for (unsigned int i = 0; i < str.length(); i++) {
    if (str[i] == '+') {
      decoded += ' ';
    } else if (str[i] == '%' && i + 2 < str.length()) {
      char hex[3] = { str[i + 1], str[i + 2], 0 };
      decoded += (char)strtol(hex, NULL, 16);
      i += 2;
    } else {
      decoded += str[i];
    }
  }
  return decoded;
}
