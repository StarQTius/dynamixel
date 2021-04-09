#pragma once

#include <Arduino.h>

enum class Endianess { little, big };
extern Endianess platform_endianess;

//
inline Endianess get_endianess() {
  uint16_t word = 0xff00;
  return (*reinterpret_cast<uint8_t*>(&word) == 0xff ? Endianess::big : Endianess::little );
}
