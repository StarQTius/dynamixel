#pragma once

#include "2.0/crc.hpp"

inline void crc_compute_crc() {
  constexpr uint8_t payload[] = {0xff, 0xff, 0xfd, 0x00, 0x01, 0x07, 0x00, 0x02, 0x84, 0x00, 0x04, 0x00};

  auto crc = dxl::compute_crc(payload, sizeof(payload));
  TEST_ASSERT_EQUAL_UINT16(0x151d, crc);
}
