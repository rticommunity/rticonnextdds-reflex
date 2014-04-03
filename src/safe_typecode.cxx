/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "safe_typecode.h"

#define GET_TYPECODE_DEF(BASIC_TYPE, TYPECODE)                            \
const DDS_TypeCode * SafeTypeCodeBase::get_typecode(const BASIC_TYPE *) { \
  return factory_->get_primitive_tc(TYPECODE);                            \
}

GET_TYPECODE_DEF(octet_t,  DDS_TK_OCTET)
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

void SafeTypeCodeBase::set_typecode(const DDS_TypeCode * tc) {
  typecode_ = const_cast<DDS_TypeCode *>(tc);
  release_ = false; // Native type. Don't release
}

void SafeTypeCodeBase::set_typecode(DDS_TypeCode * tc) {
  typecode_ = tc;
  release_ = true;
}

SafeTypeCodeBase::SafeTypeCodeBase(DDS_TypeCode * typecode)
  : factory_(DDS_TypeCodeFactory::get_instance()),
    typecode_(typecode),
    release_(true)
{
  if (factory_ == NULL) {
    std::cerr << "Invalid DDS_TypeCodeFactory\n";
    throw 0;
  }
}

SafeTypeCodeBase::SafeTypeCodeBase(const DDS_TypeCode * typecode)
  : factory_(DDS_TypeCodeFactory::get_instance()),
    typecode_(const_cast<DDS_TypeCode *>(typecode)),
    release_(false)
{
  if (factory_ == NULL) {
    std::cerr << "Invalid DDS_TypeCodeFactory\n";
    throw 0;
  }
}

SafeTypeCodeBase::SafeTypeCodeBase(DDS_TypeCodeFactory * factory,
                                   DDS_TypeCode * typecode,
                                   bool release)
  : factory_(factory),
  typecode_(typecode),
  release_(release)
{
  if (factory_ == NULL) {
    std::cerr << "Invalid DDS_TypeCodeFactory\n";
    throw 0;
  }
}

void SafeTypeCodeBase::create_array_tc(DDS_TypeCode * inner,
                                       const DDS_UnsignedLongSeq &dims)
{
  DDS_ExceptionCode_t ex;
  DDS_TypeCode * arrTc =
    factory_->create_array_tc(dims, inner, ex);

  if (ex != DDS_NO_EXCEPTION_CODE) {
    std::cerr << "! Unable to create array typecode: " << ex
      << std::endl;
    throw 0;
  }

  set_typecode(arrTc);
}

void SafeTypeCodeBase::create_seq_tc(DDS_TypeCode *inner,
                                     size_t bound)
{
  DDS_ExceptionCode_t ex;
  DDS_TypeCode * seqTc =
    factory_->create_sequence_tc(bound, inner, ex);

  if (ex != DDS_NO_EXCEPTION_CODE) {
    std::cerr << "! Unable to create sequence typecode: " << ex
      << std::endl;
    throw 0;
  }

  set_typecode(seqTc);
}

void SafeTypeCodeBase::create_string_tc(size_t bound)
{
  DDS_ExceptionCode_t ex;
  DDS_TypeCode * stringTc =
    factory_->create_string_tc(bound, ex);

  if (ex != DDS_NO_EXCEPTION_CODE) {
    std::cerr << "! Unable to create string typecode: " << ex
      << std::endl;
    throw 0;
  }
  set_typecode(stringTc);
}

void SafeTypeCodeBase::create_struct_tc(const char * name)
{
  DDS_ExceptionCode_t ex;
  DDS_TypeCode *structTc =
    factory_->create_struct_tc(name,
    DDS_StructMemberSeq(), ex);
  if (ex != DDS_NO_EXCEPTION_CODE) {
    std::cerr << "! Unable to create struct typecode, error=" << ex
      << std::endl;
    throw 0;
  }

  set_typecode(structTc);
}

void SafeTypeCodeBase::create_enum_tc(const char * name, const DDS_EnumMemberSeq & enum_seq)
{
  DDS_ExceptionCode_t ex;
  DDS_TypeCode * enumTc = factory_->create_enum_tc(name, enum_seq, ex);

  if (ex != DDS_NO_EXCEPTION_CODE) {
    std::cerr << "! Unable to create struct typecode, error=" << ex
      << std::endl;
    throw 0;
  }

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
  if (ex != DDS_NO_EXCEPTION_CODE) {
    std::cerr << "Could not create union_tc\n";
    throw 0;
  }
  set_typecode(unionTc);
}

void SafeTypeCodeBase::create_sparse_tc(const char * name)
{
  DDS_ExceptionCode_t ex;
  DDS_TypeCode *sparseTc =
    factory_->create_sparse_tc(name, DDS_VM_NONE, NULL, ex);
  if (ex != DDS_NO_EXCEPTION_CODE) {
    std::cerr << "! Unable to create sparse typecode, error=" << ex
      << std::endl;
    throw 0;
  }

  set_typecode(sparseTc);
}

SafeTypeCodeBase::~SafeTypeCodeBase()
{
  DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
  if (factory_ && typecode_ && release_)
  {
    factory_->delete_tc(typecode_, ex);

    if (ex != DDS_NO_EXCEPTION_CODE) {
      std::cerr << " Unable to delete typecode, error=" << ex
        << std::endl;
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

void SafeTypeCodeBase::swap(SafeTypeCodeBase & stc) throw ()
{
  std::swap(factory_, stc.factory_);
  std::swap(typecode_, stc.typecode_);
  std::swap(release_, stc.release_);
}

SafeTypeCode<DDS_TypeCode>::SafeTypeCode(DDS_TypeCode * tc)
  : SafeTypeCodeBase(tc)
{}

SafeTypeCode<DDS_TypeCode>::SafeTypeCode(const DDS_TypeCode * tc)
  : SafeTypeCodeBase(tc)
{ }

SafeTypeCode<DDS_TypeCode>::SafeTypeCode(proxy< SafeTypeCode<DDS_TypeCode>> p) throw()
  : SafeTypeCodeBase(p.factory_,
                     p.typecode_,
                     p.release_)
{}

SafeTypeCode<DDS_TypeCode> & 
  SafeTypeCode<DDS_TypeCode>::operator = (proxy < SafeTypeCode < DDS_TypeCode >> p) throw ()
{
  SafeTypeCode<DDS_TypeCode>(p).swap(*this);
  return *this;
}

SafeTypeCode<DDS_TypeCode>::operator proxy<SafeTypeCode<DDS_TypeCode>> () throw()
{
  proxy<SafeTypeCode<DDS_TypeCode>> p { factory_, typecode_, release_ };
  typecode_ = 0;
  return p;
}

SafeTypeCode<DDS_TypeCode> move(SafeTypeCode<DDS_TypeCode> & stc)
{
  return SafeTypeCode<DDS_TypeCode>(proxy<SafeTypeCode<DDS_TypeCode>> (stc));
}

