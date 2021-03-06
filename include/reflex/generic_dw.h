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
#include "reflex/enable_if.h"
#include "reflex/type_manager.h"

#include <memory>

namespace reflex {

  namespace detail { 

    REFLEX_DLL_EXPORT 
      std::shared_ptr<DDSDynamicDataWriter> initialize_writer(DDSDomainParticipant *participant,
                                                              const DDS_DataWriterQos & dwqos,
                                                              const char * topic_name,
                                                              const char * type_name,
                                                              DDSDynamicDataTypeSupport * support,
                                                              DDSDataWriterListener * listener,
                                                              DDS_DynamicDataTypeProperty_t props);

    REFLEX_DLL_EXPORT void dw_deleter(DDSDynamicDataWriter * ddWriter) throw();

  } // namespace detail

  /**
  * @brief Contains generic data publisher for adapted types
  */
  namespace pub {

    /**
     * @brief A datawriter for adapted aggregate types
     *
     */
    template <class T>
    class GenericDataWriter
    {
      TypeManager<T> type_manager_;
      AutoDynamicData dd_instance_;
      std::shared_ptr<DDSDynamicDataWriter> safe_datawriter_;

    public:
      GenericDataWriter(DDSDomainParticipant *participant,
                        const char * topic_name,
                        const char * type_name = 0,
                        DDS_DynamicDataTypeProperty_t props =
                          DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT)
       : type_manager_(props),
         dd_instance_(type_manager_.get_type_support()),
         safe_datawriter_(detail::initialize_writer(
                                    participant,
                                    DDS_DATAWRITER_QOS_DEFAULT,
                                    topic_name,
                                    type_name,
                                    type_manager_.get_type_support(),
                                    0, /* listener */
                                    props))
      { }

      GenericDataWriter(DDSDomainParticipant *participant,
                        DDS_DataWriterQos & dwqos,
                        const char * topic_name,
                        const char * type_name = 0,
                        DDSDataWriterListener * listener = 0,
                        DDS_DynamicDataTypeProperty_t props =
                          DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT)
        : type_manager_(props),
          dd_instance_(type_manager_.get_type_support()),
          safe_datawriter_(detail::initialize_writer(
                                      participant,
                                      dwqos,
                                      topic_name,
                                      type_name,
                                      type_manager_.get_type_support(),
                                      listener,
                                      props))
      { }

      /**
       * @brief Publish an instance of type T
       * @param data An object of type T
       * @param handle (Optional) A handle to a DDS instance
       * @return DDS_RETCODE_OK if successful.
       */
      DDS_ReturnCode_t write(const T & data, 
                             const DDS_InstanceHandle_t& handle = DDS_HANDLE_NIL)
      {
        reflex::write_dynamicdata(dd_instance_, data);
        return safe_datawriter_->write(*dd_instance_.get(), handle);
      }

      /**
      * @brief Publish an instance of type T
      * @param data An object of type T
      * @param params  Special parameters for publishing a value
      * @return DDS_RETCODE_OK if successful.
      */
      DDS_ReturnCode_t write_w_params(const T & data, DDS_WriteParams_t & params)
      {
        reflex::write_dynamicdata(dd_instance_, data);
        return safe_datawriter_->write_w_params(*dd_instance_.get(), params);
      }

      /**
       * @brief Return the underlying DDSDynamicDataWriter
       */
      DDSDynamicDataWriter * underlying() 
      {
        return safe_datawriter_.get();
      }

      /**
      * @brief Return the underlying DDSDynamicDataWriter
      */
      DDSDynamicDataWriter * operator -> () {
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

  } // namespace pub
} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/generic_dw.cxx"
#endif

#endif // RTIREFLEX_GENERIC_DW_H
