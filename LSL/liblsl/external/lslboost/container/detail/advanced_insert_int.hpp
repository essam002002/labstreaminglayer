//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2008-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.lslboost.org/LICENSE_1_0.txt)
//
// See http://www.lslboost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_CONTAINER_ADVANCED_INSERT_INT_HPP
#define BOOST_CONTAINER_ADVANCED_INSERT_INT_HPP

#if defined(_MSC_VER)
#  pragma once
#endif

#include "config_begin.hpp"
#include <lslboost/container/detail/workaround.hpp>
#include <lslboost/container/allocator_traits.hpp>
#include <lslboost/container/detail/destroyers.hpp>
#include <lslboost/aligned_storage.hpp>
#include <lslboost/move/utility.hpp>
#include <iterator>  //std::iterator_traits
#include <lslboost/assert.hpp>
#include <lslboost/detail/no_exceptions_support.hpp>

namespace lslboost { namespace container { namespace container_detail {

template<class A, class FwdIt, class Iterator>
struct move_insert_range_proxy
{
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;

   move_insert_range_proxy(A& a, FwdIt first)
      :  a_(a), first_(first)
   {}

   void uninitialized_copy_n_and_update(Iterator p, size_type n)
   {
      this->first_ = ::lslboost::container::uninitialized_move_alloc_n_source
         (this->a_, this->first_, n, p);
   }

   void copy_n_and_update(Iterator p, size_type n)
   {
      this->first_ = ::lslboost::container::move_n_source(this->first_, n, p);
   }

   A &a_;
   FwdIt first_;
};


template<class A, class FwdIt, class Iterator>
struct insert_range_proxy
{
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;

   insert_range_proxy(A& a, FwdIt first)
      :  a_(a), first_(first)
   {}

   void uninitialized_copy_n_and_update(Iterator p, size_type n)
   {
      this->first_ = ::lslboost::container::uninitialized_copy_alloc_n_source(this->a_, this->first_, n, p);
   }

   void copy_n_and_update(Iterator p, size_type n)
   {
      this->first_ = ::lslboost::container::copy_n_source(this->first_, n, p);
   }

   A &a_;
   FwdIt first_;
};


template<class A, class Iterator>
struct insert_n_copies_proxy
{
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;

   insert_n_copies_proxy(A& a, const value_type &v)
      :  a_(a), v_(v)
   {}

   void uninitialized_copy_n_and_update(Iterator p, size_type n) const
   {  lslboost::container::uninitialized_fill_alloc_n(this->a_, v_, n, p);  }

   void copy_n_and_update(Iterator p, size_type n) const
   {  std::fill_n(p, n, v_);  }

   A &a_;
   const value_type &v_;
};

template<class A, class Iterator>
struct insert_value_initialized_n_proxy
{
   typedef ::lslboost::container::allocator_traits<A> alloc_traits;
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;


   explicit insert_value_initialized_n_proxy(A &a)
      :  a_(a)
   {}

   void uninitialized_copy_n_and_update(Iterator p, size_type n) const
   {  lslboost::container::uninitialized_value_init_alloc_n(this->a_, n, p);  }

   void copy_n_and_update(Iterator, size_type) const
   {
      BOOST_ASSERT(false);
   }

   private:
   A &a_;
};

template<class A, class Iterator>
struct insert_default_initialized_n_proxy
{
   typedef ::lslboost::container::allocator_traits<A> alloc_traits;
   typedef typename allocator_traits<A>::size_type size_type;
   typedef typename allocator_traits<A>::value_type value_type;


   explicit insert_default_initialized_n_proxy(A &a)
      :  a_(a)
   {}

   void uninitialized_copy_n_and_update(Iterator p, size_type n) const
   {  lslboost::container::uninitialized_default_init_alloc_n(this->a_, n, p);  }

   void copy_n_and_update(Iterator, size_type) const
   {
      BOOST_ASSERT(false);
   }

   private:
   A &a_;
};

template<class A, class Iterator>
struct insert_copy_proxy
{
   typedef lslboost::container::allocator_traits<A> alloc_traits;
   typedef typename alloc_traits::size_type size_type;
   typedef typename alloc_traits::value_type value_type;

   insert_copy_proxy(A& a, const value_type &v)
      :  a_(a), v_(v)
   {}

   void uninitialized_copy_n_and_update(Iterator p, size_type n) const
   {
      BOOST_ASSERT(n == 1);  (void)n;
      alloc_traits::construct( this->a_
                              , container_detail::to_raw_pointer(&*p)
                              , v_
                              );
   }

   void copy_n_and_update(Iterator p, size_type n) const
   {
      BOOST_ASSERT(n == 1);  (void)n;
      *p =v_;
   }

