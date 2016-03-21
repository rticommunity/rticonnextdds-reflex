/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided "as is", with no warranty of any type, including 
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

#define DOMAIN_ID         65 

void delete_entities(DDSDomainParticipant * participant);

std::tuple<DDSDynamicDataWriter *,
           DDSDomainParticipant *> 
create_ddwriter(const char *type_name, 
                const char *topic_name,
                DDSDynamicDataTypeSupport *type_support,
                int domain_id);

void write_pointers(int);
void write_large_type(int domain_id);
void write_shape_type(int domain_id); 
void write_shape_type_extended(int domain_id);
void write_darkart_type(unsigned domain_id);
void write_pi_type(int domainid);
void write_many_members(int domainid);
void write_one_member(int domainid);
void read_shape_type(int domain_id);
void test_all_types(int domain_id);
void hello_qs_publisher(int domainid);
void hello_qs_subscriber(int domainid);
void qs_perf_publisher(int domainid);
void qs_perf_subscriber(int domainid);

void usage()
{
  std::cout << "Please specify one of the following.\n"
            << "domainId shapes [pub|sub|pubex]\n"
            << "domainId qs     [pub|sub]\n"
            << "domainId qsperf [pub|sub]\n"
            << "domainId large, darkart, many, one, all, pointers\n";
}

int main(int argc, const char **argv) 
{
  try {

    // Uncomment this to turn on additional logging
    /*NDDSConfigLogger::get_instance()->
      set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API,
                                NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    int domain_id = DOMAIN_ID;

    if (argc <= 1) {
      usage();
      return 0;
    }

    domain_id = atoi(argv[1]);

    if (std::string(argv[2]) == "shapes")
    {
      if (argc <= 3)
      {
        usage();
        return 0;
      }

      if (std::string(argv[3]) == "pub")
        write_shape_type(domain_id);
      else if (std::string(argv[3]) == "pubex")
        write_shape_type_extended(domain_id);
      else if (std::string(argv[3]) == "sub")
        read_shape_type(domain_id);
      else
        usage();
    }
    if (std::string(argv[2]) == "qs")
    {   
      if (argc <= 3)
      {   
        usage();
        return 0;
      }   

      if (std::string(argv[3]) == "pub")
        hello_qs_publisher(domain_id);
      else if (std::string(argv[3]) == "sub")
        hello_qs_subscriber(domain_id);
      else
        usage();
    }
    if (std::string(argv[2]) == "qsperf")
    {   
      if (argc <= 3)
      {   
        usage();
        return 0;
      }   

      if (std::string(argv[3]) == "pub")
        qs_perf_publisher(domain_id);
      else if (std::string(argv[3]) == "sub")
        qs_perf_subscriber(domain_id);
      else
        usage();
    }
    else if (std::string(argv[2]) == "large")
      write_large_type(domain_id);
    else if (std::string(argv[2]) == "darkart")
      write_darkart_type(domain_id);
    else if (std::string(argv[2]) == "many")
      write_many_members(domain_id);
    else if (std::string(argv[2]) == "pointers")
      write_pointers(domain_id);
    else if (std::string(argv[2]) == "one")
      write_one_member(domain_id);
    else if (std::string(argv[2]) == "all")
      test_all_types(domain_id);
    else
      usage();

    return 0;
  }
  catch (std::exception &ex) {
    std::cerr << ex.what() << std::endl;
  }
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
                DDSDynamicDataTypeSupport * type_support,
                int domain_id)

{
    DDS_ReturnCode_t         rc;
    DDSDomainParticipant *   participant = NULL;
    DDSTopic *               topic = NULL;
    DDSDataWriter *dataWriter         = NULL;
    DDSDynamicDataWriter *ddWriter    = NULL;

    participant = DDSDomainParticipantFactory::get_instance()->
                        create_participant(
                        domain_id,
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


