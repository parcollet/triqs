#pragma once

#include "./array_interface.hpp"

// two tables : numpy/Python Type  <------>  h5 + SIZE OF T
// NEed to access the storage size in C... sizeof(T).
namespace h5 {

  void h5_write(group g, std::string const &name, PyObject *ob);

  void h5_read(group g, std::string const &name, PyObject *&ob);

} // namespace h5
