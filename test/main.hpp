#if defined(UT_ONLY)

#define INTEGRATION_TEST TEST_IGNORE()

void setup();
int main() { setup(); return 0; }

#else // defined(UT_ONLY)

#define INTEGRATION_TEST
#include <Arduino.h>

#endif // defined(UT_ONLY)

#include <unity.h>

extern "C" void setUp() {}
extern "C" void tearDown() {}

#include "byte_stuffing.hpp"
#include "io_operation.hpp"