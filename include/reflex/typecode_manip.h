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
#include "reflex/safe_enum.h"
#include "reflex/disc_union.h" 
#include "reflex/tuple_iterator.h"
#include "reflex/bounded.h"
#include "reflex/dllexport.h"

namespace reflex {

  /**
  * Creates a <a href="http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structDDS__TypeCode.html">TypeCode</a>
  * for structured type T.
  * @param name Optional name for top-level struct type. 
  *        If none is provided, the unqualified name of type T is used.
  * @return The TypeCode for type T wrapped in a SafeTypeCode object.
  */
  template <class T>
  SafeTypeCode<T> make_typecode(const char * name = 0);

  namespace detail {

    DllExport void DECLSPEC
      print_IDL(
      const DDS_TypeCode * tc,
      DDS_UnsignedLong indent);

    template <class T>
    SafeTypeCode<T> make_typecode_impl(
      const char * name,
      false_type /* T has no base */)
    {
      DDS_TypeCodeFactory * factory =
        DDS_TypeCodeFactory::get_instance();

      std::string type_name_string =
        detail::StructName<typename detail::remove_refs<T>::type>::get();

      const char * type_name =
        name ? name : type_name_string.c_str();

      SafeTypeCode<T> structTc(factory, type_name);

      detail::TypelistIterator<
        T,
        0,
        detail::Size<T>::value - 1>::add(
        factory, structTc.get());

      return move(structTc);
    }

    template <class T>
    SafeTypeCode<T> make_typecode_impl(
      const char * name,
      true_type /* T has base */)
    {
      DDS_TypeCodeFactory * factory =
        DDS_TypeCodeFactory::get_instance();

      std::string type_name_string =
        detail::StructName<typename detail::remove_refs<T>::type>::get();

      const char * type_name =
        name ? name : type_name_string.c_str();

      typedef typename InheritanceTraits<T>::basetype BaseType;

      SafeTypeCode<BaseType> baseTc =
        make_typecode_impl<BaseType>(
        0,
        typename InheritanceTraits<BaseType>::has_base());

      SafeTypeCode<T> aggregateTc(factory, type_name, baseTc.get());

      detail::TypelistIterator<
        T,
        0,
        detail::Size<T>::value - 1>
        ::add(factory, aggregateTc.get());

      return move(aggregateTc);
    }


    /* TypeCode Overload Resolution Helper */
    /* Overload resolution of several free primary template 
     * functions can be notoriously brittle because of the 
     * ordering issues. If general, a function must be declared
     * before it is called. Sounds simple but in a set of mutually
     * recursive primary template functions, it is easy to lose
     * grip on the what's declared before and what's not. In 
     * general, all the functions should be declared beforehand.
     * There is an alternative solution. Put all the functions
     * in a class and make them static and use their fully
     * qualified name to call them. No upfront declarations 
     * are necessary if this idiom is used. 
     * */
    struct TC_overload_resolution_helper 
    {
      /*
      DllExport static SafeTypeCode<std::string> DECLSPEC
        get_typecode(
            DDS_TypeCodeFactory * factory,
            const std::string *);
      */
      
      template <class Str>
      static SafeTypeCode<Str> 
        get_typecode(
            DDS_TypeCodeFactory * factory,
            const Str *,
            typename enable_if<is_string<Str>::value>::type * = 0)
      {
         return SafeTypeCode<Str>(factory);
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
        SafeTypeCode<T> structTc = reflex::make_typecode<T>();
        /*SafeTypeCode<T>
          structTc(factory, StructName<T>::get().c_str());

        TypelistIterator<
          T,
          0,
          Size<T>::value - 1>
            ::add(factory, structTc.get());*/

        return move(structTc);
      }

      template <class T>
      // overload for all the primitive types.
      static SafeTypeCode<T> get_typecode(
        DDS_TypeCodeFactory * factory,
        const T *,
        typename enable_if<is_primitive<T>::value>::type * = 0)
      {
        return SafeTypeCode<T>(factory);
      }

