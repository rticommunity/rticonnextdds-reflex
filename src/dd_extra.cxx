/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <iostream>
#include <map>
#include <stdexcept>
#include <sstream>

#include "reflex/dllexport.h"
#include "reflex/dd_extra.h"
#include "reflex/dd_manip.h"

#define SET_SEQUENCE_DEF(DDS_SEQ_TYPE, FUNCTION)                      \
REFLEX_INLINE DDS_ReturnCode_t set_sequence(                          \
                              DDS_DynamicData &instance,              \
                              const MemberAccess &ma,                 \
                              const DDS_SEQ_TYPE &value)              \
{                                                                     \
  if(ma.access_by_id())                                               \
    return instance.FUNCTION(NULL, ma.get_id(), value);               \
  else                                                                \
    return instance.FUNCTION(ma.get_name(),                           \
                             DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,  \
                             value);                                  \
}

#define GET_SEQUENCE_DEF(DDS_SEQ_TYPE, FUNCTION)                      \
REFLEX_INLINE DDS_ReturnCode_t get_sequence(                          \
                              const DDS_DynamicData & instance,       \
                              DDS_SEQ_TYPE &seq,                      \
                              const MemberAccess &ma)                 \
{                                                                     \
  if(ma.access_by_id())                                               \
    return instance.FUNCTION(seq, NULL, ma.get_id());                 \
  else                                                                \
    return instance.FUNCTION(seq, ma.get_name(),                      \
                             DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED); \
}

#define SET_ARRAY_DEF(BASIC_TYPE, DDS_TYPE, FUNCTION)                     \
REFLEX_INLINE DDS_ReturnCode_t set_array(                                 \
                           DDS_DynamicData &instance,                     \
                           const MemberAccess &ma,                        \
                           DDS_UnsignedLong length,                       \
                           const BASIC_TYPE *array)                       \
{                                                                         \
  if(ma.access_by_id())                                                   \
    return instance.FUNCTION(NULL, ma.get_id(), length,                   \
                             reinterpret_cast<const DDS_TYPE *>(array));  \
  else                                                                    \
    return instance.FUNCTION(ma.get_name(),                               \
                             DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,      \
                             length,                                      \
                             reinterpret_cast<const DDS_TYPE *>(array));  \
}

