/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DD_TRAITS_H
#define RTIREFLEX_DD_TRAITS_H

#include "ndds/ndds_cpp.h"

#define DYNAMIC_DATA_SEQ_TRAITS(BASIC_TYPE,DDS_SEQ_TYPE) \
template <>                                              \
struct DynamicDataSeqTraits<BASIC_TYPE> {                \
  typedef DDS_SEQ_TYPE type;                             \
};

typedef unsigned char octet_t;

template <class T> // when T is an enum
struct DynamicDataSeqTraits
{
  typedef DDS_LongSeq type;
};

DYNAMIC_DATA_SEQ_TRAITS(octet_t,            DDS_OctetSeq)  // also uint8_t
DYNAMIC_DATA_SEQ_TRAITS(bool,               DDS_BooleanSeq)
DYNAMIC_DATA_SEQ_TRAITS(int8_t,             DDS_CharSeq) 
DYNAMIC_DATA_SEQ_TRAITS(char,               DDS_CharSeq) 

#ifndef RTI_WIN32
DYNAMIC_DATA_SEQ_TRAITS(char32_t,           DDS_WcharSeq)
#endif

DYNAMIC_DATA_SEQ_TRAITS(int16_t,            DDS_ShortSeq)
DYNAMIC_DATA_SEQ_TRAITS(uint16_t,           DDS_UnsignedShortSeq)
DYNAMIC_DATA_SEQ_TRAITS(int32_t,            DDS_LongSeq)
DYNAMIC_DATA_SEQ_TRAITS(uint32_t,           DDS_UnsignedLongSeq)
DYNAMIC_DATA_SEQ_TRAITS(int64_t,            DDS_LongLongSeq)
DYNAMIC_DATA_SEQ_TRAITS(uint64_t,           DDS_UnsignedLongLongSeq)
DYNAMIC_DATA_SEQ_TRAITS(float,              DDS_FloatSeq)
DYNAMIC_DATA_SEQ_TRAITS(double,             DDS_DoubleSeq)

#undef DYNAMIC_DATA_SEQ_TRAITS

#endif // RTIREFLEX_DD_TRAITS_H

