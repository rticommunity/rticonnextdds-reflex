/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_TYPECODE_MANIP_H
#define RTIREFLEX_TYPECODE_MANIP_H

#include <iostream>
#include <string>
#include <vector>

#include <ndds/ndds_cpp.h>
#include "reflex/safe_typecode.h"
#include "reflex/default_member_names.h"
#include "reflex/enable_if.h"
#include "reflex/disc_union.h" 
#include "reflex/tuple_iterator.h"
#include "reflex/bounded.h"
#include "reflex/dllexport.h"
#include "reflex/reflex_fwd.h"

#define GET_PRIMITIVE_TC_DEF(BASIC_TYPE, TYPECODE)      \
static const DDS_TypeCode *                             \
  get_primitive_tc(DDS_TypeCodeFactory * factory,       \
                   const BASIC_TYPE *) {                \
    return factory->get_primitive_tc(TYPECODE);         \
}

namespace reflex {

  namespace detail {

    REFLEX_DLL_EXPORT void REFLEX_DECLSPEC
      print_IDL(
      const DDS_TypeCode * tc,
      DDS_UnsignedLong indent);
    /*
    DDS_TypeCode * create_array_tc(DDS_TypeCode * inner,
      const DDS_UnsignedLongSeq &dims);

    DDS_TypeCode * create_seq_tc(DDS_TypeCode *inner,
      size_t bound);

    DDS_TypeCode * create_string_tc(size_t bound);
    DDS_TypeCode * create_struct_tc(const char * name);
    DDS_TypeCode * create_sparse_tc(const char * name);
    DDS_TypeCode * create_value_tc(const char * name,
                                   DDS_TypeCode * basetc);

    DDS_TypeCode * create_enum_tc(
      const char * name,
      const DDS_EnumMemberSeq & enum_seq,
      const std::vector<MemberInfo> &);

    DDS_TypeCode * create_union_tc(
      const char * name,
      DDS_TypeCode * discTc,
      DDS_UnsignedLong default_case,
      DDS_UnionMemberSeq & member_seq);
      */

    template <size_t I, class DimList>
    struct CopyDims;

    template <size_t I, size_t Head, size_t... Tail>
    struct CopyDims <I, dim_list<Head, Tail...>>
    {
      static void exec(DDS_UnsignedLongSeq & seq)
      {
        seq[I] = Head;
        detail::CopyDims<I + 1, dim_list<Tail...>>::exec(seq);
      }
    };

    template <size_t I, size_t Last>
    struct CopyDims<I, dim_list<Last>>
    {
      static void exec(DDS_UnsignedLongSeq & seq)
      {
        seq[I] = Last;
      }
    };

    template <class Tuple, size_t I, size_t MAX_INDEX, class Case>
    struct MatchDefaultCase;

    template <class Tuple, size_t I, size_t MAX_INDEX, class T, int Tag, int... Tags>
    struct MatchDefaultCase<Tuple, I, MAX_INDEX, match::Case<T, Tag, Tags...>>
    {
      enum {
        value =
        detail::MatchDefaultCase<Tuple,
        I + 1,
        MAX_INDEX,
        typename std::tuple_element<I + 1, Tuple>::type
        >::value
      };
    };

    template <class Tuple, size_t MAX_INDEX, class T, int Tag, int... Tags>
    struct MatchDefaultCase<Tuple, MAX_INDEX, MAX_INDEX, match::Case<T, Tag, Tags...>>
    {
      enum { value = -1 };
    };

    template <class Tuple, size_t I, size_t MAX_INDEX, class T>
    struct MatchDefaultCase<Tuple, I, MAX_INDEX, match::Case<T>>
    {
      enum { value = I };
    };

    template <class Tuple, size_t MAX_INDEX, class T>
    struct MatchDefaultCase<Tuple, MAX_INDEX, MAX_INDEX, match::Case<T>>
    {
      enum { value = MAX_INDEX };
    };

    template <class TagType, class... Cases>
    struct DefaultCaseIndex;

    template <class TagType, class... Cases>
    struct DefaultCaseIndex<match::Union<TagType, Cases...>>
    {
      typedef typename match::Union<TagType, Cases...>::case_tuple_type CaseTuple;
      enum {
        value =
        MatchDefaultCase<CaseTuple,
        1,
        Size<CaseTuple>::value - 1,
        typename std::tuple_element<1, CaseTuple>::type>::value
      };
    };

