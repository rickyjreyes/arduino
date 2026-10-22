#include <Arduino.h>

// Nonblocking reproduction of the untouched extensionless led sketch.
// Yellow pin 2: ten 100 ms on/off cycles; red pin 7: 100 ms on/off.
constexpr uint8_t YELLOW_PIN = 2;
constexpr uint8_t RED_PIN = 7;
constexpr uint32_t STEP_MS = 100;
uint32_t previousStep = 0;
uint8_t stepIndex = 0; // 0..19 yellow; 20..21 red

void showStep() {
  digitalWrite(YELLOW_PIN, stepIndex < 20 && (stepIndex % 2) == 0 ? HIGH : LOW);
  digitalWrite(RED_PIN, stepIndex == 20 ? HIGH : LOW);
}

void setup() {
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  previousStep = millis();
  showStep();
}

void loop() {
  const uint32_t now = millis();
  if (static_cast<uint32_t>(now - previousStep) >= STEP_MS) {
    // Advance one phase; no blocking delay, no catch-up burst after stalls.
    previousStep = now;
    stepIndex = static_cast<uint8_t>((stepIndex + 1U) % 22U);
    showStep();
  }
}
