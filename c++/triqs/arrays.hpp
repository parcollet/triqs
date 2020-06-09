/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet
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

// for python code generator, we need to know what to include...
#define TRIQS_INCLUDED_ARRAYS

#include <assert.h>

#include<complex>
#include <h5/h5.hpp>
#include <nda/nda.hpp>
#include <nda/h5.hpp>
#include <nda/mpi.hpp>
//#include <nda/blas.hpp>
#include <nda/lapack.hpp>
#include <nda/linalg/det_and_inverse.hpp>

#include "utility/concept_tools.hpp"
#include "utility/exceptions.hpp"

#define TRIQS_CLEF_MAKE_FNT_LAZY CLEF_MAKE_FNT_LAZY
#define TRIQS_CLEF_IMPLEMENT_LAZY_CALL CLEF_IMPLEMENT_LAZY_CALL

// FIXME
//#ifndef TRIQS_RUNTIME_ERROR
//#define TRIQS_RUNTIME_ERROR NDA_RUNTIME_ERROR
//#endif


namespace triqs { 

  namespace clef = nda::clef;
  using dcomplex = std::complex<double>;
  using nda::range;

}

namespace triqs::utility {

  template <typename T, int R> using mini_vector = std::array<T, R>;

}

namespace triqs::arrays {

  using namespace nda;

  using utility::mini_vector;

  template <typename T> using vector = nda::array<T, 1>;

  template <typename T> using vector_view = nda::array_view<T, 1>;

  template <typename T>

  nda::matrix<T> make_unit_matrix(int dim) {
    return nda::eye<T>(dim);
  }

  template <typename... T> mini_vector<size_t, sizeof...(T)> make_shape(T... args) { return {args...}; }

  /*
   // swap two indices i,j
    template <typename A>
    std::enable_if_t<is_amv_value_or_view_class<std::decay_t<A>>::value, typename std::decay_t<A>::view_type> swap_index_view(A &&a, int i, int j) {
      auto imp = a.indexmap();
      auto l   = imp.lengths();
      auto s   = imp.strides();
      std::swap(l[i], l[j]);
      std::swap(s[i], s[j]);
      using r_t = typename std::decay_t<A>::view_type;
      // FIXME : long only
      auto imp2 = typename r_t::indexmap_type{l, s, static_cast<ptrdiff_t>(imp.start_shift())};
      return r_t{imp2, a.storage()};
    }

    // Rotate the index n to 0, preserving the relative order of the other indices
    template <typename T, int R> array_const_view<T, R> rotate_index_view(array_const_view<T, R> a, int n) {
      for (int i = n; i > 0; --i) a.rebind(swap_index_view(a, i - 1, i));
      return a;
    }

    // FIXME : regroup
    // Rotate the index n to 0, preserving the relative order of the other indices
    template <typename T, int R> array_view<T, R> rotate_index_view(array_view<T, R> a, int n) {
      for (int i = n; i > 0; --i) a.rebind(swap_index_view(a, i - 1, i));
      return a;
    }
*/

} // namespace triqs::arrays
