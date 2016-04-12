/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DATAREADER_H
#define RTIREFLEX_DATAREADER_H

#include "ndds/connext_cpp/connext_cpp_infrastructure.h" 

#include "reflex/safe_typecode.h"
#include "reflex/auto_dd.h"
#include "reflex/dd2tuple.h"
#include "reflex/dllexport.h"
#include "reflex/sample.h"
#include "reflex/reflex_fwd.h"
#include "reflex/enable_if.h"
#include "reflex/type_manager.h"
#include "reflex/datareader_params.h"
#include "reflex/entity_common.h"

#include <memory>

namespace reflex {

  namespace sub {

    template <class T>
    class DataReader;

  } // namespace sub

  namespace detail {

    template <class T>
    connext::LoanedSamples<DDS_DynamicData> take_loan_impl(
      std::shared_ptr<DDSDynamicDataReader> dr,
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
        connext::LoanedSamples<DDS_DynamicData> empty;
        return move(empty);
      }

      // throw if rc != DDS_RETCODE_OK 
      detail::check_retcode("! Unable to take data from data reader, error ", rc);

      return connext::LoanedSamples<DDS_DynamicData>::move_construct_from_loans(
              dr.get(), data_seq, info_seq);
    }

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

  } // namespace detail

  /**
   * @brief Contains DataReader for adapted types
   */
  namespace sub {

    /**
    * @brief A datareader for adapted aggregate types
    */
    template <class T>
    class DataReader
    {
      std::shared_ptr<reflex::TypeManager<T>> type_manager_;
      std::shared_ptr<DDSDynamicDataReader> safe_datareader_;

    public:

      DataReader() {}

      DataReader(const DataReaderParams & params)
      {
        if(!params.domain_participant())
          throw std::logic_error("DataReaderParams: NULL DDSDomainParticipant");
        
        if(params.topic() && !params.topic_name().empty())
          throw std::logic_error("DataReaderParams: Ambiguous parameters. Both topic_name and topic provided");

        type_manager_ = 
          std::make_shared<reflex::TypeManager<T>>(params.dynamicdata_type_property());
        
        safe_datareader_ = detail::initialize_entity<DDSDynamicDataReader>(
            params.domain_participant(),
            params.subscriber(),
            params.datareader_qos(),
            params.topic(),
            params.topic_name(),
            params.type_name(),
            type_manager_->get_type_support(),
            params.listener(),
            params.listener_statusmask(),
            params.dynamicdata_type_property(),
            "DynamicDataReader");
      }

      connext::LoanedSamples<DDS_DynamicData> take(
        int max_samples = DDS_LENGTH_UNLIMITED,
        DDS_SampleStateMask sample_states = DDS_ANY_SAMPLE_STATE,
        DDS_ViewStateMask view_states = DDS_ANY_VIEW_STATE,
        DDS_InstanceStateMask instance_states = DDS_ANY_INSTANCE_STATE)
      {
        return detail::take_loan_impl<T>(
                    safe_datareader_,
                    max_samples,
                    sample_states,
                    view_states,
                    instance_states);
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
        return type_manager_->get_properties();
      }

      /**
      * Return the underlying SafeTypeCode object.
      */
      const SafeTypeCode<T> & get_safe_typecode() const
      {
        return type_manager_->get_safe_typecode();
      }

      /**
      * Return the underlying TypeCode object.
      */
      const DDS_TypeCode* get_typecode() const
      {
        return type_manager_->get_typecode();
      }

      /**
      * Return the underlying DynamicDataTypeSupport.
      */
      const DDSDynamicDataTypeSupport * get_type_support() const
      {
        return type_manager_->get_type_support();
      }

      ~DataReader() 
      {
        if(safe_datareader_.use_count() == 1)
          safe_datareader_->set_listener(0, DDS_STATUS_MASK_NONE);
      }
    };

  } // namespace sub
} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/datareader.cxx"
#endif



#endif // RTIREFLEX_DATAREADER_H

