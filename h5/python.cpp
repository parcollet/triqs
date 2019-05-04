#include <Python.h>
#include <numpy/arrayobject.h>

#include "./python.hpp"
#include "./group.hpp"
#include "./scalar.hpp"
#include "./string.hpp"
#include "./array_interface.hpp"
#include <map>

namespace h5 {

  // C Type -> Numpy Type
  std::map<std::type_index, int> _C_2_py;

  // Ctype -> size of this type
  std::map<std::type_index, int> C_size;

  // Numpy Type -> C type
  std::map<int, std::type_index> _py_2_C;

  // fill the table for a type T
  void _register2(std::type_info const &ctype, int size, int pytype) {
    auto ty         = std::type_index(ctype);
    _C_2_py[ty]     = pytype;
    _py_2_C.insert(std::make_pair(pytype, ty));
    C_size[ty]      = size;
  }

  //--------------------------------------

  static void _init() {
    _register2(typeid(bool), sizeof(bool), NPY_BOOL);
    _register2(typeid(char), sizeof(char), NPY_STRING);
    _register2(typeid(signed char), sizeof(signed char), NPY_BYTE);
    _register2(typeid(unsigned char), sizeof(unsigned char), NPY_UBYTE);
    _register2(typeid(short), sizeof(short), NPY_SHORT);
    _register2(typeid(unsigned short), sizeof(unsigned short), NPY_USHORT);
    _register2(typeid(int), sizeof(int), NPY_INT);
    _register2(typeid(unsigned int), sizeof(unsigned int), NPY_UINT);
    _register2(typeid(long), sizeof(long), NPY_LONG);
    _register2(typeid(unsigned long), sizeof(unsigned long), NPY_ULONG);
    _register2(typeid(long long), sizeof(long long), NPY_LONGLONG);
    _register2(typeid(unsigned long long), sizeof(unsigned long long), NPY_ULONGLONG);
    _register2(typeid(float), sizeof(float), NPY_FLOAT);
    _register2(typeid(double), sizeof(double), NPY_DOUBLE);
    _register2(typeid(long double), sizeof(long double), NPY_LONGDOUBLE);
    _register2(typeid(std::complex<float>), sizeof(std::complex<float>), NPY_CFLOAT);
    _register2(typeid(std::complex<double>), sizeof(std::complex<double>), NPY_CDOUBLE);
    _register2(typeid(std::complex<long double>), sizeof(std::complex<long double>), NPY_CLONGDOUBLE);
  }

  //--------------------------------------

  // C -> Py + control
  static int C_2_py(std::type_index const &ty) {
    if (_C_2_py.empty()) _init();
    auto pos = _C_2_py.find(ty);
    if (pos == _C_2_py.end()) H5_ERROR << "C++ not recognized";
    return pos->second;
  }

  //--------------------------------------

  // py -> C + control
  static std::type_index py_2_C(int ty) {
    if (_py_2_C.empty()) _init();
    auto pos = _py_2_C.find(ty);
    if (pos == _py_2_C.end()) H5_ERROR << "Python type not recognized";
    return pos->second;
  }

  // -------------------------

  static h5_array_view make_av_from_py(std::type_index const &ty, PyArrayObject *arr_obj) {

#ifdef PYTHON_NUMPY_VERSION_LT_17
    int rank         = arr_obj->nd;
    const size_t dim = arr_obj->nd; // we know that dim == rank
#else
    int rank         = PyArray_NDIM(arr_obj);
    const size_t dim = PyArray_NDIM(arr_obj); // we know that dim == rank
#endif
    std::vector<long> lengths(dim), strides(dim);
    for (size_t i = 0; i < dim; ++i) {
#ifdef PYTHON_NUMPY_VERSION_LT_17
      lengths[i] = size_t(arr_obj->dimensions[i]);
      strides[i] = std::ptrdiff_t(arr_obj->strides[i]) / C_size[ty];
#else
      lengths[i]       = size_t(PyArray_DIMS(arr_obj)[i]);
      strides[i]       = std::ptrdiff_t(PyArray_STRIDES(arr_obj)[i]) / C_size[ty];
#endif
    }
    return {ty, PyArray_DATA(arr_obj), std::move(lengths), std::move(strides)};
  }

  // -------------------------

  void _write(group g, std::string const &name, PyObject *ob) {

    // if numpy
    if (PyArray_Check(ob)) {
      PyArrayObject *arr_obj = (PyArrayObject *)ob;
#ifdef PYTHON_NUMPY_VERSION_LT_17
      int elementsType = arr_obj->descr->type_num;
#else
      int elementsType = PyArray_DESCR(arr_obj)->type_num;
#endif
      std::type_index ty = py_2_C(elementsType);
      _write(g, name, make_av_from_py(ty, arr_obj));
    } else {
      if (PyFloat_Check(ob)) {
        h5_write(g, name, PyFloat_AsDouble(ob));
      } else if (PyLong_Check(ob)) {
        h5_write(g, name, PyLong_AsLong(ob));
      } else if (PyString_Check(ob)) {
        h5_write(g, name, (const char *)PyString_AsString(ob));
      } else if (PyComplex_Check(ob)) {
        h5_write(g, name, std::complex<double>{PyComplex_RealAsDouble(ob), PyComplex_ImagAsDouble(ob)});
      }
    }
  } // namespace h5

  // -------------------------

  void _read(group g, std::string const &name, PyObject *&ob) {

    ob = NULL; // default result

    h5_lengths_type lt = get_lengths_and_h5type(g, name);

    if (lt.rank() == 0) { // it is a scalar
      if (lt.ty == typeid(double)) {
        double x;
        h5_read(g, name, x);
        ob = PyFloat_FromDouble(x);
      } else if (lt.ty == typeid(long)) { // or other int ...
        long x;
        h5_read(g, name, x);
        ob = PyLong_FromLong(x);
      } else if (lt.ty == typeid(std::string)) {
        std::string x;
        h5_read(g, name, x);
        ob = PyString_FromString(x.c_str());
      } else if (lt.ty == typeid(std::complex<double>)) {
        std::complex<double> x;
        h5_read(g, name, x);
        ob = PyComplex_FromDoubles(x.real(), x.imag());
      }

    } else {
      std::vector<npy_intp> L(lt.rank()); // check
      std::copy(lt.lengths.begin(), lt.lengths.end(), L.begin());
      int elementsType = C_2_py(lt.ty);

      ob = PyArray_SimpleNewFromDescr(int(L.size()), &L[0], PyArray_DescrFromType(elementsType));

      if (!ob) {
        if (PyErr_Occurred()) {
          PyErr_Print();
          PyErr_Clear();
        }
        H5_ERROR << "Read a numpy from h5. Object can not be build";
      }

      _read(g, name, make_av_from_py(lt.ty, (PyArrayObject*)ob), lt);
    }
  }

} // namespace h5
