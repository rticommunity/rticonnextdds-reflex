/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_GENERIC_DR_H
#define RTIREFLEX_GENERIC_DR_H

#include "reflex/safe_typecode.h"
#include "reflex/auto_dd.h"
#include "reflex/dd2tuple.h"
#include "reflex/dllexport.h"
#include "reflex/sample.h"
#include "reflex/reflex_fwd.h"
#include "reflex/enable_if.h"

#include <memory>

//REFLEX_EXPIMP_TEMPLATE template class REFLEX_DLL_EXPORT reflex::SafeTypeCode<DDS_TypeCode>;
//REFLEX_EXPIMP_TEMPLATE template class REFLEX_DLL_EXPORT std::unique_ptr<DDSDynamicDataTypeSupport>;
//REFLEX_EXPIMP_TEMPLATE template class REFLEX_DLL_EXPORT std::shared_ptr<DDSDynamicDataReader>;

namespace reflex {

  namespace detail {

    class DataReaderBase
    {
    protected:

      SafeTypeCode<DDS_TypeCode> safe_typecode_;
      std::unique_ptr<DDSDynamicDataTypeSupport> safe_typesupport_;
      std::shared_ptr<DDSDynamicDataReader> safe_datareader_;
      AutoDynamicData dd_instance_;

      REFLEX_DLL_EXPORT DataReaderBase(
        DDSDomainParticipant *participant,
        DDSDataReaderListener * listener,
        const char * topic_name,
        const char * type_name,
        DDS_TypeCode * tc,
        DDS_DynamicDataTypeProperty_t props =
        DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);

      REFLEX_DLL_EXPORT DataReaderBase(
        DDSDomainParticipant *participant,
        const DDS_DataReaderQos & drqos,
        DDSDataReaderListener * listener,
        const char * topic_name,
        const char * type_name,
        DDS_TypeCode * tc,
        DDS_DynamicDataTypeProperty_t props =
        DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);

      DDSDataReader * underlying();
      DDSDataReader * operator -> ();

      static void deleter(DDSDynamicDataReader * ddReader) throw();

    public:
      ~DataReaderBase();

      REFLEX_DLL_EXPORT std::shared_ptr<DDSDynamicDataReader> dd_reader();
      REFLEX_DLL_EXPORT DDS_TypeCode * get_typecode();
      REFLEX_DLL_EXPORT const DDS_TypeCode * get_typecode() const;
    };

    template <class T>
    class GenericDataReader;

    template <class T>
    struct GenericDataReaderListener
    {
      virtual void on_data_available(GenericDataReader<T> & dr) = 0;
    };

    template <class T>
    class DataReaderListenerAdapter : public DDSDataReaderListener
    {
      GenericDataReaderListener<T> * generic_listener_;
      GenericDataReader<T> * data_reader_;

    public:

      typedef GenericDataReader<T> DataReaderType;

      explicit DataReaderListenerAdapter(GenericDataReaderListener<T> * listener)
        : generic_listener_(listener),
        data_reader_(0)
      {
        if (!listener)
        {
          throw std::runtime_error("DataReaderListenerAdapter: NULL listener");
        }
      }

      void set_datareader(GenericDataReader<T> * dr)
      {
        data_reader_ = dr;
      }

      virtual void on_data_available(DDSDataReader *reader)
      {
#ifdef _DEBUG
        DDSDynamicDataReader *dd_reader = DDSDynamicDataReader::narrow(reader);
        if (!dd_reader) {
          std::cerr << "Not a DynamicDataReader!!!\n";
        }
#endif
        if (generic_listener_ && data_reader_)
          generic_listener_->on_data_available(*data_reader_);
        else
          std::cerr << "Missing Listener or DataReader\n";
      }
    };

    template <class T>
    DDS_ReturnCode_t take_impl(
      std::shared_ptr<DDSDynamicDataReader> dr,
      std::vector<Sample<T>> & data,
      int max_samples,
      DDS_SampleStateMask sample_states,
      DDS_ViewStateMask view_states,
      DDS_InstanceStateMask instance_states,
      DDSReadCondition * cond = 0)
    {
      DDS_DynamicDataSeq data_seq;
      DDS_SampleInfoSeq info_seq;
      DDS_ReturnCode_t rc;

      if (cond)
        rc = dr->take_w_condition(data_seq, info_seq, max_samples, cond);
      else
        rc = dr->take(data_seq, info_seq, max_samples,
        sample_states, view_states, instance_states);

      if (rc == DDS_RETCODE_NO_DATA) {
        return rc;
      }
      else if (rc != DDS_RETCODE_OK) {
        std::cerr << "! Unable to take data from data reader, error "
          << rc << std::endl;
        return rc;
      }

      if (data_seq.length())
      {
        data.resize(data_seq.length());

        for (int i = 0; i < data_seq.length(); ++i)
        {
          if (info_seq[i].valid_data)
            read_dynamicdata(data[i].data(), data_seq[i]);
          else
          {
            T temp;
            data[i].data() = temp;
          }
          data[i].info() = info_seq[i];
        }
      }

      rc = dr->return_loan(data_seq, info_seq);
      if (rc != DDS_RETCODE_OK) {
        std::cerr << "! Unable to return loan, error "
          << rc << std::endl;
      }
      return rc;
    }

    template <class T>
    class GenericDataReader : public detail::DataReaderBase
    {
    public:
      typedef GenericDataReaderListener<T> ListenerType;
      DataReaderListenerAdapter<T> * listener_adapter_;
      std::unique_ptr < DataReaderListenerAdapter<T>> safe_listener_adapter_;

