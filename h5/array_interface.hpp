#pragma once
#include <typeindex>
#include <vector>
#include <string>

#include "./group.hpp"

namespace h5 {

  struct h5_lengths_type {
    datatype ty;
    //std::type_index ty;
    std::vector<size_t> dims;

    int rank() const { return dims.size(); }
  };

  struct hyperslab {
    using v_t std::vector<size_t>;
    v_t counts;
    v_t strides; //= v_t(dims.size(), 1); // strides in the sense of h5
    v_t offset;
  };

  // scalar are array of rank 0, lengths, strides are empty, rank is 0, start is the scalar
  struct h5_array_view {
    datatype ty;
    void *start; // start of data

    int rank() const { return lengths.size(); }
  };

  // Write the view of the array to the group
  void _write(h5::group g, std::string const &name, h5_array_view const &a);

  // Write the scalar attribute if there is no such attribute
  void _write_attribute(hid_t id, std::string const &name, h5_array_view v);

  // Retrieve lengths
  h5_lengths_type get_lengths_and_h5type(group g, std::string const &name);

  // Read the data into the view. Must pass the result of get_lengths_and_h5type
  void _read(group g, std::string const &name, h5_array_view v, h5_lengths_type lt);

  // Read the scalar attribute
  void _read_attribute(hid_t id, std::string const &name, h5_array_view v);

} // namespace h5