    struct Primitive_TC_Helper
    {
      GET_PRIMITIVE_TC_DEF(match::octet_t, DDS_TK_OCTET)
      GET_PRIMITIVE_TC_DEF(bool,           DDS_TK_BOOLEAN)
      GET_PRIMITIVE_TC_DEF(char,           DDS_TK_CHAR)
      GET_PRIMITIVE_TC_DEF(int8_t,         DDS_TK_CHAR)
      GET_PRIMITIVE_TC_DEF(int16_t,        DDS_TK_SHORT)
      GET_PRIMITIVE_TC_DEF(uint16_t,       DDS_TK_USHORT)
      GET_PRIMITIVE_TC_DEF(int32_t,        DDS_TK_LONG)
      GET_PRIMITIVE_TC_DEF(uint32_t,       DDS_TK_ULONG)
      GET_PRIMITIVE_TC_DEF(int64_t,        DDS_TK_LONGLONG)
      GET_PRIMITIVE_TC_DEF(uint64_t,       DDS_TK_ULONGLONG)
      GET_PRIMITIVE_TC_DEF(float,          DDS_TK_FLOAT)
      GET_PRIMITIVE_TC_DEF(double,         DDS_TK_DOUBLE)
      GET_PRIMITIVE_TC_DEF(long double,    DDS_TK_LONGDOUBLE)

#ifndef RTI_WIN32
      GET_PRIMITIVE_TC_DEF(char32_t,       DDS_TK_WCHAR)
#endif

#if __x86_64__
      GET_PRIMITIVE_TC_DEF(long long,          DDS_TK_LONGLONG)
      GET_PRIMITIVE_TC_DEF(unsigned long long, DDS_TK_ULONGLONG)
#endif
    };

    template <class T>
    struct Enum_TC_Helper
    {
      template <size_t I, size_t MAX>
      struct SetEnumMembers
      {
        static void exec(DDS_EnumMemberSeq & enum_seq,
                         std::vector<detail::MemberInfo> & info_seq)
        {
          using namespace detail;

          info_seq[I] = EnumDef<T>::template EnumMember<I>::info();
          enum_seq[I].name = const_cast<char *>(info_seq[I].name.c_str());
          enum_seq[I].ordinal = info_seq[I].value;
          SetEnumMembers<I + 1, MAX>::exec(enum_seq, info_seq);
        }
      };

      template <size_t MAX>
      struct SetEnumMembers<MAX, MAX>
      {
        static void exec(DDS_EnumMemberSeq & enum_seq,
                         std::vector<detail::MemberInfo> & info_seq)
        {
          using namespace detail;

          info_seq[MAX] = EnumDef<T>::template EnumMember<MAX>::info();
          enum_seq[MAX].name = const_cast<char *>(info_seq[MAX].name.c_str());
          enum_seq[MAX].ordinal = info_seq[MAX].value;
        }
      };

    };

    /* TypeCode Overload Resolution Helper */
    /* Overload resolution of several free primary template 
     * functions can be notoriously brittle due to
     * ordering issues. In general, a function must be declared
     * before it is called. Sounds simple but in a set of mutually
     * recursive primary template functions, it is easy to lose
     * grip on the what's declared before and what's not. In 
     * general, all the functions should be declared beforehand.
     * There is an alternative solution that is used here.
     * Put all the functions in a class and make them static 
     * and use their fully qualified name to call them. 
     * No upfront declarations are necessary if this idiom is used. 
     * */
    struct TC_overload_resolution_helper 
    {      
      
      template <class T>
      static SafeTypeCode<T> get_typecode_struct(
        const char * name,
        false_type /* T has no base */)
      {
        DDS_TypeCodeFactory * factory =
          DDS_TypeCodeFactory::get_instance();

        std::string struct_name =
          detail::StructName<typename detail::remove_refs<T>::type>::get();

        const char * type_name =
          name ? name : struct_name.c_str();

        //SafeTypeCode<T> structTc(factory, type_name);
        DDS_ExceptionCode_t ex;
        DDS_TypeCode *structTc =
          factory->create_struct_tc(type_name,
                                    DDS_StructMemberSeq(), ex);

        SafeTypeCode<T> safetc(factory, structTc);

        check_exception_code(
          "get_typecode_struct: Unable to create struct typecode, error = ",
          ex);

        detail::TypelistIterator<
          T,
          0,
          detail::Size<T>::value - 1>::add(
            factory, safetc.get());

        return safetc;
      }

