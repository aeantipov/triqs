/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013 by O. Parcollet
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

#define TRIQS_GF_INCLUDED

#include "./gfs/gf_classes.hpp"

#include <triqs/gfs/onevar/imtime.hpp>
#include <triqs/gfs/onevar/imfreq.hpp>
#include <triqs/gfs/onevar/retime.hpp>
#include <triqs/gfs/onevar/refreq.hpp>
#include <triqs/gfs/onevar/legendre.hpp>
#include <triqs/gfs/onevar/bz.hpp>
#include <triqs/gfs/onevar/cyclic_lattice.hpp>

// multivar 
#include <triqs/gfs/multivar/product.hpp>
#include <triqs/gfs/multivar/curry.hpp>
#include <triqs/gfs/singularity/m_tail.hpp>

#include <triqs/gfs/block/block.hpp>
#include <triqs/gfs/block/block2.hpp>
#include <triqs/gfs/block/block_gf_iterator.hpp>
#include <triqs/gfs/block/map.hpp>
#include <triqs/gfs/block/mapped_functions.hpp>

#include <triqs/gfs/transform/fourier_matsubara.hpp>
#include <triqs/gfs/transform/fourier_real.hpp>
#include <triqs/gfs/transform/fourier_lattice.hpp>
#include <triqs/gfs/transform/legendre_matsubara.hpp>


