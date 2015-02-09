/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided “as is”, with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <time.h>

#ifndef RTI_WIN32
  #include <sys/time.h>
#else
  #include "gettimeofday.h"
#endif

#include "complex.h"
#include "complexSupport.h"
#include "ndds/ndds_namespace_cpp.h"

#include "reflex.h"
#include "darkart.h"

#ifndef RTI_WIN32
  #define sprintf_s(BUF, LEN, FORMAT, ...) sprintf(BUF, FORMAT, __VA_ARGS__)
#endif

using namespace DDS;

/* Delete all entities */
static int publisher_shutdown(
    DomainParticipant *participant)
{
    ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != RETCODE_OK) {
            printf("delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = TheParticipantFactory->delete_participant(participant);
        if (retcode != RETCODE_OK) {
            printf("delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    retcode = DomainParticipantFactory::finalize_instance();
    if (retcode != RETCODE_OK) {
        printf("finalize_instance error %d\n", retcode);
        status = -1;
    }
    return status;
}

int operator - (const timeval & end, const timeval & start)
{
  return (end.tv_sec*1000+end.tv_usec/1000)-(start.tv_sec*1000+start.tv_usec/1000);
}

extern "C" int publisher_main(bool noreflex, int domainId, int sample_count, DomainParticipant **part)
{
    DomainParticipant *participant = NULL;
    Publisher *publisher = NULL;
    Topic *topic = NULL;
    DataWriter *writer = NULL;
    darkart_gen::ChannelDataWriter * darkart_gen_Channel_writer = NULL;
    darkart_gen::Channel *instance = NULL;
    ReturnCode_t retcode;
    InstanceHandle_t instance_handle = HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;  
    //Duration_t send_period = {0,5*1000*1000*100};

    /* To customize participant QoS, use 
       the configuration file USER_QOS_PROFILES.xml */
    participant = TheParticipantFactory->create_participant(
        domainId, PARTICIPANT_QOS_DEFAULT, 
        NULL /* listener */, STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        publisher_shutdown(participant);
        return -1;
    }
    *part = participant;

    /* To customize publisher QoS, use 
       the configuration file USER_QOS_PROFILES.xml */
    publisher = participant->create_publisher(
        PUBLISHER_QOS_DEFAULT, NULL /* listener */, STATUS_MASK_NONE);
    if (publisher == NULL) {
        printf("create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Register type before creating topic */
    type_name = darkart_gen::ChannelTypeSupport::get_type_name();
    retcode = darkart_gen::ChannelTypeSupport::register_type(
        participant, type_name);
    if (retcode != RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize topic QoS, use 
       the configuration file USER_QOS_PROFILES.xml */
    topic = participant->create_topic(
        "darkart_gen::Channel",
        type_name, TOPIC_QOS_DEFAULT, NULL /* listener */,
        STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize data writer QoS, use 
       the configuration file USER_QOS_PROFILES.xml */
    writer = publisher->create_datawriter(
        topic, DATAWRITER_QOS_DEFAULT, NULL /* listener */,
        STATUS_MASK_NONE);
    if (writer == NULL) {
        printf("create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }
    darkart_gen_Channel_writer = darkart_gen::ChannelDataWriter::narrow(writer);
    if (darkart_gen_Channel_writer == NULL) {
        printf("DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Create data sample for writing */

    instance = darkart_gen::ChannelTypeSupport::create_data();
    
    if (instance == NULL) {
        printf("darkart_gen::ChannelTypeSupport::create_data error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* For a data type that has a key, if the same instance is going to be
       written multiple times, initialize the key here
       and register the keyed instance prior to writing */
    timeval start, end;
    time_t seed;
    seed = time(NULL);
    srand((unsigned int) seed);

    gettimeofday(&start, NULL);

    if (noreflex)
    {
      instance->board_id = 0;
      instance_handle = darkart_gen_Channel_writer->register_instance(*instance);

      for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {

        //printf("Writing darkart_gen::Channel, count %d\n", count);

        instance->channel_num = rand();
        instance->channel_id = rand();
        instance->trigger_count = rand();
        instance->sample_bits = rand();
        instance->sample_rate = rand();
        instance->trigger_index = rand();
        instance->nsamps = rand();
        instance->saturated = false;

        sprintf_s(instance->label, 256, "%d", rand());

        retcode = darkart_gen_Channel_writer->write(*instance, instance_handle);
        if (retcode != RETCODE_OK) {
          printf("write error %d\n", retcode);
        }

        //NDDSUtility::sleep(send_period);
      }
      
      /* Delete data sample */
      retcode = darkart_gen::ChannelTypeSupport::delete_data(instance);
      if (retcode != RETCODE_OK) {
        printf("darkart_gen::ChannelTypeSupport::delete_data error %d\n", retcode);
      }
    }
    else 
    {
      const char * darkart_topic = "DarkartChannelTopic";
      const char * channel_typename = "DarkartChannelType";
      reflex::GenericDataWriter<darkart::Channel>
        channel_reflex_writer(participant, darkart_topic, channel_typename);

      char buffer[32];
      darkart::Channel channel;

      gettimeofday(&start, NULL);
      for (count = 0; (sample_count == 0) || (count < sample_count); ++count) {

        //printf("Writing %s, count %d\n", channel_typename, count);

        channel.channel_num = rand();
        channel.channel_id = rand();
        channel.trigger_count = rand();
        channel.sample_bits = rand();
        channel.sample_rate = rand();
        channel.trigger_index = rand();
        channel.nsamps = rand();
        channel.saturated = false;

        sprintf_s(buffer, 32, "%d", rand());
        channel.label = buffer;

        retcode = channel_reflex_writer.write(channel);
        if (retcode != RETCODE_OK) {
          printf("write error %d\n", retcode);
        }

        //NDDSUtility::sleep(send_period);
      }
    }
    gettimeofday(&end, NULL);
    printf("Time taken = %d ms. sample_count = %d\n", end - start, sample_count);

    return 0;
}

int main(int argc, char *argv[])
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */
    bool noreflex = false;

    printf("Usage: reflex-perf [domain-id] [sample count] [noreflex]\n");

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
      sample_count = atoi(argv[2]);
    }
    if (argc >= 4) {
      if (strcmp("noreflex", argv[3]) == 0)
        noreflex = true;
    }

    /* Uncomment this to turn on additional logging
    NDDSConfigLogger::get_instance()->
        set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API, 
                                  NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */
    DomainParticipant *participant = NULL;
    publisher_main(noreflex, domainId, sample_count, &participant);
    return publisher_shutdown(participant);
}
