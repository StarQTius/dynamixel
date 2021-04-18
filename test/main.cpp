#include <unity.h>

#include "byte_stuffing.hpp"
#include "io_operation.hpp"

void setup() {
  UNITY_BEGIN();

  TEST_MESSAGE(dxl::tool::platform_endianess == dxl::tool::Endianess::little ?
    "La platforme utilise le petit-boutisme" : "La platforme utilise le gros-boutisme");

  RUN_TEST(io_operation_write_and_interpret_little_endian);
  RUN_TEST(io_operation_write_and_interpret_big_endian);
  RUN_TEST(io_operation_set_and_get_little_endian);
  RUN_TEST(io_operation_set_and_get_big_endian);
  RUN_TEST(io_operation_iterate_unaligned_data);
  RUN_TEST(io_operation_iterate_unaligned_arguments);
  RUN_TEST(io_operation_serialize_with_unaligned_arguments);

  RUN_TEST(byte_stuffing_stuff_bytes);

  UNITY_END();
}

void loop() {}
