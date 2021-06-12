#if defined(UT_ONLY)

void setup();
int main() { setup(); return 0; }

#else // defined(UT_ONLY)

#include <Arduino.h>

#include "packet.hpp"

#endif // defined(UT_ONLY)

#include "unity.h"

extern "C" void setUp() {}
extern "C" void tearDown() {}

#include "byte_stuffing.hpp"
#include "crc.hpp"
