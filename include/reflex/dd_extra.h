/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DD_EXTRA_H
#define RTIREFLEX_DD_EXTRA_H

#include <string>
#include "ndds/ndds_cpp.h"
#include "reflex/dllexport.h"

#ifdef RTI_WIN32
#include <stdint.h>
#endif

#define SET_SEQUENCE_DECL(DDS_SEQ_TYPE)                               \
DllExport DDS_ReturnCode_t set_sequence(DDS_DynamicData &instance,    \
                              const MemberAccess & ma,                \
                              const DDS_SEQ_TYPE &value);

#define GET_SEQUENCE_DECL(DDS_SEQ_TYPE)                                   \
DllExport DDS_ReturnCode_t get_sequence(const DDS_DynamicData & instance, \
                              DDS_SEQ_TYPE &seq,                          \
                              const MemberAccess & ma);

#define SET_ARRAY_DECL(BASIC_TYPE)                                    \
DllExport DDS_ReturnCode_t set_array(DDS_DynamicData &instance,       \
                           const MemberAccess & ma,                   \
                           DDS_UnsignedLong length,                   \
                           const BASIC_TYPE *array);

#define GET_ARRAY_DECL(BASIC_TYPE)                                     \
DllExport DDS_ReturnCode_t get_array(const DDS_DynamicData & instance, \
                           BASIC_TYPE *array,                          \
                           DDS_UnsignedLong *length,                   \
                           const MemberAccess & ma);

namespace reflex {

  typedef unsigned char octet_t;

  DllExport const char *get_readable_retcode(DDS_ReturnCode_t rc);
  DllExport const char *get_readable_ex_code(DDS_ExceptionCode_t ex);
  DllExport void check_retcode(const char * message, DDS_ReturnCode_t rc);
  DllExport void check_exception_code(const char * message, DDS_ExceptionCode_t ex);


  namespace detail {

      class DllExport MemberAccess
      {
        bool is_valid_id_;
        int id_;
        const char *name_;

        MemberAccess(
          bool is_valid_id,
          int id,
          const char *name);

      public:
        bool access_by_id() const;
        bool access_by_name() const;
        int get_id() const;
        const char * get_name() const;
        MemberAccess operator + (int i) const;

        static MemberAccess BY_ID(int id);
        static MemberAccess BY_NAME(const char *name = 0);
      };

      class DllExport SafeBinder
      {
      private:
        struct proxy {
          DDS_DynamicData *outer_;
          DDS_DynamicData *inner_;
        };

        DDS_DynamicData * outer_;
        DDS_DynamicData * inner_;

        SafeBinder(SafeBinder &);
        SafeBinder& operator = (SafeBinder &);

      public:
        SafeBinder();

        SafeBinder(const DDS_DynamicData & outer,
          DDS_DynamicData & inner,
          const char * member_name,
          DDS_Long member_id);

        SafeBinder(const DDS_DynamicData & outer,
          DDS_DynamicData & inner,
          const MemberAccess &ma);

        ~SafeBinder() throw();

        SafeBinder(proxy p) throw();

        void swap(SafeBinder & sb) throw();

        SafeBinder & operator = (proxy p) throw();

        operator proxy () throw();

        friend SafeBinder move(SafeBinder & sb) throw();
      };


      SET_SEQUENCE_DECL(DDS_LongSeq)
      SET_SEQUENCE_DECL(DDS_ShortSeq)
      SET_SEQUENCE_DECL(DDS_UnsignedLongSeq)
      SET_SEQUENCE_DECL(DDS_UnsignedShortSeq)
      SET_SEQUENCE_DECL(DDS_FloatSeq)
      SET_SEQUENCE_DECL(DDS_DoubleSeq)
      SET_SEQUENCE_DECL(DDS_BooleanSeq)
      SET_SEQUENCE_DECL(DDS_CharSeq)
      SET_SEQUENCE_DECL(DDS_WcharSeq)
      SET_SEQUENCE_DECL(DDS_OctetSeq)
      SET_SEQUENCE_DECL(DDS_LongLongSeq)
      SET_SEQUENCE_DECL(DDS_UnsignedLongLongSeq)

