/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_ENABLE_IF_H
#define RTIREFLEX_ENABLE_IF_H

#include <tuple>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <array>
#include <type_traits>
#include <boost/range.hpp>
#include <boost/range/any_range.hpp>

#include <boost/fusion/sequence/intrinsic/at.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/optional.hpp>

#ifdef RTI_WIN32
#include <stdint.h>
#endif

#if ((RTI_DDS_VERSION_MAJOR==5) && (RTI_DDS_VERSION_MINOR==1))

#ifndef REFLEX_STATIC_STRING_BOUND
  #define REFLEX_STATIC_STRING_BOUND 256
#endif

#ifndef REFLEX_STATIC_SEQUENCE_BOUND
  #define REFLEX_STATIC_SEQUENCE_BOUND 256
#endif

#elif ((RTI_DDS_VERSION_MAJOR==5) && (RTI_DDS_VERSION_MINOR==2))

#ifndef REFLEX_STATIC_STRING_BOUND
  #define REFLEX_STATIC_STRING_BOUND 256
#endif

#ifndef REFLEX_STATIC_SEQUENCE_BOUND
  #define REFLEX_STATIC_SEQUENCE_BOUND 256
#endif

#endif // RTI_DDS_VERSION_MAJOR



namespace reflex {

  namespace match {

    typedef unsigned char octet_t;

    template <class TagType, class... Cases>
    struct Union;

    template <class... T>
    struct Sparse;

#ifdef RTI_WIN32

  template <class T>
  struct Range
    : boost::any_range <
        T,
        boost::forward_traversal_tag,
        T, std::ptrdiff_t >
  {
    template <class... U>
    Range(U && ... u)
      : boost::any_range <
          T,
          boost::forward_traversal_tag,
          T, std::ptrdiff_t >(std::forward<U>(u)...)
    {}
  };

#else

  template <class T>
  using Range = 
    boost::any_range<T, 
    boost::forward_traversal_tag, 
    T, std::ptrdiff_t>;

#endif

    template <class T, size_t Bound>
    struct BoundedRange : Range<T>
    {
      template <class... U>
      BoundedRange(U&&... u)
        : Range<T>(std::forward<U>(u)...)
      {}
    };

  } // namespace match

  /**
  * @brief Includes C++ compile-time meta-programming facilities
  */
  namespace meta {

    struct true_type {
      enum { value = true };
    };

    struct false_type {
      enum { value = false };
    };

  } // namespace meta

  /**
  * @brief RefleX macros in \link adapt_macros.h \endlink expand static 
  *        information of user-defined types to this namespace.
  */
  namespace codegen {

    template <class T>
    struct EnumDef
    {
      enum { is_enum = 0 };
    };

    /**
     * @brief Casts integers to user-defined enumeration types.
     *
     *        C++ does not directly allow casting from integers to 
     *        user-defined enumerations. A static_cast must be used.
     *        The default implementation of this function uses
     *        a static_cast. It may not be sufficient in some cases.
     *
     *        In cases where the generated enumeration is more than
     *        the native C++ enumeration (such as a class), the 
     *        enum_cast function must overloaded explicitly to support
     *        conversion from integers to the specialized implementation
     *        of enumeration types. This function must be overloaded in 
     *        the namespace that declares the enumeration.
     *
     *        For a complete example please see $REFLEXHOME/test/osacbm/driver.cxx
     */
    template <class T>
    void enum_cast(T & dst, DDS_Long src)
    {
      dst = static_cast<T>(src); // cast required for enums         
    }


  } // namespace codegen


