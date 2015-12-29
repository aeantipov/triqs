/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011-2016 by O. Parcollet
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
#include "./functional/map.hpp"
namespace triqs { namespace arrays {

 // not for libc++ (already defined)
#if !defined(_LIBCPP_VERSION)
 // complex conjugation for integers
 inline std::complex<double> conj(int x) { return x; }
 inline std::complex<double> conj(long x) { return x; }
 inline std::complex<double> conj(long long x) { return x; }
 inline std::complex<double> conj(double x) { return x; }
#endif

 //C++14 will simply be ... 
 //template <typename A> decltype(auto) abs(A && a) { return map( [](auto const &x) { using std::abs; return abs(a);}, std::forward<A>(a));}

 using std::pow;
 template<typename T> TYPE_ENABLE_IF(T, std::is_integral<T>) pow(T x, int n) { return (n==0 ? 1 : pow(x,n-1)*x);}
 struct pow_wrap { 
   int n; 
   template<typename A> A operator()( A const & a) const { return pow(a,n);}
 };

 template <typename A>
  typename boost::lazy_enable_if_c<!ImmutableMatrix<A>::value,std::result_of<map_impl<pow_wrap,1>(A)>>::type
  pow(A && a, int n) { return map(pow_wrap{n})(std::forward<A>(a)); }

%for F in "abs real imag floor conj".split():
 ///${F} mapped of array, matrix, vector. Acts element wise.
 template <typename A, typename = std14::enable_if_t<ImmutableCuboidArray<A>::value>> 
  auto ${F}(A && a) { return map( [](auto const &x) { using std::${F}; return ${F}(a);}, std::forward<A>(a));}
%endfor

%for F in "exp cos sin tan cosh sinh tanh acos asin atan log sqrt".split() :
 ///${F} mapped of array, vector but NOT matrix. Acts element wise.
 template <typename A, typename = std14::enable_if_t<ImmutableArray<A>::value||ImmutableVector<A>::value>> 
  auto ${F}(A && a) { return map( [](auto const &x) { using std::${F}; return ${F}(a);}, std::forward<A>(a));}
%endfor

 // --------------- Computation of the matrix norm ------------------------

 inline double frobenius_norm(matrix<double> const& a) {
  return std::sqrt(fold([](double r, double x)->double {
   auto ab = std::abs(x);
   return r + ab * ab;
  })(a));
 }

}}//namespace triqs::arrays 