      GenericDataReader(
        DDSDomainParticipant *participant,
        ListenerType * listener,
        const char * topic_name,
        const char * type_name = 0,
        DataReaderListenerAdapter<T> * adapter_placeholder = 0)

        : detail::DataReaderBase(
        participant,
        listener ? (adapter_placeholder = new DataReaderListenerAdapter<T>(listener)) : 0,
        topic_name,
        type_name,
        make_typecode<T>(type_name).release()),
        safe_listener_adapter_(listener ? adapter_placeholder : 0)
      {
          if (safe_listener_adapter_)
            safe_listener_adapter_->set_datareader(this);
        }

      GenericDataReader(
        DDSDomainParticipant *participant,
        const DDS_DataReaderQos & drqos,
        ListenerType * listener,
        const char * topic_name,
        const char * type_name = 0,
        DataReaderListenerAdapter<T> * adapter_placeholder = 0)

        : detail::DataReaderBase(
        participant,
        drqos,
        listener ? (adapter_placeholder = new DataReaderListenerAdapter<T>(listener)) : 0,
        topic_name,
        type_name,
        make_typecode<T>(type_name).release()),
        safe_listener_adapter_(listener ? adapter_placeholder : 0)
      {
          if (safe_listener_adapter_)
            safe_listener_adapter_->set_datareader(this);
        }

      DDS_ReturnCode_t take_w_condition(
        T& data, DDS_SampleInfo & info, DDSReadCondition * cond = 0)
      {
        DDS_DynamicDataSeq data_seq;
        DDS_SampleInfoSeq info_seq;
        DDS_ReturnCode_t rc;
        size_t max_samples = 1;

        if (cond)
          rc = dd_reader()->take_w_condition(data_seq, info_seq, max_samples, cond);
        else
          rc = dd_reader()->take(data_seq, info_seq, max_samples,
          DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ANY_INSTANCE_STATE);

        if (rc == DDS_RETCODE_NO_DATA) {
          return rc;
        }
        else if (rc != DDS_RETCODE_OK) {
          std::cerr << "! Unable to take data from data reader, error "
            << rc << std::endl;
          return rc;
        }

        if (data_seq.length())
        {
          for (int i = 0; i < data_seq.length(); ++i)
          {
            if (info_seq[i].valid_data)
            {
              dd2tuple(data_seq[i], data);
              info = info_seq[i];
            }
          }
        }

        rc = dd_reader()->return_loan(data_seq, info_seq);
        if (rc != DDS_RETCODE_OK) {
          std::cerr << "! Unable to return loan, error "
            << rc << std::endl;
        }
        return rc;
      }

      DDS_ReturnCode_t take(
        std::vector<Sample<T>> & data,
        int max_samples = DDS_LENGTH_UNLIMITED,
        DDS_SampleStateMask sample_states = DDS_ANY_SAMPLE_STATE,
        DDS_ViewStateMask view_states = DDS_ANY_VIEW_STATE,
        DDS_InstanceStateMask instance_states = DDS_ANY_INSTANCE_STATE)
      {
        return detail::take_impl<T>(
          dd_reader(),
          data,
          max_samples,
          sample_states,
          view_states,
          instance_states);
      }

      DDS_ReturnCode_t take_w_condition(std::vector<Sample<T>> & data,
        int max_samples,
        DDSReadCondition * cond)
      {
        if (!cond)
          return DDS_RETCODE_PRECONDITION_NOT_MET;

        return detail::take_impl<T>(
          dd_reader(),
          data,
          max_samples,
          DDS_ANY_SAMPLE_STATE,
          DDS_ANY_VIEW_STATE,
          DDS_ANY_INSTANCE_STATE,
          cond);
      }
    };

    // The following class can't inherit from the one above
    // because it creates a possibility of the class inheriting from
    // itself!
    template <class... Args>
    class GenericDataReader<std::tuple<Args...>>
      : public detail::DataReaderBase
    {
      typedef typename
      detail::remove_refs<std::tuple<Args...>>::type
      NoRefsTuple;

    public:
      typedef GenericDataReaderListener<NoRefsTuple> ListenerType;
      DataReaderListenerAdapter<NoRefsTuple> * listener_adapter_;
      std::unique_ptr < DataReaderListenerAdapter<NoRefsTuple>> safe_listener_adapter_;

      GenericDataReader(DDSDomainParticipant *participant,
        GenericDataReaderListener<NoRefsTuple> * listener,
        const char * topic_name,
        const char * type_name = 0,
        DataReaderListenerAdapter<NoRefsTuple> * adapter_placeholder = 0)
        : DataReaderBase(
        participant,
        listener ? (adapter_placeholder = new DataReaderListenerAdapter<NoRefsTuple>(listener)) : 0,
        topic_name,
        type_name,
        make_typecode<NoRefsTuple>(type_name).release()),
        safe_listener_adapter_(listener ? adapter_placeholder : 0)
      {
        if (safe_listener_adapter_)
          safe_listener_adapter_->set_datareader(this);
      }

      DDS_ReturnCode_t take(std::vector<Sample<NoRefsTuple>> & data,
        int max_samples = DDS_LENGTH_UNLIMITED,
        DDS_SampleStateMask sample_states = DDS_ANY_SAMPLE_STATE,
        DDS_ViewStateMask view_states = DDS_ANY_VIEW_STATE,
        DDS_InstanceStateMask instance_states = DDS_ANY_INSTANCE_STATE)
      {
        return detail::take_impl<NoRefsTuple>(
          dd_reader(),
          data,
          max_samples,
          sample_states,
          view_states,
          instance_states);
      }
    };

  } // namespace detail
} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/generic_dr.cxx"
#endif



#endif // RTIREFLEX_GENERIC_DR_H