  /**
  * @brief Contains customizable type traits for user-defined types
  * 
  *        Type-traits for standard library container/optional types 
  *        and select boost types are already provided. For third-party 
  *        container/optional types, the respective type traits templates
  *        can be customized. 
  *
  *        For instance, if thirdparty::base64_binary is a container of 
  *        octets in the thirdparty namespace, such a container can be
  *        easily adapted using the following.
  *
  *        \code{.cpp}
  *        namespace reflex { namespace type_traits {
  *          template <> 
  *          struct is_container<thirdparty::base64_binary> 
  *                   : reflex::meta::true_type 
  *          { };
  *
  *          template <>
  *          struct container_traits<thirdparty::base64_binary>
  *          {
  *            typedef char value_type;
  *            typedef char * iterator;
  *          };
  *
  *        } } // namespace reflex::type_traits
  *        \endcode
  *
  *        The is_container type trait indicates that a certain type should be 
  *        treated as a container type. It will map to sequence typecode.
  *        Adaptable containers must export input-iterator like interface including
  *        the relevant traits. For instance, thirdparty::base64_binary exports
  *        value_type and iterator using container_traits specialization below.
  *        The container_traits specialization may not be always needed.
  *
  *        For a more complete example, please see $REFLEXHOME/include/reflex/traits/xsd_traits.h
  *
  */
  namespace type_traits {

    using reflex::meta::false_type;
    using reflex::meta::true_type;

    template <class T>
    struct is_bool : false_type {};

    template <>
    struct is_bool<bool> : true_type{};

    template <class T>
    struct is_primitive : false_type {}; // enum

    template <> struct is_primitive<match::octet_t> : true_type{};
    template <> struct is_primitive<bool>           : true_type{};
    template <> struct is_primitive<char>           : true_type{};
    template <> struct is_primitive<signed char>    : true_type{};
    template <> struct is_primitive<int16_t>        : true_type{};
    template <> struct is_primitive<uint16_t>       : true_type{};
    template <> struct is_primitive<int32_t>        : true_type{};
    template <> struct is_primitive<uint32_t>       : true_type{};
    template <> struct is_primitive<int64_t>        : true_type{};
    template <> struct is_primitive<uint64_t>       : true_type{};
    template <> struct is_primitive<float>          : true_type{};
    template <> struct is_primitive<double>         : true_type{};
    template <> struct is_primitive<long double>    : true_type{};
#ifndef RTI_WIN32
    template <> struct is_primitive<char32_t>       : true_type{};
#endif
#if __x86_64__
    template <> struct is_primitive<long long>          : true_type{};
    template <> struct is_primitive<unsigned long long> : true_type{};
#endif 

    template <class... Args>
    struct is_primitive<std::tuple<Args...>> : false_type{};

    template <>
    struct is_primitive<std::string> : false_type{};

    template <class T>
    struct is_primitive<std::vector<T>> : false_type{};

    template <class TagType, class... Cases>
    struct is_primitive<reflex::match::Union<TagType, Cases...>> : false_type{};

    template <class T>
    struct is_enum
    {
      enum { value = reflex::codegen::EnumDef<T>::is_enum };
    };

    template <class T>
    struct is_primitive_or_enum
    {
      enum { value = is_primitive<T>::value || is_enum<T>::value };
    };

    template <class T>
    struct is_bool_or_enum
    {
      enum { value = is_bool<T>::value || is_enum<T>::value };
    };

    template <class T>
    struct is_primitive_and_not_bool
    {
      enum { value = is_primitive<T>::value && !is_bool<T>::value };
    };

    template <class T>
    struct is_vector : false_type {};

    template <class T>
    struct is_vector<std::vector<T>> : true_type{};

    template <class T>
    struct is_vector<const std::vector<T>> : true_type{};

    template <class T>
    struct is_string : false_type {};

    template <>
    struct is_string<std::string> : true_type{};

    template <>
    struct is_string<const std::string> : true_type{};

    template <class T>
    struct is_tuple : false_type {};

    template <typename... Args>
    struct is_tuple<std::tuple<Args...>> : true_type{};

    template <typename... Args>
    struct is_tuple<const std::tuple<Args...>> : true_type{};

    template <typename T>
    struct is_stdarray : false_type {};

    template <typename T, size_t N>
    struct is_stdarray<std::array<T, N>> : true_type{};

    template <typename T, size_t N>
    struct is_stdarray<const std::array<T, N>> : true_type{};

    template <class C>
    struct is_stdset : false_type {};

    template <class T, class Comp, class Alloc>
    struct is_stdset<std::set<T, Comp, Alloc>> : true_type{};

    template <class T, class Comp, class Alloc>
    struct is_stdset<const std::set<T, Comp, Alloc>> : true_type{};

