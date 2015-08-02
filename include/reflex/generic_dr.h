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
#include "reflex/type_manager.h"

#include <memory>

namespace reflex {

  namespace sub {

    template <class T>
    class GenericDataReader;

    template <class T>
    class GenericDataReaderListener;

  } // namespace sub

  namespace detail {

    REFLEX_DLL_EXPORT
      std::shared_ptr<DDSDynamicDataReader> initialize_reader(DDSDomainParticipant *participant,
                                                              const DDS_DataReaderQos & drqos,
                                                              const char * topic_name,
                                                              const char * type_name,
                                                              DDSDynamicDataTypeSupport * support,
                                                              DDSDataReaderListener * listener,
                                                              DDS_DynamicDataTypeProperty_t props);

    REFLEX_DLL_EXPORT void dr_deleter(DDSDynamicDataReader * ddReader) throw();

    template <class T>
    DDS_ReturnCode_t take_impl(
      std::shared_ptr<DDSDynamicDataReader> dr,
      std::vector<reflex::sub::Sample<T>> & data,
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
            reflex::read_dynamicdata(data[i].data(), data_seq[i]);
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
    class DataReaderListenerAdapter : public DDSDataReaderListener
    {
      reflex::sub::GenericDataReaderListener<T> * generic_listener_;
      reflex::sub::GenericDataReader<T> * data_reader_;

    public:

      typedef reflex::sub::GenericDataReader<T> DataReaderType;

      explicit DataReaderListenerAdapter(reflex::sub::GenericDataReaderListener<T> * listener)
        : generic_listener_(listener),
          data_reader_(0)
      {
        if (!listener)
        {
          throw std::runtime_error("DataReaderListenerAdapter: NULL listener");
        }
      }

      void set_datareader(reflex::sub::GenericDataReader<T> * dr)
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
        generic_listener_->on_data_available(*data_reader_);
      }
    };


  } // namespace detail

  /**
   * @brief Contains generic data subscriber for adapted types
   */
  namespace sub {

    /**
     * @brief Synchornous data listener for GenericDataReader
     */
    template <class T>
    class GenericDataReaderListener
    {
    public:
      /**
       * @brief Callback invoked when new data is available to read from
       *        GenericDataReader
       */
      virtual void on_data_available(GenericDataReader<T> & dr) = 0;

      virtual ~GenericDataReaderListener() { }
    };

    /**
    * @brief A datareader for adapted aggregate types
    */
    template <class T>
    class GenericDataReader
    {
    public:
      typedef GenericDataReaderListener<T> ListenerType;

    private:
      reflex::TypeManager<T> type_manager_;
      std::shared_ptr<detail::DataReaderListenerAdapter<T>> safe_listener_adapter_;
      std::shared_ptr<DDSDynamicDataReader> safe_datareader_;

    public:

      GenericDataReader(
        DDSDomainParticipant *participant,
        const char * topic_name,
        ListenerType * listener,
        const char * type_name = 0,
        DDS_DynamicDataTypeProperty_t props =
          DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT)

        : type_manager_(props),
          safe_listener_adapter_(listener ? new detail::DataReaderListenerAdapter<T>(listener) : 0),
          safe_datareader_(detail::initialize_reader(
                               participant,
                               DDS_DATAREADER_QOS_DEFAULT,
                               topic_name,
                               type_name,
                               type_manager_.get_type_support(),
                               safe_listener_adapter_.get(),
                               props))
      {
          if (safe_listener_adapter_)
            safe_listener_adapter_->set_datareader(this);
      }

      GenericDataReader(
        DDSDomainParticipant *participant,
        const DDS_DataReaderQos & drqos,
        const char * topic_name,
        ListenerType * listener,
        const char * type_name = 0,
        DDS_DynamicDataTypeProperty_t props =
          DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT)

          : type_manager_(props),
            safe_listener_adapter_(listener ? new detail::DataReaderListenerAdapter<T>(listener) : 0),
            safe_datareader_(detail::initialize_reader(
                                participant,
                                drqos,
                                topic_name,
                                type_name,
                                type_manager_.get_support(),
                                safe_listener_adapter_.get(),
                                props))
      {
          if (safe_listener_adapter_)
            safe_listener_adapter_->set_datareader(this);
      }

      DDS_ReturnCode_t take(
        std::vector<Sample<T>> & data,
        int max_samples = DDS_LENGTH_UNLIMITED,
        DDS_SampleStateMask sample_states = DDS_ANY_SAMPLE_STATE,
        DDS_ViewStateMask view_states = DDS_ANY_VIEW_STATE,
        DDS_InstanceStateMask instance_states = DDS_ANY_INSTANCE_STATE)
      {
        return detail::take_impl<T>(
                    safe_datareader_,
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
                    safe_datareader_,
                    data,
                    max_samples,
                    DDS_ANY_SAMPLE_STATE,
                    DDS_ANY_VIEW_STATE,
                    DDS_ANY_INSTANCE_STATE,
                    cond);
      }

      /**
      * @brief Return the underlying DDSDynamicDataReader
      */
      DDSDynamicDataReader * underlying()
      {
        return safe_datareader_.get();
      }

      /**
      * @brief Return the underlying DDSDynamicDataReader
      */
      DDSDynamicDataReader * operator -> () 
      {
        return underlying();
      }

      /**
      * Return the underlying DynamicData properties.
      */
      const DDS_DynamicDataTypeProperty_t & get_properties() const
      {
        return type_manager_.get_properties();
      }

      /**
      * Return the underlying SafeTypeCode object.
      */
      const SafeTypeCode<T> & get_safe_typecode() const
      {
        return type_manager_.get_safe_typecode();
      }

      /**
      * Return the underlying TypeCode object.
      */
      const DDS_TypeCode* get_typecode() const
      {
        return type_manager_.get_typecode();
      }

      /**
      * Return the underlying DynamicDataTypeSupport.
      */
      const DDSDynamicDataTypeSupport * get_type_support() const
      {
        return type_manager_.get_type_support();
      }

    };

  } // namespace sub
} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/generic_dr.cxx"
#endif



#endif // RTIREFLEX_GENERIC_DR_H

