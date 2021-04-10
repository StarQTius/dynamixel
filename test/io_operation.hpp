#include "tool/io_operation.hpp"
#include <unity.h>

inline void write_and_interpret_in_unaligned_data_little_endian() {
  UnalignedData<4 * sizeof(uint16_t)> unaligned_data{Endianess::little};
  unaligned_data.write((uint16_t) 0xabcd, sizeof(uint16_t));
  TEST_ASSERT_EQUAL_UINT16(0xabcd, unaligned_data.interpret_as<uint16_t>(sizeof(uint16_t)));
}

inline void write_and_interpret_in_unaligned_data_big_endian() {
  UnalignedData<4 * sizeof(uint16_t)> unaligned_data{Endianess::big};
  unaligned_data.write((uint16_t) 0xabcd, sizeof(uint16_t));
  TEST_ASSERT_EQUAL_UINT16(0xabcd, unaligned_data.interpret_as<uint16_t>(sizeof(uint16_t)));
}

inline void set_and_get_in_unaligned_arguments_little_endian() {
  UnalignedArguments<int, char, long, bool> unaligned_arguments{Endianess::little};
  unaligned_arguments.set<2>(0xabcd);
  TEST_ASSERT_EQUAL(0xabcd, unaligned_arguments.get<2>());
}

inline void set_and_get_in_unaligned_arguments_big_endian() {
  UnalignedArguments<int, char, long, bool> unaligned_arguments{Endianess::big};
  unaligned_arguments.set<2>(0xabcd);
  TEST_ASSERT_EQUAL(0xabcd, unaligned_arguments.get<2>());
}