    template <class C>
    struct is_stdmap : false_type {};

    template <class Key, class T, class Comp, class Alloc>
    struct is_stdmap<std::map<Key, T, Comp, Alloc>> : true_type{};

    template <class Key, class T, class Comp, class Alloc>
    struct is_stdmap<const std::map<Key, T, Comp, Alloc>> : true_type{};

    template <class T>
    struct is_builtin_array : false_type {};

    template <class T, size_t Dim>
    struct is_builtin_array<T[Dim]> : true_type{};

    template <class T, size_t Dim>
    struct is_builtin_array<const T[Dim]> : true_type{};

    template <class T>
    struct is_builtin_array<T []> : true_type{};

    template <class T>
    struct is_builtin_array<const T []> : true_type{};

    template <class T>
    struct is_pointer : false_type {};

    template <class T>
    struct is_pointer<T *> : true_type{};

    template <class T>
    struct is_pointer<const T *> : true_type{};

    template <class T>
    struct is_container : false_type {};

    template <class T, class Alloc>
    struct is_container<std::vector<T, Alloc>> : true_type{};

    template <class T, class Alloc>
    struct is_container<const std::vector<T, Alloc>> : true_type{};

    template <class T, class Alloc>
    struct is_container<std::list<T, Alloc>> : true_type{};

    template <class T, class Alloc>
    struct is_container<const std::list<T, Alloc>> : true_type{};

    template <class Key, class Comp, class Alloc>
    struct is_container<std::set<Key, Comp, Alloc>> : true_type{};

    template <class Key, class Comp, class Alloc>
    struct is_container<const std::set<Key, Comp, Alloc>> : true_type{};

    template <class Key, class T, class Comp, class Alloc>
    struct is_container<std::map<Key, T, Comp, Alloc>> : true_type{};

    template <class Key, class T, class Comp, class Alloc>
    struct is_container<const std::map<Key, T, Comp, Alloc>> : true_type{};

    template <class T>
    struct is_union : false_type {};

    template <class TagType, class... Args>
    struct is_union<reflex::match::Union<TagType, Args...>> : true_type{};

    template <class TagType, class... Args>
    struct is_union<const reflex::match::Union<TagType, Args...>> : true_type{};

    template <class T>
    struct is_sparse : false_type {};

    template <class... Args>
    struct is_sparse<reflex::match::Sparse<Args...>> : true_type{};

    template <class... Args>
    struct is_sparse<const reflex::match::Sparse<Args...>> : true_type{};

    template <class T>
    struct is_range : false_type {};

    template <class T>
    struct is_range<reflex::match::Range<T>> : true_type{};

    template <class T>
    struct is_range<const reflex::match::Range<T>> : true_type{};

    template <class T, size_t N>
    struct is_range<reflex::match::BoundedRange<T, N>> : true_type{};

    template <class T, size_t N>
    struct is_range<const reflex::match::BoundedRange<T, N>> : true_type{};

    template <class T>
    struct is_optional : false_type {};

#ifdef RTI_WIN32
    template <class... T>
    struct is_optional<boost::optional<T...>> : true_type{};
#else
    template <class T>
    struct is_optional<boost::optional<T>> : true_type{};
#endif

    template <class C, bool>
    struct container_traits_impl;

    template <class C>
    struct container_traits_impl<C, true>
    {
      typedef typename C::value_type value_type;
      typedef typename C::iterator iterator;
    };

    template <class C>
    struct container_traits_impl<C, false>
    {
      typedef void value_type;
      typedef void iterator;
    };

    template <class C>
    struct container_traits
    {
      typedef typename
        container_traits_impl<
        C,
        is_container<C>::value>::value_type
        value_type;

      typedef typename
        container_traits_impl<
        C,
        is_container<C>::value>::iterator
        iterator;
    };

    template <class T>
    struct optional_traits;

    template <class T> struct is_char_ptr              : false_type {};
    template <> struct is_char_ptr<char *>             : true_type{};
    template <> struct is_char_ptr<const char *>       : true_type{};
    template <> struct is_char_ptr<char * const>       : true_type{};
    template <> struct is_char_ptr<const char * const> : true_type{};

