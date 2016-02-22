/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef REFLEX_ENTITY_COMMON_H
#define REFLEX_ENTITY_COMMON_H

#include <sstream>
#include <iostream>
#include <memory>

#include "ndds/ndds_cpp.h"
#include "reflex/dd_extra.h"
#include "reflex/dllexport.h"

#ifdef WIN32
#define METHOD_NAME __FUNCTION__
#else
#define METHOD_NAME __PRETTY_FUNCTION__
#endif 

namespace reflex {

  namespace detail {

    REFLEX_DLL_EXPORT
    DDSDynamicDataReader * create_entity(DDSDomainParticipant * participant,
                                         DDSSubscriber * subscriber,
                                         DDSTopic * topic,
                                         const DDS_DataReaderQos & drqos,
                                         DDSDataReaderListener * listener,
                                         DDS_StatusMask mask);

    REFLEX_DLL_EXPORT
    DDSDynamicDataWriter * create_entity(DDSDomainParticipant * participant,
                                         DDSPublisher * publisher,
                                         DDSTopic * topic,
                                         const DDS_DataWriterQos & dwqos,
                                         DDSDataWriterListener * listener,
                                         DDS_StatusMask mask);

    REFLEX_DLL_EXPORT void dr_deleter(DDSDynamicDataReader * ddreader) throw();
    REFLEX_DLL_EXPORT void dw_deleter(DDSDynamicDataWriter * ddwriter) throw();

    typedef void (* DataWriterDeleterType)(DDSDynamicDataWriter *) /* throw() */;
    typedef void (* DataReaderDeleterType)(DDSDynamicDataReader *) /* throw() */;

    REFLEX_DLL_EXPORT DataWriterDeleterType get_deleter(DDSDynamicDataWriter *);
    REFLEX_DLL_EXPORT DataReaderDeleterType get_deleter(DDSDynamicDataReader *);

/*
   DDSDynamicDataReader * create_entity(DDSSubscriber * subscriber,
                                         DDSTpoicDescription * topic_desc,
                                         const DDSDataReaderQos & drqos,
                                         const DDSDataReaderListener * listener,
                                         DDS_StatusMas mask)
    {
      return DDSDynamicDataReader::narrow(
          subscriber ?
            subscriber->create_datareader(topic_desc, drqos, listener, mask) :
            participant->create_datareader(topic_desc, drqos, listener, mask));
    }

    DDSDynamicDataWriter * create_entity(DDSPublisher * publisher,
                                         DDSTpoicDescription * topic_desc,
                                         const DDSDataWriterQos & dwqos,
                                         const DDSDataWriterListener * listener,
                                         DDS_StatusMas mask)
    {
      return DDSDynamicDataWriter::narrow(
          publisher ?
            publisher->create_datawriter(topic_desc, dwqos, listener, mask) :
            participant->create_datawriter(topic_desc, dwqos, listener, mask));
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

    void (*)(DDSDynamicDataWriter *) get_deleter(DDSDynamicDataWriter *)
    {
      return dw_deleter;
    }

    void (*)(DDSDynamicDataReader *) get_deleter(DDSDynamicDataReader *)
    {
      return dr_deleter;
    }
*/

    template <class Entity, class Parent, 
              class Qos,    class Listener>
    std::shared_ptr<Entity> initialize_entity(
        DDSDomainParticipant *participant,
        Parent *parent,
        const Qos & qos,
        DDSTopic * topic,
        const std::string & topic_name,
        const std::string & type_name,
        DDSDynamicDataTypeSupport * support,
        Listener * listener,
        DDS_StatusMask mask,
        DDS_DynamicDataTypeProperty_t props,
        const std::string & entity_type)
    {
      DDSTopicDescription * topic_desc = 0;
      std::shared_ptr<DDSTopic> auto_topic;
      std::stringstream err_stream;

      err_stream << METHOD_NAME << ": ";

      if(!participant)
      {
        err_stream << "Invalid argument: NULL DomainParticipant";
        throw std::invalid_argument(err_stream.str());
      }
      
      if(!support)
      {
        err_stream << "Invalid argument: NULL DDSDynamicDataTypeSupport";
        throw std::invalid_argument(err_stream.str());
      }

      //DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
      std::string final_type_name = 
        type_name.empty() ? support->get_type_name() : type_name;

      if(final_type_name.empty())
      {
        err_stream << "runtime_error: empty typename";
        throw std::runtime_error(err_stream.str());
      }

      DDS_ReturnCode_t rc =
        support->register_type(participant, final_type_name.c_str());

      if (rc != DDS_RETCODE_OK)
      {
        err_stream << "Unable to register type = " 
                   << final_type_name << ". Error = ";
        detail::check_retcode(err_stream.str().c_str(), rc); // this will throw
      }

      if(!topic_name.empty())
      {
        if(topic)
        {
          err_stream << "Invalid arguments: Both topic pointer and topic_name should not be specified.";
          throw std::invalid_argument(err_stream.str());
        }

        topic_desc = participant->lookup_topicdescription(topic_name.c_str());
        
        if(!topic_desc)
        {
          topic = participant->create_topic(topic_name.c_str(),
                                            final_type_name.c_str(),
                                            DDS_TOPIC_QOS_DEFAULT,
                                            NULL, /* listener */
                                            DDS_STATUS_MASK_NONE);
          if (topic)
          {
            auto_topic = 
              std::shared_ptr<DDSTopic>(
                  topic, 
                  [participant](DDSTopic *topic) mutable {
                     participant->delete_topic(topic); 
                  });
          }
          else
          {
            err_stream << "Unable to create topic " << topic_name;
            throw std::runtime_error(err_stream.str());
          }
          topic_desc = topic;
        }
      }
      else if(!topic)
      {
        err_stream << "Invalid arguments: Both topic pointer and topic_name are NULL/empty.";
        throw std::invalid_argument(err_stream.str());
      }

      Entity * entity = create_entity(participant, parent, topic, qos, listener, mask);

      if (entity == NULL)
      {
        err_stream << "Unable to narrow to " << entity_type;
        throw std::runtime_error(err_stream.str());
      }

      // everything went well. Release the deleter, if any.
      auto_topic.reset();

      return std::shared_ptr<Entity> (entity, get_deleter(entity));
    }


  } // namespace detail

} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/entity_common.cxx"
#endif 

#endif // REFLEX_ENTITY_COMMON_H

