/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "reflex/entity_common.h"

#ifdef WIN32
#define METHOD_NAME __FUNCTION__
#else
#define METHOD_NAME __PRETTY_FUNCTION__
#endif 

namespace reflex {

  namespace detail {

    REFLEX_INLINE 
      DDSDynamicDataReader * create_entity(DDSDomainParticipant * participant,
                                           DDSSubscriber * subscriber,
                                           DDSTopicDescription * topic_desc,
                                           const DDS_DataReaderQos & drqos,
                                           DDSDataReaderListener * listener,
                                           DDS_StatusMask mask)
    {
      return DDSDynamicDataReader::narrow(
          subscriber ?
            subscriber->create_datareader(topic_desc, drqos, listener, mask) :
            participant->create_datareader(topic_desc, drqos, listener, mask));
    }

    REFLEX_INLINE 
      DDSDynamicDataWriter * create_entity(DDSDomainParticipant * participant,
                                           DDSPublisher * publisher,
                                           DDSTopicDescription * topic_desc,
                                           const DDS_DataWriterQos & dwqos,
                                           DDSDataWriterListener * listener,
                                           DDS_StatusMask mask)
    {
      DDSTopic * topic = dynamic_cast<DDSTopic *>(topic_desc);

      return DDSDynamicDataWriter::narrow(
          publisher ?
            publisher->create_datawriter(topic, dwqos, listener, mask) :
            participant->create_datawriter(topic, dwqos, listener, mask));
    }

    REFLEX_INLINE void dr_deleter(DDSDynamicDataReader * ddreader) throw()
    {
      DDS_ReturnCode_t rc =
        ddreader->get_subscriber()->delete_datareader(ddreader);

      if (rc != DDS_RETCODE_OK) {
        std::cerr << METHOD_NAME << "Unable to delete DynamicDataReader.";
        // Do not throw
      }
    }

    REFLEX_INLINE void dw_deleter(DDSDynamicDataWriter * ddwriter) throw()
    {
      DDS_ReturnCode_t rc =
        ddwriter->get_publisher()->delete_datawriter(ddwriter);

      if (rc != DDS_RETCODE_OK) {
        std::cerr << METHOD_NAME << "Unable to delete DynamicDataWriter.";
        // Do not throw
      }
    }

    REFLEX_INLINE DataWriterDeleterType get_deleter(DDSDynamicDataWriter *)
    {
      return dw_deleter;
    }

    REFLEX_INLINE DataReaderDeleterType get_deleter(DDSDynamicDataReader *)
    {
      return dr_deleter;
    }

  } // namespace detail

} // namespace reflex

#undef METHOD_NAME