    template <class T>
    struct inheritance_traits {
      typedef reflex::meta::false_type has_base;
    };

    struct default_bound { };

    template <class T, class Parent = void, int Index = -1>
    struct static_string_bound : default_bound
    {
      static const unsigned int value = REFLEX_STATIC_STRING_BOUND;
    };

    template <class T, class Parent = void, int Index = -1>
    struct static_container_bound : default_bound
    {
      static const unsigned int value = REFLEX_STATIC_SEQUENCE_BOUND;
    };

    /*template <class T>
    struct is_default_member_names {
    enum { value = std::is_base_of<default_bound, T>::value };
    };*/
  } // namespace type_traits

  namespace meta {

    template <bool, class T = void>
    struct enable_if {
      typedef T type;
    };

    template <class T>
    struct enable_if<false, T> {
    };

    template <bool, class T = void>
    struct disable_if {
      typedef T type;
    };

    template <class T>
    struct disable_if<true, T> {};

    template <bool, class T = void>
    struct disable_if_lazy {
      typedef typename T::type type;
    };

    template <class T>
    struct disable_if_lazy<true, T> {};

    template <size_t... Dims>
    struct dim_list {
      enum { size = sizeof...(Dims) };
    };

    template <size_t Arg, class DimList>
    struct dim_cat;

    template <size_t Head, size_t... Args>
    struct dim_cat<Head, dim_list<Args...>> {
      typedef dim_list<Head, Args...> type;
    };

    template <size_t Head>
    struct dim_cat<Head, dim_list<0>> {
      typedef dim_list<Head> type;
    };

    template <class>
    struct make_dim_list {
      typedef dim_list<0> type;
    };

    template <class T>
    struct make_dim_list<T []>;

    struct Array_Dimensions_Must_Be_Greater_Than_Zero {};

    template <class T>
    struct make_dim_list<T[0]> {
      typedef Array_Dimensions_Must_Be_Greater_Than_Zero type;
    };

    template <class T>
    struct make_dim_list<std::array<T, 0>> {
      typedef Array_Dimensions_Must_Be_Greater_Than_Zero type;
    };

    template <class T, size_t Dim>
    struct make_dim_list<T[Dim]> {
      typedef typename
        dim_cat<Dim,
        typename make_dim_list<T>::type>::type type;
    };

    template <class T, size_t Dim>
    struct make_dim_list<std::array<T, Dim>> {
      typedef typename
        dim_cat<Dim,
        typename make_dim_list<T>::type>::type type;
    };

    template <class T>
    struct remove_reference {
      typedef T type;
    };

    template <class T>
    struct remove_reference<T &> {
      typedef T type;
    };

    template <class T>
    struct remove_const {
      typedef T type;
    };

    template <class T>
    struct remove_const<const T> {
      typedef T type;
    };

    template<class T>
    struct remove_all_extents {
      typedef T type;
    };

    template<class T>
    struct remove_all_extents<T []> {
      typedef typename remove_all_extents<T>::type type;
    };

    template<class T, size_t N>
    struct remove_all_extents<T[N]> {
      typedef typename remove_all_extents<T>::type type;
    };

    template <class T, size_t N>
    struct remove_all_extents<std::array<T, N>> {
      typedef typename remove_all_extents<T>::type type;
    };

    template <class T>
    struct remove_pointer;

    template <class T>
    struct remove_pointer <T *>
    {
      typedef T type;
    };

    template <class T, class... U>
    struct packhead {
      typedef T type;
    };

    template <class T>
    struct packhead<T> {
      typedef T type;
    };

    template <size_t Head, size_t... Tail>
    struct multiply {
      static const size_t value =
        Head * multiply<Tail...>::value;
    };

    template <size_t N>
    struct multiply<N> {
      static const uint64_t value = N;
    };

    template <class>
    struct dim_list_multiply;

    template <size_t... Dims>
    struct dim_list_multiply<dim_list<Dims...>> {
      static const uint64_t value = multiply<Dims...>::value;
    };

    template <class FusionSeq, size_t I>
    struct at
    {
      typedef typename
        boost::fusion::result_of::at_c<FusionSeq, I>::type type;
    };