      SET_ARRAY_DECL(octet_t)    // also uint8_t
      SET_ARRAY_DECL(bool)
      SET_ARRAY_DECL(char)
      SET_ARRAY_DECL(int8_t)
      SET_ARRAY_DECL(char32_t)
      SET_ARRAY_DECL(int16_t)
      SET_ARRAY_DECL(uint16_t)
      SET_ARRAY_DECL(int32_t)
      SET_ARRAY_DECL(uint32_t)
      SET_ARRAY_DECL(int64_t)
      SET_ARRAY_DECL(uint64_t)
      SET_ARRAY_DECL(float)
      SET_ARRAY_DECL(double)
#ifdef __x86_64__
      SET_ARRAY_DECL(long long int)
#endif

      GET_SEQUENCE_DECL(DDS_OctetSeq)
      GET_SEQUENCE_DECL(DDS_BooleanSeq)
      GET_SEQUENCE_DECL(DDS_CharSeq)
      GET_SEQUENCE_DECL(DDS_WcharSeq)
      GET_SEQUENCE_DECL(DDS_ShortSeq)
      GET_SEQUENCE_DECL(DDS_UnsignedShortSeq)
      GET_SEQUENCE_DECL(DDS_LongSeq)
      GET_SEQUENCE_DECL(DDS_UnsignedLongSeq)
      GET_SEQUENCE_DECL(DDS_LongLongSeq)
      GET_SEQUENCE_DECL(DDS_UnsignedLongLongSeq)
      GET_SEQUENCE_DECL(DDS_FloatSeq)
      GET_SEQUENCE_DECL(DDS_DoubleSeq)

      GET_ARRAY_DECL(octet_t)    // also uint8_t
      GET_ARRAY_DECL(bool)
      GET_ARRAY_DECL(int8_t)
      GET_ARRAY_DECL(char)
      GET_ARRAY_DECL(char32_t)
      GET_ARRAY_DECL(int16_t)
      GET_ARRAY_DECL(uint16_t)
      GET_ARRAY_DECL(int32_t)
      GET_ARRAY_DECL(uint32_t)
      GET_ARRAY_DECL(int64_t)
      GET_ARRAY_DECL(uint64_t)
      GET_ARRAY_DECL(float)
      GET_ARRAY_DECL(double)
#ifdef __x86_64__
      GET_ARRAY_DECL(long long int)
#endif

      template <class T> // When T is an enum
      static DDS_ReturnCode_t set_array(
          DDS_DynamicData &instance,
          const MemberAccess & ma,
          DDS_UnsignedLong length,
          const T *array)
      {
        if (ma.access_by_id())
          return instance.set_long_array(
              NULL,
              ma.get_id(),
              length,
              reinterpret_cast<const DDS_Long *>(array));
        else
          return instance.set_long_array(
              ma.get_name(),
              DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
              length,
              reinterpret_cast<const DDS_Long *>(array));
      }

      template <class T> // When T is an enum
      static DDS_ReturnCode_t get_array(
          const DDS_DynamicData & instance,
          T *array,
          DDS_UnsignedLong *length,
          const MemberAccess &ma)
      {
        if (ma.access_by_id())
          return instance.get_long_array(
              reinterpret_cast<DDS_Long *>(array),
              length,
              NULL,
              ma.get_id());
        else
          return instance.get_long_array(
              reinterpret_cast<DDS_Long *>(array),
              length,
              ma.get_name(),
              DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);
      }

      DllExport void set_seq_length(
          DDS_DynamicData & seq,
          size_t size,
          bool is_string);

      DllExport std::string remove_parenthesis(
          std::string);

  } // namespace detail

} // namespace reflex

#endif // RTIREFLEX_DD_EXTRA_H

