/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided “as is”, with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <tuple>

#include <ndds/ndds_cpp.h>
#include "reflex.h"
#include "large_type.h"

#define DOMAIN_ID           65
#define APPLICATION_NAME    "Declarative Dynamic Data Example"
#define PERIOD              100*1000*1000 /* 100ms */
#define LARGE_TOPIC_NAME    "N-Tuple"
#define LARGE_TYPE_NAME     "LargeTuple"


void delete_entities(DDSDomainParticipant * participant);

std::tuple<DDSDynamicDataWriter *,
           DDSDomainParticipant *> 
create_ddwriter(const char *type_name, 
                const char *topic_name,
                DDSDynamicDataTypeSupport *type_support);

void write_large_type(int domain_id);
void write_shape_type(int domain_id);
void write_darkart_type(unsigned domain_id);
void write_pi_type(int domainid);
void write_many_members(int domainid);
void read_shape_type(int domain_id);

int main(int argc, const char **argv) 
{
  if(argc <= 1) {
    std::cout << "Please specify either shapes [pub/sub], large, darkart, many\n";
    return 0;
  }
  
  if (std::string(argv[1]) == "shapes")
  {
    if (argc <= 2)
    {
      std::cout << "Please specify pub/sub\n";
      return 0;
    }

    if (std::string(argv[2]) == "pub")
      write_shape_type(DOMAIN_ID);
    else if (std::string(argv[2]) == "sub")
      read_shape_type(DOMAIN_ID);
    else
      std::cout << "Please specify pub/sub\n";
  }
  else if(std::string(argv[1]) == "large")
    write_large_type(DOMAIN_ID);
  else if(std::string(argv[1]) == "darkart")
    write_darkart_type(DOMAIN_ID);
  else if(std::string(argv[1]) == "many")
    write_many_members(DOMAIN_ID);
  else
    std::cout << "Please specify either shapes, large, darkart, many\n";

  return 0;
}


void delete_entities(DDSDomainParticipant * participant)
{
  DDS_ReturnCode_t rc;
  if (participant != NULL) 
  {
      rc = participant->delete_contained_entities();
      if (rc != DDS_RETCODE_OK) {
          std::cerr << "! Unable to delete participant contained entities: "
                    << rc << std::endl;
      }

      rc = DDSDomainParticipantFactory::get_instance()->delete_participant(
                      participant);
      if (rc != DDS_RETCODE_OK) {
          std::cerr << "! Unable to delete participant: " << rc << std::endl;
      }
  }
}

std::tuple<DDSDynamicDataWriter *,
           DDSDomainParticipant *> 
create_ddwriter(const char * type_name,
                const char * topic_name,
                DDSDynamicDataTypeSupport * type_support)
{
    DDS_ReturnCode_t         rc;
    DDSDomainParticipant *   participant = NULL;
    DDSTopic *               topic = NULL;
    DDSDataWriter *dataWriter         = NULL;
    DDSDynamicDataWriter *ddWriter    = NULL;

    participant = DDSDomainParticipantFactory::get_instance()->
                        create_participant(
                        DOMAIN_ID,
                        DDS_PARTICIPANT_QOS_DEFAULT,
                        NULL,   // Listener
                        DDS_STATUS_MASK_NONE);
    if (participant == NULL) {
        std::cerr << "! Unable to create DDS domain participant" << std::endl;
        goto exitFn;
    }

    rc = type_support->register_type(participant, type_name);
    if (rc != DDS_RETCODE_OK) {
        std::cerr << "! Unable to register dynamic type" 
                        << std::endl;
        goto exitFn;
    }

    topic = participant->create_topic(
                        topic_name,
                        type_name, 
                        DDS_TOPIC_QOS_DEFAULT,
                        NULL,           // listener
                        DDS_STATUS_MASK_NONE);
    if (topic == NULL) {
        std::cerr << "! Unable to create topic '"
                  << topic_name
                  << std::endl;
        goto exitFn;
    }
    
    dataWriter = participant->create_datawriter(
                                topic,
                                DDS_DATAWRITER_QOS_DEFAULT,
                                NULL, // listener
                                DDS_STATUS_MASK_NONE);
    if (dataWriter == NULL) {
          std::cerr << "! Unable to create DDS data writer" 
                    << std::endl;
          goto exitFn;
    }   

    ddWriter = DDSDynamicDataWriter::narrow(dataWriter);
    if (ddWriter == NULL) {
       std::cerr << "! Unable to narrow data writer into DDSDynamicDataWriter"
                 << std::endl;
       goto exitFn;
    }
    
    return std::make_tuple(ddWriter, participant);
    
exitFn:
    delete_entities(participant);

    return std::make_tuple(ddWriter=0, participant=0);
}


