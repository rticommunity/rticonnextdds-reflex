#include "qs_hello_test.h"
#include "ndds_namespace_cpp.h"
#include <cstdio>

class HelloWorldPubListener : public DDS::DataWriterListener {
  public:
    bool foundQS;
    
    virtual void on_offered_deadline_missed(
            DDSDataWriter *writer, const DDS::OfferedDeadlineMissedStatus &status) {};
     
    virtual void on_liveliness_lost(
            DDSDataWriter *writer, const DDS::LivelinessLostStatus &status) {};
     
    virtual void on_offered_incompatible_qos(
            DDSDataWriter *writer, const DDS::OfferedIncompatibleQosStatus &status) {}
     
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
     
    virtual void on_reliable_writer_cache_changed(
            DDSDataWriter *writer, const DDS::ReliableWriterCacheChangedStatus &status) {};
     
    virtual void on_reliable_reader_activity_changed(
            DDSDataWriter *writer, const DDS::ReliableReaderActivityChangedStatus &status) {};
     
    virtual void on_instance_replaced(
            DDSDataWriter *writer, const DDS::InstanceHandle_t &handle) {};
    
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

void generatedReaderGuidExpr(char * readerGuidExpr)
{
    char * ptr = readerGuidExpr;
 
    snprintf(ptr, 255, "%s%032llx)", "@related_reader_guid.value = &hex(", (long long) readerGuidExpr);
}

void hello_qs_subscriber(int domain_id)
{
/*
  char topicName[255],readerGuidExpr[255];
  DDS_DataReaderQos readerQos;
  DDS_StringSeq cftParams;
  DDSTopic *topic = NULL;

  DDS_ReturnCode_t retcode = DDS_RETCODE_OK;

  DDSDomainParticipant * participant = DDSDomainParticipantFactory::get_instance()->
	    create_participant_with_profile(
             domain_id, "ReflexQs_Library","ReflexQs_Profile",
             NULL , DDS_STATUS_MASK_NONE);
  if (participant == NULL) {
    throw std::runtime_error("Unable to create participant");
  }


  DDSDomainParticipant * participant = DDSDomainParticipantFactory::get_instance()->
	    create_participant(
             domain_id, DDS_PARTICIPANT_QOS_DEFAULT,
             NULL, DDS_STATUS_MASK_NONE);
  if (participant == NULL) {
    throw std::runtime_error("Unable to create participant");
  }
	//LeoListener leo_listener;
	LeoListener *leo_listener = new LeoListener();

 
	reflex::SafeTypeCode<single_member>
    stc(reflex::make_typecode<single_member>()); 

//register the type
	DDSDynamicDataTypeSupport obj(stc.get(),DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);
	if(obj.register_type(participant,"single_member") != DDS_RETCODE_OK)
	{
		std::cout <<"\n register_type error \n";
		return;
	}

	sprintf(topicName,"%s@%s", "Single", "SharedSubscriber");

	topic = participant->create_topic(
			topicName,
			"single_member", DDS_TOPIC_QOS_DEFAULT, NULL ,
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
// reflex reader
	reflex::sub::DataReader<single_member>
	datareader(reflex::sub::DataReaderParams(participant)
			.topic_name(topicName)
			.listener(&leo_listener));


//non-Reflex reader
DDSSubscriber *subscriber = participant->create_subscriber(
          DDS_SUBSCRIBER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
if (subscriber == NULL) {
        printf("create_subscriber error\n");
        return ; 
    }  
subscriber->get_default_datareader_qos(readerQos);

retcode = DDSPropertyQosPolicyHelper::assert_property(
        readerQos.property,
        "dds.data_reader.shared_subscriber_name",
        "SharedSubscriber",
        DDS_BOOLEAN_TRUE);
    if (retcode != DDS_RETCODE_OK) {
        printf("assert_property error %d\n", retcode);
        //subscriber_shutdown(participant);
        return ;

}

    DDSDataReader *reader = subscriber->create_datareader(
        cftTopic, readerQos, leo_listener,
        DDS_STATUS_MASK_ALL);
    if (reader == NULL) {
        printf("create_datareader error\n");
//        subscriber_shutdown(participant);
        return ;
    }
	std::cout << "\nReader created" << std::endl;
    // Wait for Queuing Service discovery 
    printf("Waiting to discover SharedReaderQueue ...\n");

  DDS_Duration_t period{ 0, 100 * 1000 * 1000 };
    while (!leo_listener->foundQS) {
        NDDSUtility::sleep(period);
    }
std::cout <<"\n found QS! ";
	for (;;)
{
	std::cout << "Polling\n";
	DDS_Duration_t poll_period = { 4, 0 };
	NDDSUtility::sleep(poll_period);
}
*/
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
             .listener(&listener));

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

