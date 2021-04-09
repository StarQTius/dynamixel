#include <unity.h>

#include "io_operation.cpp"

void setup() {
  RUN_TEST(set_and_interpret_in_unaligned_data_little_endian);
  RUN_TEST(set_and_interpret_in_unaligned_data_big_endian);
}

void loop() {}
