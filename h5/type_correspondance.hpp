#pragma once
#include <typeindex>
#include <typeinfo>
#include "./h5object.hpp"

namespace h5::types {

  // register a correspondance
  void _register(std::type_index ctype, hid_t h5type);

  // Given C++ type -> return h5 type or 0 if not registered
  datatype C_2_h5(std::type_index ty);

  // h5 type + complex --> C++ type or typeid(void) if not registered
  std::type_index h5_2_C(datatype ty, bool is_complex);

  // 
  bool is_same_type(std::type_info const & ty, datatype ty2);

} // namespace h5::types
