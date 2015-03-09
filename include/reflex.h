/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_H
#define RTIREFLEX_H

#include "reflex/dd2tuple.h"
#include "reflex/dllexport.h"
#include <memory>

EXPIMP_TEMPLATE template class DllExport reflex::SafeTypeCode<DDS_TypeCode>;
EXPIMP_TEMPLATE template class DllExport std::unique_ptr<DDSDynamicDataTypeSupport>;
EXPIMP_TEMPLATE template class DllExport std::shared_ptr<DDSDynamicDataWriter>;
EXPIMP_TEMPLATE template class DllExport std::shared_ptr<DDSDynamicDataReader>;

namespace reflex {

  template <class T>
  class Sample
  {
    T val;
    DDS_SampleInfo val_info;

  public:

    Sample()
    { }

    Sample(T &t, DDS_SampleInfo &i)
      : val(t),
      val_info(i)
    { }

    Sample(const Sample &s)
      : val(s.data()),
      val_info(s.info())
    {}

    Sample & operator = (const Sample &s)
    {
      val = s.data();
      val_info = s.info();
      return *this;
    }

    T & data()
    {
      return val;
    }

    const T & data() const
    {
      return val;
    }

    const DDS_SampleInfo & info() const
    {
      return val_info;
    }

    DDS_SampleInfo & info()
    {
      return val_info;
    }

    T * operator -> ()
    {
      return &val;
    }

    const T * operator -> () const
    {
      return &val;
    }
  };

  namespace detail {

    class DllExport DataWriterBase
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

    class DllExport DataReaderBase
    {
    protected:

      SafeTypeCode<DDS_TypeCode> safe_typecode_;
      std::unique_ptr<DDSDynamicDataTypeSupport> safe_typesupport_;
      std::shared_ptr<DDSDynamicDataReader> safe_datareader_;
      AutoDynamicData dd_instance_;

      DataReaderBase(DDSDomainParticipant *participant,
        DDSDataReaderListener * listener,
        const char * topic_name,
        const char * type_name,
        DDS_TypeCode * tc,
        DDS_DynamicDataTypeProperty_t props =
        DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);

      DataReaderBase(DDSDomainParticipant *participant,
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
      ~DataReaderBase()
      {
        safe_datareader_->set_listener(NULL, DDS_STATUS_MASK_ALL);
      }

      std::shared_ptr<DDSDynamicDataReader> dd_reader()
      {
        return safe_datareader_;
      }

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

  namespace detail {

    template <class T>
    DDS_ReturnCode_t take_impl(std::shared_ptr<DDSDynamicDataReader> dr,
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
            dd2tuple(data_seq[i], data[i].data());
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

  } // namespace detail

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

} // namespace reflex

#endif // RTIREFLEX_H

