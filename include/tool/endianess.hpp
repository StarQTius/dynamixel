#pragma once

#include "type.hpp"

namespace dxl {
namespace tool {

enum class Endianess { little, big };
extern Endianess platform_endianess;

//
inline Endianess get_endianess() {
  unsigned short word = 0xff00;
  return (*reinterpret_cast<byte_t*>(&word) == 0xff ? Endianess::big : Endianess::little );
}

} // namespace tool
} // namespace dxl
