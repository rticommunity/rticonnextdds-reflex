/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

/**  
 * @file 
 * @brief Macros to adapt user-defined types (classes, structs, and enums) 
 *        to allow RefleX to extract type-specific information at compile-time.
 *
 */

#ifdef REFLEX_DOXYGEN

/** @brief Static bound for string types. Default 256
*   @see   REFLEX_STATIC_CONTAINER_BOUND
*/
#define REFLEX_STATIC_STRING_BOUND 

/** @brief Static bound for container types. Default 256
*   @see   REFLEX_STATIC_STRING_BOUND
*/
#define REFLEX_STATIC_CONTAINER_BOUND 

#endif // REFLEX_DOXYGEN
/**
 * @brief Adapt and customize a C++ native enumeration for use with RefleX
 * 
 *        \link REFLEX_ENUM_DEF_CUSTOM \endlink and \link REFLEX_ENUM_MEMBER_DEF_CUSTOM \endlink
 *        are always used together and they allow customization of enumeration 
 *        type-name, item names and ordinals that are not possible with 
 *        \link REFLEX_ADAPT_ENUM \endlink .
 *
 *        Consider the following native C++ enumeration called FillKind.
 *   
 *        \code{.cpp} enum FillKind { SOLID_FILL=10, TRANSPARENT_FILL=11, HORIZONTAL_HATCH_FILL=12, VERTICAL_HATCH_FILL=13 };
 *        \endcode
 * 
 *        REFLEX_ENUM_DEF_CUSTOM and REFLEX_ENUM_MEMBER_DEF_CUSTOM may be used as follows.
 *
 *        \code{.cpp}
 *        REFLEX_ENUM_DEF_CUSTOM(FillKind, "FillKindStyle", 4)
 *        
 *        REFLEX_ENUM_MEMBER_DEF_CUSTOM(FillKind, 0, "SOLID_FILL_STYLE",            10)
 *        REFLEX_ENUM_MEMBER_DEF_CUSTOM(FillKind, 1, "TRANSPARENT_FILL_STYLE",      11)
 *        REFLEX_ENUM_MEMBER_DEF_CUSTOM(FillKind, 2, "HORIZONTAL_HATCH_FILL_STYLE", 12)
 *        REFLEX_ENUM_MEMBER_DEF_CUSTOM(FillKind, 3, "VERTICAL_HATCH_FILL_STYLE",   13)
 *        \endcode
 * 
 *        The above declarations results in to an X-Types typecode representing an
 *        enumeration named FillKindStyle as follows.
 *   
 *        \code{.cpp}
 *        enum FillKindStyle { SOLID_FILL_STYLE=10, TRANSPARENT_FILL_STYLE=11, HORIZONTAL_HATCH_FILL_STYLE=12, VERTICAL_HATCH_FILL_STYLE=13 };
 *        \endcode
 *        
 * @param EnumType Fully qualified type of the native C++ enumeration.
 * @param Name     The target name of the enumeration. This may be different from
 *                 the name of the native C++ enumeration.
 * @param Size     The number of items in the C++ native enumeration.
 * @see   REFLEX_ENUM_MEMBER_DEF_CUSTOM
 * @see   REFLEX_ADAPT_ENUM
 */
#define REFLEX_ENUM_DEF_CUSTOM(EnumType, Name, Size)     \
namespace reflex { namespace codegen {            \
  template <>                                     \
  struct EnumDef<EnumType>                        \
  {                                               \
      static const char *name() {                 \
        return Name;                              \
      }                                           \
                                                  \
      enum { is_enum = true };                    \
      static const unsigned int size = Size;      \
                                                  \
      template <unsigned Index>                   \
      struct EnumMember {                         \
        static MemberInfo info(); \
      };                                          \
  };                                              \
} } // namespace reflex::detail                         

/**
* @brief Adapt and customize a C++ native enumeration item names and
*        ordinals for use with RefleX
*
*        See \link REFLEX_ENUM_DEF_CUSTOM \endlink and 
*        \link REFLEX_ENUM_MEMBER_DEF_CUSTOM \endlink for more information.
* @param EnumType The fully qualified type of the enumeration to be adapted.
* @param Index The index of the member. Starts at 0.
* @param Name The target name of the member
* @param Ordinal The integer value of the enumeration item.
*/
#define REFLEX_ENUM_MEMBER_DEF_CUSTOM(EnumType, Index, Name, Ordinal)   \
namespace reflex { namespace codegen {                           \
  template<>                                                     \
  inline MemberInfo                                              \
  EnumDef<EnumType>::EnumMember<Index>::info()                   \
    {                                                            \
    return MemberInfo(Name, Ordinal);                            \
    }                                                            \
} } // namespace reflex::detail                         

