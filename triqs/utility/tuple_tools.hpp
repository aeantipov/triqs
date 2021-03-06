/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2012-2014 by O. Parcollet
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
#include<triqs/utility/macros.hpp>
#include "./c14.hpp"
#include <tuple>
#include <ostream>
#include "./mini_vector.hpp"

// adding to the std lib the reversed lazy tuple...
// overloading & specializing only the functions needed here.
namespace std {

   // Reverse
  template<typename TU> struct _triqs_reversed_tuple {TU _x;};

  template<typename ... T> _triqs_reversed_tuple<std::tuple<T...>>        reverse(std::tuple<T...> && x) { return {move(x)};}
  template<typename ... T> _triqs_reversed_tuple<std::tuple<T...>&>       reverse(std::tuple<T...> & x) { return {x};}
  template<typename ... T> _triqs_reversed_tuple<std::tuple<T...>const &> reverse(std::tuple<T...> const & x) { return {x};}

  template<int pos, typename TU> AUTO_DECL get(_triqs_reversed_tuple<TU> const & t)
   RETURN(std::get<std::tuple_size<std14::decay_t<TU>>::value-1-pos>(t._x));

  template<int pos, typename TU> AUTO_DECL get(_triqs_reversed_tuple<TU> & t)
   RETURN(std::get<std::tuple_size<std14::decay_t<TU>>::value-1-pos>(t._x));

  template<int pos, typename TU> AUTO_DECL get(_triqs_reversed_tuple<TU> && t)
   RETURN(std::get<std::tuple_size<std14::decay_t<TU>>::value-1-pos>(move(t)._x));

  template <typename TU> class tuple_size<_triqs_reversed_tuple<TU>> : public tuple_size<std14::decay_t<TU>> {};

  // Zipped tuple:
  template <typename... T> struct _triqs_zipped_tuple {
   std::tuple<T...> _tu;
   template <typename... U> _triqs_zipped_tuple(U &&... u) : _tu(std::forward<U>(u)...) {}

   template <size_t I, size_t... Is> auto _get(std::c14::index_sequence<Is...>) RETURN(std::tie(std::get<I>(std::get<Is>(_tu))...));
   template <size_t I, size_t... Is> auto _get(std::c14::index_sequence<Is...>) const RETURN(std::tie(std::get<I>(std::get<Is>(_tu))...));
  };

  template <size_t I, typename... T>
  AUTO_DECL get(_triqs_zipped_tuple<T...> const &tu) RETURN(tu.template _get<I>(std14::make_index_sequence<sizeof...(T)>()));

  template <typename T0, typename... T> class tuple_size<_triqs_zipped_tuple<T0, T...>> : public std::tuple_size<std14::decay_t<T0>> {};

  template <typename... T> _triqs_zipped_tuple<T...> zip(T &&... x) {
   return {std::forward<T>(x)...};
  }

}

namespace triqs { namespace tuple {

 /// _get_seq<T>() : from a tuple T, return the index sequence of the tuple length
 template <typename T> std14::make_index_sequence<std::tuple_size<std14::decay_t<T>>::value> _get_seq() {
  return {};
 }

 // _get_seq_len<T> : constexpr : return the length of the tuple
 template <typename Tu> constexpr int _get_seq_len() { return std::tuple_size<std14::decay_t<Tu>>::value; }

 // basic tools
 template <int N> struct _int {};
 template <int... Is> struct all_indices : _int<Is>... {};

 // ---  impl complement -----
 template <int N, typename hole_seq, int... Is> struct complement_sequence_impl {
  static complement_sequence_impl<N - 1, hole_seq, Is...> get(_int<N>) {}
  static complement_sequence_impl<N - 1, hole_seq, N, Is...> get(...) {}
  using type = typename decltype(get(hole_seq()))::type;
 };

 template <typename hole_seq, int... Is> struct complement_sequence_impl<-1, hole_seq, Is...> {
  using type = std14::index_sequence<Is...>;
 };

