#include "main.hpp"

void setup() {
  UNITY_BEGIN();

  RUN_TEST(byte_stuffing_stuff_bytes);
  RUN_TEST(crc_compute_crc);

  UNITY_END();
}

void loop() {}
