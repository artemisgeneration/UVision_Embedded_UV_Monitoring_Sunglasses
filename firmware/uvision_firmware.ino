/*
 * Project: UVision - Embedded Wearable UV Monitoring System
 * Author: Victoria Morris
 *
 * Description:
 *   Embedded firmware for a wearable UV monitoring system using the
 *   SparkFun AS7331 UV sensor. The system computes a real-time UV Index,
 *   displays user feedback on a 128x32 OLED, and activates a vibration
 *   motor when exposure exceeds a defined threshold.
 *
 * Hardware:
 *   - Arduino Nano ESP32
 *   - SparkFun AS7331 UV Sensor (I2C, interrupt-driven)
 *   - SSD1306 128x32 OLED Display (I2C, address 0x3C)
 *   - PWM-capable vibration motor module
 *
 * Libraries Required:
 *   - SparkFun_AS7331
 *   - Adafruit_GFX
 *   - Adafruit_SSD1306
 *   - Wire (built-in)
 *
 * Last Updated: November 2025
 */

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_AS7331.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* --------------------------------------------------------------------------
 * Hardware Configuration
 * -------------------------------------------------------------------------- */

// I2C UV sensor object
SfeAS7331ArdI2C myUVSensor;

// OLED configuration
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pin assignments
const uint8_t SYN_PIN        = 3;  // Triggers synchronous UV measurements
const uint8_t INT_PIN        = 2;  // AS7331 data-ready interrupt
const uint8_t MOTOR_PIN     = 5;  // Vibration motor control (PWM-capable)

// Update timing
const unsigned long UPDATE_INTERVAL_MS = 1000;

/* --------------------------------------------------------------------------
 * Global State
 * -------------------------------------------------------------------------- */

volatile bool newDataReady = false;
unsigned long lastUpdateTime = 0;

/* --------------------------------------------------------------------------
 * Interrupt Service Routine
 * -------------------------------------------------------------------------- */

void IRAM_ATTR dataReadyInterrupt() {
  newDataReady = true;
}

/* --------------------------------------------------------------------------
 * Utility Functions
 * -------------------------------------------------------------------------- */

// Returns WHO-style guidance text based on UV Index
String getWHOGuidance(float uvIndex) {
  if (uvIndex < 3.0) {
    return "Low: No protection needed";
  } else if (uvIndex < 8.0) {
    return "Moderate: Use SPF30+, hat, sunglasses";
  } else {
    return "Extreme: Stay indoors if possible!";
  }
}

// Draw word-wrapped text on OLED display
void drawWrappedText(const String& text, int x, int y, int maxWidth) {
  int cursorX = x;
  int cursorY = y;
  const int charWidth  = 6;
  const int lineHeight = 10;

  String word;

  for (int i = 0; i <= text.length(); i++) {
    char c = text[i];
    if (c == ' ' || c == '\0') {
      int wordWidth = word.length() * charWidth;
      if (cursorX + wordWidth >= maxWidth) {
        cursorX = x;
        cursorY += lineHeight;
      }
      display.setCursor(cursorX, cursorY);
      display.print(word);
      cursorX += wordWidth + charWidth;
      word = "";
    } else {
      word += c;
    }
  }
}

/* --------------------------------------------------------------------------
 * Setup
 * -------------------------------------------------------------------------- */

void setup() {
  Serial.begin(115200);

  // Allow time for serial monitor connection (non-blocking)
  unsigned long startTime = millis();
  while (!Serial && millis() - startTime < 2000) {
    delay(10);
  }

  Serial.println("Initializing UVision system...");

  // Initialize I2C (ESP32 default pins)
  Wire.begin(21, 22);

  // Motor output
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERROR: OLED not detected.");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Initializing...");
  display.display();

  // Sync and interrupt pins
  pinMode(SYN_PIN, OUTPUT);
  digitalWrite(SYN_PIN, HIGH);
  pinMode(INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), dataReadyInterrupt, RISING);

  // Initialize UV sensor
  if (!myUVSensor.begin()) {
    Serial.println("ERROR: UV sensor initialization failed.");
    while (true);
  }

  if (!myUVSensor.prepareMeasurement(MEAS_MODE_SYNS)) {
    Serial.println("ERROR: UV sensor measurement setup failed.");
    while (true);
  }

  myUVSensor.setStartState(true);

  // Trigger initial measurement
  digitalWrite(SYN_PIN, LOW);
  delay(1);
  digitalWrite(SYN_PIN, HIGH);

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Sensor Ready!");
  display.display();

  Serial.println("System ready.");
}

/* --------------------------------------------------------------------------
 * Main Loop
 * -------------------------------------------------------------------------- */

void loop() {
  if (!newDataReady) return;
  if (millis() - lastUpdateTime < UPDATE_INTERVAL_MS) return;

  newDataReady = false;
  lastUpdateTime = millis();

  if (myUVSensor.readAllUV() != ksfTkErrOk) {
    Serial.println("ERROR: Failed to read UV data.");
    return;
  }

  // Empirical scaling (prototype calibration)
  const float SCALE_PW_PER_CM2 = 0.005;

  float uva = myUVSensor.getUVA() * SCALE_PW_PER_CM2;
  float uvb = myUVSensor.getUVB() * SCALE_PW_PER_CM2;

  // Temporary fallback if UVB channel is unreliable
  uvb = uva;

  float uvIndex = (uvb * 4.0 + uva) * 0.01;
  String guidance = getWHOGuidance(uvIndex);

  Serial.print("UV Index: ");
  Serial.println(uvIndex, 2);
  Serial.print("Guidance: ");
  Serial.println(guidance);

  // Update OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("UV Index: ");
  display.println(uvIndex, 2);
  drawWrappedText(guidance, 0, 12, SCREEN_WIDTH);
  display.display();

  // Haptic feedback
  if (uvIndex >= 8.0) {
    Serial.println("High UV detected. Activating vibration alert.");
    digitalWrite(MOTOR_PIN, HIGH);
    delay(500);
    digitalWrite(MOTOR_PIN, LOW);
  }

  // Trigger next synchronized measurement
  digitalWrite(SYN_PIN, LOW);
  delay(1);
  digitalWrite(SYN_PIN, HIGH);
}

