#include "qs_hello_test.h"
#include "ndds_namespace_cpp.h"
#include <cstdio>

class HelloWorldPubListener : public DDS::DataWriterListener {
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
        const char * response;
        
        if (info.valid_response_data) {
            if (info.response_data.value[0] == 1) {
                response = "Accepted";
            } else {
                response = "Rejected";
            }
            
            printf("Received ACK from QS for sample with sequence number: %d Process result: %s\n",
                    info.sample_identity.sequence_number.low, 
                    response);
        }
    }
    
    HelloWorldPubListener() {
        foundQS = false;
    }
};
class ReaderListener : public DDSDataReaderListener
{
  reflex::sub::DataReader<HelloWorld> dr;
  public:
  bool foundQS;

  void on_data_available(DDSDataReader * reader) override
  {
    DDS_AckResponseData_t responseData;
    if(dr.underlying())
    {
      std::vector<reflex::sub::Sample<HelloWorld>> samples;
      dr.take(samples);
      responseData.value.ensure_length(1,1);
      responseData.value[0] = 1;
      for (auto &ss : samples)
      {
        if (ss.info().valid_data)
        {
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
  void set_reflex_datareader(reflex::sub::DataReader<HelloWorld> reader)
  {
    dr = reader;
  }
  ReaderListener()
  {
    foundQS= false;
  }
};


void generatedReaderGuidExpr(char * readerGuidExpr)
{
    char * ptr = readerGuidExpr;
 
    snprintf(ptr, 255, "%s%032llx)", "@related_reader_guid.value = &hex(", (long long) readerGuidExpr);
    
}

void hello_qs_subscriber(int domain_id)
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
    ReaderListener reader_listener;

    reflex::SafeTypeCode<HelloWorld>
      stc(reflex::make_typecode<HelloWorld>()); 

    //register the type
    DDSDynamicDataTypeSupport obj(stc.get(),DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);
    if(obj.register_type(participant,"HelloWorld") != DDS_RETCODE_OK)
    {
      std::cout <<"\n register_type error \n";
      return;
    }

    snprintf(topicName,255,"%s@%s", "HelloWorldTopic", "SharedSubscriber");

    topic = participant->create_topic(
        topicName,
        "HelloWorld", DDS_TOPIC_QOS_DEFAULT, NULL ,
        DDS_STATUS_MASK_NONE);
    if (topic == NULL)
    {	
      std::cout <<"\n topic error! \n";
      return;
    } 
    generatedReaderGuidExpr(readerGuidExpr);

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
    reflex::sub::DataReader<HelloWorld>
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
      std::cout << "Polling\n";
      DDS_Duration_t poll_period = { 4, 0 };
      NDDSUtility::sleep(poll_period);
    }

}

void hello_qs_publisher(int domain_id)
{
  DDS_ReturnCode_t         rc = DDS_RETCODE_OK;
  DDSDomainParticipant *   participant = NULL;
  DDS_DynamicDataTypeProperty_t props;
  DDS_Duration_t period{ 0, 100 * 1000 * 1000 };
  const char *topic_name = "HelloWorldTopic";

  HelloWorldPubListener writerListener;
  participant = DDSDomainParticipantFactory::get_instance()->
  	    create_participant(
               domain_id,DDS_PARTICIPANT_QOS_DEFAULT,
               NULL /* listener */, DDS_STATUS_MASK_NONE);
  
  if (participant == NULL) {
    std::cerr << "! Unable to create DDS domain participant" 
              << std::endl;
    return;
  } 

  HelloWorldPubListener listener;

  reflex::pub::DataWriter<HelloWorld>
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
    HelloWorld obj;
    obj.messageId = i++;
    rc = writer.write(obj);
    if (rc != DDS_RETCODE_OK) {
         std::cerr << "Write error = "
                   << reflex::detail::get_readable_retcode(rc)
                   << std::endl;
    }

    NDDSUtility::sleep(period);
  }
}

