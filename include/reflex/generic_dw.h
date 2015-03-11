/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_GENERIC_DW_H
#define RTIREFLEX_GENERIC_DW_H

#include "reflex/safe_typecode.h"
#include "reflex/auto_dd.h"
#include "reflex/dllexport.h"
#include "reflex/reflex_fwd.h"

#include <memory>

REFLEX_EXPIMP_TEMPLATE template class REFLEX_DLL_EXPORT std::shared_ptr<DDSDynamicDataWriter>;

namespace reflex {

  namespace detail {

    class REFLEX_DLL_EXPORT DataWriterBase
    {
    protected:

      SafeTypeCode<DDS_TypeCode> safe_typecode_;
      std::unique_ptr<DDSDynamicDataTypeSupport> safe_typesupport_;
      std::shared_ptr<DDSDynamicDataWriter> safe_datawriter_;
      AutoDynamicData dd_instance_;

      DataWriterBase(DDSDomainParticipant *participant,
        const char * topic_name,
        const char * type_name,
        DDS_TypeCode * tc,
        DDS_DynamicDataTypeProperty_t props =
        DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);

      DataWriterBase(DDSDomainParticipant *participant,
        const DDS_DataWriterQos & dwqos,
        const char * topic_name,
        const char * type_name,
        DDS_TypeCode * tc,
        void * listener,
        DDS_DynamicDataTypeProperty_t props =
        DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);

      DDSDataWriter * underlying();
      DDSDataWriter * operator -> ();

      static void deleter(DDSDynamicDataWriter * ddWriter) throw();

    public:
      DDS_TypeCode * get_typecode() const;
    };

  } // namespace detail

  template <class T>
  class GenericDataWriter : public detail::DataWriterBase
  {
  public:
    GenericDataWriter(DDSDomainParticipant *participant,
      const char * topic_name,
      const char * type_name = 0)
      : detail::DataWriterBase(participant,
      topic_name,
      type_name,
      make_typecode<T>(type_name).release())
    { }

    GenericDataWriter(DDSDomainParticipant *participant,
      DDS_DataWriterQos & dwqos,
      const char * topic_name,
      const char * type_name = 0,
      void * listener = 0)
      : detail::DataWriterBase(participant,
      dwqos,
      topic_name,
      type_name,
      make_typecode<T>(type_name).release(),
      listener)
    { }

    template <class U>
    DDS_ReturnCode_t write(U & data)
    {
      fill_dd(data, *dd_instance_.get());
      return safe_datawriter_->write(*dd_instance_.get(), DDS_HANDLE_NIL);
    }

    template <class U>
    DDS_ReturnCode_t write_w_params(U & data, DDS_WriteParams_t & params)
    {
      fill_dd(data, *dd_instance_.get());
      return safe_datawriter_->write_w_params(*dd_instance_.get(), params);
    }
  };

  template <class... Args>
  class GenericDataWriter<std::tuple<Args...>>
    : public detail::DataWriterBase
  {
    typedef typename detail::remove_refs<std::tuple<Args...>>::type NoRefsTuple;

  public:
    GenericDataWriter(DDSDomainParticipant *participant,
      const char * topic_name,
      const char * type_name)
      : DataWriterBase(participant,
      topic_name,
      type_name,
      make_typecode<NoRefsTuple>(type_name).release())
    { }

    template <class U>
    DDS_ReturnCode_t write(U & data)
    {
      fill_dd(data, *dd_instance_.get());
      return safe_datawriter_->write(*dd_instance_.get(), DDS_HANDLE_NIL);
    }

    template <class U>
    DDS_ReturnCode_t write_w_params(U & data, DDS_WriteParams_t & params)
    {
      fill_dd(data, *dd_instance_.get());
      return safe_datawriter_->write_w_params(*dd_instance_.get(), params);
    }
  };

} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/generic_dw.cxx"
#endif

#endif // RTIREFLEX_GENERIC_DW_H