/**
* @brief Adapt and customize a std::tuple type for use with RefleX
*
*        By default, std::tuple types do not require any adaptations to 
*        use with RefleX. Adaptations might be desirable to allow more
*        descriptive (human-readable) names for tuple types and members.
*
*        Without customization, a std::tuple maps to "DefaultTupleNameN" 
*        where N is unique for each std::tuple type. The member names
*        default to "m0", "m1", "m2" etc. \link REFLEX_STRUCT_NAME_DEF_CUSTOM \endlink 
*        and \link REFLEX_STRUCT_MEMBER_DEF_CUSTOM \endlink allow customization 
*        of the typename and member names. They are always used together.
*
*        Consider the following std::tuple definition.
*
*        \code{.cpp} typedef std::tuple<std::string, int, int, int> ShapeTypeTuple;
*        \endcode
*
*        Without any customization, ShapeTypeTuple maps to the following X-Types 
*        struct type. Note that there is no key attribute specified.
*
*        \code{.cpp}
*        struct DefaultTupleName0 {
*          string<REFLEX_STATIC_STRING_BOUND> m0;
*          long m1;
*          long m2;
*          long m3;
*        };
*        \endcode
*
*        With \link REFLEX_STRUCT_NAME_DEF_CUSTOM \endlink and 
*        \link REFLEX_STRUCT_MEMBER_DEF_CUSTOM \endlink the name and the members
*        of the ShapeTypeTuple can be easily customized. 
*
*        \code{.cpp}
*        REFLEX_STRUCT_NAME_DEF_CUSTOM(ShapeTypeTuple, "ShapeType")
*
*        REFLEX_STRUCT_MEMBER_DEF_CUSTOM(ShapeTypeTuple, 0, "color",     REFLEX_KEY);
*        REFLEX_STRUCT_MEMBER_DEF_CUSTOM(ShapeTypeTuple, 1, "x",         REFLEX_REQUIRED);
*        REFLEX_STRUCT_MEMBER_DEF_CUSTOM(ShapeTypeTuple, 2, "y",         REFLEX_REQUIRED);
*        REFLEX_STRUCT_MEMBER_DEF_CUSTOM(ShapeTypeTuple, 3, "shapesize", REFLEX_REQUIRED);
*        \endcode
*
*        The above declarations results in to an X-Types typecode representing a
*        struct named ShapeType as follows.
*
*        \code{.cpp}
*        struct ShapeType {
*          string<REFLEX_STATIC_STRING_BOUND> color; //@key
*          long x;
*          long y;
*          long shapesize;
*        };
*        \endcode
*
* @param FullyQualifiedType The fully qualified type of the C++ tuple type.
* @param Name     The target name of the tuple type. 
* @see   REFLEX_ADAPT_STRUCT
*/
#define REFLEX_STRUCT_NAME_DEF_CUSTOM(FullyQualifiedType, Name)   \
namespace reflex { namespace codegen {                            \
  template <>                                                     \
  struct StructName<FullyQualifiedType>                           \
  {                                                               \
    static std::string get() {                                    \
    return Name;                                                  \
  }                                                               \
  };                                                              \
} } // namespace reflex::detail                         

/**
* @brief Adapt and customize members of a std::tuple type for use with RefleX
*
*        See \link REFLEX_STRUCT_NAME_DEF_CUSTOM \endlink and
*        \link REFLEX_ADAPT_STRUCT \endlink for more information.
* @param FullyQualifiedType The fully qualified type of the type to be adapted.
* @param Index The index of the member. Starts at 0.
* @param Name The target name of the member
* @param Flags One of \link REFLEX_KEY \endlink, \link REFLEX_REQUIRED \endlink, 
*        and \link REFLEX_OPTIONAL \endlink
*/
#define REFLEX_STRUCT_MEMBER_DEF_CUSTOM(FullyQualifiedType, Index, Name, Flags)   \
namespace reflex { namespace codegen {                                            \
  template <>                                                                     \
  struct MemberTraits<FullyQualifiedType, Index>                                  \
    {                                                                             \
    static MemberInfo member_info()                               \
        {                                                                         \
      return MemberInfo(Name, Flags);                             \
        }                                                                         \
    };                                                                            \
} } // namespace reflex::detail                         

