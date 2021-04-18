#include "tool/io_operation.hpp"
#include <unity.h>

inline void io_operation_write_and_interpret_little_endian() {
  using namespace dxl::tool;

  UnalignedData<4 * sizeof(uint16_t)> unaligned_data{Endianess::little};
  unaligned_data.write(uint16_t{0xabcd}, sizeof(uint16_t));
  TEST_ASSERT_EQUAL_UINT16(0xabcd, unaligned_data.interpret_as<uint16_t>(sizeof(uint16_t)));
}

inline void io_operation_write_and_interpret_big_endian() {
  using namespace dxl::tool;

  UnalignedData<4 * sizeof(uint16_t)> unaligned_data{Endianess::big};
  unaligned_data.write(uint16_t{0xabcd}, sizeof(uint16_t));
  TEST_ASSERT_EQUAL_UINT16(0xabcd, unaligned_data.interpret_as<uint16_t>(sizeof(uint16_t)));
}

inline void io_operation_set_and_get_little_endian() {
  using namespace dxl::tool;

  UnalignedArguments<int, char, long, bool> unaligned_arguments{Endianess::little};
  unaligned_arguments.set<2>(0xabcd);
  TEST_ASSERT_EQUAL_UINT16(0xabcd, unaligned_arguments.get<2>());
}

inline void io_operation_set_and_get_big_endian() {
  using namespace dxl::tool;

  UnalignedArguments<int, char, long, bool> unaligned_arguments{Endianess::big};
  unaligned_arguments.set<2>(0xabcd);
  TEST_ASSERT_EQUAL_UINT16(0xabcd, unaligned_arguments.get<2>());
}

inline void io_operation_iterate_unaligned_data() {
  using namespace dxl::tool;

  byte_t raw_data[] {0xaa, 0xbb, 0xcc};
  UnalignedData<sizeof(raw_data)> unaligned_data{raw_data, Endianess::little};
  size_t i = 0;
  for (auto byte : unaligned_data) TEST_ASSERT_EQUAL_UINT16(raw_data[i++], byte);
}

inline void io_operation_iterate_unaligned_arguments() {
  using namespace dxl::tool;

  byte_t raw_data[] {0xaa, 0xcc, 0xbb, 0x00, 0xff, 0xee, 0xdd};
  UnalignedArguments<uint8_t, uint16_t, uint32_t> unaligned_data{Endianess::little, 0xaa, 0xbbcc, 0xddeeff00};
  size_t i = 0;
  for (auto byte : unaligned_data) TEST_ASSERT_EQUAL_UINT16(raw_data[i++], byte);
}

inline void io_operation_serialize_with_unaligned_arguments() {
  using namespace dxl::tool;

  byte_t raw_data[] {0xaa, 0xbb, 0xcc, 0xdd};
  auto unaligned_arguments = make_unaligned_arguments(Endianess::big, byte_t{0xaa}, byte_t{0xbb}, uint16_t{0xccdd});
  TEST_ASSERT_EQUAL_UINT8(raw_data[0], unaligned_arguments.raw_data()[0]);
  TEST_ASSERT_EQUAL_UINT8(raw_data[1], unaligned_arguments.raw_data()[1]);
  TEST_ASSERT_EQUAL_UINT8(raw_data[2], unaligned_arguments.raw_data()[2]);
  TEST_ASSERT_EQUAL_UINT8(raw_data[3], unaligned_arguments.raw_data()[3]);
}
