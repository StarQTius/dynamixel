#include <Arduino.h>
#include <SoftwareSerial.h>

#include "2.0/instruction.hpp"
#include "2.0/packet.hpp"

SoftwareSerial software_serial{D10, D11};

void setup() {
  software_serial.begin(9600);
}

void loop() {
  dxl::send_packet(software_serial, 0, dxl::Instruction::write, uint16_t{116}, uint32_t{0x400});
}
