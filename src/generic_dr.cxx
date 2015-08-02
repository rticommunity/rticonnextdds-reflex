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

#include "reflex/generic_dr.h"
#include "reflex/dd_extra.h"

namespace reflex {

  namespace detail {

    REFLEX_INLINE
    std::shared_ptr<DDSDynamicDataReader> initialize_reader(DDSDomainParticipant *participant,
                                                            const DDS_DataReaderQos & drqos,
                                                            const char * topic_name,
                                                            const char * type_name,
                                                            DDSDynamicDataTypeSupport * support,
                                                            DDSDataReaderListener * listener,
                                                            DDS_DynamicDataTypeProperty_t props)
    {
      DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      type_name = type_name ? type_name : support->get_type_name();

      detail::check_exception_code(
        "DataReaderBase::DataWriterBase: Can't get typecode name. ex = ",
        ex);

      DDS_ReturnCode_t rc =
        support->register_type(participant, type_name);

      if (rc != DDS_RETCODE_OK)
      {
        std::stringstream stream;
        stream << "DataReaderBase::DataReaderBase: Unable to register type = "
          << type_name
          << ". Error = ";
        detail::check_retcode(stream.str().c_str(), rc);
      }

      DDSTopic * topic = participant->create_topic(
                                        topic_name,
                                        type_name,
                                        DDS_TOPIC_QOS_DEFAULT,
                                        NULL,           /* listener */
                                        DDS_STATUS_MASK_NONE);

      if (topic == NULL)
      {
        std::stringstream stream;
        stream << "DataReaderBase::DataReaderBase: Unable to create topic "
               << topic_name;
        throw std::runtime_error(stream.str());
      }

      DDSSubscriber *subscriber = NULL;
      subscriber = participant->create_subscriber(
        DDS_SUBSCRIBER_QOS_DEFAULT,
        NULL,
        DDS_STATUS_MASK_NONE);

      if (subscriber == NULL)
      {
        std::stringstream stream;
        stream << "DataReaderBase::DataReaderBase: Unable to create subscriber";
        throw std::runtime_error(stream.str());
      }

      DDSDataReader *dataReader =
        subscriber->create_datareader(topic,
        drqos,
        listener,
        DDS_DATA_AVAILABLE_STATUS);

      if (dataReader == NULL)
      {
        std::stringstream stream;
        stream << "DataReaderBase::DataReaderBase: Unable to create DataReader";
        throw std::runtime_error(stream.str());
      }

      DDSDynamicDataReader * ddReader =
        DDSDynamicDataReader::narrow(dataReader);

      if (ddReader == NULL)
      {
        std::stringstream stream;
        stream << "DataReaderBase::DataReaderBase: Unable to narrow to DDSDynamicDataReader";
        throw std::runtime_error(stream.str());
      }

      return std::shared_ptr<DDSDynamicDataReader> (ddReader, &dr_deleter);
    }

    REFLEX_INLINE void dr_deleter(DDSDynamicDataReader * ddReader) throw()
    {
      DDS_ReturnCode_t rc =
        ddReader->get_subscriber()->delete_datareader(ddReader);

      if (rc != DDS_RETCODE_OK) {
        std::cerr << "DataReaderBase::DataReaderBase: Error deleting DynamicDataReader.\n";
        // Do not throw
      }
    }

  } // namespace detail

} // namespace reflex