      template <class T>
      // overload for user-defined enumeration types.
      static SafeTypeCode<T> get_typecode(
        DDS_TypeCodeFactory * factory,
        const T *,
        typename enable_if<detail::is_enum<T>::value>::type * = 0)
      {
        return SafeTypeCode<T>(factory, EnumDef<T>::name());
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

          return SafeTypeCode<reflex::match::Bounded<C, Bound>>(factory, innerTc);
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

          return SafeTypeCode<C>(factory, innerTc);
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

          return SafeTypeCode<std::array<T, N>>(factory, basicTc);
      }


      template <size_t Bound>
      static SafeTypeCode<reflex::match::Bounded<std::string, Bound>>
        get_typecode(
            DDS_TypeCodeFactory * factory,
            const reflex::match::Bounded<std::string, Bound> *)
      {
          SafeTypeCode<std::string> innerTc
            = TC_overload_resolution_helper::get_typecode(
                factory, 
                static_cast<std::string *>(0));

          return SafeTypeCode<reflex::match::Bounded<std::string, Bound>>(factory);
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

        return SafeTypeCode<reflex::match::Range<T>>(factory, innerTc);
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

        return SafeTypeCode<Opt>(factory, move(innerTc));
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

          return SafeTypeCode<reflex::match::BoundedRange<T, Bound>>(
              factory, innerTc);
      }
      
      template <class... Args>
      static SafeTypeCode<::reflex::match::Sparse<Args...>> get_typecode(
          DDS_TypeCodeFactory * factory,
          const ::reflex::match::Sparse<Args...> *)
      {
          SafeTypeCode<::reflex::match::Sparse<Args...>>
            sparseTc(factory, 
                     StructName<::reflex::match::Sparse<Args...>>::get().c_str());

          typedef typename 
            ::reflex::match::Sparse<Args...>::raw_tuple_type RawTuple;

          TypelistIterator<RawTuple,
                           0,
                           Size<RawTuple>::value - 1>::add(
                             factory, sparseTc.get());

          return move(sparseTc);
      }
    
      template <class TagType, class... Cases>
      static SafeTypeCode<reflex::match::Union<TagType, Cases...>>
        get_typecode(
            DDS_TypeCodeFactory * factory,
            const reflex::match::Union<TagType, Cases...> *)
      {
          SafeTypeCode<TagType> discriminatorTc =
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

          SafeTypeCode<reflex::match::Union<TagType, Cases...>>
            unionTc(factory,
            StructName<reflex::match::Union<TagType, Cases...>>::get().c_str(),
            discriminatorTc,
            umember_seq);

          TypelistIterator<CaseTuple,
            0,
            Size<CaseTuple>::value - 1>::delete_typecodes(factory, umember_seq);

          return move(unionTc);
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
      const T *,
      typename disable_if<is_builtin_array<T>::value>::type * = 0)
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
        "add_member: Unable to add inner typecode, error = ", 
        ex);
    }

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
      // paramters because there is no way to say [0...N]. Sigh!
      typedef typename remove_all_extents<T>::type BasicType;

      SafeTypeCode<BasicType> basicTc =
        TC_overload_resolution_helper::get_typecode(
            factory, 
            static_cast<BasicType *>(0));

      SafeTypeCode<BasicType, typename make_dim_list<T>::type>
        arrayTc(factory, basicTc);

      DDS_ExceptionCode_t ex;
      outerTc->add_member(member_name,
        DDS_TYPECODE_MEMBER_ID_INVALID,
        arrayTc.get(),
        flags,
        ex);

      detail::check_exception_code(
        "add_member: Unable to add inner array typecode, error = ",
        ex);
    }

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
        DDS_UnionMember & umember,
        typename disable_if<
                            is_builtin_array<typename Case::type
                           >::value>::type * = 0)
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
      // typecodes is deleted in get_typecode() overload for Union.
      umember.type =
        TC_overload_resolution_helper::get_typecode(
            factory, 
            static_cast<CaseTypeNoRef *>(0)).release();

      umember.labels = label_seq;
    };

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

#endif // RTIREFLEX_TYPECODE_MANIP_H

