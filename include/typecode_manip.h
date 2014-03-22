/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
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
#include "safe_typecode.h"
#include "default_member_names.h"
#include "enable_if.h"
#include "safe_enum.h"
#include "disc_union.h" 
#include "tuple_iterator.h"
#include "bounded.h"
#include "dllexport.h"

DllExport void DECLSPEC 
print_IDL(const DDS_TypeCode * tc,
          DDS_UnsignedLong indent);

DllExport SafeTypeCode<std::string> DECLSPEC
get_typecode(DDS_TypeCodeFactory * factory, const std::string *);

template <class T>
// overload for 
// 1. user-defined structs adapted as a Fusion Sequence
// 2. tuples.
SafeTypeCode<T> get_typecode(DDS_TypeCodeFactory * factory, 
                             const T *,
                             typename disable_if<std::is_enum<T>::value ||
                                                 is_primitive<T>::value ||
                                                 is_container<T>::value>::type * = 0)
{
  SafeTypeCode<T> 
    structTc(factory, StructName<T>::get().c_str());
  
  TypelistIterator<T, 
                   0, 
                   Size<T>::value-1>
                     ::add(factory, structTc.get());

  return move(structTc);
}

template <class T>
// overload for all the primitive types.
SafeTypeCode<T> get_typecode(DDS_TypeCodeFactory * factory, 
                             const T *,
                             typename enable_if<is_primitive<T>::value>::type * = 0)
{
  return SafeTypeCode<T>(factory);
}

template <class T>
// overload for user-defined enumeration types.
SafeTypeCode<T> get_typecode(DDS_TypeCodeFactory * factory, 
                             const T *,
                             typename enable_if<std::is_enum<T>::value>::type * = 0)
{
  return SafeTypeCode<T>(factory, EnumDef<T>::name());
}

template <class C>
SafeTypeCode<C> 
get_typecode(DDS_TypeCodeFactory * factory,
             const C *, 
             typename enable_if<is_container<C>::value>::type * = 0)
{
  SafeTypeCode<typename C::value_type> innerTc
    = get_typecode(factory, 
                   static_cast<typename C::value_type *>(0));

  return SafeTypeCode<C>(factory, innerTc);
}

template <class C, size_t Bound>
SafeTypeCode<bounded<C, Bound>> 
get_typecode(DDS_TypeCodeFactory * factory,
             const bounded<C, Bound> *,
             typename enable_if<is_container<C>::value>::type * = 0)
{
  SafeTypeCode<typename C::value_type> innerTc
    = get_typecode(factory, 
                   static_cast<typename C::value_type *>(0));

  return SafeTypeCode<bounded<C, Bound>>(factory, innerTc);
}

template <size_t Bound>
SafeTypeCode<bounded<std::string, Bound>> 
get_typecode(DDS_TypeCodeFactory * factory,
             const bounded<std::string, Bound> *)
{
  SafeTypeCode<std::string> innerTc 
    = get_typecode(factory, static_cast<std::string *>(0));

  return SafeTypeCode<bounded<std::string, Bound>>(factory);
}

template <class T>
SafeTypeCode<Range<T>>
  get_typecode(DDS_TypeCodeFactory * factory,
  const Range<T> *)
{
  SafeTypeCode<typename remove_reference<T>::type> innerTc
    = get_typecode(factory,
    static_cast<typename remove_reference<T>::type *>(0));

  return SafeTypeCode<Range<T>>(factory, innerTc);
}

template <class T, size_t Bound>
SafeTypeCode<BoundedRange<T, Bound>>
  get_typecode(DDS_TypeCodeFactory * factory,
  const BoundedRange<T, Bound> *)
{
  SafeTypeCode<typename remove_reference<T>::type> innerTc
    = get_typecode(factory,
    static_cast<typename remove_reference<T>::type *>(0));

  return SafeTypeCode<BoundedRange<T, Bound>>(factory, innerTc);
}