      template <class T>
      static SafeTypeCode<T> get_typecode_struct(
        const char * name,
        true_type /* T has base */)
      {
        DDS_TypeCodeFactory * factory =
          DDS_TypeCodeFactory::get_instance();

        std::string struct_name =
          detail::StructName<typename detail::remove_refs<T>::type>::get();

        const char * type_name =
          name ? name : struct_name.c_str();

        typedef typename InheritanceTraits<T>::basetype BaseType;

        SafeTypeCode<BaseType> baseTc =
          get_typecode_struct<BaseType>(
             0,
             typename InheritanceTraits<BaseType>::has_base());

        DDS_ExceptionCode_t ex;
        DDS_TypeCode *valueTc =
          factory->create_value_tc(
            type_name,
            DDS_EXTENSIBLE_EXTENSIBILITY,
            DDS_VM_NONE,
            baseTc.get(),
            DDS_ValueMemberSeq(),
            ex);

        SafeTypeCode<T> aggregateTc(factory, valueTc);

        check_exception_code(
          "get_typecode_struct: Unable to create valuetype typecode, error = ",
          ex);

        detail::TypelistIterator<
          T,
          0,
          detail::Size<T>::value - 1>
          ::add(factory, aggregateTc.get());

        return aggregateTc;
      }

      template <class Str>
      static SafeTypeCode<Str> 
        get_typecode(
            DDS_TypeCodeFactory * factory,
            const Str *,
            typename enable_if<is_string<Str>::value>::type * = 0)
      {
          DDS_ExceptionCode_t ex;

          SafeTypeCode<Str> stringTc(
            factory, 
            factory->create_string_tc(detail::static_string_bound<Str>::value, 
                                      ex));
          
          check_exception_code(
            "InnerTypeCodeBase::create_string_tc: Unable to create string typecode, error = ",
            ex);

          return stringTc;
      }

      template <class T>
      // overload for 
      // 1. user-defined structs adapted as a Fusion Sequence
      // 2. tuples.
      static SafeTypeCode<T> get_typecode(
        DDS_TypeCodeFactory * factory,
        const T *,
        typename disable_if<detail::is_enum<T>::value ||
                            is_primitive<T>::value    ||
                            is_container<T>::value    ||
                            is_optional<T>::value     ||
                            is_string<T>::value       ||
                            is_stdarray<T>::value>::type * = 0)
      {
        return get_typecode_struct<T>(
                0 /* name */,
                typename detail::InheritanceTraits<T>::has_base());
      }

      template <class T>
      // overload for all the primitive types.
      static SafeTypeCode<T> get_typecode(
        DDS_TypeCodeFactory * factory,
        const T * primitive,
        typename enable_if<is_primitive<T>::value>::type * = 0)
      {
        return SafeTypeCode<T>(factory, 
                               Primitive_TC_Helper::get_primitive_tc(factory, primitive));
      }

      template <class T>
      // overload for user-defined enumeration types.
      static SafeTypeCode<T> get_typecode(
        DDS_TypeCodeFactory * factory,
        const T *,
        typename enable_if<detail::is_enum<T>::value>::type * = 0)
      {
        // The lifetime of info_seq must be until create_enum_tc is invoked
        // because enum_seq[i].name is just a pointer to MemberInfo.name strings

        DDS_ExceptionCode_t ex;
        DDS_EnumMemberSeq enum_seq;
        std::vector<MemberInfo> info_seq(EnumDef<T>::size);
        enum_seq.ensure_length(EnumDef<T>::size, EnumDef<T>::size);
        Enum_TC_Helper<T>::SetEnumMembers<0, EnumDef<T>::size - 1>::exec(enum_seq, info_seq);
        
        SafeTypeCode<T> enumTc(
          factory, 
          factory->create_enum_tc(EnumDef<T>::name(), enum_seq, ex));

        check_exception_code(
          "get_typecode: Unable to create enum typecode, error = ",
          ex);
        
        return enumTc;
      }

