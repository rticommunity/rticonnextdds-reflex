/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/
#include "reflex/safe_typecode.h"

/*
#include "reflex/dllexport.h"
#include "reflex/safe_typecode.h"

#define GET_TYPECODE_DEF(BASIC_TYPE, TYPECODE)          \
REFLEX_INLINE const DDS_TypeCode *                      \
  InnerTypeCodeBase::get_typecode(const BASIC_TYPE *) {  \
    return factory_->get_primitive_tc(TYPECODE);        \
}

namespace reflex {

  namespace detail {

      GET_TYPECODE_DEF(match::octet_t,  DDS_TK_OCTET)

      GET_TYPECODE_DEF(bool,     DDS_TK_BOOLEAN)
      GET_TYPECODE_DEF(char,     DDS_TK_CHAR)
      GET_TYPECODE_DEF(int8_t,   DDS_TK_CHAR)
      GET_TYPECODE_DEF(int16_t,  DDS_TK_SHORT)
      GET_TYPECODE_DEF(uint16_t, DDS_TK_USHORT)
      GET_TYPECODE_DEF(int32_t,  DDS_TK_LONG)
      GET_TYPECODE_DEF(uint32_t, DDS_TK_ULONG)
      GET_TYPECODE_DEF(int64_t,  DDS_TK_LONGLONG)
      GET_TYPECODE_DEF(uint64_t, DDS_TK_ULONGLONG)

#ifndef RTI_WIN32
      GET_TYPECODE_DEF(char32_t, DDS_TK_WCHAR)
#endif

      GET_TYPECODE_DEF(float, DDS_TK_FLOAT)
      GET_TYPECODE_DEF(double, DDS_TK_DOUBLE)
#if __x86_64__
      GET_TYPECODE_DEF(long long, DDS_TK_LONGLONG)
      GET_TYPECODE_DEF(unsigned long long, DDS_TK_ULONGLONG)
#endif 
      GET_TYPECODE_DEF(long double, DDS_TK_LONGDOUBLE)

#undef GET_TYPECODE_DEF

      REFLEX_INLINE void InnerTypeCodeBase::set_typecode(const DDS_TypeCode * tc) {
        typecode_ = const_cast<DDS_TypeCode *>(tc);
        release_ = false; // Native type. Don't release
      }

      REFLEX_INLINE void InnerTypeCodeBase::set_typecode(DDS_TypeCode * tc) {
        typecode_ = tc;
        release_ = true;
      }

      REFLEX_INLINE void check_factory(DDS_TypeCodeFactory * factory) {
        if (factory == NULL) {
          throw std::runtime_error("InnerTypeCodeBase: Invalid DDS_TypeCodeFactory");
        }
      }

      REFLEX_INLINE InnerTypeCodeBase::InnerTypeCodeBase(DDS_TypeCode * typecode)
      : factory_(DDS_TypeCodeFactory::get_instance()),
        typecode_(typecode),
        release_(true)
    {
      check_factory(factory_);
    }

      REFLEX_INLINE InnerTypeCodeBase::InnerTypeCodeBase(const DDS_TypeCode * typecode)
      : factory_(DDS_TypeCodeFactory::get_instance()),
        typecode_(const_cast<DDS_TypeCode *>(typecode)),
        release_(false)
    {
      check_factory(factory_);
    }

      REFLEX_INLINE InnerTypeCodeBase::InnerTypeCodeBase(DDS_TypeCodeFactory * factory,
                                       DDS_TypeCode * typecode,
                                       bool release)
      : factory_(factory),
        typecode_(typecode),
        release_(release)
    {
      check_factory(factory_);
    }

      REFLEX_INLINE void InnerTypeCodeBase::create_array_tc(DDS_TypeCode * inner,
      const DDS_UnsignedLongSeq &dims)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * arrTc =
        factory_->create_array_tc(dims, inner, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_array_tc: Unable to create array typecode, error = ", 
        ex);

      set_typecode(arrTc);
    }

      REFLEX_INLINE void InnerTypeCodeBase::create_seq_tc(DDS_TypeCode *inner,
      size_t bound)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * seqTc =
        factory_->create_sequence_tc(bound, inner, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_seq_tc: Unable to create sequence typecode, error = ",
        ex);

      set_typecode(seqTc);
    }

      REFLEX_INLINE void InnerTypeCodeBase::create_string_tc(size_t bound)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * stringTc =
        factory_->create_string_tc(bound, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_string_tc: Unable to create string typecode, error = ",
        ex);

      set_typecode(stringTc);
    }

      REFLEX_INLINE void InnerTypeCodeBase::create_struct_tc(const char * name)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode *structTc =
        factory_->create_struct_tc(name,
        DDS_StructMemberSeq(), ex);

      check_exception_code(
        "InnerTypeCodeBase::create_struct_tc: Unable to create struct typecode, error = ",
        ex);

      set_typecode(structTc);
    }

      REFLEX_INLINE void InnerTypeCodeBase::create_value_tc(
        const char * name, 
        DDS_TypeCode * basetc)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode *valueTc =
        factory_->create_value_tc(
          name,
          DDS_EXTENSIBLE_EXTENSIBILITY,
          DDS_VM_NONE,
          basetc,
          DDS_ValueMemberSeq(), 
          ex);

      check_exception_code(
        "InnerTypeCodeBase::create_value_tc: Unable to create valuetype typecode, error = ",
        ex);

      set_typecode(valueTc);
    }

      REFLEX_INLINE void InnerTypeCodeBase::create_enum_tc(const char * name,
      const DDS_EnumMemberSeq & enum_seq,
      const std::vector<MemberInfo> &)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * enumTc = factory_->create_enum_tc(name, enum_seq, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_enum_tc: Unable to create enum typecode, error = ",
        ex);

      set_typecode(enumTc);
    }

      REFLEX_INLINE void InnerTypeCodeBase::create_union_tc(const char * name,
      DDS_TypeCode * discTc,
      DDS_UnsignedLong default_case,
      DDS_UnionMemberSeq & member_seq)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * unionTc =
        factory_->create_union_tc(name,
        discTc,
        default_case,
        member_seq,
        ex);
      
      check_exception_code(
        "InnerTypeCodeBase::create_union_tc: Unable to create union typecode, error = ",
        ex);

      set_typecode(unionTc);
    }

      REFLEX_INLINE void InnerTypeCodeBase::create_sparse_tc(const char * name)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode *sparseTc =
        factory_->create_sparse_tc(name, DDS_VM_NONE, NULL, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_sparse_tc: Unable to create sparse typecode, error = ",
        ex);

      set_typecode(sparseTc);
    }

      REFLEX_INLINE InnerTypeCodeBase::~InnerTypeCodeBase()
    {
      DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      if (factory_ && typecode_ && release_)
      {
        factory_->delete_tc(typecode_, ex);

        if (ex != DDS_NO_EXCEPTION_CODE) 
        {
          std::cerr << "~InnerTypeCodeBase: Unable to delete typecode, error = "
                    << get_readable_ex_code(ex) << std::endl;
          // Do not throw.
        }
      }
    }

      REFLEX_INLINE const DDS_TypeCode * InnerTypeCodeBase::get() const
      {
        return typecode_;
      }

      REFLEX_INLINE DDS_TypeCode * InnerTypeCodeBase::get()
      {
        return typecode_;
      }

      REFLEX_INLINE DDS_TypeCode * InnerTypeCodeBase::release() {
        release_ = false;
        return typecode_;
      }

      REFLEX_INLINE void InnerTypeCodeBase::swap(InnerTypeCodeBase & stc) throw()
      {
        std::swap(factory_, stc.factory_);
        std::swap(typecode_, stc.typecode_);
        std::swap(release_, stc.release_);
      }

    REFLEX_INLINE InnerTypeCode<DDS_TypeCode>::InnerTypeCode(DDS_TypeCode * tc)
      : InnerTypeCodeBase(tc)
    {}

    REFLEX_INLINE InnerTypeCode<DDS_TypeCode>::InnerTypeCode(const DDS_TypeCode * tc)
      : InnerTypeCodeBase(tc)
    { }

    REFLEX_INLINE InnerTypeCode<DDS_TypeCode>::InnerTypeCode(
        proxy< InnerTypeCode<DDS_TypeCode>> p) throw()
      : InnerTypeCodeBase(
           p.factory_,
           p.typecode_,
           p.release_)
    {}

    REFLEX_INLINE InnerTypeCode<DDS_TypeCode> &
      InnerTypeCode<DDS_TypeCode>::operator = (
        proxy < InnerTypeCode < DDS_TypeCode >> p) throw()
    {
        InnerTypeCode<DDS_TypeCode>(p).swap(*this);
        return *this;
      }

    REFLEX_INLINE InnerTypeCode<DDS_TypeCode>::operator
      proxy<InnerTypeCode<DDS_TypeCode>>() throw()
    {
      detail::proxy<InnerTypeCode<DDS_TypeCode>> p{ factory_, typecode_, release_ };
      typecode_ = 0;
      return p;
    }

    REFLEX_INLINE InnerTypeCode<DDS_TypeCode> move(InnerTypeCode<DDS_TypeCode> & stc)
    {
      return InnerTypeCode<DDS_TypeCode>(detail::proxy<InnerTypeCode<DDS_TypeCode>>(stc));
    }

    } // namespace detail
} // namespace reflex
*/