#define GET_ARRAY_DEF(BASIC_TYPE, DDS_TYPE, FUNCTION)                 \
REFLEX_INLINE DDS_ReturnCode_t get_array(                             \
                           const DDS_DynamicData & instance,          \
                           BASIC_TYPE *array,                         \
                           DDS_UnsignedLong *length,                  \
                           const MemberAccess &ma)                    \
{                                                                     \
  if(ma.access_by_id())                                               \
    return instance.FUNCTION(reinterpret_cast<DDS_TYPE *>(array),     \
                             length, NULL, ma.get_id());              \
  else                                                                \
    return instance.FUNCTION(reinterpret_cast<DDS_TYPE *>(array),     \
                             length, ma.get_name(),                   \
                             DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED); \
}

  namespace reflex {

    namespace detail {

      REFLEX_DLL_EXPORT DDS_LongDouble to_LongDouble(long double);
      REFLEX_DLL_EXPORT long double  from_LongDouble(DDS_LongDouble);

        REFLEX_INLINE void check_retcode(
          const char * message,
          DDS_ReturnCode_t rc)
        {
          if (rc != DDS_RETCODE_OK)
          {
            std::stringstream stream;
            stream << message << get_readable_retcode(rc);
            throw std::runtime_error(stream.str());
          }
        }

        REFLEX_INLINE void check_exception_code(
          const char * message,
          DDS_ExceptionCode_t ex)
        {
          if (ex != DDS_NO_EXCEPTION_CODE)
          {
            std::stringstream stream;
            stream << message << get_readable_ex_code(ex);
            throw std::runtime_error(stream.str());
          }
        }

        SET_SEQUENCE_DEF(DDS_LongSeq,             set_long_seq)
        SET_SEQUENCE_DEF(DDS_ShortSeq,            set_short_seq)
        SET_SEQUENCE_DEF(DDS_UnsignedLongSeq,     set_ulong_seq)
        SET_SEQUENCE_DEF(DDS_UnsignedShortSeq,    set_ushort_seq)
        SET_SEQUENCE_DEF(DDS_FloatSeq,            set_float_seq)
        SET_SEQUENCE_DEF(DDS_DoubleSeq,           set_double_seq)
        //SET_SEQUENCE_DEF(DDS_LongDoubleSeq,       set_longdouble_seq)
        SET_SEQUENCE_DEF(DDS_BooleanSeq,          set_boolean_seq)
        SET_SEQUENCE_DEF(DDS_CharSeq,             set_char_seq)
        SET_SEQUENCE_DEF(DDS_WcharSeq,            set_wchar_seq)
        SET_SEQUENCE_DEF(DDS_OctetSeq,            set_octet_seq)
        SET_SEQUENCE_DEF(DDS_LongLongSeq,         set_longlong_seq)
        SET_SEQUENCE_DEF(DDS_UnsignedLongLongSeq, set_ulonglong_seq)

        SET_ARRAY_DEF(match::octet_t, DDS_Octet,    set_octet_array)    // also uint8_t
        SET_ARRAY_DEF(bool,           DDS_Boolean,  set_boolean_array)
        SET_ARRAY_DEF(int8_t,         DDS_Char,     set_char_array)
        SET_ARRAY_DEF(char,           DDS_Char,     set_char_array)

#ifndef RTI_WIN32
        SET_ARRAY_DEF(char32_t,  DDS_Wchar,            set_wchar_array)
#endif

        SET_ARRAY_DEF(int16_t,   DDS_Short,             set_short_array)
        SET_ARRAY_DEF(uint16_t,  DDS_UnsignedShort,     set_ushort_array)
        SET_ARRAY_DEF(int32_t,   DDS_Long,              set_long_array)
        SET_ARRAY_DEF(uint32_t,  DDS_UnsignedLong,      set_ulong_array)
        SET_ARRAY_DEF(int64_t,   DDS_LongLong,          set_longlong_array)
        SET_ARRAY_DEF(uint64_t,  DDS_UnsignedLongLong,  set_ulonglong_array)
        SET_ARRAY_DEF(float,     DDS_Float,             set_float_array)
        SET_ARRAY_DEF(double,    DDS_Double,            set_double_array)
#ifdef __x86_64__
        SET_ARRAY_DEF(long long int, DDS_LongLong,    set_longlong_array)
#endif
        // Also see set_array function template for enums in dd_manip.h 

        GET_SEQUENCE_DEF(DDS_OctetSeq,            get_octet_seq)
        GET_SEQUENCE_DEF(DDS_BooleanSeq,          get_boolean_seq)
        GET_SEQUENCE_DEF(DDS_CharSeq,             get_char_seq)
        GET_SEQUENCE_DEF(DDS_WcharSeq,            get_wchar_seq)
        GET_SEQUENCE_DEF(DDS_ShortSeq,            get_short_seq)
        GET_SEQUENCE_DEF(DDS_UnsignedShortSeq,    get_ushort_seq)
        GET_SEQUENCE_DEF(DDS_LongSeq,             get_long_seq)
        GET_SEQUENCE_DEF(DDS_UnsignedLongSeq,     get_ulong_seq)
        GET_SEQUENCE_DEF(DDS_LongLongSeq,         get_longlong_seq)
        GET_SEQUENCE_DEF(DDS_UnsignedLongLongSeq, get_ulonglong_seq)
        GET_SEQUENCE_DEF(DDS_FloatSeq,            get_float_seq)
        GET_SEQUENCE_DEF(DDS_DoubleSeq,           get_double_seq)
        //GET_SEQUENCE_DEF(DDS_LongDoubleSeq,       get_longdouble_seq)

        GET_ARRAY_DEF(match::octet_t, DDS_Octet,    get_octet_array)    // also uint8_t
        GET_ARRAY_DEF(bool,           DDS_Boolean,  get_boolean_array)
        GET_ARRAY_DEF(char,           DDS_Char,     get_char_array)
        GET_ARRAY_DEF(int8_t,         DDS_Char,     get_char_array)

#ifndef RTI_WIN32
        GET_ARRAY_DEF(char32_t,  DDS_Wchar,           get_wchar_array) 
#endif

        GET_ARRAY_DEF(int16_t,  DDS_Short,            get_short_array)
        GET_ARRAY_DEF(uint16_t, DDS_UnsignedShort,    get_ushort_array)
        GET_ARRAY_DEF(int32_t,  DDS_Long,             get_long_array)
        GET_ARRAY_DEF(uint32_t, DDS_UnsignedLong,     get_ulong_array)
        GET_ARRAY_DEF(int64_t,  DDS_LongLong,         get_longlong_array)
        GET_ARRAY_DEF(uint64_t, DDS_UnsignedLongLong, get_ulonglong_array)
        GET_ARRAY_DEF(float,    DDS_Float,            get_float_array)
        GET_ARRAY_DEF(double,   DDS_Double,           get_double_array)

#ifdef __x86_64__
        GET_ARRAY_DEF(long long, DDS_LongLong,        get_longlong_array)
#endif
      
       REFLEX_INLINE DDS_ReturnCode_t set_array(
            DDS_DynamicData &instance,
            const MemberAccess &ma, 
            DDS_UnsignedLong length, 
            const long double *src)                       
        {
          DDS_ReturnCode_t rc = DDS_RETCODE_OK;
          DDS_LongDouble dest[256];
          DDS_LongDouble *dest_ptr = 0;
          
          if (length > 256)
            dest_ptr = new DDS_LongDouble[length];
          else
            dest_ptr = dest;

          if (!dest_ptr)
            return DDS_RETCODE_ERROR;

          for (DDS_UnsignedLong i = 0; i < length; i++)
            dest_ptr[i] = to_LongDouble(src[i]);

          if (ma.access_by_id())
          {
            rc = instance.set_longdouble_array(NULL, ma.get_id(), length, dest_ptr);
          }
          else
          {
            rc = instance.set_longdouble_array(ma.get_name(),
                  DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
                  length, dest_ptr);
          }
          
          if(dest_ptr != dest)
            delete[] dest_ptr;

          return rc;
        }

        REFLEX_INLINE DDS_ReturnCode_t get_array(
          const DDS_DynamicData & instance, 
          long double *dest, 
          DDS_UnsignedLong *length, 
          const MemberAccess &ma)
        {
          DDS_ReturnCode_t rc = DDS_RETCODE_OK;
          DDS_LongDouble src[256];
          DDS_LongDouble *src_ptr = 0;

          if (*length > 256)
            src_ptr = new DDS_LongDouble[*length];
          else
            src_ptr = src;

          if (!src_ptr)
            return DDS_RETCODE_ERROR;

          if (ma.access_by_id())
            rc = instance.get_longdouble_array(src_ptr, length, NULL, ma.get_id()); 
          else
            rc = instance.get_longdouble_array(src_ptr, length, ma.get_name(),
                  DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED); 

          if (rc == DDS_RETCODE_OK)
          {
            for (DDS_UnsignedLong i = 0; i < *length; i++)
              dest[i] = from_LongDouble(src_ptr[i]);
          }

          if (src_ptr != src)
            delete[] src_ptr;

          return rc;
        }

        
      REFLEX_INLINE SafeBinder::SafeBinder()
        : outer_(0), inner_(0)
      {}

      REFLEX_INLINE SafeBinder::SafeBinder(const DDS_DynamicData & outer,
        DDS_DynamicData & inner,
        const char * member_name,
        DDS_Long member_id)
        : outer_(&const_cast<DDS_DynamicData &>(outer)),
          inner_(&inner)
      {
        DDS_ReturnCode_t rc =
          outer_->bind_complex_member(*inner_, member_name, member_id);

        detail::check_retcode("SafeBinder::bind_complex_member error = ", rc);
      }

      REFLEX_INLINE SafeBinder::SafeBinder(const DDS_DynamicData & outer,
        DDS_DynamicData & inner,
        const MemberAccess &ma)
        : outer_(&const_cast<DDS_DynamicData &>(outer)),
        inner_(&inner)
      {
        const char * member_name
          = ma.access_by_id() ? NULL : ma.get_name();

        int id
          = ma.access_by_id() ?
          ma.get_id() : DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED;

        DDS_ReturnCode_t rc =
          outer_->bind_complex_member(*inner_, member_name, id);

        detail::check_retcode("SafeBinder::bind_complex_member error = ", rc);
      }

      REFLEX_INLINE SafeBinder::~SafeBinder() throw() {
        if (outer_)
          outer_->unbind_complex_member(*inner_);
      }

      REFLEX_INLINE SafeBinder::SafeBinder(SafeBinder::proxy p) throw()
        : outer_(p.outer_),
        inner_(p.inner_)
      {}

      REFLEX_INLINE void SafeBinder::swap(SafeBinder & sb) throw() {
        std::swap(outer_, sb.outer_);
        std::swap(inner_, sb.inner_);
      }

      REFLEX_INLINE SafeBinder & SafeBinder::operator = (SafeBinder::proxy p) throw() {
        SafeBinder(p).swap(*this);
        return *this;
      }

      REFLEX_INLINE SafeBinder::operator SafeBinder::proxy() throw() {
        SafeBinder::proxy p{ outer_, inner_ };
        inner_ = outer_ = 0;
        return p;
      }

      REFLEX_INLINE SafeBinder move(SafeBinder & sb) throw() {
        return SafeBinder(SafeBinder::proxy(sb));
      }

      REFLEX_INLINE MemberAccess::MemberAccess(bool is_valid_id,
        int id,
        const char *name)
        : is_valid_id_(is_valid_id),
        id_(id),
        name_(name)
      {}

      REFLEX_INLINE bool MemberAccess::access_by_id() const
      {
        return is_valid_id_;
      }

      REFLEX_INLINE bool MemberAccess::access_by_name() const
      {
        return !is_valid_id_;
      }

      REFLEX_INLINE int MemberAccess::get_id() const
      {
        return id_;
      }

      REFLEX_INLINE const char * MemberAccess::get_name() const
      {
        return name_;
      }

      REFLEX_INLINE MemberAccess MemberAccess::operator + (int i) const {
        if (is_valid_id_) {
          return MemberAccess::BY_ID(id_ + i);
        }
        else
          throw std::runtime_error(
              "MemberAccess: PRECONDITION_NOT_MET: MemberAccess has no valid id");
      }


      REFLEX_INLINE MemberAccess MemberAccess::BY_ID(int id) {
        return MemberAccess(true, id, 0);
      }

      REFLEX_INLINE MemberAccess MemberAccess::BY_NAME(const char *name) {
        return MemberAccess(false, -1, name);
      }

      REFLEX_INLINE void set_seq_length(
        DDS_DynamicData & seq,
        size_t size,
        bool is_string)
      {
        if (is_string)
          return;

        if (size == 0)
          size = 1;

        // FIXME: Why am I binding a single element of an unknown type?
        DDS_DynamicData item(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
        SafeBinder binder(seq, item, NULL, static_cast<DDS_Long>(size));
      }

      REFLEX_INLINE const char *get_readable_retcode(DDS_ReturnCode_t rc)
      {
        switch (rc) 
        {
          case DDS_RETCODE_OK:
            return "DDS_RETCODE_OK";
          case DDS_RETCODE_ERROR:
            return "DDS_RETCODE_ERROR";
          case DDS_RETCODE_UNSUPPORTED:
            return "DDS_RETCODE_UNSUPPORTED";
          case DDS_RETCODE_BAD_PARAMETER:
            return "DDS_RETCODE_BAD_PARAMETER";
          case DDS_RETCODE_PRECONDITION_NOT_MET:
            return "DDS_RETCODE_PRECONDITION_NOT_MET";
          case DDS_RETCODE_OUT_OF_RESOURCES:
            return "DDS_RETCODE_OUT_OF_RESOURCES";
          case DDS_RETCODE_NOT_ENABLED:
            return "DDS_RETCODE_NOT_ENABLED";
          case DDS_RETCODE_IMMUTABLE_POLICY:
            return "DDS_RETCODE_IMMUTABLE_POLICY";
          case DDS_RETCODE_INCONSISTENT_POLICY:
            return "DDS_RETCODE_OK";
          case DDS_RETCODE_ALREADY_DELETED:
            return "DDS_RETCODE_ALREADY_DELETED";
          case DDS_RETCODE_TIMEOUT:
            return "DDS_RETCODE_TIMEOUT";
          case DDS_RETCODE_NO_DATA:
            return "DDS_RETCODE_NO_DATA";
          case DDS_RETCODE_ILLEGAL_OPERATION:
            return "DDS_RETCODE_ILLEGAL_OPERATION";
          default:
            return "Unknown DDS_ReturnCode_t";
        }
      }

      REFLEX_INLINE const char *get_readable_ex_code(DDS_ExceptionCode_t ex)
      {
        switch (ex) 
        {
          case DDS_NO_EXCEPTION_CODE:
            return "DDS_NO_EXCEPTION_CODE";
          case DDS_USER_EXCEPTION_CODE:
            return "DDS_USER_EXCEPTION_CODE";
          case DDS_SYSTEM_EXCEPTION_CODE:
            return "DDS_SYSTEM_EXCEPTION_CODE";
          case DDS_BAD_PARAM_SYSTEM_EXCEPTION_CODE:
            return "DDS_BAD_PARAM_SYSTEM_EXCEPTION_CODE";
          case DDS_NO_MEMORY_SYSTEM_EXCEPTION_CODE:
            return "DDS_NO_MEMORY_SYSTEM_EXCEPTION_CODE";
          case DDS_BAD_TYPECODE_SYSTEM_EXCEPTION_CODE:
            return "DDS_BAD_TYPECODE_SYSTEM_EXCEPTION_CODE";
          case DDS_BADKIND_USER_EXCEPTION_CODE:
            return "DDS_BADKIND_USER_EXCEPTION_CODE";
          case DDS_BOUNDS_USER_EXCEPTION_CODE:
            return "DDS_BOUNDS_USER_EXCEPTION_CODE";
          case DDS_IMMUTABLE_TYPECODE_SYSTEM_EXCEPTION_CODE:
            return "DDS_IMMUTABLE_TYPECODE_SYSTEM_EXCEPTION_CODE";
          case DDS_BAD_MEMBER_NAME_USER_EXCEPTION_CODE:
            return "DDS_BAD_MEMBER_NAME_USER_EXCEPTION_CODE";
          case DDS_BAD_MEMBER_ID_USER_EXCEPTION_CODE:
            return "DDS_BAD_MEMBER_ID_USER_EXCEPTION_CODE";
          default:
            return "Unknown DDS_ExceptionCode_t";
        }
      }

    } // namespace detail
  } // namespace reflex
