/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "reflex/safe_typecode.h"

#define GET_TYPECODE_DEF(BASIC_TYPE, TYPECODE)                            \
const DDS_TypeCode * SafeTypeCodeBase::get_typecode(const BASIC_TYPE *) { \
  return factory_->get_primitive_tc(TYPECODE);                            \
}

namespace reflex {

  namespace detail {

      GET_TYPECODE_DEF(octet_t, DDS_TK_OCTET)
      GET_TYPECODE_DEF(bool, DDS_TK_BOOLEAN)
      GET_TYPECODE_DEF(char, DDS_TK_CHAR)
      GET_TYPECODE_DEF(int8_t, DDS_TK_CHAR)
      GET_TYPECODE_DEF(int16_t, DDS_TK_SHORT)
      GET_TYPECODE_DEF(uint16_t, DDS_TK_USHORT)
      GET_TYPECODE_DEF(int32_t, DDS_TK_LONG)
      GET_TYPECODE_DEF(uint32_t, DDS_TK_ULONG)
      GET_TYPECODE_DEF(int64_t, DDS_TK_LONGLONG)
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

      void SafeTypeCodeBase::set_typecode(const DDS_TypeCode * tc) {
        typecode_ = const_cast<DDS_TypeCode *>(tc);
        release_ = false; // Native type. Don't release
      }

    void SafeTypeCodeBase::set_typecode(DDS_TypeCode * tc) {
      typecode_ = tc;
      release_ = true;
    }

    void check_factory(DDS_TypeCodeFactory * factory) {
      if (factory == NULL) {
        throw std::runtime_error("SafeTypeCodeBase: Invalid DDS_TypeCodeFactory");
      }
    }

    SafeTypeCodeBase::SafeTypeCodeBase(DDS_TypeCode * typecode)
      : factory_(DDS_TypeCodeFactory::get_instance()),
        typecode_(typecode),
        release_(true)
    {
      check_factory(factory_);
    }

    SafeTypeCodeBase::SafeTypeCodeBase(const DDS_TypeCode * typecode)
      : factory_(DDS_TypeCodeFactory::get_instance()),
        typecode_(const_cast<DDS_TypeCode *>(typecode)),
        release_(false)
    {
      check_factory(factory_);
    }

    SafeTypeCodeBase::SafeTypeCodeBase(DDS_TypeCodeFactory * factory,
                                       DDS_TypeCode * typecode,
                                       bool release)
      : factory_(factory),
        typecode_(typecode),
        release_(release)
    {
      check_factory(factory_);
    }

    void SafeTypeCodeBase::create_array_tc(DDS_TypeCode * inner,
      const DDS_UnsignedLongSeq &dims)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * arrTc =
        factory_->create_array_tc(dims, inner, ex);

      check_exception_code(
        "SafeTypeCodeBase::create_array_tc: Unable to create array typecode, error = ", 
        ex);

      set_typecode(arrTc);
    }

    void SafeTypeCodeBase::create_seq_tc(DDS_TypeCode *inner,
      size_t bound)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * seqTc =
        factory_->create_sequence_tc(bound, inner, ex);

      check_exception_code(
        "SafeTypeCodeBase::create_seq_tc: Unable to create sequence typecode, error = ",
        ex);

      set_typecode(seqTc);
    }

    void SafeTypeCodeBase::create_string_tc(size_t bound)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * stringTc =
        factory_->create_string_tc(bound, ex);

      check_exception_code(
        "SafeTypeCodeBase::create_string_tc: Unable to create string typecode, error = ",
        ex);

      set_typecode(stringTc);
    }

    void SafeTypeCodeBase::create_struct_tc(const char * name)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode *structTc =
        factory_->create_struct_tc(name,
        DDS_StructMemberSeq(), ex);

      check_exception_code(
        "SafeTypeCodeBase::create_struct_tc: Unable to create struct typecode, error = ",
        ex);

      set_typecode(structTc);
    }

    void SafeTypeCodeBase::create_value_tc(
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
        "SafeTypeCodeBase::create_value_tc: Unable to create valuetype typecode, error = ",
        ex);

      set_typecode(valueTc);
    }

    void SafeTypeCodeBase::create_enum_tc(const char * name,
      const DDS_EnumMemberSeq & enum_seq,
      const std::vector<MemberInfo> &)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * enumTc = factory_->create_enum_tc(name, enum_seq, ex);

      check_exception_code(
        "SafeTypeCodeBase::create_enum_tc: Unable to create enum typecode, error = ",
        ex);

      set_typecode(enumTc);
    }

    void SafeTypeCodeBase::create_union_tc(const char * name,
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
        "SafeTypeCodeBase::create_union_tc: Unable to create union typecode, error = ",
        ex);

      set_typecode(unionTc);
    }

    void SafeTypeCodeBase::create_sparse_tc(const char * name)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode *sparseTc =
        factory_->create_sparse_tc(name, DDS_VM_NONE, NULL, ex);

      check_exception_code(
        "SafeTypeCodeBase::create_sparse_tc: Unable to create sparse typecode, error = ",
        ex);

      set_typecode(sparseTc);
    }

    SafeTypeCodeBase::~SafeTypeCodeBase()
    {
      DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      if (factory_ && typecode_ && release_)
      {
        factory_->delete_tc(typecode_, ex);

        if (ex != DDS_NO_EXCEPTION_CODE) 
        {
          std::cerr << "~SafeTypeCodeBase: Unable to delete typecode, error = "
                    << get_readable_ex_code(ex) << std::endl;
          // Do not throw.
        }
      }
    }

    DDS_TypeCode * SafeTypeCodeBase::get() const
    {
      return typecode_;
    }

    DDS_TypeCode * SafeTypeCodeBase::release() {
      release_ = false;
      return typecode_;
    }

    void SafeTypeCodeBase::swap(SafeTypeCodeBase & stc) throw()
    {
      std::swap(factory_, stc.factory_);
      std::swap(typecode_, stc.typecode_);
      std::swap(release_, stc.release_);
    }

  } // namespace detail

  SafeTypeCode<DDS_TypeCode>::SafeTypeCode(DDS_TypeCode * tc)
    : SafeTypeCodeBase(tc)
  {}

  SafeTypeCode<DDS_TypeCode>::SafeTypeCode(const DDS_TypeCode * tc)
    : SafeTypeCodeBase(tc)
  { }

  SafeTypeCode<DDS_TypeCode>::SafeTypeCode(
      detail::proxy< SafeTypeCode<DDS_TypeCode>> p) throw()
    : SafeTypeCodeBase(
         p.factory_,
         p.typecode_,
         p.release_)
  {}

  SafeTypeCode<DDS_TypeCode> &
    SafeTypeCode<DDS_TypeCode>::operator = (
      detail::proxy < SafeTypeCode < DDS_TypeCode >> p) throw()
  {
      SafeTypeCode<DDS_TypeCode>(p).swap(*this);
      return *this;
    }

  SafeTypeCode<DDS_TypeCode>::operator 
    detail::proxy<SafeTypeCode<DDS_TypeCode>>() throw()
  {
    detail::proxy<SafeTypeCode<DDS_TypeCode>> p{ factory_, typecode_, release_ };
    typecode_ = 0;
    return p;
  }

  SafeTypeCode<DDS_TypeCode> move(SafeTypeCode<DDS_TypeCode> & stc)
  {
    return SafeTypeCode<DDS_TypeCode>(detail::proxy<SafeTypeCode<DDS_TypeCode>>(stc));
  }

} // namespace reflex
