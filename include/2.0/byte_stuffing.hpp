#pragma once

#include <string.h>

namespace dxl {

//
//
//
constexpr uint8_t escaped_pattern[] = {0xff, 0xff, 0xfd};
constexpr uint8_t escaping_character = 0xfd;

//
inline size_t stuff_bytes(uint8_t* dest, const uint8_t* src, size_t length) {
  constexpr auto pattern_size = sizeof(escaped_pattern);

  if (length < pattern_size) {
    memcpy(dest, src, length);
    return length;
  }

  auto src_cursor = src + pattern_size;
  auto dest_cursor = dest + pattern_size;
  memcpy(dest, src, pattern_size);

  for (; src_cursor < src + length; src_cursor++, dest_cursor++) {
    if (!memcmp(src_cursor - pattern_size, escaped_pattern, pattern_size))
      *dest_cursor++ = escaping_character;
    *dest_cursor = *src_cursor;
  }

  return dest_cursor - dest;
}

} // namespace dxl
