#include "../firmware/HachiIOBench/BenchLogic.h"
#include <cassert>
#include <cstdint>
#include <iostream>

int main() {
  using namespace bench;
  assert(!fromEchoMicroseconds(0).valid);
  assert(!fromEchoMicroseconds(30).valid);
  assert(!fromEchoMicroseconds(30000).valid);
  assert(fromEchoMicroseconds(116).valid && fromEchoMicroseconds(116).cm == 2);
  assert(fromEchoMicroseconds(580).valid && fromEchoMicroseconds(580).cm == 10);
  assert(fromEchoMicroseconds(23200).valid && fromEchoMicroseconds(23200).cm == 400);
  assert(!manualDistance(-1).valid && !manualDistance(0).valid);
  assert(!manualDistance(1).valid && !manualDistance(401).valid);
  assert(manualDistance(2).valid && manualDistance(400).valid);
  assert(classify({false, 0}, FAR) == NO_READING);
  assert(classify({true, 0}, FAR) == NO_READING);
  assert(classify({true, 401}, FAR) == NO_READING);
  assert(classify({true, 50}, NO_READING) == CLOSE);
  assert(classify({true, 51}, NO_READING) == NEAR);
  assert(classify({true, 149}, NO_READING) == NEAR);
  assert(classify({true, 150}, NO_READING) == FAR);
  // Transition hysteresis, both directions, and invalid recovery.
  assert(classify({true, 55}, CLOSE) == CLOSE);
  assert(classify({true, 60}, CLOSE) == CLOSE);
  assert(classify({true, 61}, CLOSE) == NEAR);
  assert(classify({true, 45}, NEAR) == NEAR);
  assert(classify({true, 40}, NEAR) == CLOSE);
  assert(classify({true, 155}, NEAR) == NEAR);
  assert(classify({true, 160}, NEAR) == FAR);
  assert(classify({true, 145}, FAR) == FAR);
  assert(classify({true, 140}, FAR) == FAR);
  assert(classify({true, 139}, FAR) == NEAR);
  assert(classify({true, 20}, FAR) == CLOSE);
  assert(classify({true, 200}, CLOSE) == FAR);
  assert(classify({true, 80}, NO_READING) == NEAR);
  assert(elapsed(100, 0, 100));
  assert(!elapsed(99, 0, 100));
  assert(elapsed(3, UINT32_MAX - 3U, 7));
  assert(!elapsed(2, UINT32_MAX - 3U, 7));
  assert(stateName(NO_READING)[0] == 'N');
  std::cout << "bench logic tests passed\n";
}