      template <class C, size_t Bound>
      static SafeTypeCode<reflex::match::Bounded<C, Bound>> get_typecode(
            DDS_TypeCodeFactory * factory,
            const reflex::match::Bounded<C, Bound> *,
            typename enable_if<is_container<C>::value>::type * = 0)
      {
          SafeTypeCode<typename C::value_type> innerTc
            = TC_overload_resolution_helper::get_typecode(
                factory,
                static_cast<typename C::value_type *>(0));

          DDS_ExceptionCode_t ex;
          SafeTypeCode<reflex::match::Bounded<C, Bound>> seqTc
            (factory,
             factory->create_sequence_tc(Bound, innerTc.get(), ex));

          check_exception_code(
            "get_typecode: Unable to create sequence typecode, error = ",
            ex);

          return seqTc;
      }

      template <class C>
      static SafeTypeCode<C> get_typecode(
         DDS_TypeCodeFactory * factory,
         const C *,
         typename enable_if<is_container<C>::value>::type * = 0)
      {
          SafeTypeCode<typename container_traits<C>::value_type> innerTc
            = TC_overload_resolution_helper::get_typecode(
                factory,
                static_cast<typename container_traits<C>::value_type *>(0));

          DDS_ExceptionCode_t ex;
          SafeTypeCode<C> seqTc(
            factory, 
            factory->create_sequence_tc(detail::static_container_bound<C>::value, 
                                        innerTc.get(), 
                                        ex));

          check_exception_code(
            "get_typecode: Unable to create sequence typecode, error = ",
            ex);

          return seqTc;
      }

      template <class T, size_t N>
      static SafeTypeCode<std::array<T, N>> get_typecode(
          DDS_TypeCodeFactory * factory,
          const std::array<T, N> *)
      {
          typedef typename remove_all_extents<T>::type BasicType;
          SafeTypeCode<BasicType> basicTc
            = TC_overload_resolution_helper::get_typecode(
                factory, 
                static_cast<BasicType *>(0));

          DDS_UnsignedLongSeq dims;
          typedef typename
            detail::make_dim_list<std::array<T, N>>::type DimList;
          dims.ensure_length(DimList::size, DimList::size);
          detail::CopyDims<0, DimList>::exec(dims);

          DDS_ExceptionCode_t ex;
          SafeTypeCode<std::array<T, N>> arrayTc(
             factory,
             factory->create_array_tc(dims, basicTc.get(), ex));

          check_exception_code(
            "InnerTypeCodeBase::create_array_tc: Unable to create array typecode, error = ",
            ex);

          return arrayTc;
      }

      template <size_t Bound>
      static SafeTypeCode<reflex::match::Bounded<std::string, Bound>>
        get_typecode(
            DDS_TypeCodeFactory * factory,
            const reflex::match::Bounded<std::string, Bound> *)
      {
        DDS_ExceptionCode_t ex;

        SafeTypeCode<match::Bounded<std::string, Bound>> 
          stringTc(factory, factory->create_string_tc(Bound, ex));

        check_exception_code(
          "get_typecode::create_string_tc: Unable to create string typecode, error = ",
          ex);

        return stringTc;
      }

      template <class T>
      static SafeTypeCode<match::Range<T>> get_typecode(
        DDS_TypeCodeFactory * factory,
        const reflex::match::Range<T> *)
      {
        SafeTypeCode<typename remove_reference<T>::type> innerTc
          = TC_overload_resolution_helper::get_typecode(
                  factory,
                  static_cast<typename remove_reference<T>::type *>(0));

        DDS_ExceptionCode_t ex;
        SafeTypeCode<match::Range<T>> seqTc(
          factory,
          factory->create_sequence_tc(
            detail::static_container_bound<match::Range<T>>::value, 
            innerTc.get(), 
            ex));

        check_exception_code(
          "get_typecode: Unable to create sequence typecode, error = ",
          ex);

        return seqTc;
      }

/*
#ifdef RTI_WIN32

      template <class... T>
      static SafeTypeCode<boost::optional<T...>> get_typecode(
        DDS_TypeCodeFactory * factory,
        const boost::optional<T...> *)
      {
        typedef typename PackHead<T...>::type HeadT;
        SafeTypeCode<HeadT> innerTc =
          TC_overload_resolution_helper::get_typecode(
              factory,
              static_cast<HeadT *>(0));

        return SafeTypeCode<boost::optional<T...>>(factory, move(innerTc));
      }
#else
      template <class T>
      static SafeTypeCode<boost::optional<T>> get_typecode(
        DDS_TypeCodeFactory * factory,
        const boost::optional<T> *)
      {
        SafeTypeCode<T> innerTc =
          TC_overload_resolution_helper::get_typecode(
            factory,
            static_cast<T *>(0));

        return SafeTypeCode<boost::optional<T>>(factory, move(innerTc));
      }
#endif
*/      
      
