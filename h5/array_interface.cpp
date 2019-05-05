#include "./array_interface.hpp"
#include "./string.hpp"

#include <hdf5.h>
#include <hdf5_hl.h>

namespace h5::details {

  std::string get_name_of_h5_type(hid_t ty) {

    return std::to_string(ty); // FIXME : implement a table hid_t -> name
  }

  //------------------------------------------------
  // the dataspace corresponding to the array. Contiguous data only...
  dataspace get_data_space(h5_array_view const &v) {

    dataspace ds = H5Screate_simple(v.slab.count.size(), v.slab.count.data(), NULL);
    if (!ds.is_valid()) throw std::runtime_error("Cannot create the dataset");

    herr_t err = H5Sselect_hyperslab(ds, H5S_SELECT_SET, v.slab.offset.data(), v.slab.stride.data(), v.slab.count.data(),
                                     (v.slab.block.empty() ? nullptr : v.slab.block.data()));
    if (err < 0) throw std::runtime_error("Cannot set hyperslab");

    return ds;
  }

  //--------------------------------------------------------

  void write(group g, std::string const &name, h5_array_view const &a) {

    g.unlink_key_if_exists(name);

    bool is_scalar    = (a.rank() == 0);
    bool _is_complex  = a.is_complex;
    dataspace d_space = (is_scalar ? dataspace(H5Screate(H5S_SCALAR)) : get_data_space(a));

    // FIXME : is it a good idea ??
    proplist cparms = H5P_DEFAULT;
    if (!is_scalar) { // if we wish to compress, yes by default ?
      int n_dims = a.rank() + (_is_complex ? 1 : 0);
      hsize_t chunk_dims[n_dims];
      for (int i = 0; i < a.rank(); ++i) chunk_dims[i] = std::max(a.slab.count[i], hsize_t{1});
      if (_is_complex) chunk_dims[n_dims - 1] = 2;
      cparms = H5Pcreate(H5P_DATASET_CREATE);
      H5Pset_chunk(cparms, n_dims, chunk_dims);
      H5Pset_deflate(cparms, 8);
    }

    datatype dt = a.ty;
    dataset ds  = g.create_dataset(name, dt, d_space, cparms);

    if (H5Sget_simple_extent_npoints(d_space) > 0) {
      auto err = (is_scalar ? H5Dwrite(ds, dt, H5S_ALL, H5S_ALL, H5P_DEFAULT, a.start) : H5Dwrite(ds, dt, d_space, H5S_ALL, H5P_DEFAULT, a.start));
      if (err < 0) throw std::runtime_error("Error writing the scalar dataset " + name + " in the group" + g.name());
    }

    // if complex, to be python compatible, we add the __complex__ attribute
    if (_is_complex) h5_write_attribute(ds, "__complex__", "1");
  }

  //-------------------------------------------------------------

  void write_attribute(hid_t id, std::string const &name, h5_array_view v) {

    if (v.rank() != 0) throw std::runtime_error("Non scalar attribute not implemented");

    if (H5LTfind_attribute(id, name.c_str()) != 0)
      throw std::runtime_error("The attribute " + name + " is already present. Can not overwrite"); // not present

    bool is_scalar    = (v.rank() == 0);
    dataspace d_space = (is_scalar ? dataspace{H5Screate(H5S_SCALAR)} : get_data_space(v));

    auto dt        = v.ty;
    attribute attr = H5Acreate2(id, name.c_str(), dt, d_space, H5P_DEFAULT, H5P_DEFAULT);
    if (!attr.is_valid()) throw std::runtime_error("Cannot create the attribute " + name);

    auto status = H5Awrite(attr, dt, v.start);
    if (status < 0) throw std::runtime_error("Cannot write the attribute " + name);
  }

  //--------------------------------------------------------

  h5_lengths_type get_lengths_and_h5type(group g, std::string const &name) {

    dataset ds = g.open_dataset(name);

    bool is_complex   = H5LTfind_attribute(ds, "__complex__"); // the array in file should be interpreted as a complex
    dataspace d_space = H5Dget_space(ds);
    int rank_in_file  = H5Sget_simple_extent_ndims(d_space);
    int rank          = rank_in_file - (is_complex ? 1 : 0);

    // need to use hsize_t here and the vector is truncated at rank
    v_t res(rank);
    hsize_t dims_out[rank_in_file];
    H5Sget_simple_extent_dims(d_space, dims_out, NULL);
    for (int u = 0; u < rank; ++u) res[u] = dims_out[u];

    //  get the type from the file
    datatype ty = H5Dget_type(ds);
    return {std::move(res), ty, is_complex};
  }

  //--------------------------------------------------------

  void read(group g, std::string const &name, h5_array_view v, h5_lengths_type lt) {

    dataset ds             = g.open_dataset(name);
    dataspace file_d_space = H5Dget_space(ds);

    // Checks
    if (v.ty != lt.ty)
      throw std::runtime_error("h5 read. Type mismatch : expecting a " + get_name_of_h5_type(v.ty)
                               + " while the array stored in the hdf5 file has type = " + get_name_of_h5_type(lt.ty));

    if (lt.rank() != v.rank())
      throw std::runtime_error("h5 read. Rank mismatch : expecting a rank " + std::to_string(v.rank())
                               + " while the array stored in the hdf5 file has rank = " + std::to_string(lt.rank()));

    if (lt.lengths != v.slab.count)
      throw std::runtime_error("h5 read. Lengths mismatch : expecting a rank " + std::to_string(v.rank())
                               + " while the array stored in the hdf5 file has rank = " + std::to_string(lt.rank()));

    if (H5Sget_simple_extent_npoints(file_d_space) > 0) {
      herr_t err;
      if (v.rank() == 0)
        err = H5Dread(ds, v.ty, H5S_ALL, file_d_space, H5P_DEFAULT, v.start);
      else
        err = H5Dread(ds, v.ty, get_data_space(v), file_d_space, H5P_DEFAULT, v.start);
      if (err < 0) throw std::runtime_error("Error reading the scalar dataset " + name + " in the group" + g.name());
    }
  }

  //-------------------------------------------------------------

  void read_attribute(hid_t id, std::string const &name, h5_array_view v) {

    if (v.rank() != 0) throw std::runtime_error("Non scalar attribute not implemented");

    attribute attr = H5Aopen(id, name.c_str(), H5P_DEFAULT);
    if (!attr.is_valid()) throw std::runtime_error("Cannot open the attribute " + name);

    dataspace space = H5Aget_space(attr);
    int rank        = H5Sget_simple_extent_ndims(space);
    if (rank != 0) throw std::runtime_error("Reading a scalar attribute and got rank !=0");

    auto eq = H5Tequal(H5Aget_type(attr), v.ty);
    if (eq < 0) throw std::runtime_error("Type comparison failure in reading attribute");
    if (eq == 0) throw std::runtime_error("Type mismatch in reading attribute");

    auto err = H5Aread(attr, v.ty, v.start);
    if (err < 0) throw std::runtime_error("Cannot read the attribute " + name);
  }

} // namespace h5::details
