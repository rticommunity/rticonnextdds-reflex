#include "qs_perf_test.h"
#include "ndds_namespace_cpp.h"
#include <cstdio>

#define MOD 10000

class PerfHelloWorldPubListener : public DDS::DataWriterListener {
    int accept_count;
    int reject_count;
    int count;
  public:
    bool foundQS;
    
    virtual void on_publication_matched(
            DDSDataWriter *writer, const DDS::PublicationMatchedStatus &status) 
    {
      DDS::SubscriptionBuiltinTopicData subscriptionData;
      DDS::ReturnCode_t retcode;
        
        if (status.current_count_change > 0) {
            retcode = writer->get_matched_subscription_data (
                    subscriptionData, 
                    status.last_subscription_handle);
            
            if (retcode == DDS_RETCODE_OK) {
                if (subscriptionData.service.kind == DDS_QUEUING_SERVICE_QOS ) {
                    foundQS = true;
                }
            }
        }
    }
     
    virtual void on_application_acknowledgment(DDSDataWriter *writer, const DDS::AcknowledgmentInfo &info)
    {
        if (info.valid_response_data) 
        {
            count++;
            if (info.response_data.value[0] == 1) {
                accept_count++;
            } else {
                reject_count++;
            }
            
            if(count % MOD == 0)
            {
              printf("Accepted %d and rejected %d.\n", accept_count, reject_count);
            }
        }
    }
    
    PerfHelloWorldPubListener() {
        foundQS = false;
        count = 0;
        accept_count = 0;
        reject_count = 0;
    }
};
class PerfReaderListener : public DDSDataReaderListener
{
  reflex::sub::DataReader<PerfHelloWorld> dr;
  int count;

  public:
  bool foundQS;

  void on_data_available(DDSDataReader * reader) override
  {
    DDS_AckResponseData_t responseData;
    if(dr.underlying())
    {
      std::vector<reflex::sub::Sample<PerfHelloWorld>> samples;
      dr.take(samples);
      responseData.value.ensure_length(1,1);
      responseData.value[0] = 1;
      for (auto &ss : samples)
      {
        if (ss.info().valid_data)
        {
          count++;
          if((count % MOD) == 0)
            std::cout << "\nmessageId = " << ss->messageId;
        }
        if (dr.underlying()->acknowledge_all(responseData) != DDS_RETCODE_OK)
          std::cout<<"acknowledge_all error"<<std::endl;
      }
    }
  }

  void on_subscription_matched(DDSDataReader* reader, 
      const DDS::SubscriptionMatchedStatus& status)
  {
    DDS_PublicationBuiltinTopicData publicationData;
    DDS_ReturnCode_t retcode;
    if (status.current_count_change > 0) {
      retcode = reader->get_matched_publication_data (
          publicationData,
          status.last_publication_handle);

      if (retcode == DDS_RETCODE_OK) {
        if (publicationData.service.kind == DDS_QUEUING_SERVICE_QOS ) { 
          foundQS = true;
        }   
      }   
    }   
  }
  void set_reflex_datareader(reflex::sub::DataReader<PerfHelloWorld> reader)
  {
    dr = reader;
  }
  PerfReaderListener()
  {
    foundQS= false;
    count = 0;
  }
};


void getReaderGuidExpr(char * readerGuidExpr)
{
    char * ptr = readerGuidExpr;
 
    snprintf(ptr, 255, "%s%032llx)", "@related_reader_guid.value = &hex(", (long long) readerGuidExpr);
}