template <class T, size_t N>
SafeTypeCode<std::array<T, N>> 
get_typecode(DDS_TypeCodeFactory * factory,
             const std::array<T, N> *)
{
  typedef typename remove_all_extents<T>::type BasicType;
  SafeTypeCode<BasicType> basicTc 
    = get_typecode(factory, static_cast<BasicType *>(0));

  return SafeTypeCode<std::array<T, N>>(factory, basicTc);
}

template <class... Args>
SafeTypeCode<Sparse<Args...>>
get_typecode(DDS_TypeCodeFactory * factory,
             const Sparse<Args...> *)
{
  SafeTypeCode<Sparse<Args...>> 
    sparseTc(factory, StructName<Sparse<Args...>>::get().c_str());
  
  typedef typename Sparse<Args...>::raw_tuple_type RawTuple;
  TypelistIterator<RawTuple, 
                   0, 
                   Size<RawTuple>::value-1>
                     ::add(factory, sparseTc.get());

  return move(sparseTc);
}

template <class TagType, class... Cases>
void add_union_members(DDS_TypeCodeFactory * factory,
                       DDS_UnionMemberSeq & seq,
                       const Union<TagType, Cases...> *)
{
  typedef typename Union<TagType, Cases...>::case_tuple_type CaseTuple;
  TypelistIterator<CaseTuple, 
                   0, 
                   Size<CaseTuple>::value-1>::add_union_member(factory, seq);
}

template <class TagType, class... Cases>
SafeTypeCode<Union<TagType, Cases...>>
get_typecode(DDS_TypeCodeFactory * factory,
             const Union<TagType, Cases...> *)
{
  SafeTypeCode<TagType> discriminatorTc = 
    get_typecode(factory, static_cast<TagType *>(0));

  typedef typename Union<TagType, Cases...>::case_tuple_type CaseTuple;
  DDS_UnionMemberSeq umember_seq;
  const size_t ncases = Size<CaseTuple>::value;
  umember_seq.ensure_length(ncases, ncases);

  add_union_members(factory,
                    umember_seq, 
                    static_cast<Union<TagType, Cases...> *>(0));
  
  SafeTypeCode<Union<TagType, Cases...>> 
    unionTc(factory,
            StructName<Union<TagType, Cases...>>::get().c_str(),
            discriminatorTc,
            umember_seq);

  TypelistIterator<CaseTuple,
                   0,
                   Size<CaseTuple>::value-1>::delete_typecodes(factory, umember_seq);

  return move(unionTc);
}

template <class T>
void add_member(DDS_TypeCodeFactory * factory,
                DDS_TypeCode * outerTc,
                const char * member_name,
                unsigned char flags,
                int id,
                const T *,
                typename disable_if<is_builtin_array<T>::value>::type *)
{
  DDS_ExceptionCode_t ex;
  SafeTypeCode<T> innerTc = 
    get_typecode(factory, static_cast<T *>(0));
  
  DDS_Long member_id = 
    (outerTc->kind(ex) == DDS_TK_SPARSE)? id : DDS_TYPECODE_MEMBER_ID_INVALID;

  outerTc->add_member(member_name,
                      member_id,
                      innerTc.get(),
                      flags,
                      ex);
  if (ex != DDS_NO_EXCEPTION_CODE) {
      std::cerr << "! Unable to add inner typecode, error=" 
                << get_readable_ex_code(ex) << std::endl;
      throw 0;
  }
}

template <class T>
void add_member(DDS_TypeCodeFactory * factory,
                DDS_TypeCode * outerTc,
                const char * member_name,
                unsigned char flags,
                int id,
                const T *,
                typename enable_if<is_builtin_array<T>::value>::type *)
{
  // Simple overloading is not used to get typecode for built-in
  // arrays because square brackets must be expanded syntactically.
  // I.e., For N-dimensional array you have to write a template of N 
  // paramters because there is no way to say [0...N]. Sigh!
  typedef typename remove_all_extents<T>::type BasicType;

  SafeTypeCode<BasicType> basicTc = 
    get_typecode(factory, static_cast<BasicType *>(0));

  SafeTypeCode<BasicType, typename make_dim_list<T>::type> 
    arrayTc(factory, basicTc);
  
  DDS_ExceptionCode_t ex;
  outerTc->add_member(member_name,
                      DDS_TYPECODE_MEMBER_ID_INVALID,
                      arrayTc.get(),
                      flags,
                      ex);
  if (ex != DDS_NO_EXCEPTION_CODE) {
      std::cerr << "! Unable to add inner array typecode, error=" << ex 
                      << std::endl;
      throw 0;
  }
}

