#pragma once

#include <Arduino.h>

#include "ct_magic.hpp"

#include "endianess.hpp"

/***********************************************************************************************************************
***********************************************************************************************************************/

using byte_t = uint8_t;

namespace detail {

//
//
//
template<typename T, size_t N>
struct array_wrapper {
  array_wrapper() = default;
  array_wrapper(const T* ptr) { memcpy(content, ptr, N); }

  T& operator[](size_t i) { return content[i]; }
  T& operator*() { return *content; }
  T* operator->() { return content; }
  T* operator+(size_t n) { return content + n; }
  T* operator-(size_t n) { return content - n; }

  const T& operator[](size_t i) const { return content[i]; }
  const T& operator*() const { return *content; }
  const T* operator->() const { return content; }
  const T* operator+(size_t n) const { return content + n; }
  const T* operator-(size_t n) const { return content - n; }

  T content[N];
};

//
inline void rmemcpy(byte_t* dest, const byte_t* src, size_t len) {
  for (size_t i = 0; i < len; i++)
    dest[i] = src[len - i - 1];
}

} // namespace detail

//
//
//
template<size_t N>
class UnalignedData {
public:
  //
  UnalignedData() = default;

  //
  UnalignedData(byte_t raw_data[N]) : raw_data{raw_data} {}

  //
  UnalignedData(Stream& stream) : raw_data{0} {
    for (auto& byte : raw_data) byte = stream.read();
  }

  //
  template<typename T>
  auto interpret_as(ctm::type_h<T>, size_t i, Endianess target_endianess) const
    -> ctm::enable_t<ctm::category::integer.has(ctm::type_h<T>{}), T> {
    alignas(T) byte_t byte_rep[sizeof(T)];
    if (platform_endianess == target_endianess)
      memcpy(byte_rep, raw_data + i, sizeof(T));
    else
      detail::rmemcpy(byte_rep, raw_data + i, sizeof(T));
    return *reinterpret_cast<T*>(byte_rep);
  }

  //
  template<typename T>
  auto interpret_as(ctm::type_h<T>, size_t i, Endianess target_endianess) const
    -> ctm::enable_t<
      ctm::is_array<T>::value,
      detail::array_wrapper<ctm::element_t<T>, ctm::introspect_array<T>::size>> {
    using Element = ctm::element_t<T>;
    constexpr auto Size = ctm::introspect_array<T>::size;

    detail::array_wrapper<Element, Size> array_wrapper;
    for (size_t j = 0; j < Size; j++)
      array_wrapper[j] = interpret_as<Element>(i + j * sizeof(Element), target_endianess);

    return array_wrapper;
  }

  //
  template<typename T>
  auto set(const T& x, size_t i, Endianess target_endianess)
    -> ctm::enable_t<ctm::category::integer.has(ctm::type_h<T>{})> {
    if (platform_endianess == target_endianess)
      memcpy(raw_data + i, reinterpret_cast<const void*>(&x), sizeof(T));
    else
      detail::rmemcpy(raw_data + i, reinterpret_cast<const byte_t*>(&x), sizeof(T));
  }

  //
  template<typename T>
  auto set(const T& array, size_t i, Endianess target_endianess)
    -> ctm::enable_t<ctm::is_array<T>::value> {
    using Element = ctm::element_t<T>;
    constexpr auto Size = ctm::introspect_array<T>::size;
    for (size_t j = 0; j < Size; j++) set(array[j], i + j * sizeof(Element), target_endianess);
  }

private:
  byte_t raw_data[N];

};

template<>
class UnalignedData<0> {
  UnalignedData(Stream&) {};
};

//
//
//
template<typename... Ts>
class UnalignedArguments : UnalignedData<ctm::sum(sizeof(Ts)...)> {
  constexpr static auto list = ctm::typelist<Ts...>{};

public:
  template<size_t I>
  using Arg_t = ctm::grab<decltype(list.pop(ctm::size_h<I>{}).head())>;
  constexpr static auto size = ctm::sum(sizeof(Ts)...);

  //
  template<typename... Args>
  UnalignedArguments(const Args&... args) : UnalignedData<size>{} {
    lay(ctm::srange<0, sizeof...(Ts)>{}, FWD(args)...);
  }

  //
  UnalignedArguments(Stream& stream) : UnalignedData<size>{stream} {}

  //
  template<size_t I>
  auto get(ctm::size_h<I>)
    -> decltype(UnalignedData<size>::template interpret_as<Arg_t<I>>(0)) {
    constexpr auto offset = ctm::slist<sizeof(Ts)...>{}
      .take(ctm::size_h<I>{})
      .accumulate(0, ctm::sum<size_t, size_t>);

    return UnalignedData<size>::template interpret_as<Arg_t<I>>(offset);
  }

private:
  //
  template<size_t... Is, typename... Args>
  void lay(ctm::size_h<Is...>, const Args&... args) {
    ([](const Args&...) {})(lay_one(ctm::size_h<Is>{}, FWD(args))...); // TODO : à changer pour quelque chose de plus propre
  }

  //
  template<size_t I, typename T>
  void lay_one(ctm::size_h<I>, const T& x) {
    constexpr auto offset = ctm::slist<sizeof(Ts)...>{}
      .take(ctm::size_h<I>{})
      .accumulate(0, ctm::sum<size_t, size_t>);

    UnalignedData<size>::set(x, offset, Endianess::big);
  }

};

template<>
class UnalignedArguments<> {
public:
  UnalignedArguments(Stream&) {}
};

template<typename... Args>
UnalignedArguments<Args...> make_unaligned_argument(Args&&... args) {

}

//
template<typename T>
void insert(Stream& stream, const T& value) {
  union ByteRep { T base; byte_t raw[sizeof(T)]; };
  auto& byte_rep = reinterpret_cast<const ByteRep&>(value);
  for (auto byte : byte_rep.raw) stream.write(byte);
}
