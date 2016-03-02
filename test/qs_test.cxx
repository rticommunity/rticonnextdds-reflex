/* Reflex with Queuing service
*
*
*/

#include "leo_test.h"
class LeoListener : public reflex::sub::DataReaderListener<single_member>
{
public:


  void on_data_available(reflex::sub::DataReader<single_member> & dr) override
  {
    std::vector<reflex::sub::Sample<single_member>> samples;
    dr.take(samples);
    for (auto &ss : samples)
    {
      if (ss.info().valid_data)
        std::cout << "m1 = " << ss->m1 << std::endl;
 
      if( dr.underlying()->acknowledge_all() != DDS_RETCODE_OK )
            std::cout<<"acknowledge_all error"<<std::endl;
      else
            std::cout << "\n acknowledged all !! \n" <<std::endl;
 	std::cout <<"--------------------------------------\n";
    }
  }
};

void generatedReaderGuidExpr(char * readerGuidExpr)
{
    char * ptr = readerGuidExpr;
 
    strcpy(ptr, "@related_reader_guid.value = &hex(");
    ptr+= strlen(ptr);
    sprintf(ptr,"%032llx",(long long)readerGuidExpr);
    ptr+= strlen(ptr);
    strcpy(ptr,")");
}

void read_single_member(int domain_id)
{
  char topicName[255],readerGuidExpr[255];
  //DDSContentFilteredTopic * cftTopic = NULL;
  DDS_DataReaderQos readerQos;
  DDS_StringSeq cftParams;
  DDSTopic *topic = NULL;

  DDS_ReturnCode_t retcode = DDS_RETCODE_OK;

  DDSDomainParticipant * participant =
    DDSDomainParticipantFactory::get_instance()->
      create_participant_with_profile(domain_id,
                         "DefaultLibrary",
			 "Reliable",
                         NULL,   // Listener
                         DDS_STATUS_MASK_NONE);

  if (participant == NULL) {
    throw std::runtime_error("Unable to create participant");
  }

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
/*DDSSubscriber *subscriber = participant->create_subscriber(
	DDS_SUBSCRIBER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
    if (subscriber == NULL) {
        printf("create_subscriber error\n");
        return ;
    }
*/

  LeoListener leo_listener;

  std::cout << "Subscribed to Single\n";
 
reflex::SafeTypeCode<single_member>
    stc(reflex::make_typecode<single_member>()); 
DDSDynamicDataTypeSupport obj(stc.get(),DDS_DYNAMIC_DATA_TYPE_PROPERTY_DEFAULT);
//register the type
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

   std::cout << "\n In leo_test.cxx topic is  "<< \
   (DDSContentFilteredTopic *)cftTopic <<"\nBefore reader is created" << std::endl;
 
/* reflex::sub::DataReader<single_member>
    datareader(reflex::sub::DataReaderParams(participant)
                .topic( (DDSTopic *) cftTopic)
                .listener(&leo_listener));
*/



  reflex::sub::DataReader<single_member>
    datareader(reflex::sub::DataReaderParams(participant)
                .topic_name(topicName)
                .listener(&leo_listener));
 
 std::cout << "\nReader created" << std::endl;

  for (;;)
  {
    std::cout << "Polling\n";
    DDS_Duration_t poll_period = { 4, 0 };
    NDDSUtility::sleep(poll_period);
  }
}

void write_single_member(int domain_id)
{
  DDS_ReturnCode_t         rc = DDS_RETCODE_OK;
  DDSDomainParticipant *   participant = NULL;
  DDS_DynamicDataTypeProperty_t props;
  DDS_Duration_t period{ 0, 100 * 1000 * 1000 };

  const char *topic_name = "Single";

  reflex::SafeTypeCode<single_member>
    stc(reflex::make_typecode<single_member>()); //stc is of type single_member

  reflex::detail::print_IDL(stc.get(), 0); //get() return a DDSTypeCode

  participant = DDSDomainParticipantFactory::get_instance()->
    create_participant_with_profile(domain_id,
		       "DefaultLibrary",
		       "Reliable",	
                       NULL,   // Listener
                       DDS_STATUS_MASK_NONE);

  if (participant == NULL) {
    std::cerr << "! Unable to create DDS domain participant" << std::endl;
    return;
  }

  DDS_DataWriterQos datawriter_qos;
  rc = participant->get_default_datawriter_qos(datawriter_qos);
  if (rc != DDS_RETCODE_OK) {
     std::cerr << "Could not get DW QoS" << std::endl;
     return;
  }

  reflex::pub::DataWriter<single_member>
    writer(reflex::pub::DataWriterParams(participant)
             .topic_name(topic_name)
             .type_name("single_member")
             .datawriter_qos(datawriter_qos));
    int i = 1;
for(;;){

    single_member obj; //question- can we use stc instead of single_member ?
    obj.m1 = i++;
    rc = writer.write(obj);
if (rc != DDS_RETCODE_OK) {
       std::cerr << "Write error = "
                 << reflex::detail::get_readable_retcode(rc)
                 << std::endl;
}


NDDSUtility::sleep(period);
}
/*
rc = writer.write(*d1.get(), DDS_HANDLE_NIL);
    if (rc != DDS_RETCODE_OK) {
      std::cerr << "Write error = "
                << reflex::detail::get_readable_retcode(rc)
                << std::endl;
      //break;
    }
    else
        std::cout <<"\n wrote obj to subscriber\n";
 //delete_entities(participant);
*/  }

