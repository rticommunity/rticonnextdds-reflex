/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_MEMBERWISE_H
#define RTIREFLEX_MEMBERWISE_H

#include <vector>
#include <type_traits>

#include "ndds/ndds_cpp.h"
#include "reflex/dd_extra.h"
#include "reflex/enable_if.h"

namespace reflex {
  namespace detail {

    template <class T> struct is_char_ptr : false_type {};
    template <> struct is_char_ptr<char *> : true_type{};
    template <> struct is_char_ptr<const char *> : true_type{};
    template <> struct is_char_ptr<char * const> : true_type{};
    template <> struct is_char_ptr<const char * const> : true_type{};

    struct DefaultMemberNames;

    template <int... I> struct Indices;
    //template <class TagType, class... Cases> struct Union;
    //template <class... Args> struct Sparse;

    template <class T>
    void set_member_by_index(DDS_DynamicData &instance,
      unsigned index,
      const T & val)
    {
      // set_member_value<DefaultMemberNames, 0>(instance, MemberAccess::BY_ID(index + 1), val);
    }

    template <class T>
    void set_member_by_name(DDS_DynamicData &instance,
      const char * name,
      const T & val)
    {
      // set_member_value<DefaultMemberNames, 0>(instance, MemberAccess::BY_NAME(name), val);
    }

    template <class T>
    void recurse_and_set(DDS_DynamicData &outer,
      const std::vector<int> & bind_points,
      unsigned i,
      const T & val)
    {
      if (i < bind_points.size() - 1)
      {
        DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
        SafeBinder binder(outer, inner, MemberAccess::BY_ID(bind_points[i] + 1));
        recurse_and_set(inner, bind_points, i + 1, val);
      }
      else
        set_member_by_index(outer, bind_points[i], val);
    }

    template <class T>
    void recurse_and_set(DDS_DynamicData &outer,
      const std::vector<const char *> & bind_points,
      unsigned i,
      const T & val)
    {
      if (i < bind_points.size() - 1)
      {
        DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
        SafeBinder binder(outer, inner, MemberAccess::BY_NAME(bind_points[i]));
        recurse_and_set(inner, bind_points, i + 1, val);
      }
      else
        set_member_by_name(outer, bind_points[i], val);
    }

    template <class T>
    void set_member_by_indices(DDS_DynamicData &instance,
      std::vector<int> indices,
      const T & val)
    {
      recurse_and_set(instance, indices, 0, val);
    }

    template <class T>
    void set_member_by_names(DDS_DynamicData &instance,
      std::vector<const char *> names,
      const T & val)
    {
      recurse_and_set(instance, names, 0, val);
    }

    template <class T>
    void set_member(DDS_DynamicData &instance,
      const char *expr,
      const T & val)
    {

    }

    struct Too_Many_Indices;

    template <class T, class Indices>
    struct GetNestedType
    {
      typedef Too_Many_Indices type;
    };

    template <class... Args, int Head, int... Tail>
    struct GetNestedType<std::tuple<Args...>, Indices<Head, Tail...>>
    {
      typedef typename std::tuple_element<Head, std::tuple<Args...>>::type TE;
      typedef typename GetNestedType<TE, Indices<Tail...>>::type type;
    };

    template <class... Args, int Head>
    struct GetNestedType<std::tuple<Args...>, Indices<Head>>
    {
      typedef typename std::tuple_element<Head, std::tuple<Args...>>::type type;
    };

    template <class TagType, class... Args, int Head, int... Tail>
    struct GetNestedType<match::Union<TagType, Args...>, Indices<Head, Tail...>>
    {
      typedef typename match::Union<TagType, Args...>::case_tuple_type CaseTuple;
      typedef typename std::tuple_element<Head, CaseTuple>::type Case;
      typedef typename GetNestedType<typename Case::type, Indices<Tail...>>::type type;
    };

    template <class TagType, class... Args, int Head>
    struct GetNestedType<match::Union<TagType, Args...>, Indices<Head>>
    {
      typedef typename match::Union<TagType, Args...>::case_tuple_type CaseTuple;
      typedef typename std::tuple_element<Head, CaseTuple>::type Case;
      typedef typename Case::type type;
    };

    template <class... Args, int Head, int... Tail>
    struct GetNestedType<match::Sparse<Args...>, Indices<Head, Tail...>>
    {
      typedef typename match::Sparse<Args...>::raw_tuple_type RawTuple;
      typedef typename std::tuple_element<Head, RawTuple>::type Raw;
      typedef typename GetNestedType<Raw, Indices<Tail...>>::type type;
    };

    template <class... Args, int Head>
    struct GetNestedType<match::Sparse<Args...>, Indices<Head>>
    {
      typedef typename match::Sparse<Args...>::raw_tuple_type RawTuple;
      typedef typename std::tuple_element<Head, RawTuple>::type type;
    };

    template <class T, int Head, int... Tail>
    struct GetNestedType<std::vector<T>, Indices<Head, Tail...>>
    {
      typedef typename GetNestedType<T, Indices<Tail...>>::type type;
    };

    template <class T, int Head>
    struct GetNestedType<std::vector<T>, Indices<Head>>
    {
      typedef T type;
    };

    template <class T, class Tuple, class Indices>
    struct TraverseTupleByIndices;

    template <class T, class Tuple, int Head, int... Tail>
    struct TraverseTupleByIndices<T, Tuple, Indices<Head, Tail...>>
    {
      static void exec(DDS_DynamicData & outer, const T & val)
      {
        std::cerr << "binding " << Head << "\n";
        DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
        SafeBinder binder(outer, inner, MemberAccess::BY_ID(Head + 1));
        TraverseTupleByIndices<T, Tuple, Indices<Tail...>>::exec(inner, val);
        std::cerr << "unbinding\n";
      }
    };

    template <class T, class Tuple, int Last>
    struct TraverseTupleByIndices<T, Tuple, Indices<Last>>
    {
      static void exec(DDS_DynamicData & outer, const T & val)
      {
        std::cerr << "set_member_by_index Last = " << Last;
        set_member_by_index(outer, Last, val);
        std::cerr << "set_member_by_index Last = " << Last;
      }
    };

    template <class T, class Tuple, int Head, int... I>
    void set_member_by_indices(DDS_DynamicData &instance,
      const T & val)
    {
      // This functionality is buggy in case of unions. especially default cases.
      typedef typename GetNestedType<Tuple, Indices<Head, I...>>::type Nested;
      static_assert(std::is_same<Nested, T>::value, "The types don't match");
      TraverseTupleByIndices<T, Tuple, Indices<Head, I...>>::exec(instance, val);
    }

  } // namespace detail

} // namespace reflex

#endif // RTIREFLEX_MEMBERWISE_H