   A &a_;
   const value_type &v_;
};


template<class A, class Iterator>
struct insert_move_proxy
{
   typedef lslboost::container::allocator_traits<A> alloc_traits;
   typedef typename alloc_traits::size_type size_type;
   typedef typename alloc_traits::value_type value_type;

   insert_move_proxy(A& a, value_type &v)
      :  a_(a), v_(v)
   {}

   void uninitialized_copy_n_and_update(Iterator p, size_type n) const
   {
      BOOST_ASSERT(n == 1);  (void)n;
      alloc_traits::construct( this->a_
                              , container_detail::to_raw_pointer(&*p)
                              , ::lslboost::move(v_)
                              );
   }

   void copy_n_and_update(Iterator p, size_type n) const
   {
      BOOST_ASSERT(n == 1);  (void)n;
      *p = ::lslboost::move(v_);
   }

   A &a_;
   value_type &v_;
};

template<class It, class A>
insert_move_proxy<A, It> get_insert_value_proxy(A& a, BOOST_RV_REF(typename std::iterator_traits<It>::value_type) v)
{
   return insert_move_proxy<A, It>(a, v);
}

template<class It, class A>
insert_copy_proxy<A, It> get_insert_value_proxy(A& a, const typename std::iterator_traits<It>::value_type &v)
{
   return insert_copy_proxy<A, It>(a, v);
}

}}}   //namespace lslboost { namespace container { namespace container_detail {

#ifdef BOOST_CONTAINER_PERFECT_FORWARDING

#include <lslboost/container/detail/variadic_templates_tools.hpp>
#include <lslboost/move/utility.hpp>
#include <typeinfo>
//#include <iostream> //For debugging purposes

namespace lslboost {
namespace container {
namespace container_detail {

template<class A, class Iterator, class ...Args>
struct insert_non_movable_emplace_proxy
{
   typedef lslboost::container::allocator_traits<A>   alloc_traits;
   typedef typename alloc_traits::size_type        size_type;
   typedef typename alloc_traits::value_type       value_type;

   typedef typename build_number_seq<sizeof...(Args)>::type index_tuple_t;

   explicit insert_non_movable_emplace_proxy(A &a, Args&&... args)
      : a_(a), args_(args...)
   {}

   void uninitialized_copy_n_and_update(Iterator p, size_type n)
   {  this->priv_uninitialized_copy_some_and_update(index_tuple_t(), p, n);  }

   private:
   template<int ...IdxPack>
   void priv_uninitialized_copy_some_and_update(const index_tuple<IdxPack...>&, Iterator p, size_type n)
   {
      BOOST_ASSERT(n == 1); (void)n;
      alloc_traits::construct( this->a_
                              , container_detail::to_raw_pointer(&*p)
                              , ::lslboost::forward<Args>(get<IdxPack>(this->args_))...
                              );
   }

   protected:
   A &a_;
   tuple<Args&...> args_;
};

template<class A, class Iterator, class ...Args>
struct insert_emplace_proxy
   :  public insert_non_movable_emplace_proxy<A, Iterator, Args...>
{
   typedef insert_non_movable_emplace_proxy<A, Iterator, Args...> base_t;
   typedef lslboost::container::allocator_traits<A>   alloc_traits;
   typedef typename base_t::value_type             value_type;
   typedef typename base_t::size_type              size_type;
   typedef typename base_t::index_tuple_t          index_tuple_t;

   explicit insert_emplace_proxy(A &a, Args&&... args)
      : base_t(a, ::lslboost::forward<Args>(args)...)
   {}

   void copy_n_and_update(Iterator p, size_type n)
   {  this->priv_copy_some_and_update(index_tuple_t(), p, n);  }

   private:

   template<int ...IdxPack>
   void priv_copy_some_and_update(const index_tuple<IdxPack...>&, Iterator p, size_type n)
   {
      BOOST_ASSERT(n ==1); (void)n;
      aligned_storage<sizeof(value_type), alignment_of<value_type>::value> v;
      value_type *vp = static_cast<value_type *>(static_cast<void *>(&v));
      alloc_traits::construct(this->a_, vp,
         ::lslboost::forward<Args>(get<IdxPack>(this->args_))...);
      BOOST_TRY{
         *p = ::lslboost::move(*vp);
      }
      BOOST_CATCH(...){
         alloc_traits::destroy(this->a_, vp);
         BOOST_RETHROW
      }
      BOOST_CATCH_END
      alloc_traits::destroy(this->a_, vp);
   }
};

}}}   //namespace lslboost { namespace container { namespace container_detail {

#else //#ifdef BOOST_CONTAINER_PERFECT_FORWARDING

#include <lslboost/container/detail/preprocessor.hpp>
#include <lslboost/container/detail/value_init.hpp>

