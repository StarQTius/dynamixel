#pragma once

#include "2.0/byte_stuffing.hpp"

void byte_stuffing_stuff_bytes() {
  using dxl::stuff_bytes;

  uint8_t src[] = {
    0xff, 0xff, 0xff, 0xfd,
    0xaa, 0xff, 0xfd, 0xff,
    0xff, 0xbb, 0xff, 0x00,
    0xfd, 0xff, 0xff, 0xfd,
    0xff, 0xcc, 0xfd, 0xdd };
  uint8_t dest[64];
  auto length = stuff_bytes(dest, src, sizeof(src));

  uint8_t expected[] = {
    0xff, 0xff, 0xff, 0xfd, 0xfd,
    0xaa, 0xff, 0xfd, 0xff,
    0xff, 0xbb, 0xff, 0x00,
    0xfd, 0xff, 0xff, 0xfd, 0xfd,
    0xff, 0xcc, 0xfd, 0xdd };
  TEST_ASSERT_EQUAL_UINT16(sizeof(src) + 2, length);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, dest, length);
}
