#pragma once
#include "./h5object.hpp"

namespace h5 {

  /**
  *  \brief A little handler for the file
  */
  class file : public h5_object {

    public:
    /**
   * Open the file name.
   * Flag char can be :
   *   - 'a' H5F_ACC_RDWR
   *   - 'r' H5F_ACC_RDONLY
   *   - 'w' H5F_ACC_TRUNC
   */
    file(const char *name, char flags);

    /**
   * Open the file name.
   * Flags can be :
   *   - H5F_ACC_RDWR
   *   - H5F_ACC_RDONLY
   *   - H5F_ACC_TRUNC
   *   - H5F_ACC_EXCL
   */
    file(const char *name, unsigned flags);

    /// Cf previous constructor
    file(std::string const &name, unsigned flags) : file(name.c_str(), flags) {}

    ///
    file(std::string const &name, char flags) : file(name.c_str(), flags) {}

    protected: /// Internal : from an hdf5 id.
    file(hid_t id);
    //file(h5_object obj);

    public:
    /// Name of the file
    std::string name() const;
  };

  /**
  *  \brief A file in a memory buffer
  */
  class memory_file : public file {

    public:
    /// Construct a writable file in memory with a buffer
    memory_file();

    /// Construct a read_only file on top on the buffer.
    memory_file(std::vector<unsigned char> const &buf);

    /// Get a copy of the buffer
    std::vector<unsigned char> as_buffer() const;
  };

} // namespace h5
