#pragma once

/***********************************************************************************************************************
  Dans le protocole de communication 2.0, les paquets ont un octet correspondant à l'instruction. Les énumérateurs
  d'Instruction correspondent chacun à une des instructions disponibles.
***********************************************************************************************************************/

namespace dxl {

enum class Instruction : uint8_t {
  ping = 0x1,
  read = 0x2,
  write = 0x3,
  reg_write = 0x4,
  action = 0x5,
  factory_reset = 0x6,
  reboot = 0x8,
  clear = 0x10,
  status = 0x55,
  sync_read = 0x82,
  sync_write = 0x83,
  bulk_read = 0x92,
  bulk_write = 0x93
};

} // namespace dxl