 // An index sequence of elements of [0,N-1] which are NOT Is
 template <int N, int... Is> using complement_sequence = typename complement_sequence_impl<N, all_indices<Is...>>::type;

  /**
  * make_tuple_repeat<N>(x)
  * N : a non-negative integer
  * x : an object
  * Returns : a tuple with N elements, each equal x
  */
 template <typename T, std::size_t... Is>
 auto make_tuple_repeat_impl(T const& x, std14::index_sequence<Is...>) RETURN(std::make_tuple(((void)Is,x)...));

 template <int N, typename T>
 auto make_tuple_repeat(T const& x)
 RETURN(make_tuple_repeat_impl(x, std14::make_index_sequence<N>()));

  /**
  * apply(f, t)
  * f : a callable object
  * t a tuple
  * Returns : f(t0, t1 ...)
  * Cf : http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3915.pdf
  * for the idea of using the sequence, which is used several times below.
  */
 template <typename F, typename T, size_t... Is>
 AUTO_DECL apply_impl(F &&f, T &&t, std14::index_sequence<Is...>) RETURN(f(std::get<Is>(std::forward<T>(t))...));

 template <typename F, typename T>
 AUTO_DECL apply(F &&f, T &&t) RETURN(apply_impl(std::forward<F>(f), std::forward<T>(t), _get_seq<T>()));

 /*
  * apply_construct<C>(t)
  * C : a class
  * t a tuple
  * Returns : C{t0, t1, ....}
  */
 template <typename C, typename T, size_t... Is>
 AUTO_DECL apply_construct_impl(T &&t, std14::index_sequence<Is...>) RETURN(C{std::get<Is>(std::forward<T>(t))...});

 template <typename C, typename T>
 AUTO_DECL apply_construct(T &&t) RETURN(apply_construct_impl<C>(std::forward<T>(t), _get_seq<T>()));

 /*
  * apply_construct_parenthesis<C>(t)
  * C : a class
  * t a tuple
  * Returns : C(t0, t1, ....)
  */
 template <typename C, typename T, size_t... Is>
 AUTO_DECL apply_construct_parenthesis_impl(T &&t, std14::index_sequence<Is...>) RETURN(C(std::get<Is>(std::forward<T>(t))...));

 template <typename C, typename T>
 AUTO_DECL apply_construct_parenthesis(T &&t) RETURN(apply_construct_parenthesis_impl<C>(std::forward<T>(t), _get_seq<T>()));

 /*
  * called_on_tuple(f)
  * f : a callable object
  * Wrapping a function to make it callable from a tuple
  * called_on_tuple(f)( std::tie(x0,x1,x2)) is equivalent to f(x0,x1,x2)
  */
 template <typename F> struct _called_on_tuple {
  F _f;
  template <typename Tu> AUTO_DECL operator()(Tu &&tu) RETURN(apply(_f, std::forward<Tu>(tu)));
 };

 template <typename F> _called_on_tuple<F> called_on_tuple(F &&f) {
  return {std::forward<F>(f)};
 }

 // Implementation tools
 // _for_each_impl (f, x0, x1, ..., xn) calls f(x0); f(x1); ... f(xn); IN THIS ORDER
 template <typename F> void _for_each_impl(F &&f) {}

 template <typename F, typename T0, typename... T> void _for_each_impl(F &&f, T0 &&x0, T &&... x) {
  f(std::forward<T0>(x0));
  _for_each_impl(f, std::forward<T>(x)...);
 }

 // _for_each_apply_impl (f, t0, t1, ..., tn) calls apply(f,x0); apply(f,t1); ... apply(f,tn);
 template <typename F> void _for_each_apply_impl(F &&f) {}

 template <typename F, typename T0, typename... T> void _for_each_apply_impl(F &&f, T0 &&t0, T &&... t) {
  apply(f, std::forward<T0>(t0));
  _for_each_apply_impl(f, std::forward<T>(t)...);
 }

