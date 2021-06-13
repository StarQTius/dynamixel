#pragma once

#include <Arduino.h>

#include "byte_stuffing.hpp"
#include "instruction.hpp"
#include "crc.hpp"

#include "upd/format.hpp"
#include "upd/storage.hpp"

namespace dxl {
namespace detail {

constexpr uint8_t packet_header[] = {0xff, 0xff, 0xfd, 0x00};

//
inline void send_packet(
  Stream& stream,
  uint8_t* buffer,
  uint8_t id,
  const uint8_t* payload,
  uint16_t payload_size)
{
  using namespace upd;

  auto inserted_bytes_nb = stuff_bytes(buffer, payload, payload_size);
  auto header = make_tuple<endianess::LITTLE, signed_mode::TWO_COMPLEMENT>(
    packet_header,
    id,
    static_cast<uint16_t>(inserted_bytes_nb + 2));

  auto crc = compute_crc(0, header.begin(), header.size);
  crc = compute_crc(crc, buffer, inserted_bytes_nb);

  auto serialized_crc = make_tuple<endianess::LITTLE, signed_mode::TWO_COMPLEMENT>(crc);

  for (auto byte : header) stream.write(byte);
  for (size_t i = 0; i < inserted_bytes_nb; i++) stream.write(buffer[i]);
  for (auto byte : serialized_crc) stream.write(byte);
}

} // namespace detail

//
template<typename... Args>
inline void send_packet(Stream& stream, uint8_t id, Instruction instruction, const Args&... args) {
  using namespace upd;

  auto arguments = make_tuple<endianess::LITTLE, signed_mode::TWO_COMPLEMENT>(
    static_cast<uint8_t>(instruction),
    args...);
  constexpr auto raw_size = arguments.size;
  uint8_t buffer[4 * (raw_size + 1) / 3];
  detail::send_packet(stream, buffer, id, arguments.begin(), raw_size);
}

} // namespace dxl
