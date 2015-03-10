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

#include "reflex.h"

namespace reflex {

  namespace detail {

    DataWriterBase::DataWriterBase(
                       DDSDomainParticipant *participant,
                       const char * topic_name,
                       const char * type_name,
                       DDS_TypeCode * tc,
                       DDS_DynamicDataTypeProperty_t props)
      : DataWriterBase(participant,
                       DDS_DATAWRITER_QOS_DEFAULT,
                       topic_name,
                       type_name,
                       tc,
                       0,
                       props)
    { }

    DataWriterBase::DataWriterBase(
                      DDSDomainParticipant *participant,
                      const DDS_DataWriterQos & dwqos,
                      const char * topic_name,
                      const char * type_name,
                      DDS_TypeCode * tc,
                      void * listener,
                      DDS_DynamicDataTypeProperty_t props)
      : safe_typecode_(tc), // tc will be deleted automatically
        safe_typesupport_(new DDSDynamicDataTypeSupport(safe_typecode_.get(), props)),
        safe_datawriter_(), // default constructor
        dd_instance_(safe_typesupport_.get())
    {
      DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      type_name = type_name ? type_name : tc->name(ex);

      if (ex != DDS_NO_EXCEPTION_CODE) {
        std::cerr << "DataWriterBase: Can't get typecode name. ex = "
          << detail::get_readable_ex_code(ex) << std::endl;
      }

      DDS_ReturnCode_t rc =
        safe_typesupport_->register_type(participant, type_name);

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
        NULL,           /* listener */
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
        (DDSDataWriterListener *) listener, /* listener */
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

      safe_datawriter_.reset(ddWriter, &DataWriterBase::deleter);
    }

    void DataWriterBase::deleter(DDSDynamicDataWriter * ddWriter) throw()
    {
      DDS_ReturnCode_t rc =
        ddWriter->get_publisher()->delete_datawriter(ddWriter);

      if (rc != DDS_RETCODE_OK) {
        std::cerr << "Error deleting Dynamic Data Writer."
                  << std::endl;
        // Do not throw.
      }
    }

    DDS_TypeCode * DataWriterBase::get_typecode() const
    {
      return safe_typecode_.get();
    }

    DDSDataWriter * DataWriterBase::underlying()
    {
      return safe_datawriter_.get();
    }

    DDSDataWriter * DataWriterBase::operator -> () {
      return underlying();
    }

    DataReaderBase::DataReaderBase(
                      DDSDomainParticipant *participant,
                      DDSDataReaderListener * listener,
                      const char * topic_name,
                      const char * type_name,
                      DDS_TypeCode * tc,
                      DDS_DynamicDataTypeProperty_t props)
      : DataReaderBase(participant,
                       DDS_DATAREADER_QOS_DEFAULT,
                       listener,
                       topic_name,
                       type_name,
                       tc,
                       props)
    {}

    DataReaderBase::DataReaderBase(
      DDSDomainParticipant *participant,
      const DDS_DataReaderQos & drqos,
      DDSDataReaderListener * listener,
      const char * topic_name,
      const char * type_name,
      DDS_TypeCode * tc,
      DDS_DynamicDataTypeProperty_t props)

      : safe_typecode_(tc), // tc will be deleted automatically
        safe_typesupport_(new DDSDynamicDataTypeSupport(safe_typecode_.get(), props)),
        safe_datareader_(), // default constructor
        dd_instance_(safe_typesupport_.get())
    {
      DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      type_name = type_name ? type_name : tc->name(ex);

      detail::check_exception_code(
        "DataReaderBase::DataWriterBase: Can't get typecode name. ex = ", 
        ex);

      DDS_ReturnCode_t rc =
        safe_typesupport_->register_type(participant, type_name);

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
               << topic_name
               << ".";
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
        DDS_STATUS_MASK_ALL);

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

      safe_datareader_.reset(ddReader, &DataReaderBase::deleter);
    }

    void DataReaderBase::deleter(DDSDynamicDataReader * ddReader) throw()
    {
      DDS_ReturnCode_t rc =
        ddReader->get_subscriber()->delete_datareader(ddReader);

      if (rc != DDS_RETCODE_OK) {
        std::cerr << "DataReaderBase::DataReaderBase: Error deleting DynamicDataReader.\n";
        // Do not throw
      }
    }

    DDS_TypeCode * DataReaderBase::get_typecode() const
    {
      return safe_typecode_.get();
    }

    DDSDataReader * DataReaderBase::underlying()
    {
      return safe_datareader_.get();
    }

    DDSDataReader * DataReaderBase::operator -> ()
    {
      return underlying();
    }


  } // namespace detail

} // namespace reflex