 /*
  * for_each(t, f)
  * t: a tuple
  * f: a callable object
  * calls f on all tuple elements in the order of the tuple: f(x) for all x in t
  */
 template <typename F, typename T, size_t... Is> void for_each_impl(F &&f, T &&t, std14::index_sequence<Is...>) {
  _for_each_impl(f, std::get<Is>(t)...);
 }

 template <typename T, typename F> void for_each(T &&t, F &&f) { for_each_impl(f, std::forward<T>(t), _get_seq<T>()); }

 /* for_each_enumerate(t, f)
  * t: a tuple
  * f: a callable object
  * calls f on all tuple elements:
  *  Python equivalent :
  *    for n,x in enumrate(t): f(n,x)
  */
 template <typename F, typename T, size_t... Is> void _for_each_enum_impl(F &&f, T &&t, std14::index_sequence<Is...>) {
  _for_each_apply_impl(f, std::tuple<int, decltype(std::get<Is>(t))>(Is, std::get<Is>(t))...);
  // not make_tuple here, we keep the ref given by the get
 }

 template <typename T, typename F> void for_each_enumerate(T &&t, F &&f) {
  _for_each_enum_impl(f, std::forward<T>(t), _get_seq<T>());
 }

 /*
  * for_each_zip(f, t1,t2)
  * f : a callable object
  * t1,t2 : two tuples
  * calls f on all tuple elements: f(x1,x2) for x1 in t1 for x2 in t2
  */
 template <typename F, typename T0, typename T1, size_t... Is>
 void _for_each_zip_impl(std14::index_sequence<Is...>, F &&f, T0 &&t0, T1 &&t1) {
  _for_each_impl(called_on_tuple(std::forward<F>(f)), std::tie(std::get<Is>(t0), std::get<Is>(t1))...);
 }

 template <typename F, typename T0, typename T1> void for_each_zip(F &&f, T0 &&t0, T1 &&t1) {
  _for_each_zip_impl(_get_seq<T0>(), std::forward<F>(f), std::forward<T0>(t0), std::forward<T1>(t1));
 }

 /*
  * for_each_zip(f, t1,t2,t3)
  * f : a callable object
  * t1,t2,t3 : three tuples
  * calls f on all tuple elements: f(x1,x2,x3) for x1 in t1 for x2 in t2 for x3 in t3
  */
 template <typename F, typename T0, typename T1, typename T2, size_t... Is>
 void _for_each_zip_impl(std14::index_sequence<Is...>, F &&f, T0 &&t0, T1 &&t1, T2 &&t2) {
  _for_each_impl(called_on_tuple(std::forward<F>(f)), std::tie(std::get<Is>(t0), std::get<Is>(t1), std::get<Is>(t2))...);
 }

 template <typename F, typename T0, typename T1, typename T2> void for_each_zip(F &&f, T0 &&t0, T1 &&t1, T2 &&t2) {
  _for_each_zip_impl(_get_seq<T0>(), std::forward<F>(f), std::forward<T0>(t0), std::forward<T1>(t1), std::forward<T2>(t2));
 }

 /*
  * map(f, t)
  * f : a callable object
  * t tuple
  * Returns : [f(i) for i in t]
  */
 template <typename F, typename T, size_t... Is>
 AUTO_DECL _map_impl(F &&f, T &&t, std14::index_sequence<Is...>)
     RETURN(std::make_tuple(std::forward<F>(f)(std::get<Is>(t))...));

 template <typename T, typename F>
 AUTO_DECL map(F &&f, T &&t) RETURN(_map_impl(std::forward<F>(f), std::forward<T>(t), _get_seq<T>()));

 /*
  * map_on_zip(f, t1, t2)
  * f : a callable object
  * t1, t2 two tuples of the same size
  * Returns : [f(i,j) for i,j in zip(t1,t2)]
  */
 template <typename F, typename T0, typename T1, size_t... Is>
 auto _map_impl(F &&f, T0 &&t0, T1 &&t1, std14::index_sequence<Is...>)
     RETURN(std::make_tuple(std::forward<F>(f)(std::get<Is>(t0), std::get<Is>(t1))...));