      template <class Opt>
      static SafeTypeCode<Opt> get_typecode(
        DDS_TypeCodeFactory * factory,
        const Opt * opt,
        typename detail::enable_if<detail::is_optional<Opt>::value>::type * = 0)
      {
        typedef 
            typename detail::remove_const<
                        typename detail::remove_reference<
                          decltype(**opt)
                        >::type
                     >::type
            OptValuetype;
            
        SafeTypeCode<OptValuetype> innerTc =
          TC_overload_resolution_helper::get_typecode(
            factory,
            static_cast<OptValuetype *>(0));

        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
        SafeTypeCode<Opt> optionalTc(
          factory, 
          factory->clone_tc(innerTc.get(), ex));

        check_exception_code(
          "get_typecode<optional>: Unable to clone typecode, error = ",
          ex);

        return optionalTc;
      }

      template <class T, size_t Bound>
      static SafeTypeCode<match::BoundedRange<T, Bound>> get_typecode(
            DDS_TypeCodeFactory * factory,
            const reflex::match::BoundedRange<T, Bound> *)
      {
          SafeTypeCode<typename remove_reference<T>::type> innerTc
            = TC_overload_resolution_helper::get_typecode(
                factory,
                static_cast<typename remove_reference<T>::type *>(0));

          DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
          SafeTypeCode<match::BoundedRange<T, Bound>> seqTc(
            factory,
            factory->create_sequence_tc(Bound, innerTc.get(), ex));

          check_exception_code(
            "get_typecode<BoundedRange>: Unable to create sequence typecode, error = ",
            ex);

          return seqTc;
      }
      
      template <class... Args>
      static SafeTypeCode<reflex::match::Sparse<Args...>> get_typecode(
          DDS_TypeCodeFactory * factory,
          const reflex::match::Sparse<Args...> *)
      {
          DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
          SafeTypeCode<reflex::match::Sparse<Args...>> sparseTc(
            factory,
            factory->create_sparse_tc(StructName<reflex::match::Sparse<Args...>>::get().c_str(), 
                                      DDS_VM_NONE,
                                      NULL, 
                                      ex));

          check_exception_code(
            "get_typecode<Sparse>: Unable to create sparse typecode, error = ",
            ex);

          typedef typename 
            ::reflex::match::Sparse<Args...>::raw_tuple_type RawTuple;

          TypelistIterator<RawTuple,
                           0,
                           Size<RawTuple>::value - 1>::add(
                             factory, sparseTc.get());

          return sparseTc;
      }
    
      template <class TagType, class... Cases>
      static SafeTypeCode<reflex::match::Union<TagType, Cases...>>
        get_typecode(
            DDS_TypeCodeFactory * factory,
            const reflex::match::Union<TagType, Cases...> *)
      {
          SafeTypeCode<TagType> discTc =
            TC_overload_resolution_helper::get_typecode(
                factory, 
                static_cast<TagType *>(0));

          typedef typename 
            reflex::match::Union<TagType, Cases...>::case_tuple_type CaseTuple;

          DDS_UnionMemberSeq umember_seq;
          const size_t ncases = Size<CaseTuple>::value;
          umember_seq.ensure_length(ncases, ncases);

          TC_overload_resolution_helper::add_union_members(
              factory,
              umember_seq,
              static_cast<reflex::match::Union<TagType, Cases...> *>(0));

          DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
          SafeTypeCode<reflex::match::Union<TagType, Cases...>> unionTc(
            factory,
            factory_->create_union_tc(StructName<reflex::match::Union<TagType, Cases...>>::get().c_str(),
                                      discTc.get(),
                                      detail::DefaultCaseIndex<match::Union<TagType, Cases...>>::value,
                                      umember_seq,
                                      ex));

          check_exception_code(
            "get_typecode<Union>: Unable to create union typecode, error = ",
            ex);

          TypelistIterator<CaseTuple,
            0,
            Size<CaseTuple>::value - 1>::delete_typecodes(factory, umember_seq);

          return unionTc;
      }

