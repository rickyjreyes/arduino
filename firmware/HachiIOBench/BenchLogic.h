#ifndef HACHI_IO_BENCH_LOGIC_H
#define HACHI_IO_BENCH_LOGIC_H

#include <stdint.h>

// Hardware-independent bench logic. This is supplemental instrumentation,
// not collision detection or a validated safety decision.
namespace bench {

enum State : uint8_t { NO_READING = 0, CLOSE = 1, NEAR = 2, FAR = 3 };

struct Observation {
  bool valid;
  uint16_t cm;
};

inline Observation fromEchoMicroseconds(uint32_t duration) {
  // Typical HC-SR04 usable range is approximately 2-400 cm; 0 is no echo.
  // Convert round-trip echo time to approximate distance (58 us/cm).
  if (duration < 87U || duration > 23229U) return {false, 0};
  const uint32_t cm = (duration + 29U) / 58U;
  if (cm < 2U || cm > 400U) return {false, 0};
  return {true, static_cast<uint16_t>(cm)};
}

inline Observation manualDistance(long cm) {
  if (cm < 2 || cm > 400) return {false, 0};
  return {true, static_cast<uint16_t>(cm)};
}

inline State classify(Observation observation, State previous) {
  if (!observation.valid || observation.cm < 2 || observation.cm > 400)
    return NO_READING;
  const uint16_t d = observation.cm;
  // Initial bands: <=50 CLOSE, 51..149 NEAR, >=150 FAR.
  // A 10 cm hysteresis margin avoids oscillation at both transitions.
  switch (previous) {
    case CLOSE: return d <= 60 ? CLOSE : (d >= 160 ? FAR : NEAR);
    case NEAR:  return d <= 40 ? CLOSE : (d >= 160 ? FAR : NEAR);
    case FAR:   return d >= 140 ? FAR : (d <= 40 ? CLOSE : NEAR);
    default:    return d <= 50 ? CLOSE : (d >= 150 ? FAR : NEAR);
  }
}

inline const char *stateName(State state) {
  switch (state) {
    case CLOSE: return "CLOSE";
    case NEAR: return "NEAR";
    case FAR: return "FAR";
    default: return "NO_READING";
  }
}

// uint32_t unsigned subtraction handles micros()/millis() rollover for
// intervals strictly smaller than 2^31 ticks.
inline bool elapsed(uint32_t now, uint32_t since, uint32_t interval) {
  return static_cast<uint32_t>(now - since) >= interval;
}

} // namespace bench
#endif
