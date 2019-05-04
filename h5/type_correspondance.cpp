#include "./type_correspondance.hpp"
#include "./type_correspondance.hpp"
#include <hdf5.h>
#include <hdf5_hl.h>
#include <unordered_map>

namespace h5::types {

  std::unordered_map<std::type_index, hid_t> _C_2_h5;
  std::unordered_map<hid_t, std::type_index> _h5_2_C;

  void _register(std::type_index ctype, hid_t h5type) {
    _C_2_h5[ctype]     = h5type;
    _h5_2_C.insert(std::make_pair(h5type, ctype));
  }

  //--------------------------------------

  static void _init() {

    _register(typeid(char), H5T_NATIVE_CHAR);
    _register(typeid(signed char), H5T_NATIVE_SCHAR);
    _register(typeid(unsigned char), H5T_NATIVE_UCHAR);
    _register(typeid(short), H5T_NATIVE_SHORT);
    _register(typeid(unsigned short), H5T_NATIVE_USHORT);
    _register(typeid(int), H5T_NATIVE_INT);
    _register(typeid(unsigned), H5T_NATIVE_UINT);
    _register(typeid(long), H5T_NATIVE_LONG);
    _register(typeid(unsigned long), H5T_NATIVE_ULONG);
    _register(typeid(long long), H5T_NATIVE_LLONG);
    _register(typeid(unsigned long long), H5T_NATIVE_ULLONG);
    _register(typeid(float), H5T_NATIVE_FLOAT);
    _register(typeid(double), H5T_NATIVE_DOUBLE);
    _register(typeid(long double), H5T_NATIVE_LDOUBLE);
    //_register(typeid(bool), H5T_NATIVE_SCHAR);
  
    //  _register(typeid(std::string), H5T_C_S1);
    _register(typeid(std::string), H5T_STRING);

    // Complex like double[2]
    _register(typeid(std::complex<double>), H5T_NATIVE_DOUBLE);

    // bool
    datatype bool_enum_h5type = H5Tenum_create(H5T_NATIVE_CHAR);
    char val;
    H5Tenum_insert(bool_enum_h5type, "FALSE", (val = 0, &val));
    H5Tenum_insert(bool_enum_h5type, "TRUE", (val = 1, &val));
    _register(typeid(bool), bool_enum_h5type);
  }

  //--------------------------------------

  datatype C_2_h5(std::type_index ty) {
    if (_C_2_h5.empty()) _init();
    auto pos = _C_2_h5.find(std::type_index(ty));
    return (pos != _C_2_h5.end() ? pos->second : hid_t{0});
  }

  //--------------------------------------

  std::type_index h5_2_C(datatype h5ty, bool is_complex) {
    if (_h5_2_C.empty()) _init();
    auto pos = _h5_2_C.find(h5ty);
    if (pos == _h5_2_C.end()) return typeid(void);
    if (!is_complex) return pos->second;
    if (pos->second != typeid(std::complex<double>)) H5_ERROR << "Complex type unknown";
    return typeid(std::complex<double>);
  }

  //--------------------------------------

  bool is_same_type(std::type_info const &ty, datatype dt) { return H5Tequal(dt, C_2_h5(std::type_index(ty))); }

} // namespace h5::types