      template<class T>
      static SafeTypeCode<T> get_typecode(
        DDS_TypeCodeFactory * factory,
        T *,
        typename detail::enable_if<is_builtin_array<T>::value>::type * = 0)
      {
        typedef typename remove_all_extents<T>::type InnerType;

        SafeTypeCode<InnerType> innerTc =
          TC_overload_resolution_helper::get_typecode(
          factory,
          static_cast<InnerType *>(0));

        typedef typename make_dim_list<T>::type DimList;

        DDS_UnsignedLongSeq dims;
        dims.ensure_length(DimList::size, DimList::size);
        detail::CopyDims<0, DimList>::exec(dims);

        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
        SafeTypeCode<T> arrayTc(
          factory,
          factory->create_array_tc(dims, innerTc.get(), ex));

        check_exception_code(
          "add_member_impl<BuiltInArray>: Unable to create array typecode, error = ",
          ex);

        return arrayTc;
      }

      private:

      template <class TagType, class... Cases>
      static void add_union_members(
          DDS_TypeCodeFactory * factory,
          DDS_UnionMemberSeq & seq,
          const reflex::match::Union<TagType, Cases...> *)
      {
        typedef typename 
          reflex::match::Union<TagType, Cases...>::case_tuple_type CaseTuple;
        TypelistIterator<CaseTuple,
          0,
          Size<CaseTuple>::value - 1>::add_union_member(factory, seq);
      }

    }; // struct TC_overload_resolution_helper

    template <class T>
    void add_member_impl(
      DDS_TypeCodeFactory * factory,
      DDS_TypeCode * outerTc,
      const char * member_name,
      unsigned char flags,
      int id,
      T *)//,
      //typename disable_if<is_builtin_array<T>::value>::type * = 0)
    {
      DDS_ExceptionCode_t ex;
      SafeTypeCode<T> innerTc =
        TC_overload_resolution_helper::get_typecode(
            factory, 
            static_cast<T *>(0));

      DDS_Long member_id =
        (outerTc->kind(ex) == DDS_TK_SPARSE) ? id : DDS_TYPECODE_MEMBER_ID_INVALID;

      outerTc->add_member(member_name,
                          member_id,
                          innerTc.get(),
                          flags,
                          ex);
      
      detail::check_exception_code(
        "add_member_impl<T>: Unable to add inner typecode, error = ", 
        ex);
    }
    /*
    template <class T>
    void add_member_impl(
      DDS_TypeCodeFactory * factory,
      DDS_TypeCode * outerTc,
      const char * member_name,
      unsigned char flags,
      int id,
      const T *,
      typename enable_if<is_builtin_array<T>::value>::type * = 0)
    {
      // Simple overloading is not used to get typecode for built-in
      // arrays because square brackets must be expanded syntactically.
      // I.e., For N-dimensional array you have to write a template of N 
      // integral parameters because there is no way to say [0...N]. Sigh!
      typedef typename remove_all_extents<T>::type InnerType;

      SafeTypeCode<InnerType> innerTc =
        TC_overload_resolution_helper::get_typecode(
            factory, 
            static_cast<InnerType *>(0));

      typedef typename make_dim_list<T>::type DimList;

      DDS_UnsignedLongSeq dims;
      dims.ensure_length(DimList::size, DimList::size);
      detail::CopyDims<0, DimList::exec(dims);

      DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      SafeTypeCode<T> arrayTc(
        factory,
        factory->create_array_tc(dims, innerTc.get(), ex);

      check_exception_code(
        "add_member_impl<BuiltInArray>: Unable to create array typecode, error = ",
        ex);

      DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      outerTc->add_member(member_name,
                          DDS_TYPECODE_MEMBER_ID_INVALID,
                          arrayTc.get(),
                          flags,
                          ex);

      detail::check_exception_code(
        "add_member_impl<BuiltInArray>: Unable to add inner array typecode, error = ",
        ex);
    }
    */
    template <class T>
    void add_member_forward(
      DDS_TypeCodeFactory * factory,
      DDS_TypeCode * outerTc,
      const char * member_name,
      unsigned char flags,
      int id,
      const T * tptr)
    {
      if (is_optional<T>::value)
      {
        if (flags == DDS_TYPECODE_KEY_MEMBER)
        {
          std::stringstream stream;
          stream << "add_member_forward: optinal member "
                 << member_name
                 << " can't be a key.";
          throw std::runtime_error(stream.str());
        }
        else
          flags = DDS_TYPECODE_NONKEY_MEMBER;
      }
        
      add_member_impl(
        factory, 
        outerTc, 
        member_name, 
        flags, 
        id, 
        const_cast<T *>(tptr));
    }

