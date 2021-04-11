#include <unity.h>

#include "io_operation.hpp"

void setup() {
  UNITY_BEGIN();

  TEST_MESSAGE(platform_endianess == Endianess::little ?
    "La platforme utilise le petit-boutisme" : "La platforme utilise le gros-boutisme");

  RUN_TEST(write_and_interpret_in_unaligned_data_little_endian);
  RUN_TEST(write_and_interpret_in_unaligned_data_big_endian);
  RUN_TEST(set_and_get_in_unaligned_arguments_little_endian);
  RUN_TEST(set_and_get_in_unaligned_arguments_big_endian);
  RUN_TEST(iterate_unaligned_data);

  UNITY_END();
}

void loop() {}
