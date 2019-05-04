#pragma once
#include <complex>
#include "./type_desc.hpp"

namespace h5 {

  using hid_t = int64_t;

  //------------- general hdf5 object ------------------
  // HDF5 uses a reference counting system, similar to python.
  // Same as pyref in python wrapper, its handle the ref. counting of the hdf5 object
  // using a RAII pattern.
  // We are going to store the id of the various h5 object in such a wrapper
  // to provide clean decref, and h5_exception safety.
  class h5_object {

    protected:
    hid_t id;

    public:
    // make an h5_object when the id is now owned (simply inc. the ref).
    static h5_object from_borrowed(hid_t id);

    // constructor from an owned id (or 0). It will NOT incref, it takes ownership
    h5_object(hid_t id = 0) : id(id) {}

    h5_object(h5_object const &x); // a new copy, a new ref.

    h5_object(h5_object &&x) noexcept : id(x.id) { x.id = 0; } // steal the ref.

    h5_object &operator=(h5_object const &x) { return operator=(h5_object(x)); } //rewriting with the next overload

    h5_object &operator=(h5_object &&x) noexcept; //steals the ref, after properly decref its own.

    ~h5_object();

    void close();

    /// cast operator to use it in the C function as its id
    operator hid_t() const { return id; }

    bool is_valid() const;
  };

  //-----------------------------

  using dataset = h5_object;
  using datatype = h5_object;
  using dataspace = h5_object;
  using proplist = h5_object;
  using attribute = h5_object;

  //------------- type_desc ------------------
  // a class T has either :
  //  1- a static member hdf5_type_desc -> std::string (or a constexpr char * ?)
  //  2- specializes hdf5_type_desc_impl
  // user function is get_hdf5_type_desc <T>() in all cases.
  // A claass which is not default constructible :
  //  -- 1 : implement static T h5_read_construct(gr, name) : rebuilt  a new T
  //  -- 2 : NOT IMPLEMENTED : if we want to make it non intrusive,
  //  specialize with a struct similarly to hdf5_type_desc_impl
  // to be implemented if needed.

  template <typename T> struct hdf5_type_desc_impl {
    static std::string invoke() { return T::hdf5_type_desc(); }
  };

#define H5_AS_STRING(X) H5_AS_STRING2(X)
#define H5_AS_STRING2(X) #X

#define H5_SPECIALIZE_TYPE_DESC2(X, Y)                                                                                                          \
  template <> struct hdf5_type_desc_impl<X> {                                                                                                           \
    static std::string invoke() { return H5_AS_STRING(Y); }                                                                                             \
  };

#define H5_SPECIALIZE_TYPE_DESC(X) H5_SPECIALIZE_TYPE_DESC2(X, X)

  H5_SPECIALIZE_TYPE_DESC(bool);
  H5_SPECIALIZE_TYPE_DESC(int);
  H5_SPECIALIZE_TYPE_DESC(long);
  H5_SPECIALIZE_TYPE_DESC(long long);
  H5_SPECIALIZE_TYPE_DESC(unsigned int);
  H5_SPECIALIZE_TYPE_DESC(unsigned long);
  H5_SPECIALIZE_TYPE_DESC(unsigned long long);
  H5_SPECIALIZE_TYPE_DESC(float);
  H5_SPECIALIZE_TYPE_DESC(double);
  H5_SPECIALIZE_TYPE_DESC(long double);
  H5_SPECIALIZE_TYPE_DESC2(std::complex<double>, complex);

  template <typename T> std::string get_type_desc() { return hdf5_type_desc_impl<T>::invoke(); }

// ---------------- EXCEPTION

#define H5_ERROR                                                                                                                                     \
  throw h5_exception() << "H5 error"                                                                                                                   \
                     << " at " << __FILE__ << " : " << __LINE__ << "\n\n"

#define CHECK_OR_THROW(Cond, Mess)                                                                                                                   \
  if (!(Cond)) H5_ERROR << H5_AS_STRING(Cond) << " : " << Mess;

  class h5_exception : public std::exception {
    std::stringstream acc;
    std::string _trace;
    mutable std::string _what;

    public:
    h5_exception() noexcept : std::exception() { }//_trace = triqs::utility::stack_trace(); }
    h5_exception(h5_exception const &e) noexcept : acc(e.acc.str()), _trace(e._trace), _what(e._what) {}
    virtual ~h5_exception() noexcept {}
    template <typename T> h5_exception &operator<<(T const &x) {
      acc << x;
      return *this;
    }
    h5_exception &operator<<(const char *mess) {
      (*this) << std::string(mess);
      return *this;
    }
    virtual const char *what() const noexcept {
      std::stringstream out;
      out << acc.str() << "\n.. Error occurred on node ";
      _what = out.str();
      return _what.c_str();
    }
    virtual const char *trace() const noexcept { return _trace.c_str(); }
  };

}; // namespace h5
