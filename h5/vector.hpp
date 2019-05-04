#pragma once
#include <vector>
#include <complex>
#include "./group.hpp"
#include "./string.hpp"
#include "./scalar.hpp"

namespace h5 {

  template <typename T> h5_array_view h5_array_view_from_vector(std::vector<T> const &v) {
    return {typeid(std::decay_t<T>), (void *)(&v[0]), {long(v.size())}, {1}};
  }

  template <typename T> void h5_write(group g, std::string const &name, std::vector<T> const &v) {
    auto gr = g.create_group(name);
    if constexpr (is_scalar_v<T>) {
      _write(g, name, h5_array_view_from_vector(v));
    } else { // generic type
      gr.write_hdf5_scheme(v);
      for (int i = 0; i < v.size(); ++i) h5_write(gr, std::to_string(i), v[i]);
    }
  }

  // ----------------------------------------------------------------------------

  template <typename T> void h5_read(group f, std::string name, std::vector<T> &v) {
    auto g = f.open_group(name);
    if constexpr (is_scalar_v<T>) {
      h5_lengths_type lt = get_lengths_and_h5type(g, name);
      if (lt.rank() != 1) H5_ERROR << "h5 : reading a vector and I got an array of rank" << lt.rank();
      v.resize(lt.lengths[0]);
      _read(g, name, h5_array_view_from_vector(v), lt);
    } else { // generic type
      v.resize(g.get_all_dataset_names().size() + g.get_all_subgroup_names().size());
      for (int i = 0; i < v.size(); ++i) { h5_read(g, std::to_string(i), v[i]); }
    }
  }
  // ----------------------------------------------------------------------------
  // FIXME : CLEAN THIS
  // --------------   Special case of vector < string >

  TRIQS_SPECIALIZE_HDF5_SCHEME2(std::vector<std::string>, vector<string>);

  template <typename T> struct hdf5_scheme_impl<std::vector<T>> {
    static std::string invoke() { return "PythonListWrap"; } //std::vector<" + hdf5_scheme_impl<T>::invoke() + ">"; }
    //static std::string invoke() { return "std::vector<" + hdf5_scheme_impl<T>::invoke() + ">"; }
  };

  void h5_write(group f, std::string const &name, std::vector<std::string> const &V);
  void h5_read(group f, std::string const &name, std::vector<std::string> &V);

  void h5_write_attribute(hid_t ob, std::string const &name, std::vector<std::vector<std::string>> const &V);
  void h5_read_attribute(hid_t ob, std::string const &name, std::vector<std::vector<std::string>> &V);

  //void h5_write_attribute (hid_t ob, std::string const & name, std::vector<std::string> const & V);
  //void h5_read_attribute (hid_t ob, std::string const & name, std::vector<std::string> & V);

} // namespace h5
