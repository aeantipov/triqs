/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2013-2016 by O. Parcollet
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
#include <triqs/arrays.hpp>

namespace triqs {
namespace arrays {
%for ARR in ['matrix', 'array'] : 
%for IsConst in [True, False] :
<% CLASSNAME = ('const_' if IsConst else '') + ARR + '_proxy'%>

 template <typename A>
 struct ${CLASSNAME} : TRIQS_CONCEPT_TAG_NAME(${'Imm' if IsConst else 'M'}utable${ARR.capitalize()}) {

  using A_t = std14::decay_t<A>;
  static constexpr bool is_const = ${'true' if IsConst else 'false'};
  static constexpr int rank = A_t::rank;

  A a;
  long n;

  using value_type = typename A_t::value_type;
  using traversal_order_t = typename A_t::traversal_order_t;
  using slicer_t = indexmaps::slicer<typename A_t::indexmap_type, long, ellipsis>;
  using indexmap_type = typename slicer_t::r_type;
  using domain_type = typename indexmap_type::domain_type;
  using view_type = ${ARR}_const_view<value_type>;
  using const_view_type = view_type;

  template <typename AA> ${CLASSNAME} (AA &&a, long n) : a(std::forward<AA>(a)), n(n) {}

  indexmap_type indexmap() const { return slicer_t::invoke(a.indexmap(), n, ellipsis()); }
  domain_type domain() const { return indexmap().domain(); }

  auto storage() const DECL_AND_RETURN(a.storage());
  value_type const *restrict data_start() const { return &storage()[indexmap().start_shift()]; }
  value_type *restrict data_start() { return &storage()[indexmap().start_shift()]; }

  view_type operator()() const { return *this; }

  template <typename... Args>
  std::c14::enable_if_t<!triqs::clef::is_any_lazy<Args...>::value, value_type const &> operator()(Args &&... args) const {
   return a(n, std::forward<Args>(args)...);
  }
  TRIQS_CLEF_IMPLEMENT_LAZY_CALL();

%if IsConst:
  template <typename RHS> ${CLASSNAME}  &operator=(const RHS &X) =delete; // cannot assign to a const 
  TRIQS_DELETE_COMPOUND_OPERATORS(${CLASSNAME} );

%else:

 template <typename RHS> ${CLASSNAME}  &operator=(const RHS &X) {
   triqs_arrays_assign_delegation(*this, X);
   return *this;
  }
  // same for the same class (not captured by the template).
  ${CLASSNAME} &operator=(${CLASSNAME}  const &X) {
   triqs_arrays_assign_delegation(*this, X);
   return *this;
  }

  TRIQS_DEFINE_COMPOUND_OPERATORS(${CLASSNAME} );

  // auto_assign like the main classes
  template <typename F> friend void triqs_clef_auto_assign(${CLASSNAME} x, F &&f) {
   foreach (x, array_auto_assign_worker<${CLASSNAME}, F>{x, f});
  }

%endif
 friend std::ostream &operator<<(std::ostream &out, ${CLASSNAME} const &x) { return out << const_view_type(x); }
 };

 template <typename A, bool IsMatrix>
 auto get_shape(${CLASSNAME} <A> const &x) DECL_AND_RETURN(get_shape(x.a).front_pop());

%endfor
%endfor

 // factory
 template <typename A>
 const_tensor_proxy<A> make_const_tensor_proxy(A &&a, long n) { return {std::forward<A>(a), n}; }

 // factories
 template <typename A>
 const_matrix_proxy<A> make_const_matrix_proxy(A &&a, long n) { return {std::forward<A>(a), n}; }

 // factories
 template <typename A>
 tensor_proxy<A> make_tensor_proxy(A &&a, long n) { return {std::forward<A>(a), n}; }

 // factories
 template <typename A>
 matrix_proxy<A> make_matrix_proxy(A &&a, long n) { return {std::forward<A>(a), n}; }

}
}