    template <class... Args, size_t I>
    struct at<std::tuple<Args...>, I>
    {
      typedef typename std::tuple_element<I, std::tuple<Args...>>::type type;
    };

    template <class... Args, size_t I>
    struct at<const std::tuple<Args...>, I>
    {
      // This is crazy!: I can't pass const tuple to std::tuple_element.
      // I've to attach const to the result of tuple_element.
      // This is unlike Boost Fusion Sequences up above.
      //
      // I'm not sure if this will even work for references
      // because const references is redundant because references never change.
      typedef typename
        std::tuple_element<I, std::tuple<Args...>>::type const type;
    };

    template <class First, class Second, size_t I>
    struct at<std::pair<First, Second>, I>
    {
      typedef typename
        std::tuple_element<I, std::pair<First, Second>>::type type;
    };

    template <class First, class Second, size_t I>
    struct at<const std::pair<First, Second>, I>
    {
      typedef typename
        std::tuple_element<I, std::pair<First, Second>>::type const type;
    };

    template <class FusionSeq>
    struct size
    {
      enum {
        value =
        boost::fusion::result_of::size<FusionSeq>::type::value
      };
    };

    template <class First, class Second>
    struct size<std::pair<First, Second>>
    {
      enum { value = 2 };
    };

    template <class First, class Second>
    struct size<const std::pair<First, Second>>
    {
      enum { value = 2 };
    };

    template <class... Args>
    struct size<std::tuple<Args...>>
    {
      enum { value = std::tuple_size<std::tuple<Args...>>::value };
    };

    template <class... Args>
    struct size<const std::tuple<Args...>>
    {
      enum { value = std::tuple_size<const std::tuple<Args...>>::value };
    };

    template <size_t I, class FusionSeq>
    typename disable_if_lazy <reflex::type_traits::is_tuple<FusionSeq>::value,
      at<FusionSeq, I >> ::type
      get(FusionSeq & seq)
    {
      return boost::fusion::at_c<I>(seq);
    }

    template <size_t I, class FusionSeq>
    typename disable_if_lazy <reflex::type_traits::is_tuple<FusionSeq>::value,
      at<const FusionSeq, I >> ::type
      get(const FusionSeq & seq)
    {
      return boost::fusion::at_c<I>(seq);
    }

    template <size_t I, class... Args>
    // Note reference (ref) at the end.
    typename at<std::tuple<Args...>, I>::type &
      get(std::tuple<Args...> & tuple)
    {
      return std::get<I>(tuple);
    }

    template <size_t I, class... Args>
    // Note reference (ref) at the end.
    typename at<const std::tuple<Args...>, I>::type &
      get(const std::tuple<Args...> & tuple)
    {
      return std::get<I>(tuple);
    }

    template <size_t I, class First, class Second>
    // Note reference (ref) at the end.
    typename at<std::pair<First, Second>, I>::type &
      get(std::pair<First, Second> & pair)
    {
      return std::get<I>(pair);
    }

    template <size_t I, class First, class Second>
    // Note reference (ref) at the end.
    typename at<const std::pair<First, Second>, I>::type &
      get(const std::pair<First, Second> & pair)
    {
      return std::get<I>(pair);
    }

    template <class T>
    struct remove_refs
    {
      typedef T type;
    };

    template <class... Args>
    struct remove_refs<std::tuple<Args...>>
    {
      typedef std::tuple<typename remove_reference<Args>::type...> type;
    };

    template <class... Args>
    struct remove_refs<const std::tuple<Args...>>
    {
      typedef const std::tuple<typename remove_reference<Args>::type...> type;
    };


  } // namespace meta

  namespace match {

    template <class T, size_t I, size_t... J>
    struct MultiDimArray
    {
      // using Nested = typename MultiDimArray<T, J...>::type;
      typedef typename MultiDimArray<T, J...>::type Nested;
      // using type = std::array<Nested, I>;
      typedef std::array<Nested, I> type;
    };

    template <class T, size_t I>
    struct MultiDimArray<T, I>
    {
      // using type = std::array<T, I>;
      typedef std::array<T, I> type;
    };

  } // namespace match
} // namespace reflex

#endif // RTIREFLEX_ENABLE_IF_H

