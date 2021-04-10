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
  UnalignedData(Endianess endianess) : m_endianess{endianess} {};

  //
  UnalignedData(const byte_t* raw_data, Endianess endianess) : m_endianess{endianess} {
    memcpy(m_raw_data, raw_data, N);
  }

  //
  UnalignedData(Stream& stream, Endianess endianess) : m_raw_data{0}, m_endianess{endianess} {
    for (auto& byte : m_raw_data) byte = stream.read();
  }

  //
  template<typename T>
  auto interpret_as(size_t i) const
    -> ctm::enable_t<ctm::category::integer.has(ctm::type_h<T>{}), T> {
    alignas(T) byte_t byte_rep[sizeof(T)];
    if (platform_endianess == m_endianess)
      memcpy(byte_rep, m_raw_data + i, sizeof(T));
    else
      detail::rmemcpy(byte_rep, m_raw_data + i, sizeof(T));
    return *reinterpret_cast<T*>(byte_rep);
  }

  //
  template<typename T>
  auto interpret_as(size_t i) const
    -> ctm::enable_t<
      ctm::is_array<T>::value,
      detail::array_wrapper<ctm::element_t<T>, ctm::introspect_array<T>::size>> {
    using element_t = ctm::element_t<T>;
    constexpr auto size = ctm::introspect_array<T>::size;

    detail::array_wrapper<element_t, size> array_wrapper;
    for (size_t j = 0; j < size; j++)
      array_wrapper[j] = interpret_as<element_t>(i + j * sizeof(element_t));

    return array_wrapper;
  }

  //
  template<typename T>
  auto write(const T& x, size_t i)
    -> ctm::enable_t<ctm::category::integer.has(ctm::type_h<T>{})> {
    if (platform_endianess == m_endianess)
      memcpy(m_raw_data + i, reinterpret_cast<const void*>(&x), sizeof(T));
    else
      detail::rmemcpy(m_raw_data + i, reinterpret_cast<const byte_t*>(&x), sizeof(T));
  }

  //
  template<typename T>
  auto write(const T& array, size_t i)
    -> ctm::enable_t<ctm::is_array<T>::value> {
    using element_t = ctm::element_t<T>;
    constexpr auto size = ctm::introspect_array<T>::size;
    for (size_t j = 0; j < size; j++) write(array[j], i + j * sizeof(element_t), m_endianess);
  }

  //
  const byte_t* raw_data() const { return raw_data; }

private:
  byte_t m_raw_data[N];
  Endianess m_endianess;

};

//
//
//
template<>
class UnalignedData<0> {
public:
  UnalignedData() {}
  UnalignedData(Endianess) {}
  UnalignedData(Stream&, Endianess) {}
  UnalignedData(const byte_t*, Endianess) {}
};

//
template<size_t N>
UnalignedData<N> make_unaligned_data(const byte_t (&raw_data)[N], Endianess endianess) {
  return UnalignedData<N>{raw_data, endianess};
}
template<size_t N>
UnalignedData<N> make_unaligned_data(byte_t (&&raw_data)[N], Endianess endianess) {
  return UnalignedData<N>{ctm::move(raw_data), endianess};
}

//
//
//
template<typename... Ts>
class UnalignedArguments : UnalignedData<ctm::sum(sizeof(Ts)...)> {
  constexpr static auto list = ctm::typelist<Ts...>{};

public:
  template<size_t I>
  using arg_t = decltype(list.pop(ctm::size_h<I>{}).head());
  constexpr static auto size = ctm::sum(sizeof(Ts)...);

  //
  UnalignedArguments(Endianess endianess) : UnalignedData<size>{endianess} {}

  //
  template<typename... Args, typename = ctm::enable_t<sizeof...(Args) == sizeof...(Ts)>>
  UnalignedArguments(Endianess endianess, const Args&... args) : UnalignedData<size>{endianess} {
    lay(ctm::srange<0, sizeof...(Ts)>{}, args...);
  }

  //
  UnalignedArguments(Stream& stream, Endianess endianess) : UnalignedData<size>{stream} {}

  //
  template<size_t I>
  auto get() const
    -> decltype(UnalignedData<size>::template interpret_as<arg_t<I>>(0)) {
    constexpr auto offset = ctm::slist<sizeof(Ts)...>{}
      .take(ctm::size_h<I>{})
      .accumulate(0, ctm::sum<size_t, size_t>);

    return UnalignedData<size>::template interpret_as<arg_t<I>>(offset);
  }

  //
  template<size_t I>
  void set(const arg_t<I>& x) {
    constexpr auto offset = ctm::slist<sizeof(Ts)...>{}
      .take(ctm::size_h<I>{})
      .accumulate(0, ctm::sum<size_t, size_t>);

    UnalignedData<size>::write(x, offset);
  }

private:
  //
  template<size_t... Is, typename... Args>
  void lay(ctm::size_h<Is...>, const Args&... args) {
    ([](const Args&...) {})(write(args, ctm::size_h<Is>{})...); // TODO : à changer pour quelque chose de plus propre
  }

};

//
//
//
template<>
class UnalignedArguments<> : UnalignedData<0> {
public:
  UnalignedArguments() {}
  UnalignedArguments(Endianess) {}
  UnalignedArguments(Stream&, Endianess) {}
};

//
template<typename... Args>
UnalignedArguments<Args...> make_unaligned_arguments(Endianess endianess, const Args&... args) {
  return UnalignedArguments<Args...>{endianess, args...};
}
