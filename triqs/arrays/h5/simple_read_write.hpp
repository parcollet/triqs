/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2014 by O. Parcollet
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/
#pragma once
#include <triqs/arrays/array.hpp>
#include <triqs/arrays/vector.hpp>
#include <h5/array_interface.hpp>

namespace triqs::arrays {

  //template <typename A> h5::details::h5_array_view make_h5_array_view(A const &a) { }

  /*
     * Read an array or a view from an hdf5 file
     *
     * @tparam A  Type of the array/matrix/vector or view
     * @param a The array to be stored
     * @param  g  The h5 group
     * @param name The name of the hdf5 array in the file/group where the stack will be stored
     *
     * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
     */
  template <typename ArrayType> h5_read(::h5::group g, std::string const &name, A &a) REQUIRES(is_amv_value_or_view_class<A>::value) {

    ::h5::details::h5_array_view v(h5::hdf5_type<typename A::value_type>, (void *)a.datastart(), A::rank);
    for (int u = 0; u < A::rank; ++u) {
      v.slab.count[u]  = a.shape(u);
      v.slab.stride[u] = 1; // FIXME DESIGN PB
                            // v.slab.offset stays at 0
                            // v.slab rank is rank +1 if complex, that is already set
                            // v.slab block is unused for this class
    }

    h5::details::read(g, name, v);
  }

  /*
     * Write an array or a view into an hdf5 file
     * ArrayType The type of the array/matrix/vector, etc..
     * g The h5 group
     * name The name of the hdf5 array in the file/group where the stack will be stored
     * A The array to be stored
     * The HDF5 exceptions will be caught and rethrown as TRIQS_RUNTIME_ERROR (with a full stackstrace, cf triqs doc).
     */
  template <typename ArrayType> h5_write(::h5::group g, std::string const &name, A const &a) REQUIRES(is_amv_value_or_view_class<A>::value) {

    ::h5::details::h5_array_view v(h5::hdf5_type<typename A::value_type>, (void *)a.datastart(), A::rank);
    for (int u = 0; u < A::rank; ++u) {
      v.slab.count[u]  = a.shape(u);
      v.slab.stride[u] = 1; // FIXME DESIGN PB
                            // v.slab.offset stays at 0
                            // v.slab rank is rank +1 if complex, that is already set
                            // v.slab block is unused for this class
    }

    h5::details::write(g, name, v, true);
  }

} // namespace triqs::arrays
