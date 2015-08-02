/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <sstream>
#include <iostream>

#include "reflex/generic_dw.h"
#include "reflex/dd_extra.h"

namespace reflex {

  namespace detail {

    REFLEX_INLINE std::shared_ptr<DDSDynamicDataWriter> initialize_writer(
                        DDSDomainParticipant *participant,
                        const DDS_DataWriterQos & dwqos,
                        const char * topic_name,
                        const char * type_name,
                        DDSDynamicDataTypeSupport * support,
                        DDSDataWriterListener * listener,
                        DDS_DynamicDataTypeProperty_t props)
    {
      DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      type_name = type_name ? type_name : support->get_type_name();

      detail::check_exception_code("DataWriterBase: Can't get typecode name. ex = ", ex);

      DDS_ReturnCode_t rc =
        support->register_type(participant, type_name);

      if (rc != DDS_RETCODE_OK)
      {
        std::stringstream stream;
        stream << "DataWriterBase::DataWriterBase: Unable to register type = "
               << type_name
               << ". Error = ";
        detail::check_retcode(stream.str().c_str(), rc);
      }

      DDSTopic * topic = participant->create_topic(
        topic_name,
        type_name,
        DDS_TOPIC_QOS_DEFAULT,
        NULL, /* topic listener */
        DDS_STATUS_MASK_NONE);

      if (topic == NULL)
      {
        std::stringstream stream;
        stream << "DataWriterBase::DataWriterBase: Unable to create topic = "
               << topic_name;
        throw std::runtime_error(stream.str());
      }

      DDSPublisher *publisher = NULL;
      publisher = participant->create_publisher(
        DDS_PUBLISHER_QOS_DEFAULT,
        NULL,
        DDS_STATUS_MASK_NONE);

      if (publisher == NULL)
      {
        std::stringstream stream;
        stream << "DataWriterBase::DataWriterBase: Unable to create publisher";
        throw std::runtime_error(stream.str());
      }

      DDSDataWriter *dataWriter =
        publisher->create_datawriter(topic,
        dwqos,
        listener, 
        DDS_STATUS_MASK_ALL | DDS_DATA_WRITER_SAMPLE_REMOVED_STATUS);

      if (dataWriter == NULL)
      {
        std::stringstream stream;
        stream << "DataWriterBase::DataWriterBase: Unable to create DataWriter";
        throw std::runtime_error(stream.str());
      }

      DDSDynamicDataWriter * ddWriter =
        DDSDynamicDataWriter::narrow(dataWriter);

      if (ddWriter == NULL)
      {
        std::stringstream stream;
        stream << "DataWriterBase::DataWriterBase: "
          << "Unable to narrow data writer into DDSDynamicDataWriter";
        throw std::runtime_error(stream.str());
      }

      return std::shared_ptr<DDSDynamicDataWriter>(ddWriter, &dw_deleter);
    }

    REFLEX_INLINE void dw_deleter(DDSDynamicDataWriter * ddWriter) throw()
    {
      DDS_ReturnCode_t rc =
        ddWriter->get_publisher()->delete_datawriter(ddWriter);

      if (rc != DDS_RETCODE_OK) {
        std::cerr << "Error deleting Dynamic Data Writer."
                  << std::endl;
        // Do not throw.
      }
    }

  } // namespace detail

} // namespace reflex
