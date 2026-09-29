#include <DFRobotDFPlayerMini.h>

// =============================
// PINS
// =============================

#define PIEZO1_PIN 34
#define PIEZO2_PIN 35

#define GREEN_LED 25
#define RED_LED   26

#define DFPLAYER_RX 16
#define DFPLAYER_TX 17

HardwareSerial dfSerial(2);
DFRobotDFPlayerMini player;

// =============================
// SETTINGS
// =============================

const int HIT_THRESHOLD_1 = 300;
const int HIT_THRESHOLD_2 = 300;

const unsigned long COOLDOWN_1 = 120;
const unsigned long COOLDOWN_2 = 120;

const unsigned long LED_TIME = 150;

// =============================
// TIMERS
// =============================

unsigned long lastHit1 = 0;
unsigned long lastHit2 = 0;

unsigned long greenLedStart = 0;
unsigned long redLedStart = 0;

bool greenLedOn = false;
bool redLedOn = false;

// =============================
// SETUP
// =============================

void setup() {

  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  dfSerial.begin(
    9600,
    SERIAL_8N1,
    DFPLAYER_RX,
    DFPLAYER_TX
  );

  delay(2000);

  Serial.println();
  Serial.println("Starting 2-Piezo Electronic Drum...");

  if (!player.begin(dfSerial)) {

    Serial.println("DFPlayer NOT detected!");
    Serial.println("Check power, RX/TX, SD card.");

    while (true) {

      digitalWrite(RED_LED, HIGH);
      delay(300);

      digitalWrite(RED_LED, LOW);
      delay(300);
    }
  }

  Serial.println("DFPlayer detected!");

  player.volume(30);

  Serial.println("System Ready");
  Serial.println("Piezo 1 -> Track 1");
  Serial.println("Piezo 2 -> Track 2");
}

// =============================
// LOOP
// =============================

void loop() {

  handleLEDs();
  handlePiezo1();
  handlePiezo2();

  delay(2);
}

// =============================
// LED CONTROL
// =============================

void handleLEDs() {

  if (greenLedOn &&
      millis() - greenLedStart >= LED_TIME) {

    digitalWrite(GREEN_LED, LOW);
    greenLedOn = false;
  }

  if (redLedOn &&
      millis() - redLedStart >= LED_TIME) {

    digitalWrite(RED_LED, LOW);
    redLedOn = false;
  }
}

// =============================
// PIEZO 1
// =============================

void handlePiezo1() {

  int value = analogRead(PIEZO1_PIN);

  if (
    value > HIT_THRESHOLD_1 &&
    millis() - lastHit1 >= COOLDOWN_1
  ) {

    int peak = value;

    unsigned long startTime = millis();

    while (millis() - startTime < 15) {

      int reading = analogRead(PIEZO1_PIN);

      if (reading > peak) {
        peak = reading;
      }
    }

    Serial.print("PIEZO 1 HIT. Peak = ");
    Serial.println(peak);

    digitalWrite(GREEN_LED, HIGH);

    greenLedStart = millis();
    greenLedOn = true;

    // Track 1
    player.play(1);

    lastHit1 = millis();
  }
}

// =============================
// PIEZO 2
// =============================

void handlePiezo2() {

  int value = analogRead(PIEZO2_PIN);

  if (
    value > HIT_THRESHOLD_2 &&
    millis() - lastHit2 >= COOLDOWN_2
  ) {

    int peak = value;

    unsigned long startTime = millis();

    while (millis() - startTime < 15) {

      int reading = analogRead(PIEZO2_PIN);

      if (reading > peak) {
        peak = reading;
      }
    }

    Serial.print("PIEZO 2 HIT. Peak = ");
    Serial.println(peak);

    digitalWrite(RED_LED, HIGH);

    redLedStart = millis();
    redLedOn = true;

    // Track 2
    player.play(2);

    lastHit2 = millis();
  }
}
