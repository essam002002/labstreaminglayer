/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga        2006-2014
// (C) Copyright Microsoft Corporation  2014
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_DETAIL_MPL_HPP
#define BOOST_INTRUSIVE_DETAIL_MPL_HPP

#ifndef BOOST_CONFIG_HPP
#  include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <boost/intrusive/detail/config_begin.hpp>
#include <boost/move/detail/type_traits.hpp>
#include <cstddef>

namespace lslboost {
namespace intrusive {
namespace detail {
   
using lslboost::move_detail::is_same;
using lslboost::move_detail::add_const;
using lslboost::move_detail::remove_const;
using lslboost::move_detail::remove_cv;
using lslboost::move_detail::remove_reference;
using lslboost::move_detail::add_reference;
using lslboost::move_detail::remove_pointer;
using lslboost::move_detail::add_pointer;
using lslboost::move_detail::true_type;
using lslboost::move_detail::false_type;
using lslboost::move_detail::enable_if_c;
using lslboost::move_detail::enable_if;
using lslboost::move_detail::disable_if_c;
using lslboost::move_detail::disable_if;
using lslboost::move_detail::is_convertible;
using lslboost::move_detail::if_c;
using lslboost::move_detail::if_;
using lslboost::move_detail::is_const;
using lslboost::move_detail::identity;
using lslboost::move_detail::alignment_of;
using lslboost::move_detail::is_empty;
using lslboost::move_detail::addressof;
using lslboost::move_detail::integral_constant;
using lslboost::move_detail::enable_if_convertible;
using lslboost::move_detail::disable_if_convertible;
using lslboost::move_detail::bool_;
using lslboost::move_detail::true_;
using lslboost::move_detail::false_;
using lslboost::move_detail::yes_type;
using lslboost::move_detail::no_type;
using lslboost::move_detail::apply;
using lslboost::move_detail::eval_if_c;
using lslboost::move_detail::eval_if;
using lslboost::move_detail::unvoid_ref;
using lslboost::move_detail::add_const_if_c;

template<std::size_t S>
struct ls_zeros
{
   static const std::size_t value = (S & std::size_t(1)) ? 0 : (1 + ls_zeros<(S>>1u)>::value);
};

template<>
struct ls_zeros<0>
{
   static const std::size_t value = 0;
};

template<>
struct ls_zeros<1>
{
   static const std::size_t value = 0;
};

// Infrastructure for providing a default type for T::TNAME if absent.
#define BOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(TNAME)     \
   template <typename T, typename DefaultType>                    \
   struct boost_intrusive_default_type_ ## TNAME                  \
   {                                                              \
      template <typename X>                                       \
      static char test(int, typename X::TNAME*);                  \
                                                                  \
      template <typename X>                                       \
      static int test(...);                                       \
                                                                  \
      struct DefaultWrap { typedef DefaultType TNAME; };          \
                                                                  \
      static const bool value = (1 == sizeof(test<T>(0, 0)));     \
                                                                  \
      typedef typename                                            \
         ::lslboost::intrusive::detail::if_c                         \
            <value, T, DefaultWrap>::type::TNAME type;            \
   };                                                             \
   //

#define BOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT(INSTANTIATION_NS_PREFIX, T, TNAME, TIMPL)   \
      typename INSTANTIATION_NS_PREFIX                                                       \
         boost_intrusive_default_type_ ## TNAME< T, TIMPL >::type                            \
//

#define BOOST_INTRUSIVE_INSTANTIATE_EVAL_DEFAULT_TYPE_TMPLT(TNAME)\
   template <typename T, typename DefaultType>                    \
   struct boost_intrusive_eval_default_type_ ## TNAME             \
   {                                                              \
      template <typename X>                                       \
      static char test(int, typename X::TNAME*);                  \
                                                                  \
      template <typename X>                                       \
      static int test(...);                                       \
                                                                  \
      struct DefaultWrap                                          \
      { typedef typename DefaultType::type TNAME; };              \
                                                                  \
      static const bool value = (1 == sizeof(test<T>(0, 0)));     \
                                                                  \
      typedef typename                                            \
         ::lslboost::intrusive::detail::eval_if_c                    \
            < value                                               \
            , ::lslboost::intrusive::detail::identity<T>             \
            , ::lslboost::intrusive::detail::identity<DefaultWrap>   \
            >::type::TNAME type;                                  \
   };                                                             \
//

#define BOOST_INTRUSIVE_OBTAIN_TYPE_WITH_EVAL_DEFAULT(INSTANTIATION_NS_PREFIX, T, TNAME, TIMPL) \
      typename INSTANTIATION_NS_PREFIX                                                          \
         boost_intrusive_eval_default_type_ ## TNAME< T, TIMPL >::type                          \
//

#define BOOST_INTRUSIVE_INTERNAL_STATIC_BOOL_IS_TRUE(TRAITS_PREFIX, TYPEDEF_TO_FIND) \
template <class T>\
struct TRAITS_PREFIX##_bool\
{\
   template<bool Add>\
   struct two_or_three {yes_type _[2 + Add];};\
   template <class U> static yes_type test(...);\
   template <class U> static two_or_three<U::TYPEDEF_TO_FIND> test (int);\
   static const std::size_t value = sizeof(test<T>(0));\
};\
\
template <class T>\
struct TRAITS_PREFIX##_bool_is_true\
{\
   static const bool value = TRAITS_PREFIX##_bool<T>::value > sizeof(yes_type)*2;\
};\
//

#define BOOST_INTRUSIVE_HAS_STATIC_MEMBER_FUNC_SIGNATURE(TRAITS_NAME, FUNC_NAME) \
  template <typename U, typename Signature> \
  class TRAITS_NAME \
  { \
  private: \
  template<Signature> struct helper;\
  template<typename T> \
  static ::lslboost::intrusive::detail::yes_type test(helper<&T::FUNC_NAME>*); \
  template<typename T> static ::lslboost::intrusive::detail::no_type test(...); \
  public: \
  static const bool value = sizeof(test<U>(0)) == sizeof(::lslboost::intrusive::detail::yes_type); \
  }; \
//

#define BOOST_INTRUSIVE_HAS_MEMBER_FUNC_CALLED(TRAITS_NAME, FUNC_NAME) \
template <typename Type> \
struct TRAITS_NAME \
{ \
   struct BaseMixin \
   { \
      void FUNC_NAME(); \
   }; \
   struct Base : public Type, public BaseMixin { Base(); }; \
   template <typename T, T t> class Helper{}; \
   template <typename U> \
   static ::lslboost::intrusive::detail::no_type  test(U*, Helper<void (BaseMixin::*)(), &U::FUNC_NAME>* = 0); \
   static ::lslboost::intrusive::detail::yes_type test(...); \
   static const bool value = sizeof(::lslboost::intrusive::detail::yes_type) == sizeof(test((Base*)(0))); \
};\
//

#define BOOST_INTRUSIVE_HAS_MEMBER_FUNC_CALLED_IGNORE_SIGNATURE(TRAITS_NAME, FUNC_NAME) \
BOOST_INTRUSIVE_HAS_MEMBER_FUNC_CALLED(TRAITS_NAME##_ignore_signature, FUNC_NAME) \
\
template <typename Type, class> \
struct TRAITS_NAME \
   : public TRAITS_NAME##_ignore_signature<Type> \
{};\
//

} //namespace detail
} //namespace intrusive
} //namespace lslboost

#include <boost/intrusive/detail/config_end.hpp>

#endif //BOOST_INTRUSIVE_DETAIL_MPL_HPP
