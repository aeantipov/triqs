/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012 by M. Ferrero, O. Parcollet
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
#include <triqs/utility/first_include.hpp>
#include <utility>
#include <triqs/arrays.hpp>
#include <triqs/arrays/matrix_tensor_proxy.hpp>
#include <triqs/utility/view_tools.hpp>

namespace triqs {
namespace gfs {

 //---------------------------- common stuff for array proxies ----------------------------------

 template <typename T, int D> struct data_proxy_array_common {
  using storage_t = arrays::array<T, D>;
  using storage_view_t = typename storage_t::view_type;
  using storage_const_view_t = typename storage_t::const_view_type;

  template <typename S, typename RHS> static void assign_to_scalar(S& data, RHS&& rhs) { data() = std::forward<RHS>(rhs); }
  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) { data.rebind(rhs.data()); }
 };

 //---------------------------- generic case array of dim R----------------------------------

 template <typename T, int R> struct data_proxy_array : data_proxy_array_common<T, R> {
  using B = data_proxy_array_common<T, R>;
  auto operator()(typename B::storage_t& data, long i) const { return arrays::make_tensor_proxy(data, i); }
  auto operator()(typename B::storage_t const& data, long i) const { return arrays::make_const_tensor_proxy(data, i); }
  auto operator()(typename B::storage_view_t& data, long i) const { return arrays::make_tensor_proxy(data, i); }
  auto operator()(typename B::storage_view_t const& data, long i) const { return arrays::make_const_tensor_proxy(data, i); }
  auto operator()(typename B::storage_const_view_t& data, long i) const { return arrays::make_const_tensor_proxy(data, i); }
  auto operator()(typename B::storage_const_view_t const& data, long i) const { return arrays::make_const_tensor_proxy(data, i); }
 };

 //---------------------------- 3d array : returns matrices in this case ! ----------------------------------

 template <typename T> struct data_proxy_array<T, 3> : data_proxy_array_common<T, 3> {
  using B = data_proxy_array_common<T, 3>;
  auto operator()(typename B::storage_t& data, long i) const { return arrays::make_matrix_proxy(data, i); }
  auto operator()(typename B::storage_t const& data, long i) const { return arrays::make_const_matrix_proxy(data, i); }
  auto operator()(typename B::storage_view_t& data, long i) const { return arrays::make_matrix_proxy(data, i); }
  auto operator()(typename B::storage_view_t const& data, long i) const { return arrays::make_const_matrix_proxy(data, i); }
  auto operator()(typename B::storage_const_view_t& data, long i) const { return arrays::make_const_matrix_proxy(data, i); }
  auto operator()(typename B::storage_const_view_t const& data, long i) const { return arrays::make_const_matrix_proxy(data, i); }
 };

 //---------------------------- 1d array ----------------------------------

 template <typename T> struct data_proxy_array<T, 1> : data_proxy_array_common<T, 1> {
  template <typename S> decltype(auto) operator()(S& data, long i) const { return data(i); }
 };

 //---------------------------- multi variable ----------------------------------

 template <typename T, int TotalDim> struct data_proxy_array_multivar : data_proxy_array_common<T, TotalDim> {
  // using the standard technique from tuple::apply with a sequence
  template <typename S, typename Tu, size_t... Is>
  decltype(auto) _impl(S& data, Tu const& tu, std14::index_sequence<Is...>) const {
   return data(std::get<Is>(tu)..., arrays::ellipsis());
  }
  template <typename S, typename Tu> decltype(auto) operator()(S& data, Tu const& tu) const {
   return _impl(data, tu, triqs::tuple::_get_seq<Tu>());
  }
 };

 //---------------------------- multi variable ----------------------------------

 template <typename T, int TotalDim> struct data_proxy_array_multivar_matrix_valued : data_proxy_array_common<T, TotalDim> {
  // using the standard technique from tuple::apply with a sequence
  template <typename S, typename Tu, size_t... Is>
  decltype(auto) _impl(S& data, Tu const& tu, std14::index_sequence<Is...>) const {
   return make_matrix_view(data(std::get<Is>(tu)..., arrays::range(), arrays::range()));
  }
  template <typename S, typename Tu> decltype(auto) operator()(S& data, Tu const& tu) const {
   return _impl(data, tu, triqs::tuple::_get_seq<Tu>());
  }
 };

 //---------------------------- vector ----------------------------------

 template <typename V> struct view_proxy : public V {
  using regular_type = triqs::Regular<V>;
  view_proxy() : V(typename V::regular_type()) {}
  view_proxy(V const& v) : V(v){};
  view_proxy(view_proxy const& p) : V(p){};
  template <typename... Args> explicit view_proxy(Args&&... args) : V(std::forward<Args>(args)...) {}
  view_proxy& operator=(view_proxy const& cp) {
   this->rebind(cp);
   return *this;
  }
  view_proxy& operator=(V const& v) {
   this->rebind(v);
   return *this;
  }
  using V::operator=;
 };

 //-----------------------

 template <typename T> struct data_proxy_vector {
  using Tv = typename T::view_type;
  using Tcv = typename T::const_view_type;

  /// The storage
  using storage_t = std::vector<T>;
  using storage_view_t = std::vector<view_proxy<Tv>>;
  using storage_const_view_t = std::vector<view_proxy<Tcv>>;

  /// The data access
  template <typename S> decltype(auto) operator()(S& data, size_t i) const { return data[i]; }

  template <typename S, typename RHS> static void assign_to_scalar(S& data, RHS&& rhs) {
   for (auto& x : data) x = rhs;
  }

  template <typename ST, typename RHS> static void rebind(ST& data, RHS&& rhs) {
   data.clear();
   for (auto& x : rhs.data()) data.push_back(x);
  }
 };

 //--------------------------

 template <typename T> struct data_proxy_vector2 {
  using Tv = typename T::view_type;
  using Tcv = typename T::const_view_type;

  /// The storage
  using storage_t = std::vector<std::vector<T>>;
  using storage_view_t = std::vector<std::vector<view_proxy<Tv>>>;
  using storage_const_view_t = std::vector<std::vector<view_proxy<Tcv>>>;

  /// The data access
  template <typename S> decltype(auto) operator()(S& data, std::tuple<long, long> const& i) const {
   return data[std::get<0>(i)][std::get<1>(i)];
  }

  template <typename S, typename RHS> static void assign_to_scalar(S& data, RHS&& rhs) {
   for (auto& x : data)
    for (auto& y : x) y = rhs;
  }

  template <typename ST, typename RHS>
  static void rebind(ST& data, RHS&& rhs) = delete; // useful only for python interface, not here
                                                    // but easy to write if needed at some point.
 };
}
}