template <size_t I, class Case>
struct LabelAdder;

template <size_t I, class T, int Head, int... Tail>
struct LabelAdder<I, Case<T, Head, Tail...>>
{
  enum { count = 1 + sizeof...(Tail) };

  static void exec(DDS_LongSeq & seq)
  {
    seq[I] = Head;
    LabelAdder<I+1, Case<T, Tail...>>::exec(seq);
  }
};

template <size_t I, class T, int Last>
struct LabelAdder<I, Case<T, Last>>
{
  enum { count = 1 };

  static void exec(DDS_LongSeq & seq)
  {
    seq[I] = Last;
  }
};

template <size_t I, class T>
struct LabelAdder<I, Case<T>>
{
  enum { count = 1 };

  static void exec(DDS_LongSeq & seq)
  {
    seq[I] = -1; // default member
  }
};


template <class Case>
void case_add(DDS_TypeCodeFactory * factory, 
              const char * member_name,
              DDS_UnionMember & umember,
              typename disable_if<is_builtin_array<typename Case::type>::value>::type *)
{
  DDS_LongSeq label_seq;
  label_seq.ensure_length(LabelAdder<0, Case>::count, 
                          LabelAdder<0, Case>::count);
  LabelAdder<0, Case>::exec(label_seq);

  umember.name = const_cast<char *>(member_name);
  umember.is_pointer = 
    is_pointer<typename Case::type>::value;

  typedef typename remove_reference<typename Case::type>::type CaseTypeNoRef;
  // typecodes is deleted in get_typecode() overload for Union.
  umember.type = 
    get_typecode(factory, static_cast<CaseTypeNoRef *>(0)).release();

  umember.labels = label_seq;
};

template <class Case>
void case_add(DDS_TypeCodeFactory * factory, 
              const char * member_name,
              DDS_UnionMember & umember,
              typename enable_if<is_builtin_array<typename Case::type>::value>::type *)
{
  DDS_LongSeq label_seq;
  label_seq.ensure_length(LabelAdder<0, Case>::count, 
                          LabelAdder<0, Case>::count);
  LabelAdder<0, Case>::exec(label_seq);

  umember.name = const_cast<char *>(member_name);
  umember.is_pointer = 
    is_pointer<typename Case::type>::value;

  // typecodes is deleted in get_typecode() overload for Union.
  typedef typename 
    remove_all_extents<typename Case::type>::type BasicType;
 
  SafeTypeCode<BasicType> basicTc = 
    get_typecode(factory, static_cast<BasicType *>(0));
  
  SafeTypeCode<BasicType, typename make_dim_list<typename Case::type>::type> 
    arrayTc(factory, basicTc);

  umember.type = arrayTc.release();

  umember.labels = label_seq;
};

template <class T>
void deleteTc(DDS_TypeCodeFactory * factory, 
              DDS_TypeCode * tc,
              const T *, // Can't combine para 3 & 4 because SFINAE is non-deducible context.
              typename enable_if<is_primitive<T>::value, T>::type *)
{
  // No-op
}

template <class T>
void deleteTc(DDS_TypeCodeFactory * factory, 
              DDS_TypeCode * tc,
              const T *, // Can't combine para 3 & 4 because SFINAE is non-deducible context.
              typename disable_if<is_primitive<T>::value, T>::type *)
{
  DDS_ExceptionCode_t ex;
  factory->delete_tc(tc, ex);
  if(DDS_NO_EXCEPTION_CODE != ex) {
    std::cerr << "Unable to delete typecode\n";
    throw 0;
  }
}


#endif // RTIREFLEX_TYPECODE_MANIP_H