 template <typename T0, typename T1, typename F>
 auto map_on_zip(F &&f, T0 &&t0, T1 &&t1)
     RETURN(_map_impl(std::forward<F>(f), std::forward<T0>(t0), std::forward<T1>(t1), _get_seq<T0>()));

 template <typename T0, typename T1, typename F>
 auto map_on_zip_v2(F &&f, T0 &&t0, T1 &&t1)
     RETURN(map(called_on_tuple(f), zip(t0,t1)));

 /*
  * map_on_zip(f,t0,t1,t2)
  * f : a callable object
  * t0, t1, t2 two tuples of the same size
  * Returns : [f(i,j,k) for i,j,k in zip(t0,t1,t2)]
  */
 template <typename F, typename T0, typename T1, typename T2, size_t... Is>
 auto _map_impl(F &&f, T0 &&t0, T1 &&t1, T2 &&t2, std14::index_sequence<Is...>)
     RETURN(std::make_tuple(std::forward<F>(f)(std::get<Is>(t0), std::get<Is>(t1), std::get<Is>(t2))...));

 template <typename T0, typename T1, typename T2, typename F>
 auto map_on_zip(F &&f, T0 &&t0, T1 &&t1, T2 &&t2)
     RETURN(_map_impl(std::forward<F>(f), std::forward<T0>(t0), std::forward<T1>(t1), std::forward<T2>(t2), _get_seq<T0>()));

#ifdef TRIQS_C11
}}
#else

