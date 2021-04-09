#include "tool/io_operation.hpp"
#include <unity.h>

inline void set_and_interpret_in_unaligned_data_little_endian() {
  UnalignedData<sizeof(uint16_t)> unaligned_data;
  unaligned_data.set((uint16_t) 0xabcd, 0, Endianess::little);
  TEST_ASSERT_EQUAL_UINT16(0xabcd, unaligned_data.interpret_as(ctm::type_h<uint16_t>{}, 0, Endianess::little));
}

inline void set_and_interpret_in_unaligned_data_big_endian() {
  UnalignedData<sizeof(uint16_t)> unaligned_data;
  unaligned_data.set((uint16_t) 0xabcd, 0, Endianess::big);
  TEST_ASSERT_EQUAL_UINT16(0xabcd, unaligned_data.interpret_as(ctm::type_h<uint16_t>{}, 0, Endianess::big));
}
