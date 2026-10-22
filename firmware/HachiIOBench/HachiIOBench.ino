#include "BenchLogic.h"
#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

// UNO/Nano (5 V) bench wiring: HC-SR04 TRIG=11 ECHO=12;
// common-cathode discrete RGB channels: R=2 G=4 B=6 with resistors.
// For 3.3 V boards: level-shift the HC-SR04 echo; verify pin mapping.
constexpr uint8_t PIN_TRIG = 11;
constexpr uint8_t PIN_ECHO = 12;
constexpr uint8_t PIN_RED = 2;
constexpr uint8_t PIN_GREEN = 4;
constexpr uint8_t PIN_BLUE = 6;
constexpr uint32_t SAMPLE_INTERVAL_US = 100000UL;
constexpr uint32_t ECHO_TIMEOUT_US = 30000UL;

enum Phase : uint8_t { TRIGGER_LOW, TRIGGER_HIGH, WAIT_RISE, WAIT_FALL, WAIT_SAMPLE };
Phase phase = WAIT_SAMPLE;
uint32_t phaseSince = 0;
uint32_t lastSample = 0;
uint32_t riseAt = 0;
bench::State indicator = bench::NO_READING;
bool automatic = true;
char command[24];
uint8_t used = 0;
bool overflow = false;

void setIndicator(bench::State state) {
  indicator = state;
  // FAR means a valid distant echo, NEVER an all-clear or safe-to-proceed.
  digitalWrite(PIN_RED, state == bench::CLOSE || state == bench::NEAR ? HIGH : LOW);
  digitalWrite(PIN_GREEN, state == bench::NEAR || state == bench::FAR ? HIGH : LOW);
  digitalWrite(PIN_BLUE, state == bench::NO_READING ? HIGH : LOW);
}

void report(bench::Observation observation, const char *source) {
  setIndicator(bench::classify(observation, indicator));
  Serial.print(F("SAMPLE,"));
  Serial.print(millis());
  Serial.print(',');
  Serial.print(source);
  Serial.print(',');
  Serial.print(observation.valid ? 1 : 0);
  Serial.print(',');
  if (observation.valid) Serial.print(observation.cm);
  else Serial.print(F("NA"));
  Serial.print(',');
  Serial.println(bench::stateName(indicator));
}

void finishEcho(uint32_t duration) {
  report(bench::fromEchoMicroseconds(duration), "AUTO");
  lastSample = micros();
  phase = WAIT_SAMPLE;
}

void pollUltrasonic(uint32_t now) {
  switch (phase) {
    case WAIT_SAMPLE:
      if (bench::elapsed(now, lastSample, SAMPLE_INTERVAL_US)) {
        digitalWrite(PIN_TRIG, LOW);
        phaseSince = now;
        phase = TRIGGER_LOW;
      }
      break;
    case TRIGGER_LOW:
      if (bench::elapsed(now, phaseSince, 3U)) {
        digitalWrite(PIN_TRIG, HIGH);
        phaseSince = now;
        phase = TRIGGER_HIGH;
      }
      break;
    case TRIGGER_HIGH:
      if (bench::elapsed(now, phaseSince, 10U)) {
        digitalWrite(PIN_TRIG, LOW);
        phaseSince = now;
        phase = WAIT_RISE;
      }
      break;
    case WAIT_RISE:
      if (digitalRead(PIN_ECHO) == HIGH) {
        riseAt = now;
        phase = WAIT_FALL;
      } else if (bench::elapsed(now, phaseSince, ECHO_TIMEOUT_US)) {
        finishEcho(0);
      }
      break;
    case WAIT_FALL:
      if (digitalRead(PIN_ECHO) == LOW) finishEcho(static_cast<uint32_t>(now - riseAt));
      else if (bench::elapsed(now, riseAt, ECHO_TIMEOUT_US)) finishEcho(0);
      break;
  }
}

void handleCommand(char *line) {
  // No String/heap allocation. Accepted lines: ?, A, X, M <cm>.
  if (strcmp(line, "?") == 0) {
    Serial.println(F("HELP,?,A(auto),X(inject no reading),M <2..400>(inject cm)"));
  } else if (strcmp(line, "A") == 0) {
    automatic = true;
    lastSample = micros();
    phase = WAIT_SAMPLE;
    Serial.println(F("MODE,AUTO"));
  } else if (strcmp(line, "X") == 0) {
    automatic = false;
    digitalWrite(PIN_TRIG, LOW);
    report({false, 0}, "INJECT");
  } else if (line[0] == 'M' && line[1] == ' ') {
    char *end = nullptr;
    long value = strtol(line + 2, &end, 10);
    if (end == line + 2 || *end != '\0' || value < 2 || value > 400) {
      Serial.println(F("ERR,RANGE_OR_FORMAT"));
      return;
    }
    automatic = false;
    digitalWrite(PIN_TRIG, LOW);
    report(bench::manualDistance(value), "INJECT");
  } else {
    Serial.println(F("ERR,UNKNOWN_COMMAND"));
  }
}

void pollSerial() {
  // Bound work per loop so sustained input cannot starve echo timing.
  for (uint8_t count = 0; count < 8 && Serial.available(); ++count) {
    const char c = static_cast<char>(Serial.read());
    if (c == '\r') continue;
    if (c == '\n') {
      if (overflow) Serial.println(F("ERR,LINE_TOO_LONG"));
      else if (used) { command[used] = '\0'; handleCommand(command); }
      used = 0;
      overflow = false;
    } else if (!overflow) {
      if (used < sizeof(command) - 1) command[used++] = c;
      else overflow = true;
    }
  }
}

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  digitalWrite(PIN_TRIG, LOW);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);
  setIndicator(bench::NO_READING);
  Serial.begin(115200);
  lastSample = micros();
  Serial.println(F("HACHI_IO_BENCH,1,UNVALIDATED_HARDWARE"));
  Serial.println(F("HELP,send ? then newline"));
}

void loop() {
  pollSerial();
  if (automatic) pollUltrasonic(micros());
}