/**
* @brief Adapt a C++ native enumeration for use with RefleX
*
*        Consider the following native C++ enumeration called FillKind.
*
*        \code{.cpp} enum ShapeFillKind { SOLID_FILL=10, TRANSPARENT_FILL=11, HORIZONTAL_HATCH_FILL=12, VERTICAL_HATCH_FILL=13 };
*        \endcode
* 
*        It may be adapted as follows.
*
*        \code{.cpp}
*        REFLEX_ADAPT_ENUM(
*          FillKind,
*          (SOLID_FILL,            10)
*          (TRANSPARENT_FILL,      11)
*          (HORIZONTAL_HATCH_FILL, 12)
*          (VERTICAL_HATCH_FILL,   13))
*        \endcode
*
*        The above declarations results into an X-Types typecode representing an
*        enumeration named FillKind that matches the C++ native declaration.
*
* @param EnumType The fully qualified type of the native C++ enumeration.
* @param Attributes A list of parenthesis describing the enumeration items and their ordinals. 
*                   Parenthesis are not separated by comma.
* @see   REFLEX_ENUM_MEMBER_DEF_CUSTOM
* @see   REFLEX_ENUM_DEF_CUSTOM
*/
#define REFLEX_ADAPT_ENUM(EnumType, Attributes)              \
namespace reflex { namespace codegen {                       \
  template <>                                                \
  struct EnumDef<EnumType>                                   \
        {                                                    \
    static const char *name() {                              \
      return                                                 \
        detail::DefaultMemberNames::basename                 \
         (#EnumType);                                        \
                }                                            \
                                                             \
    enum { is_enum = true };                                 \
                                                             \
    static const unsigned int size =                         \
      BOOST_PP_SEQ_SIZE(RTI_PARENTHESIZE(Attributes));       \
                                                             \
    template <unsigned Index>                                \
    struct EnumMember {                                      \
      static MemberInfo info();             \
                };                                           \
        };                                                   \
} } /* namespace reflex::detail */                           \
BOOST_PP_SEQ_FOR_EACH_I(RTI_ENUM_MEMBER_INFO_INTERNAL,       \
                        EnumType,                            \
                        RTI_PARENTHESIZE(Attributes))

/**
* @brief Adapt a native C++ struct/class for use with RefleX
*
*        Consider the following native C++ struct called ShapeType.
*
*        \code{.cpp} 
*        namespace geometry {
*          struct ShapeType {
*            std::string color;
*            int x;
*            int y;
*            int shapesize;
*          };
*        } // namespace geometry
*        \endcode
*
*        It may be adapted as follows.
*
*        \code{.cpp}
*        REFLEX_ADAPT_STRUCT(
*          geometry::ShapeType,
*          (std::string,  color)
*          (int,          x)
*          (int,          y)
*          (int,          shapesize))
*        \endcode
*
*        The above declarations results into an X-Types typecode representing an
*        struct type named ShapeType that matches the C++ native declaration.
*        The namespaces are ignored and the resulting typecode has EXTENSIBLE_EXTENSIBILITY.
*        The default bounds for strings and containers are \link REFLEX_STATIC_STRING_BOUND
*        \endlink and \link REFLEX_STATIC_CONTAINER_BOUND \endlink.
*
*        If the type of a member contains a comma, such as in std::pair or std::tuple,
*        such types must be typedef-ed so that commas are no longer needed.
*
*        If the type is a class, getters may be used instead of the actual member names.
*        The name of the getter is used to define the member name. The getter name should
*        include the trailing parenthesis as in color(), x(), y(), and shapesize().
*
* @param Type The fully qualified type of the native C++ struct/class.
* @param Attributes A list of parenthesis describing the members (the type and the member name).
*                   Parenthesis are not separated by comma.
* @see   REFLEX_STRUCT_MEMBER_DEF_CUSTOM
* @see   REFLEX_STRUCT_NAME_DEF_CUSTOM
*/
#define REFLEX_ADAPT_STRUCT(Type,Attributes)        \
  RTI_STRUCT_NAME_DEF_INTERNAL(Type)                \
  RTI_MEMBER_TRAITS(Type, Attributes)               \
  BOOST_FUSION_ADAPT_STRUCT(Type, RTI_TRANSFORM_TRIPLETS_TO_PAIRS(Attributes))

/**
* @brief Adapt a C++ struct/class hierarchy for use with RefleX
*
* @param Type The fully qualified type of the C++ struct/class.
* @param Base The fully qualified type of the C++ struct/class that Type
*             inherits from.
* @param Attributes A list of parenthesis describing the members (the type and the member name).
*                   Parenthesis are not separated by comma.
* @see   REFLEX_ADAPT_STRUCT
*/
#define REFLEX_ADAPT_VALUETYPE(Type, Base, Attributes)  \
  RTI_INHERITANCE_TRAITS(Type, Base)                    \
  REFLEX_ADAPT_STRUCT(Type, Attributes)

// The parenthesis below are essential for the RTI_DUMMY macro.
/** @brief Indicate in \link REFLEX_ADAPT_STRUCT \endlink that a member is optional */
#define REFLEX_OPTIONAL    (DDS_TYPECODE_NONKEY_MEMBER)

/** @brief Indicate in \link REFLEX_ADAPT_STRUCT \endlink that a member is key */
#define REFLEX_KEY         (DDS_TYPECODE_KEY_MEMBER)

/** @brief Indicate in \link REFLEX_ADAPT_STRUCT \endlink that a member is required */
#define REFLEX_REQUIRED    (DDS_TYPECODE_NONKEY_REQUIRED_MEMBER)

/** @brief Indicate the base struct/class in \link REFLEX_ADAPT_VALUETYPE \endlink */
#define REFLEX_EXTENDS(X) X

#ifndef REFLEX_DOXYGEN

#define RTI_STRUCT_NAME_DEF_INTERNAL(FullyQualifiedType)          \
  namespace reflex { namespace codegen {                          \
  template <>                                                     \
  struct StructName<FullyQualifiedType>                           \
    {                                                             \
    static std::string get()                                      \
        {                                                         \
      return detail::DefaultMemberNames::basename(#FullyQualifiedType);   \
        }                                                         \
    };                                                            \
} } // namespace reflex::detail                         

#define RTI_INHERITANCE_TRAITS(Derived, Base)      \
namespace reflex { namespace type_traits {         \
  template <>                                      \
  struct InheritanceTraits<Derived> {              \
    typedef true_type has_base;                    \
    typedef Base basetype;                         \
    };                                             \
} } // namespace reflex::detail                    

#ifdef RTI_WIN32
#define RTI_DUMMY(X) X
#define RTI_DUMMY_VAR   DDS_Octet RTI_DUMMY=REFLEX_REQUIRED

#define RTI_GET_NAME_FLAGS(Type, Name, ...)   \
    #Name, RTI_DUMMY ## __VA_ARGS__
#else
// Not needed outside Visual Studio
#define RTI_DUMMY_VAR

#define RTI_WITH_FLAG(Time, Name, Flag)  #Name, Flag
#define RTI_WITHOUT_FLAG(Time, Name)     #Name, REFLEX_REQUIRED

#define RTI_PUSH_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4

// A pair    will push out RTI_WITHOUT_FLAGS
// A triplet will push out RTI_WITH_FLAGS
#define RTI_FLAGS_CHOOSER(...) RTI_PUSH_4TH_ARG(__VA_ARGS__, RTI_WITH_FLAG, RTI_WITHOUT_FLAG)

// __VA_ARGS__ is either a pair or a triplet.
#define RTI_GET_NAME_FLAGS(...)            \
    RTI_FLAGS_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#endif

#define RTI_GET_NAME_ORDINAL(Name, Ordinal) #Name, Ordinal

// Note that Attr is already has parenthesis.
// Attr is either a pair or a triplet
// RTI_GET_NAME_FLAGS macro eats the parenthesis around Attr.
// RTI_DUMMY_VAR is used on for Win32 only
#define RTI_MEMBER_INFO_INTERNAL(R,Data,I,Attr)        \
namespace reflex { namespace codegen {                 \
  template<>                                           \
  MemberInfo MemberTraits<Data, I>::member_info()      \
    {                                                  \
      RTI_DUMMY_VAR;                                   \
      return MemberInfo(                               \
        RTI_GET_NAME_FLAGS Attr);                      \
    }                                                  \
} } // namespace reflex::detail      

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
// RTI_SKIP_LAST_0 Attr                                   ---->
// RTI_SKIP_LAST_0 (a,1,A)(b,2,B)(c,3,C)                  ---->
// (a,1) RTI_SKIP_LAST_1 (b,2,B)(c,3,C)                   ---->
// (a,1) (b,2) RTI_SKIP_LAST_0 (c,3,C)                    ---->
// (a,1) (b,2) (c,3) RTI_SKIP_LAST_1                      ---->
// BOOST_PP_CAT((a,1) (b,2) (c,3) RTI_SKIP_LAST_1, _END)  ---->
// (a,1) (b,2) (c,3) RTI_SKIP_LAST_1_END                  ---->
// (a,1) (b,2) (c,3)                                      ---->
// voila!!!

#define RTI_TRANSFORM_TRIPLETS_TO_PAIRS(Attributes) \
  BOOST_PP_CAT(RTI_SKIP_LAST_0 Attributes, _END)


#define RTI_ENUM_MEMBER_INFO_INTERNAL(R,EnumType,Index,Attr)       \
namespace reflex { namespace codegen {                             \
  template<>                                                       \
  MemberInfo EnumDef<EnumType>::EnumMember<Index>::info()          \
    {                                                              \
      return MemberInfo(RTI_GET_NAME_ORDINAL Attr);                \
    }                                                              \
} } // namespace reflex::detail      

#endif // REFLEX_DOXYGEN

