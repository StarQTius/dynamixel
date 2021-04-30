#pragma once

#include <Arduino.h>

#include "byte_stuffing.hpp"
#include "crc.hpp"
#include "endianess.hpp"

#include "upd/storage.hpp"

namespace dxl {
namespace detail {

constexpr uint8_t packet_header[] = {0xff, 0xff, 0xfd, 0x00};

//
inline void send_packet(
  Stream& stream,
  tool::uint8_t* buffer,
  uint8_t id,
  const tool::uint8_t* payload,
  uint16_t payload_size)
{
  using namespace upd;

  auto inserted_bytes_nb = stuff_bytes(buffer, payload, payload_size);
  auto first_part = make_unaligned_arguments(endianess::little, packet_header, id, inserted_bytes_nb + 2);
  auto crc = make_unaligned_arguments(endianess::little, compute_crc(buffer, inserted_bytes_nb));

  for (auto byte : first_part) stream.write(byte);
  for (size_t i = 0; i < inserted_bytes_nb; i++) stream.write(buffer[i]);
  for (auto byte : crc) stream.write(byte);
}

} // namespace detail

//
template<typename... Args>
inline void send_packet(Stream& stream, uint8_t id, Instruction instruction, const Args&... args) {
  auto unaligned_arguments = tool::make_unaligned_arguments(
    dxl::tool::Endianess::little,
    static_cast<uint8_t>(instruction),
    args...);
  constexpr auto raw_size = decltype(unaligned_arguments)::size;
  tool::uint8_t buffer[4 * (raw_size + 1) / 3];
  detail::send_packet(stream, buffer, id, unaligned_arguments.raw_data(), raw_size);
}

} // namespace dxl