void qs_perf_subscriber(int domain_id)
{
  char topicName[255],readerGuidExpr[255];
  DDS_DataReaderQos readerQos;
  DDS_StringSeq cftParams;
  DDSTopic *topic = NULL;

  DDS_ReturnCode_t retcode = DDS_RETCODE_OK;
  DDSDomainParticipant * participant = DDSDomainParticipantFactory::get_instance()->
	    create_participant(
             domain_id, DDS_PARTICIPANT_QOS_DEFAULT,
             NULL, DDS_STATUS_MASK_NONE);
  if (participant == NULL) {
    throw std::runtime_error("Unable to create participant");
  }

  participant->get_default_datareader_qos(readerQos);
  retcode = DDSPropertyQosPolicyHelper::assert_property(
        readerQos.property,
        "dds.data_reader.shared_subscriber_name",
        "SharedSubscriber",
        DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
      printf("assert_property error %d\n", retcode);
      return ;

    }
    PerfReaderListener reader_listener;

    reflex::SafeTypeCode<PerfHelloWorld>
      stc(reflex::make_typecode<PerfHelloWorld>()); 

    //register the type
    DDSDynamicDataTypeSupport obj(stc.get(),DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);
    if(obj.register_type(participant,"PerfHelloWorld") != DDS_RETCODE_OK)
    {
      std::cout <<"\n register_type error \n";
      return;
    }

    sprintf(topicName,"%s@%s", "PerfHelloWorldTopic", "SharedSubscriber");

    topic = participant->create_topic(
        topicName,
        "PerfHelloWorld", DDS_TOPIC_QOS_DEFAULT, NULL ,
        DDS_STATUS_MASK_NONE);
    if (topic == NULL)
    {	
      std::cout <<"\n topic error! \n";
      return;
    } 
    getReaderGuidExpr(readerGuidExpr);

    DDSContentFilteredTopic * cftTopic = participant->create_contentfilteredtopic(
        topicName,
        topic,
        readerGuidExpr,
        cftParams);
    if ( cftTopic == NULL)
    {
      std::cout <<"\n cft error\n";
      return;
    }
    reflex::sub::DataReader<PerfHelloWorld>
      datareader(reflex::sub::DataReaderParams(participant)
          .topic_name(topicName)
          .listener(&reader_listener)
          .datareader_qos(readerQos));

    reader_listener.set_reflex_datareader(datareader);

    printf("Waiting to discover SharedReaderQueue ...\n");

    DDS_Duration_t period{ 0, 100 * 1000 * 1000 };
    while (!reader_listener.foundQS) {
      NDDSUtility::sleep(period);
    }
    std::cout <<"Found SharedReaderQueue!\n";
    for (;;)
    {       
      //std::cout << "Polling\n";
      DDS_Duration_t poll_period = { 4, 0 };
      NDDSUtility::sleep(poll_period);
    }

}

void qs_perf_publisher(int domain_id)
{
  DDS_ReturnCode_t         rc = DDS_RETCODE_OK;
  DDSDomainParticipant *   participant = NULL;
  DDS_DynamicDataTypeProperty_t props;
  DDS_Duration_t period{ 0, 1 * 1000 * 1000 };
  const char *topic_name = "PerfHelloWorldTopic";

  PerfHelloWorldPubListener writerListener;
  participant = DDSDomainParticipantFactory::get_instance()->
  	    create_participant(
               domain_id,DDS_PARTICIPANT_QOS_DEFAULT,
               NULL /* listener */, DDS_STATUS_MASK_NONE);
  
  if (participant == NULL) {
    std::cerr << "! Unable to create DDS domain participant" 
              << std::endl;
    return;
  } 

  PerfHelloWorldPubListener listener;

  reflex::pub::DataWriter<PerfHelloWorld>
     writer(reflex::pub::DataWriterParams(participant)
             .topic_name(topic_name)
             .listener_statusmask(DDS_STATUS_MASK_ALL)
             .listener(&listener));

  /* Wait for Queuing Service discovery */
     std::cout <<"\nWaiting to discover SharedReaderQueue ...\n";

  while (!listener.foundQS) {
    NDDSUtility::sleep(period);
  }
     
  std::cout<<"\nSharedReaderQueue discovered...\n";

  int i = 1;
  while(1)
  {
    PerfHelloWorld obj;
    obj.messageId = i++;
    rc = writer.write(obj);
    if((i % MOD) == 0)
    {
      std::cout <<"Wrote "<< i << "\n";
      if (rc != DDS_RETCODE_OK) {
           std::cerr << "Write error = "
                     << reflex::detail::get_readable_retcode(rc)
                     << std::endl;
      }
    }
    NDDSUtility::sleep(period);
  }
}