/*
 * fold(f, t1, r_init)
 * f : a callable object : f(x,r) -> r'
 * t a tuple
 * Returns : f(xN,f(x_N-1,...f(x0,r_init)) on the tuple
 */
 template <int pos, typename F, typename T, typename R> decltype(auto) fold_impl(_int<pos>, F &&f, T &&t, R &&r) {
  return fold_impl(_int<pos - 1>(), std::forward<F>(f), std::forward<T>(t),
                   f(std::get<_get_seq_len<T>() - 1 - pos>(t), std::forward<R>(r)));
 }
 template <typename F, typename T, typename R> R fold_impl(_int<-1>, F &&f, T &&t, R &&r) { return std::forward<R>(r); }

 template <typename F, typename T, typename R> decltype(auto) fold(F &&f, T &&t, R &&r) {
  return fold_impl(_int<_get_seq_len<T>() - 1>(), std::forward<F>(f), std::forward<T>(t), std::forward<R>(r));
 }

 /*
  * fold(f, r_init, t1, t2)
  * f : a callable object
  * t1, t2 two tuples of the same size
  * Returns : f(x0,y0,f(x1,y1,,f(....)) for t1 = (x0,x1 ...) and t2 = (y0,y1...).
  */

 template <int pos, typename F, typename T0, typename T1, typename R>
 decltype(auto) fold_impl(_int<pos>, F &&f, T0 &&t0, T1 &&t1, R &&r) {
  constexpr int n = _get_seq_len<T0>() - 1 - pos;
  return fold_impl(_int<pos - 1>(), std::forward<F>(f), std::forward<T0>(t0), std::forward<T1>(t1),
                   f(std::get<n>(t0), std::get<n>(t1), std::forward<R>(r)));
 }

 template <typename F, typename T0, typename T1, typename R> R fold_impl(_int<-1>, F &&f, T0 &&t0, T1 &&t1, R &&r) {
  return std::forward<R>(r);
 }

 template <typename F, typename T0, typename T1, typename R> decltype(auto) fold(F &&f, T0 &&t0, T1 &&t1, R &&r) {
  return fold_impl(_int<_get_seq_len<T0>() - 1>(), std::forward<F>(f), std::forward<T0>(t0), std::forward<T1>(t1),
                   std::forward<R>(r));
 }

 /*
  * replace<int ... I>(t,r)
  *  Given a tuple t, and integers, returns the tuple where the elements at initial position I are replaced by r
  */
 template <int I, typename T, typename R> R _get_rpl(T &&x, R &&r, _int<I>) { return std::forward<R>(r); }
 template <int I, typename T, typename R> T _get_rpl(T &&x, R &&r, ...) { return std::forward<T>(x); }

 template <size_t... Is, typename Tu, typename R, typename AllIndices>
 auto _replace_impl(Tu &&tu, R &&r, AllIndices _, std14::index_sequence<Is...>)
     RETURN(std::make_tuple(_get_rpl<Is>(std::get<Is>(tu), r, _)...));

 template <int... I, typename Tu, typename R>
 auto replace(Tu &&tu, R &&r) RETURN(_replace_impl(tu, r, all_indices<I...>(), _get_seq<Tu>()));

 /*
  * filter<int ... I>(t) :
  *  Given a tuple t, and integers, returns the tuple where the elements at initial position I are kept.
  */
 template <typename Tu, size_t... I> using filter_t = std::tuple<typename std::tuple_element<I, std14::decay_t<Tu>>::type...>;

 template <size_t... I, typename Tu> filter_t<Tu, I...> filter(Tu &&tu) {
  return filter_t<Tu, I...>(std::get<I>(std::forward<Tu>(tu))...);
 }

 template <size_t... I, typename Tu> filter_t<Tu, I...> filter(Tu &&tu, std14::index_sequence<I...>) {
  return filter_t<Tu, I...>(std::get<I>(std::forward<Tu>(tu))...);
 }

 /*
  * filter_out<int ... I>(t) :
  *  Given a tuple t, and integers, returns the tuple where the elements at initial position I are dropped.
  */
 template <int... I, typename Tu>
 AUTO_DECL filter_out(Tu &&tu) RETURN(filter(tu, complement_sequence<std::tuple_size<std14::decay_t<Tu>>::value - 1, I...>()));

 template <typename Tu, int... I> using filter_out_t = std14::decay_t<decltype(filter_out<I...>(std::declval<Tu>()))>;

 /*
  * t : a tuple
  * x : anything
  * push_back (t,x) -> returns new tuple with x append at the end
  */
 template <typename T, typename X>
 auto push_back(T &&t, X &&x) RETURN(std::tuple_cat(std::forward<T>(t), std::make_tuple(std::forward<X>(x))));

 /*
  * t : a tuple
  * x : anything
  * push_front (t,x) -> returns new tuple with x append at the first position
  */
 template <typename T, typename X>
 auto push_front(T &&t, X &&x) RETURN(std::tuple_cat(std::make_tuple(std::forward<X>(x)), std::forward<T>(t)));

 // To be rewritten ....
 /*
  * inverse_filter<int L, int ... I>(t,x)
  *  Given a tuple t, and integers, returns the tuple R of size L such that filter<I...>(R) == t
  *  and the missing position are filled with object x.
  *  Precondition (static_assert : sizeof...(I)==size of t)
  *  and max (I) < L
  */

 // pos = position in the tuple, c : counter tuplesize-1 ->0, I : position to filter
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_impl;

 // default case where pos != the first I
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_impl<pos, pos_in, c, pos, I...> {
  template<typename TupleIn, typename TupleOut, typename X> auto operator() (TupleIn const & t, TupleOut && out, X const & x) const
   DECL_AND_RETURN( inverse_filter_impl<pos+1,pos_in+1,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),std::get<pos_in>(t)),x));
 };

 // when pos == first I
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_impl {
  template<typename TupleIn, typename TupleOut, typename X> auto operator() (TupleIn const & t, TupleOut && out, X const & x) const
   DECL_AND_RETURN( inverse_filter_impl<pos+1,pos_in,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),x), x));
 };

 template<int pos, int pos_in, int ... I> struct inverse_filter_impl <pos,pos_in, -1, I...> {
  template<typename TupleIn, typename TupleOut, typename X> TupleOut operator() (TupleIn const &, TupleOut && out, X const &) const {return out;}
 };

 // put out for clearer error message
 template< typename Tu, typename X, int L, int ...I> struct inverse_filter_r_type {
  static_assert(sizeof...(I) == std::tuple_size<Tu>::value, "inverse filter : the # of int must be equal to the tuple size !!");
  typedef inverse_filter_impl<0,0,L-1, I...> type;
 };

 template<int L, int ...I, typename Tu, typename X>
  auto inverse_filter(Tu const & tu, X const &x) DECL_AND_RETURN ( typename inverse_filter_r_type<Tu, X, L,  I...>::type ()(tu, std::make_tuple(),x));

 /*
  * inverse_filter_out<int ... I>(t,x)
  *  Given a tuple t, and integers, returns the tuple R such that filter_out<I...>(R) == t
  *  and the missing position are filled with object x.
  */

 // pos = position in the tuple, c : counter tuplesize-1 ->0, I : position to filter
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_out_impl;

 // default case where pos != the first I
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_out_impl {
  template<typename TupleIn, typename TupleOut, typename X> auto operator() (TupleIn const & t, TupleOut && out, X const & x) const
   DECL_AND_RETURN( inverse_filter_out_impl<pos+1,pos_in+1,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),std::get<pos_in>(t)),x));
 };

 // when pos == first I
 template<int pos, int pos_in, int c, int ... I> struct inverse_filter_out_impl <pos, pos_in, c, pos, I...> {
  template<typename TupleIn, typename TupleOut, typename X> auto operator() (TupleIn const & t, TupleOut && out, X const & x) const
   DECL_AND_RETURN( inverse_filter_out_impl<pos+1,pos_in,c-1, I...> ()( t, push_back(std::forward<TupleOut>(out),x), x));
 };

 template<int pos, int pos_in, int ... I> struct inverse_filter_out_impl <pos,pos_in, -1, I...> {
  template<typename TupleIn, typename TupleOut, typename X> TupleOut operator() (TupleIn const &, TupleOut && out, X const &) const {return out;}
 };

 template<int ...I, typename Tu, typename X>
  auto inverse_filter_out(Tu const & tu, X const &x) DECL_AND_RETURN ( inverse_filter_out_impl<0,0,std::tuple_size<Tu>::value+sizeof...(I)-1, I...>()(tu, std::make_tuple(),x));

 /*
 * print a tuple
 */
 /*inline void _triqs_print_tuple_impl(std::ostream &os) {}
 template <typename T0, typename... T> void _triqs_print_tuple_impl(std::ostream &os, T0 const &x0, T const &... x) {
  os << x0;
  if (sizeof...(T) > 0) os << ',';
  _triqs_print_tuple_impl(os, x...);
 }

 template <size_t... Is, typename... T>
 void _triqs_print_tuple(std::ostream &os, std::tuple<T...> const &t, std14::index_sequence<Is...>) {
  _triqs_print_tuple_impl(os, std::get<Is>(t)...);
 }
*/

}
}

namespace std {

template <typename... T> std::ostream &operator<<(std::ostream &os, std::tuple<T...> const &t) {
 os << "(";
 triqs::tuple::for_each(t, [&os, c=0](auto & x) mutable {if (c++) os << ','; os << x;});
 //triqs::tuple::_triqs_print_tuple(os, t, std14::make_index_sequence<sizeof...(T)>());
 return os << ")";
}

// do not move this up....
namespace c14 {

 // a little helper class to wait for the correction that tuple construct is NOT explicit
 template <typename... Args> class tuple : public std::tuple<Args...> {
  public:
  template <typename... Args2> tuple(Args2 &&... args2) : std::tuple<Args...>(std::forward<Args2>(args2)...) {}
 };
}

// minimal hack to get the metaprogramming work with this tuple too....
template <int i, typename... Args>
auto get(c14::tuple<Args...> const &t) DECL_AND_RETURN(std::get<i>(static_cast<std::tuple<Args...>>(t)));

template <typename... Args> class tuple_size<c14::tuple<Args...>> : public tuple_size<std::tuple<Args...>> {};
}
#endif
