/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_MEMBER_NAMES_H
#define RTIREFLEX_MEMBER_NAMES_H

#include <string>
#include <typeinfo>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "ndds/ndds_cpp.h"

#include "reflex/dllexport.h"
#include "reflex/enable_if.h"
#include "reflex/dd_extra.h"
#include "reflex/adapt_macros.h"

namespace reflex {

  namespace detail {

    REFLEX_DLL_EXPORT std::string remove_parenthesis(std::string);

  } // namespace detail

  namespace match {

    template <class... Args>
    struct Sparse;

    template <class TagType, class... Cases>
    struct Union;
  } // namespace match

  namespace codegen 
  {
    struct MemberInfo
    {
      std::string name;
      unsigned char value;

      MemberInfo();
      MemberInfo(const std::string & n, unsigned char v);
    };
  } // namespace codegen

  namespace detail {

    struct DefaultMemberNames
    {
      REFLEX_DLL_EXPORT static reflex::codegen::MemberInfo get_member_info(int i);
      REFLEX_DLL_EXPORT static const char * basename(const char *);
      REFLEX_DLL_EXPORT static std::string type_name(const char * prefix);
      REFLEX_DLL_EXPORT static std::string demangle(const char* name);
    };

  } // namespace detail

  namespace codegen 
  {

    // specialize this trait for every user-defined type
    template <class T, unsigned I>
    struct MemberTraits
    {
      static MemberInfo member_info() {
        return reflex::detail::DefaultMemberNames::get_member_info(I);
      }
    };

    template <class First, class Second>
    struct MemberTraits<std::pair<First, Second>, 0>
    {
      static MemberInfo member_info() {
        return MemberInfo("first", DDS_TYPECODE_NONKEY_REQUIRED_MEMBER);
      }
    };

    template <class First, class Second>
    struct MemberTraits<std::pair<First, Second>, 1>
    {
      static MemberInfo member_info() {
        return MemberInfo("second", DDS_TYPECODE_NONKEY_REQUIRED_MEMBER);
      }
    };


    template <class T>
    struct StructName
    {
      static std::string get()
      {
#ifdef HAVE_CXA_DEMANGLE
        return reflex::detail::DefaultMemberNames::demangle(typeid(T).name());
#else
        return reflex::detail::DefaultMemberNames::type_name("DefaultTypeName");
#endif
      }
    };

    template <class First, class Second>
    struct StructName<std::pair<First, Second>>
    {
      static std::string get()
      {
        return reflex::detail::DefaultMemberNames::type_name("DefaultPairTypeName");
      }
    };

    template <class... Args>
    struct StructName<match::Sparse<Args...>>
    {
      static std::string get()
      {
        return reflex::detail::DefaultMemberNames::type_name("DefaultSparseTypeName");
      }
    };

    template <class TagType, class... Cases>
    struct StructName<match::Union<TagType, Cases...>>
    {
      static std::string get()
      {
        return reflex::detail::DefaultMemberNames::type_name("DefaultUnionName");
      }
    };

    template <class... Args>
    struct StructName<std::tuple<Args...>>
    {
      static std::string get()
      {
        return reflex::detail::DefaultMemberNames::type_name("DefaultTupleName");
      }
    };

  } // namespace codegen


} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/member_names.cxx"
#endif

#endif // RTIREFLEX_MEMBER_NAMES_H