namespace lslboost {
namespace container {
namespace container_detail {

#define BOOST_PP_LOCAL_MACRO(N)                                                     \
template<class A, class Iterator BOOST_PP_ENUM_TRAILING_PARAMS(N, class P) >        \
struct BOOST_PP_CAT(insert_non_movable_emplace_proxy_arg, N)                        \
{                                                                                   \
   typedef lslboost::container::allocator_traits<A> alloc_traits;                      \
   typedef typename alloc_traits::size_type size_type;                              \
   typedef typename alloc_traits::value_type value_type;                            \
                                                                                    \
   BOOST_PP_CAT(insert_non_movable_emplace_proxy_arg, N)                            \
      ( A &a BOOST_PP_ENUM_TRAILING(N, BOOST_CONTAINER_PP_PARAM_LIST, _) )          \
      : a_(a)                                                                       \
      BOOST_PP_ENUM_TRAILING(N, BOOST_CONTAINER_PP_PARAM_INIT, _)                   \
   {}                                                                               \
                                                                                    \
   void uninitialized_copy_n_and_update(Iterator p, size_type n)                    \
   {                                                                                \
      BOOST_ASSERT(n == 1); (void)n;                                                \
      alloc_traits::construct                                                       \
         ( this->a_                                                                 \
         , container_detail::to_raw_pointer(&*p)                                    \
         BOOST_PP_ENUM_TRAILING(N, BOOST_CONTAINER_PP_MEMBER_FORWARD, _)            \
         );                                                                         \
   }                                                                                \
                                                                                    \
   void copy_n_and_update(Iterator, size_type)                                      \
   {  BOOST_ASSERT(false);   }                                                      \
                                                                                    \
   protected:                                                                       \
   A &a_;                                                                           \
   BOOST_PP_REPEAT(N, BOOST_CONTAINER_PP_PARAM_DEFINE, _)                           \
};                                                                                  \
                                                                                    \
template<class A, class Iterator BOOST_PP_ENUM_TRAILING_PARAMS(N, class P) >        \
struct BOOST_PP_CAT(insert_emplace_proxy_arg, N)                                    \
   : BOOST_PP_CAT(insert_non_movable_emplace_proxy_arg, N)                          \
         < A, Iterator BOOST_PP_ENUM_TRAILING_PARAMS(N, P) >                        \
{                                                                                   \
   typedef BOOST_PP_CAT(insert_non_movable_emplace_proxy_arg, N)                    \
         <A, Iterator BOOST_PP_ENUM_TRAILING_PARAMS(N, P) > base_t;                 \
   typedef typename base_t::value_type       value_type;                            \
   typedef typename base_t::size_type  size_type;                                   \
   typedef lslboost::container::allocator_traits<A> alloc_traits;                      \
                                                                                    \
   BOOST_PP_CAT(insert_emplace_proxy_arg, N)                                        \
      ( A &a BOOST_PP_ENUM_TRAILING(N, BOOST_CONTAINER_PP_PARAM_LIST, _) )          \
      : base_t(a BOOST_PP_ENUM_TRAILING(N, BOOST_CONTAINER_PP_PARAM_FORWARD, _) )   \
   {}                                                                               \
                                                                                    \
   void copy_n_and_update(Iterator p, size_type n)                                  \
   {                                                                                \
      BOOST_ASSERT(n == 1); (void)n;                                                \
      aligned_storage<sizeof(value_type), alignment_of<value_type>::value> v;       \
      value_type *vp = static_cast<value_type *>(static_cast<void *>(&v));          \
      alloc_traits::construct(this->a_, vp                                          \
         BOOST_PP_ENUM_TRAILING(N, BOOST_CONTAINER_PP_MEMBER_FORWARD, _));          \
      BOOST_TRY{                                                                    \
         *p = ::lslboost::move(*vp);                                                   \
      }                                                                             \
      BOOST_CATCH(...){                                                             \
         alloc_traits::destroy(this->a_, vp);                                       \
         BOOST_RETHROW                                                              \
      }                                                                             \
      BOOST_CATCH_END                                                               \
      alloc_traits::destroy(this->a_, vp);                                          \
   }                                                                                \
};                                                                                  \
//!
#define BOOST_PP_LOCAL_LIMITS (0, BOOST_CONTAINER_MAX_CONSTRUCTOR_PARAMETERS)
#include BOOST_PP_LOCAL_ITERATE()

}}}   //namespace lslboost { namespace container { namespace container_detail {

#endif   //#ifdef BOOST_CONTAINER_PERFECT_FORWARDING

#include <lslboost/container/detail/config_end.hpp>

#endif //#ifndef BOOST_CONTAINER_ADVANCED_INSERT_INT_HPP
