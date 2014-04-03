/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "reflex.h"

DataWriterBase::DataWriterBase(DDSDomainParticipant *participant,
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

DataWriterBase::DataWriterBase(DDSDomainParticipant *participant,
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
      << get_readable_ex_code(ex) << std::endl;
  }

  DDS_ReturnCode_t rc =
    safe_typesupport_->register_type(participant, type_name);

  if (rc != DDS_RETCODE_OK) {
    std::cerr << "! Unable to register type = "
      << type_name
      << std::endl;
    throw 0;
  }

  DDSTopic * topic = participant->create_topic(
    topic_name,
    type_name,
    DDS_TOPIC_QOS_DEFAULT,
    NULL,           /* listener */
    DDS_STATUS_MASK_NONE);
  if (topic == NULL) {
    std::cerr << "! Unable to create topic '"
      << topic_name
      << std::endl;
    throw 0;
  }

  DDSPublisher *publisher = NULL;
  publisher = participant->create_publisher(
                 DDS_PUBLISHER_QOS_DEFAULT,
                 NULL,
                 DDS_STATUS_MASK_NONE);
  if (publisher == NULL) {
     printf("create_publisher error\n");
     throw 0;
  }

  DDSDataWriter *dataWriter =
    publisher->create_datawriter(topic,
    dwqos,
    (DDSDataWriterListener *) listener, /* listener */
    DDS_STATUS_MASK_ALL | DDS_DATA_WRITER_SAMPLE_REMOVED_STATUS);
  if (dataWriter == NULL) {
    std::cerr << "! Unable to create DDS data writer"
      << std::endl;
    throw 0;
  }

  DDSDynamicDataWriter * ddWriter =
    DDSDynamicDataWriter::narrow(dataWriter);
  if (ddWriter == NULL) {
    std::cerr << "! Unable to narrow data writer into DDSDynamicDataWriter"
      << std::endl;
    throw 0;
  }

  safe_datawriter_.reset(ddWriter, &DataWriterBase::deleter);
}

void DataWriterBase::deleter(DDSDynamicDataWriter * ddWriter) throw ()
{
  DDS_ReturnCode_t rc =
    ddWriter->get_publisher()->delete_datawriter(ddWriter);

  if (rc != DDS_RETCODE_OK) {
    std::cerr << "Error deleting Dynamic Data Writer.\n";
  }
}

DDS_TypeCode * DataWriterBase::get_typecode() const
{
  return safe_typecode_.get();
}

DataReaderBase::DataReaderBase(DDSDomainParticipant *participant,
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

DataReaderBase::DataReaderBase(DDSDomainParticipant *participant,
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

  if (ex != DDS_NO_EXCEPTION_CODE) {
    std::cerr << "DataWriterBase: Can't get typecode name. ex = "
      << get_readable_ex_code(ex) << std::endl;
  }

  DDS_ReturnCode_t rc =
    safe_typesupport_->register_type(participant, type_name);

  if (rc != DDS_RETCODE_OK) {
    std::cerr << "! Unable to register type = "
      << type_name
      << std::endl;
    throw 0;
  }

  DDSTopic * topic = participant->create_topic(
    topic_name,
    type_name,
    DDS_TOPIC_QOS_DEFAULT,
    NULL,           /* listener */
    DDS_STATUS_MASK_NONE);
  if (topic == NULL) {
    std::cerr << "! Unable to create topic '"
      << topic_name
      << std::endl;
    throw 0;
  }

  DDSDataReader *dataReader =
    participant->create_datareader(topic,
    drqos, 
    listener,
    DDS_STATUS_MASK_ALL);
  if (dataReader == NULL) {
    std::cerr << "! Unable to create DDS data reader"
      << std::endl;
    throw 0;
  }

  DDSDynamicDataReader * ddReader =
    DDSDynamicDataReader::narrow(dataReader);
  if (ddReader == NULL) {
    std::cerr << "! Unable to narrow data reader into DDSDynamicDataReader"
      << std::endl;
    throw 0;
  }

  safe_datareader_.reset(ddReader, &DataReaderBase::deleter);
}

void DataReaderBase::deleter(DDSDynamicDataReader * ddReader) throw ()
{
  DDS_ReturnCode_t rc =
    ddReader->get_subscriber()->delete_datareader(ddReader);

  if (rc != DDS_RETCODE_OK) {
    std::cerr << "Error deleting Dynamic Data Reader.\n";
  }
}

DDS_TypeCode * DataReaderBase::get_typecode() const
{
  return safe_typecode_.get();
}





