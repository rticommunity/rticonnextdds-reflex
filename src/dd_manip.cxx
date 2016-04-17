/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "reflex/dllexport.h"
#include "reflex/dd_manip.h"

#include <algorithm>

#define SET_MEMBER_VALUE_DEF(TYPE, FUNCTION)                                     \
REFLEX_INLINE void set_member_overload_resolution_helper::set_member_value(      \
      DDS_DynamicData & instance,                                                \
      const MemberAccess &ma,                                                    \
      const TYPE & val)                                                          \
{                                                                                \
  DDS_ReturnCode_t rc;                                                           \
  if (ma.access_by_id())                                                         \
    rc = instance.FUNCTION(NULL, ma.get_id(), val);                              \
  else                                                                           \
    rc = instance.FUNCTION(ma.get_name(),                                        \
                           DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,               \
                           val);                                                 \
  detail::check_retcode(#FUNCTION " error = ", rc);                              \
}


#define GET_MEMBER_VALUE_DEF(DDS_TYPE, TYPE, FUNCTION)                             \
REFLEX_INLINE void get_member_overload_resolution_helper::get_member_value(        \
        const DDS_DynamicData & instance,                                          \
        const MemberAccess &ma,                                                    \
        TYPE & val)                                                                \
{                                                                                  \
  DDS_ReturnCode_t rc;                                                             \
  DDS_TYPE out;                                                                    \
  if (ma.access_by_id())                                                           \
    rc = instance.FUNCTION(out, NULL, ma.get_id());                                \
  else                                                                             \
    rc = instance.FUNCTION(out,                                                    \
                           ma.get_name(),                                          \
                           DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);                \
  detail::check_retcode(#FUNCTION " error = ", rc);                                \
  val = out;                                                                       \
}


namespace reflex {

  namespace detail {

      SET_MEMBER_VALUE_DEF(match::octet_t, set_octet)    // also uint8_t
      SET_MEMBER_VALUE_DEF(bool,           set_boolean)
      SET_MEMBER_VALUE_DEF(int8_t,         set_char)
      SET_MEMBER_VALUE_DEF(char,           set_char)
      SET_MEMBER_VALUE_DEF(int16_t,        set_short)
      SET_MEMBER_VALUE_DEF(uint16_t,       set_ushort)
      SET_MEMBER_VALUE_DEF(int32_t,        set_long)
      SET_MEMBER_VALUE_DEF(uint32_t,       set_ulong)
      SET_MEMBER_VALUE_DEF(int64_t,        set_longlong)
      SET_MEMBER_VALUE_DEF(uint64_t,       set_ulonglong)
      SET_MEMBER_VALUE_DEF(float,          set_float)
      SET_MEMBER_VALUE_DEF(double,         set_double)
#ifndef RTI_WIN32
      SET_MEMBER_VALUE_DEF(char32_t, set_wchar)
#endif
#ifdef __x86_64__
      SET_MEMBER_VALUE_DEF(long long int, set_longlong)
#endif



      GET_MEMBER_VALUE_DEF(DDS_Octet, match::octet_t,  get_octet)    // also uint8_t
      GET_MEMBER_VALUE_DEF(DDS_Char,  char,            get_char)
      GET_MEMBER_VALUE_DEF(DDS_Char,  int8_t,          get_char)
      // GET_MEMBER_VALUE_DEF DDS_Boolean defined below.
      GET_MEMBER_VALUE_DEF(DDS_Short,            int16_t,        get_short)
      GET_MEMBER_VALUE_DEF(DDS_UnsignedShort,    uint16_t,       get_ushort)
      GET_MEMBER_VALUE_DEF(DDS_Long,             int32_t,        get_long)
      GET_MEMBER_VALUE_DEF(DDS_UnsignedLong,     uint32_t,       get_ulong)
      GET_MEMBER_VALUE_DEF(DDS_LongLong,         int64_t,        get_longlong)
      GET_MEMBER_VALUE_DEF(DDS_UnsignedLongLong, uint64_t,       get_ulonglong)
      GET_MEMBER_VALUE_DEF(DDS_Float,            float,          get_float)
      GET_MEMBER_VALUE_DEF(DDS_Double,           double,         get_double)
#ifndef RTI_WIN32
      GET_MEMBER_VALUE_DEF(DDS_Wchar, char32_t, get_wchar)
#endif
#ifdef __x86_64__
      GET_MEMBER_VALUE_DEF(DDS_LongLong, long long, get_longlong)
#endif

      REFLEX_INLINE DDS_LongDouble to_LongDouble(long double src)
      {
#if (RTI_CDR_SIZEOF_LONG_DOUBLE == 16)
        return src;
#else
        using namespace std;
        DDS_LongDouble dst = { { 0 } };
        memcpy(&dst, &src, min(sizeof(long double), sizeof(DDS_LongDouble)));
        return dst;
#endif         
      }
      
      REFLEX_INLINE long double  from_LongDouble(DDS_LongDouble src)
      {
#if (RTI_CDR_SIZEOF_LONG_DOUBLE == 16)
        return src;
#else
        using namespace std;
        long double dst = 0;
        memcpy(&dst, &src, min(sizeof(long double), sizeof(DDS_LongDouble)));
        return dst;
#endif         
      }

      REFLEX_INLINE DDS_Char * primitive_ptr_cast(signed char * ptr)
      {
        // This signed char overload is needed because DDS does not natively
        // support signed char. It supports char and octet, which in C++
        // are char and unsigned char but signed char is a third kind of char
        // and is separate from the first two. It must be explicitly casted 
        // to char. Hence the overload.
        return reinterpret_cast<DDS_Char *>(ptr);
      }

      REFLEX_INLINE DDS_UnsignedLongLong * primitive_ptr_cast(long unsigned int *ptr)
      {
        return reinterpret_cast<DDS_UnsignedLongLong *>(ptr);
      }

      REFLEX_INLINE DDS_LongLong * primitive_ptr_cast(long int *ptr)
      {
        return reinterpret_cast<DDS_LongLong *>(ptr);
      }

      REFLEX_INLINE void set_member_overload_resolution_helper::set_member_value(      
            DDS_DynamicData & instance,                                                
            const MemberAccess &ma,                                                    
            const long double & val)                                                          
      {                                                                                
        DDS_ReturnCode_t rc;
        DDS_LongDouble longdouble = to_LongDouble(val);
       
        if (ma.access_by_id())                                                         
          rc = instance.set_longdouble(NULL, ma.get_id(), longdouble);                              
        else                                                                           
          rc = instance.set_longdouble(ma.get_name(),                                        
                                       DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,               
                                       longdouble);                                                 
        detail::check_retcode("set_longdouble error = ", rc);                              
      }

      REFLEX_INLINE void get_member_overload_resolution_helper::get_member_value(
              const DDS_DynamicData & instance,
              const MemberAccess &ma,
              long double & val)
      {
        DDS_ReturnCode_t rc;
        DDS_LongDouble longdouble = { { 0 } };

        if (ma.access_by_id())
          rc = instance.get_longdouble(longdouble, NULL, ma.get_id());
        else
          rc = instance.get_longdouble(
                    longdouble,
                    ma.get_name(),
                    DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

        detail::check_retcode("DDS_DynamicData::get_longdouble error = ", rc);
        val = from_LongDouble(longdouble);
      }

      REFLEX_INLINE void get_member_overload_resolution_helper::get_member_value(
              const DDS_DynamicData & instance,
              const MemberAccess &ma,
              bool & val)
      {
        // Implemented separately to avoid Visual Studio warning 4800:
        // Warning C4800: 'DDS_Boolean' : forcing value to bool 
        // 'true' or 'false' (performance warning)

        DDS_ReturnCode_t rc;
        DDS_Boolean out;

        if (ma.access_by_id())
          rc = instance.get_boolean(out, NULL, ma.get_id());
        else
          rc = instance.get_boolean(
          out,
          ma.get_name(),
          DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

        detail::check_retcode("DDS_DynamicData::get_boolean error = ", rc);

        val = out ? true : false;
      }

      REFLEX_INLINE void set_member_overload_resolution_helper::set_member_value(
                DDS_DynamicData & instance,
                const MemberAccess &ma,
                const std::vector<long double> & val,
                void *)
      {
        // Sequences of long double native type is loaned and unloaned as an optimization.

        if (!val.empty())
        {
          DynamicDataSeqTraits<long double>::type seq;

#if (RTI_CDR_SIZEOF_LONG_DOUBLE == 16)          
          std::vector<long double> & nc_val = 
            const_cast<std::vector<long double> &>(val);
          
          if (seq.loan_contiguous(&nc_val[0], val.size(), val.capacity()) != true)
          {
            throw std::runtime_error("set_member_value: sequence loaning failed");
          }

          DDS_ReturnCode_t rc = set_sequence(instance, ma, seq);
          seq.unloan();
          detail::check_retcode("set_member_value: Error setting sequence, error = ", rc);
#else
          seq.ensure_length(static_cast<DDS_Long>(val.size()), 
                            static_cast<DDS_Long>(val.size()));

          DDS_Long i = 0;
          for (auto const & elem: val)
          {
            seq[i] = to_LongDouble(elem);
            ++i;
          }

          DDS_ReturnCode_t rc = set_sequence(instance, ma, seq);
          detail::check_retcode("set_member_value: Error setting sequence, error = ", rc);
#endif
        }
      }

      REFLEX_INLINE void get_member_overload_resolution_helper::get_member_value(
                const DDS_DynamicData & instance,
                const MemberAccess &ma,
                std::vector<long double> & val,
                void *)
      {
        DDS_DynamicDataMemberInfo seq_info;

        const char * member_name =
          ma.access_by_id() ? NULL : ma.get_name();
        int id = ma.access_by_id() ?
          ma.get_id() : DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED;

        instance.get_member_info(seq_info, member_name, id);
        right_size(val, seq_info.element_count); // shrink/grow as needed.

        if (seq_info.element_count > 0)
        {
          DynamicDataSeqTraits<long double>::type seq;
          seq.ensure_length(seq_info.element_count,
                            seq_info.element_count);

          DDS_ReturnCode_t rc = detail::get_sequence(instance, seq, ma);
          detail::check_retcode("get_member_value: get_sequence error = ", rc);
          for(DDS_UnsignedLong i = 0;i < seq_info.element_count; ++i)
          {
            val[i] = from_LongDouble(seq[i]);
          }
        }
      }

      // Also see get_array function template for enums in dd_manip.h 
/*
      void get_member_overload_resolution_helper::get_member_value(
            const DDS_DynamicData & instance,
            const MemberAccess &ma,
            std::string & val)
      {
#ifdef RTI_WIN32  
        // NOTE: static
        static char * buf = new char[MAX_STRING_SIZE + 1];
#else
        char buffer[MAX_STRING_SIZE + 1];
        char *buf = buffer;
#endif
        DDS_UnsignedLong size = MAX_STRING_SIZE;

        const char * member_name =
          ma.access_by_id() ? NULL : ma.get_name();
        int id = ma.access_by_id() ?
          ma.get_id() : DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED;

        // Try to get the string into a preallocated buffer
        DDS_ReturnCode_t rc =
          instance.get_string(buf, &size, member_name, id);

        if (rc == DDS_RETCODE_OK)
        {
          val.assign(buf);
        }
        else if (rc == DDS_RETCODE_OUT_OF_RESOURCES)
        {
          // Preallocated buffer is insufficient.
          // Have the m/w provide it.
          char * ptr = 0; // TODO: null or empty?
          size = 0;
          rc = instance.get_string(ptr, &size, member_name, id);
          detail::check_retcode("DDS_DynamicData::get_string failed. error = ", rc);
          val.assign(ptr);
          DDS_String_free(ptr);
        }
        else
          detail::check_retcode("DDS_DynamicData::get_string failed. error = ", rc);
      }
*/

   
  } // namespace detail

} // namespace reflex
