#include <Arduino.h>
#include <SoftwareSerial.h>

#include "2.0/instruction.hpp"
#include "2.0/packet.hpp"

SoftwareSerial software_serial{D10, D11};
uint8_t ADR = 8;

void setup() {

  Serial.begin(9600);
  software_serial.begin(57600);

  dxl::send_packet(software_serial, ADR, dxl::Instruction::write, uint16_t{64}, uint8_t{1});
  delay(2000);

  pinMode(D11, OUTPUT);
  dxl::send_packet(software_serial, ADR, dxl::Instruction::write, uint16_t{116}, uint32_t{100});
  delayMicroseconds(250);
  pinMode(D11, INPUT);
  delay(2000);
}

void loop() {
  delay(100);
  dxl::send_packet(software_serial, ADR, dxl::Instruction::write, uint16_t{64}, uint8_t{0});
  delay(100);
  dxl::send_packet(software_serial, ADR, dxl::Instruction::write, uint16_t{64}, uint8_t{1});

  /*dxl::send_packet(software_serial, 1, dxl::Instruction::write, uint16_t{116}, uint32_t{10});
  delay(2000);

  dxl::send_packet(software_serial, 1, dxl::Instruction::write, uint16_t{116}, uint32_t{20});
  delay(2000);

  dxl::send_packet(software_serial, 1, dxl::Instruction::write, uint16_t{116}, uint32_t{30});
  delay(2000);*/
}
