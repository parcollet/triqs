#pragma once
#include "./group.hpp"
#include "./array_interface.hpp"
namespace h5 {

  // a trait to detect scalar type for hdf5
  template <typename T> struct _is_scalar : std::false_type {};
  template <typename T> struct _is_scalar<std::complex<T>> : std::true_type {};
  template <typename T> struct _is_scalar<const T> : _is_scalar<T> {};
  template <> struct _is_scalar<int> : std::true_type {};
  template <> struct _is_scalar<long> : std::true_type {};
  template <> struct _is_scalar<long long> : std::true_type {};
  template <> struct _is_scalar<unsigned long long> : std::true_type {};
  template <> struct _is_scalar<size_t> : std::true_type {};
  template <> struct _is_scalar<bool> : std::true_type {};
  template <> struct _is_scalar<char> : std::true_type {};
  template <> struct _is_scalar<double> : std::true_type {};

  template <typename T> inline constexpr bool is_scalar_v = _is_scalar<T>::value;

  template <typename S> h5_array_view h5_array_view_from_scalar(S &&s) { return {typeid(std::decay_t<S>),(void *)(&s), {}, {} }; }
 
  template <typename T> std::enable_if_t<is_scalar_v<T>> h5_write(group g, std::string const &name, T const &x) {
    _write(g, name, h5_array_view_from_scalar(x));
  }

  template <typename T> std::enable_if_t<is_scalar_v<T>> h5_read(group g, std::string const &name, T &x) {
    _read(g, name, h5_array_view_from_scalar(x), get_lengths_and_h5type(g, name));
  }

  template <typename T> std::enable_if_t<is_scalar_v<T>> h5_write_attribute(hid_t id, std::string const &name, T const &x) {
    _write_attribute(id, name, h5_array_view_from_scalar(x));
  }

  template <typename T> std::enable_if_t<is_scalar_v<T>> h5_read_attribute(hid_t id, std::string const &name, T &x) {
    _read_attribute(id, name, h5_array_view_from_scalar(x));
  }

} // namespace h5
