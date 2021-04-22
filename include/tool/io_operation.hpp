#pragma once

#include <Arduino.h>

#include "ct_magic.hpp"

#include "endianess.hpp"

/***********************************************************************************************************************
***********************************************************************************************************************/

namespace dxl {
namespace tool {
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
inline void rmemcpy(uint8_t* dest, const uint8_t* src, size_t len) {
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
  constexpr static auto size = N;

  //
  class iterator {
  public:
    iterator() : ptr{nullptr} {}
    iterator(uint8_t* ptr) : ptr{ptr} {}

    iterator operator++() {
      iterator previous;
      previous.ptr = ptr;
      ptr++;
      return previous;
    }

    uint8_t& operator*() const { return *ptr; }
    bool operator!=(const iterator& other) const { return ptr != other.ptr; }

  private:
    uint8_t* ptr;

  };

  //
  class const_iterator {
  public:
    const_iterator() : ptr{nullptr} {}
    const_iterator(const uint8_t* ptr) : ptr{ptr} {}

    const_iterator operator++() {
      const_iterator previous;
      previous.ptr = ptr;
      ptr++;
      return previous;
    }

    const uint8_t& operator*() const { return *ptr; }
    bool operator!=(const iterator& other) const { return ptr != other.ptr; }

  private:
    const uint8_t* ptr;

  };

  //
  UnalignedData(Endianess endianess) : m_endianess{endianess} {};

  //
  UnalignedData(const uint8_t* raw_data, Endianess endianess) : m_endianess{endianess} {
    memcpy(m_raw_data, raw_data, N);
  }

  //
  UnalignedData(Stream& stream, Endianess endianess) : m_raw_data{0}, m_endianess{endianess} {
    for (auto& byte : m_raw_data) byte = stream.read();
  }

  //
  const iterator begin() { return {m_raw_data}; }
  const iterator end() { return {m_raw_data + N}; }
  const const_iterator begin() const { return {m_raw_data}; }
  const const_iterator end() const { return {m_raw_data + N}; }

  //
  uint8_t& operator[](size_t i) { return raw_data[i]; }
  const uint8_t& operator[](size_t i) const { return raw_data[i]; }

  //
  template<typename T>
  auto interpret_as(size_t i) const
    -> ctm::enable_t<ctm::category::integer.has(ctm::type_h<T>{}), T> {
    union { T base; uint8_t raw[sizeof(T)]; } byte_rep;
    if (platform_endianess == m_endianess)
      memcpy(byte_rep.raw, m_raw_data + i, sizeof(T));
    else
      detail::rmemcpy(byte_rep.raw, m_raw_data + i, sizeof(T));
    return byte_rep.base;
  }

  //
  template<typename T>
  auto interpret_as(size_t i) const
    -> ctm::enable_t<
      ctm::is_array<T>::value,
      detail::array_wrapper<ctm::element_t<T>, ctm::introspect_array<T>::size>> {
    using element_t = ctm::element_t<T>;
    constexpr auto array_size = ctm::introspect_array<T>::size;

    detail::array_wrapper<element_t, size> array_wrapper;
    for (size_t j = 0; j < array_size; j++)
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
      detail::rmemcpy(m_raw_data + i, reinterpret_cast<const uint8_t*>(&x), sizeof(T));
  }

  //
  template<typename T>
  auto write(const T& array, size_t i)
    -> ctm::enable_t<ctm::is_array<T>::value> {
    using element_t = ctm::element_t<T>;
    constexpr auto array_size = ctm::introspect_array<T>::size;
    for (size_t j = 0; j < array_size; j++) write(array[j], i + j * sizeof(element_t));
  }

  //
  const uint8_t* raw_data() const { return m_raw_data; }

private:
  uint8_t m_raw_data[N];
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
  UnalignedData(const uint8_t*, Endianess) {}
};

//
template<size_t N>
UnalignedData<N> make_unaligned_data(const uint8_t (&raw_data)[N], Endianess endianess) {
  return UnalignedData<N>{raw_data, endianess};
}
template<size_t N>
UnalignedData<N> make_unaligned_data(uint8_t (&&raw_data)[N], Endianess endianess) {
  return UnalignedData<N>{ctm::move(raw_data), endianess};
}

//
//
//
template<typename... Ts>
class UnalignedArguments : public UnalignedData<ctm::sum(sizeof(Ts)...)> {
  constexpr static auto list = ctm::typelist<Ts...>{};

public:
  template<size_t I>
  using arg_t = ctm::grab<decltype(list.get(ctm::size_h<I>{}))>;
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
    -> decltype(ctm::declval<UnalignedData<size>>().template interpret_as<arg_t<I>>(0)) {
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
     // TODO : à changer pour quelque chose de plus propre
     using discard = int[];
     discard {0, (set<Is>(args), 0)...};
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

} // namespace tool
} // namespace dxl