    template <size_t I, class Case>
    struct LabelAdder;

    template <size_t I, class T, int Head, int... Tail>
    struct LabelAdder<I, match::Case<T, Head, Tail...>>
    {
      enum { count = 1 + sizeof...(Tail) };

      static void exec(DDS_LongSeq & seq)
      {
        seq[I] = Head;
        LabelAdder<I + 1, match::Case<T, Tail...>>::exec(seq);
      }
    };

    template <size_t I, class T, int Last>
    struct LabelAdder<I, match::Case<T, Last>>
    {
      enum { count = 1 };

      static void exec(DDS_LongSeq & seq)
      {
        seq[I] = Last;
      }
    };

    template <size_t I, class T>
    struct LabelAdder<I, match::Case<T>>
    {
      enum { count = 1 };

      static void exec(DDS_LongSeq & seq)
      {
        seq[I] = -1; // default member
      }
    };


    template <class Case>
    void case_add_impl(
        DDS_TypeCodeFactory * factory,
        const char * member_name,
        DDS_UnionMember & umember)//,
        //typename disable_if<
        //                    is_builtin_array<typename Case::type
        //                   >::value>::type * = 0)
    {
      DDS_LongSeq label_seq;
      label_seq.ensure_length(
          LabelAdder<0, Case>::count,
          LabelAdder<0, Case>::count);
      LabelAdder<0, Case>::exec(label_seq);

      umember.name = const_cast<char *>(member_name);
      umember.is_pointer =
        is_pointer<typename Case::type>::value;

      typedef typename remove_reference<typename Case::type>::type CaseTypeNoRef;
      // typecodes are deleted in get_typecode() overload for Union.
      umember.type =
        TC_overload_resolution_helper::get_typecode(
            factory, 
            static_cast<CaseTypeNoRef *>(0)).release();

      umember.labels = label_seq;
    };
    /*
    template <class Case>
    void case_add_impl(
        DDS_TypeCodeFactory * factory,
        const char * member_name,
        DDS_UnionMember & umember,
        typename enable_if<
                           is_builtin_array<typename Case::type>::value
                          >::type * = 0) 
    {
      DDS_LongSeq label_seq;
      label_seq.ensure_length(
          LabelAdder<0, Case>::count,
          LabelAdder<0, Case>::count);
      LabelAdder<0, Case>::exec(label_seq);

      umember.name = const_cast<char *>(member_name);
      umember.is_pointer =
        is_pointer<typename Case::type>::value;

      // typecodes is deleted in get_typecode() overload for Union.
      typedef typename
        remove_all_extents<typename Case::type>::type BasicType;

      SafeTypeCode<BasicType> basicTc =
        TC_overload_resolution_helper::get_typecode(
            factory, 
            static_cast<BasicType *>(0));

      SafeTypeCode<BasicType, 
                   typename make_dim_list<typename Case::type>::type>
        arrayTc(factory, basicTc);

      umember.type = arrayTc.release();

      umember.labels = label_seq;
    };
    */
    template <class Case>
    void case_add_forward(
        DDS_TypeCodeFactory * factory,
        const char * member_name,
        DDS_UnionMember & umember)
    {
      case_add_impl<Case>(factory, member_name, umember);
    }

    template <class T>
    void deleteTc_impl(
            DDS_TypeCodeFactory * factory,
            DDS_TypeCode * tc,
            typename enable_if<is_primitive<T>::value, T>::type * = 0)
    {
      // No-op
    }

    template <class T>
    void deleteTc_impl(
            DDS_TypeCodeFactory * factory,
            DDS_TypeCode * tc,
            typename disable_if<is_primitive<T>::value, T>::type * = 0)
    {
      DDS_ExceptionCode_t ex;
      factory->delete_tc(tc, ex);

      if (ex != DDS_NO_EXCEPTION_CODE)
      {
        std::cerr << "deleteTc: Unable to delete typecode, error = "
                  << detail::get_readable_ex_code(ex) 
                  << std::endl;
        // do not throw
      }
    }

    template <class TC>
    void deleteTc_forward(
            DDS_TypeCodeFactory * factory,
            DDS_TypeCode * tc)
    {
      deleteTc_impl<TC>(factory, tc);
    }

  } // namespace detail

} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
  #include "reflex/../../src/typecode_manip.cxx"
#endif

#endif // RTIREFLEX_TYPECODE_MANIP_H

