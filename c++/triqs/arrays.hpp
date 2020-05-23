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

#include <h5/h5.hpp>
#include <nda/nda.hpp>
#include <nda/h5.hpp>
#include <nda/mpi.hpp>
//#include <nda/blas.hpp>
#include <nda/lapack.hpp>

namespace triqs::arrays { 

  using namespace nda;


 template<typename T> 
   using vector = nda::array<T, 1>;

 template<typename T> 
   using vector_view = nda::array_view<T, 1>;





}
