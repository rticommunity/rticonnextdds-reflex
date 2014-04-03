/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DEFAULT_MEMBER_NAMES_H
#define RTIREFLEX_DEFAULT_MEMBER_NAMES_H

#include "ndds/ndds_cpp.h"
#include "dllexport.h"

#include <string>
#include <typeinfo>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "dd_extra.h"

template <class Enum>
struct EnumDef;

struct MemberInfo
{
   std::string name;
   unsigned char value;
   
   MemberInfo() {}

   MemberInfo(std::string n,
              unsigned char v)
     : name(remove_parenthesis(n)), value(v)
   {}
};

struct DefaultMemberNames
{
  DllExport static const MemberInfo members [];

  DllExport static const char * basename(const char *);
  DllExport static std::string type_name(const char * prefix);
  DllExport static std::string demangle(const char* name);
};

template <class T>
struct StructName 
{ 
  static std::string get() 
  {
    #ifdef HAVE_CXA_DEMANGLE
      return DefaultMemberNames::demangle(typeid(T).name());
    #else
      return DefaultMemberNames::type_name("DefaultTypeName");
    #endif
  }
};

// specialize this trait for every user-defined type
template <class T, unsigned I>
struct MemberTraits
{
  static MemberInfo member_info() {
    return DefaultMemberNames::members[I];
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

template <class... Args>
struct Sparse;

template <class TagType, class... Cases>
struct Union;

template <class First, class Second>
struct StructName<std::pair<First, Second>>
{
  static std::string get()
  {
    return DefaultMemberNames::type_name("DefaultPairTypeName");
  }
};

template <class... Args>
struct StructName<Sparse<Args...>>
{
  static std::string get()
  {
    return DefaultMemberNames::type_name("DefaultSparseTypeName");
  }
};

template <class TagType, class... Cases>
struct StructName<Union<TagType, Cases...>>
{ 
  static std::string get() 
  {
    return DefaultMemberNames::type_name("DefaultUnionName");
  }
};

template <class... Args>
struct StructName<std::tuple<Args...>>
{ 
  static std::string get() 
  {
    return DefaultMemberNames::type_name("DefaultTupleName");
  }
};

#define ENUM_DEF(EnumType, Name, Size)        \
template <>                                   \
struct EnumDef<EnumType> {                    \
  static const char *name() { return Name; }  \
  static const unsigned int size = Size;      \
                                              \
  template <unsigned Index>                   \
  struct EnumMember {                         \
    static MemberInfo info();                 \
  };                                          \
};

#define ENUM_MEMBER_DEF(EnumType, Index, Name, Ordinal)  \
template<>                                               \
inline MemberInfo                                        \
EnumDef<EnumType>::EnumMember<Index>::info() {           \
  return MemberInfo(Name,  Ordinal);                     \
}

#define STRUCT_NAME_DEF_INTERNAL(FullyQualifiedType)            \
template <>                                                     \
struct StructName<FullyQualifiedType> {                         \
  static std::string get()                                      \
  { return DefaultMemberNames::basename(#FullyQualifiedType); } \
};

#define STRUCT_NAME_DEF_CUSTOM(FullyQualifiedType, Name)        \
template <>                                                     \
struct StructName<FullyQualifiedType> {                         \
  static std::string get() { return Name; }                     \
};

#define MEMBER_TRAITS_DEF_CUSTOM(FullyQualifiedType, I, Name, Flags)   \
template <>                                                            \
struct MemberTraits<FullyQualifiedType, I> {                           \
  static MemberInfo member_info() {                                    \
    return MemberInfo(Name, Flags);                                    \
  }                                                                    \
};

// The parenthesis below are essential for the RTI_DUMMY macro.
#define _OPTIONAL    (DDS_TYPECODE_NONKEY_MEMBER)
#define _KEY         (DDS_TYPECODE_KEY_MEMBER)
#define _REQUIRED    (DDS_TYPECODE_NONKEY_REQUIRED_MEMBER)

#ifdef RTI_WIN32
  #define RTI_DUMMY(X) X
  #define RTI_DUMMY_VAR   DDS_Octet RTI_DUMMY=_REQUIRED;

  #define RTI_GET_NAME_FLAGS(Type, Name, ...)   \
    #Name, RTI_DUMMY ## __VA_ARGS__
#else
  // Not needed outside Visual Studio
  #define RTI_DUMMY_VAR

  #define WITH_FLAG(Time, Name, Flag)  #Name, Flag
  #define WITHOUT_FLAG(Time, Name)     #Name, _REQUIRED

  #define PUSH_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4

  // A pair    will push out WITHOUT_FLAGS
  // A triplet will push out WITH_FLAGS
  #define FLAGS_CHOOSER(...) PUSH_4TH_ARG(__VA_ARGS__, WITH_FLAG, WITHOUT_FLAG)

  // __VA_ARGS__ is either a pair or a triplet.
  #define RTI_GET_NAME_FLAGS(...)            \
    FLAGS_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#endif

#define RTI_GET_NAME_ORDINAL(Name, Ordinal) #Name, Ordinal

// Note that Attr is already has parenthesis.
// Attr is either a pair or a triplet
// RTI_GET_NAME_FLAGS macro eats the parenthesis around Attr.
// RTI_DUMMY_VAR is used on for Win32 only
#define RTI_MEMBER_INFO_INTERNAL(R,Data,I,Attr)     \
template<>                                          \
MemberInfo MemberTraits<Data, I>::member_info() {   \
    RTI_DUMMY_VAR                                   \
    return MemberInfo(RTI_GET_NAME_FLAGS Attr);     \
}                                                   \

#define RTI_FILLER_0(...) ((__VA_ARGS__)) RTI_FILLER_1
#define RTI_FILLER_1(...) ((__VA_ARGS__)) RTI_FILLER_0
#define RTI_FILLER_0_END
#define RTI_FILLER_1_END

// Convert (a,1,A)(b,2,B)(c,3,C) into ((a,1,A))((b,2,B))((c,3,C))
// Otherwise preprocessor will get confused due to commas.
// The PARENTHESIZE macro expands as follows:
//
// RTI_FILLER_0 Attr                                               ---->
// RTI_FILLER_0 (a,1,A)(b,2,B)(c,3,C)                              ---->
// ((a,1,A)) RTI_FILLER_1 (b,2,B)(c,3,C)                           ---->
// ((a,1,A)) ((b,2,B)) RTI_FILLER_0 (c,3,C)                        ---->
// ((a,1,A)) ((b,2,B)) ((c,3,C)) RTI_FILLER_1                      ---->
// BOOST_PP_CAT(((a,1,A)) ((b,2,B)) ((c,3,C)) RTI_FILLER_1, _END)  ---->
// ((a,1,A)) ((b,2,B)) ((c,3,C)) RTI_FILLER_1_END                  ---->
// ((a,1,A)) ((b,2,B)) ((c,3,C))                                   ---->
// voila!!!

#define RTI_PARENTHESIZE(Attr) BOOST_PP_CAT(RTI_FILLER_0 Attr,_END)

#define RTI_MEMBER_TRAITS(Name,Attributes) \
  BOOST_PP_SEQ_FOR_EACH_I(RTI_MEMBER_INFO_INTERNAL,Name,RTI_PARENTHESIZE(Attributes))

#define RTI_SKIP_LAST_0(X, Y, ...) (X, Y) RTI_SKIP_LAST_1
#define RTI_SKIP_LAST_1(X, Y, ...) (X, Y) RTI_SKIP_LAST_0
#define RTI_SKIP_LAST_0_END
#define RTI_SKIP_LAST_1_END


// Convert (a,1,A)(b,2,B)(c,3,C) into (a,1)(b,2)(c,3)
// Otherwise BOOST_FUSION_ADAPT_STRUCT macro will be confused.
// The TRANSFORM macro expands as follows:
//
// RTI_SKIP_LAST_0 Attr                                ---->
// RTI_SKIP_LAST_0 (a,1,A)(b,2,B)(c,3,C)               ---->
// (a,1) RTI_SKIP_LAST_1 (b,2,B)(c,3,C)                ---->
// (a,1) (b,2) RTI_SKIP_LAST_0 (c,3,C)                 ---->
// (a,1) (b,2) (c,3) RTI_SKIP_LAST_1                   ---->
// BOOST_PP_CAT((a,1) (b,2) (c,3) RTI_FILLER_1, _END)  ---->
// (a,1) (b,2) (c,3) RTI_SKIP_LAST_1_END               ---->
// (a,1) (b,2) (c,3)                                   ---->
// voila!!!

#define RTI_TRANSFORM_TRIPLETS_TO_PAIRS(Attributes) \
  BOOST_PP_CAT(RTI_SKIP_LAST_0 Attributes, _END)
  
#define RTI_ADAPT_STRUCT(Name,Attributes)        \
  STRUCT_NAME_DEF_INTERNAL(Name)                 \
  RTI_MEMBER_TRAITS(Name, Attributes)            \
  BOOST_FUSION_ADAPT_STRUCT(Name,RTI_TRANSFORM_TRIPLETS_TO_PAIRS(Attributes))


#define RTI_ENUM_MEMBER_INFO_INTERNAL(R,EnumType,Index,Attr)   \
template<>                                                     \
inline MemberInfo                                              \
EnumDef<EnumType>::EnumMember<Index>::info() {                 \
  return MemberInfo(RTI_GET_NAME_ORDINAL Attr);                \
}


#define RTI_ADAPT_ENUM(EnumType, Attributes)           \
template <>                                            \
struct EnumDef<EnumType>                               \
{                                                      \
  static const char *name()                            \
  { return DefaultMemberNames::basename(#EnumType); }  \
                                                       \
  static const unsigned int size =                     \
    BOOST_PP_SEQ_SIZE(RTI_PARENTHESIZE(Attributes));   \
                                                       \
  template <unsigned Index>                            \
  struct EnumMember {                                  \
    static MemberInfo info();                          \
  };                                                   \
};                                                     \
BOOST_PP_SEQ_FOR_EACH_I(RTI_ENUM_MEMBER_INFO_INTERNAL, \
                        EnumType,                      \
                        RTI_PARENTHESIZE(Attributes))



#endif // RTIREFLEX_DEFAULT_MEMBER_NAMES_H

