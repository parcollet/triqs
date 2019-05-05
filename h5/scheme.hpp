#pragma once

namespace h5 {

  // FIXME : scalar by default ?
  template <typename T> struct io {
    static std::string scheme();

    static void write(group g, std::string const &name, T const &);
    static void read(group g, std::string const &name, T &);

    static void write_attribute(group g, std::string const &name, T const &);
    static void read_attribute(group g, std::string const &name, T &);

    static T h5_read_construct(group g, std::string const &name);
  };

  template <typename T> void h5_write(group g, std::string const &name, T const &x) { io<T>::write(g, name, x); }
  template <typename T> void h5_read(group g, std::string const &name, T &x) { io<T>::read(g, name, x); }

  template <typename T> void h5_write_attribute(group g, std::string const &name, T const &x) { io<T>::write_attribute(g, name, x); }
  template <typename T> void h5_read_attribute(group g, std::string const &name, T &x) { io<T>::read_attribute(g, name, x); }

  // a class T has either :
  //  1- a static member hdf5_scheme -> std::string (or a constexpr char * ?)
  //  2- specializes hdf5_scheme_impl
  // user function is get_hdf5_scheme <T>() in all cases.
  // A claass which is not default constructible :
  //  -- 1 : implement static T h5_read_construct(gr, name) : rebuilt  a new T
  //  -- 2 : NOT IMPLEMENTED : if we want to make it non intrusive,
  //  specialize with a struct similarly to hdf5_scheme_impl
  // to be implemented if needed.

  // FIXME : faire une function SIMPLE <> + specialzaiton 
#define AS_STRING(X) AS_STRING2(X)
#define AS_STRING2(X) #X

#define TRIQS_SPECIALIZE_HDF5_SCHEME2(X, Y)                                                                                                          \
  template <> std::string io<X>::scheme<X>() { return AS_STRING(Y); }

#define TRIQS_SPECIALIZE_HDF5_SCHEME(X) TRIQS_SPECIALIZE_HDF5_SCHEME2(X, X)

  TRIQS_SPECIALIZE_HDF5_SCHEME(bool);
  TRIQS_SPECIALIZE_HDF5_SCHEME(int);
  TRIQS_SPECIALIZE_HDF5_SCHEME(long);
  TRIQS_SPECIALIZE_HDF5_SCHEME(long long);
  TRIQS_SPECIALIZE_HDF5_SCHEME(unsigned int);
  TRIQS_SPECIALIZE_HDF5_SCHEME(unsigned long);
  TRIQS_SPECIALIZE_HDF5_SCHEME(unsigned long long);
  TRIQS_SPECIALIZE_HDF5_SCHEME(float);
  TRIQS_SPECIALIZE_HDF5_SCHEME(double);
  TRIQS_SPECIALIZE_HDF5_SCHEME(long double);
  TRIQS_SPECIALIZE_HDF5_SCHEME2(std::complex<double>, complex);

  template <typename T> std::string get_hdf5_scheme() { return io<T>::scheme(); }

} // namespace h5
