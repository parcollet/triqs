#pragma once
#include <vector>
#include <string>

#include "./group.hpp"

namespace h5::details {

  using v_t = std::vector<size_t>;

  // Stores the hdf5 type and the dims
  struct h5_lengths_type {
    v_t lengths;
    datatype ty;
    bool is_complex = false; //

    int rank() const { return lengths.size(); }
  };

  // Store HDF5 hyperslab info, as in HDF5 manual
  // http://davis.lbl.gov/Manuals/HDF5-1.8.7/UG/12_Dataspaces.html
  struct hyperslab {
    v_t offset; // offset of the pointer from the start of data
    v_t stride; // stride (in the HDF5 sense)
    v_t count;  //length in each direction
    v_t block;  //block in each direction
  };

  // Stores a view of an array.
  // scalar are array of rank 0, lengths, strides are empty, rank is 0, start is the scalar
  struct h5_array_view {
    void *start;             // start of data. It MUST be a pointer of T* with ty = hdf5_type<T>
    datatype ty;             // HDF5 type
    bool is_complex = false; //
    hyperslab slab;

    int rank() const { return slab.count.size(); }
  };

  // Retrieve lengths and hdf5 type from a file
  h5_lengths_type get_h5_lengths_type(group g, std::string const &name);

  // Write the view of the array to the group
  void write(group g, std::string const &name, h5_array_view const &a);

  // INTERNAL [python interface only]
  // Same as before, but if lt = get_h5_lengths_type(g,name) has already been computed
  // it can be passed here to avoid a second call.
  void read(group g, std::string const &name, h5_array_view v, h5_lengths_type lt);

  // Read the data into the view.
  inline void read(group g, std::string const &name, h5_array_view v) { read(g, name, v, get_h5_lengths_type(g, name)); }

  // Write as attribute
  void write_attribute(hid_t id, std::string const &name, h5_array_view v);

  // Read as attribute
  void read_attribute(hid_t id, std::string const &name, h5_array_view v);

} // namespace h5::details
