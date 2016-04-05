
#include "qs_perf_test_leo.h"
#include "ndds_namespace_cpp.h"
#include <cstdio>
#include <sys/time.h>
#include <stdlib.h>

#define MOD 10000

int EXTRA = 100;

int SAMPLE = 0;
timeval  begin, end;
std::vector<long> Time;

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
            
           if( (SAMPLE!=0) && (count % SAMPLE == 0))
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
  bool done;
  long time_start, time_end;

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
          if(count > EXTRA) 
          {
            gettimeofday(&end, NULL);
            long time = (end.tv_sec*1000000 + end.tv_usec) - ss->timestamp;
            Time.push_back(time);
            if(count == EXTRA + 1)
            {
              time_start = (end.tv_sec*1000 + end.tv_usec/1000); //time in milliseconds
            }
            if (count % MOD ==0 )
            {

              gettimeofday(&begin,NULL);
              time_end = begin.tv_sec*1000 + begin.tv_usec/1000;
              std::cout <<"\n Throughput is "<< (count)*1000.0 / (time_end - time_start);
            }
          }
          if(count == SAMPLE+EXTRA)  //last message.
          {
            gettimeofday(&begin,NULL);
            time_end = begin.tv_sec*1000 + begin.tv_usec/1000;
            done = true;
          }
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
    time_start = 0;
    time_end = 0;
    count = 0;
    done = false;
  }
};


void getReaderGuidExpr(char * readerGuidExpr)
{
    char * ptr = readerGuidExpr;

    snprintf(ptr, 255, "%s%032llx)", "@related_reader_guid.value = &hex(", (long long) readerGuidExpr);
}

void qs_perf_subscriber(int domain_id,int samples, int id)
{
  char topicName[255],readerGuidExpr[255];
  SAMPLE = samples;
  DDS_DataReaderQos readerQos;
  DDS_StringSeq cftParams;
  DDSTopic *topic = NULL;
  Time.reserve(SAMPLE);
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
  readerGuidExpr[254] = id; //unique filter
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
  DDS_Duration_t poll_period = { 2, 0 };
  std::cout <<"Found SharedReaderQueue!\n";
  while(!reader_listener.done){
    std::cout << "Polling\n";
    NDDSUtility::sleep(poll_period);
  }
  Stats latency_hist(SAMPLE);
  for(int i= 0; i< SAMPLE; i++)
  {
    latency_hist.insert(Time[i]);
  }
  latency_hist.sort_keys();
  //latency_hist.print_histogram();
  std::cout << "\naverage latency = " << latency_hist.average() << std::endl;
  std::cout << "90 percentile = " << latency_hist.percentile(0.90) << std::endl;
  std::cout << "95 percentile = " << latency_hist.percentile(0.95) << std::endl;
  std::cout << "99 percentile = " << latency_hist.percentile(0.99) << std::endl;
  std::cout << "99.99 percentile = " << latency_hist.percentile(0.9999) << std::endl;
  std::cout << " Throughput details: " << (SAMPLE*1000.0)/(reader_listener.time_end - reader_listener.time_start);
  NDDSUtility::sleep(poll_period);
}

void qs_perf_publisher(int domain_id,int samples, int no_readers, int hertz)
{
  DDS_ReturnCode_t         rc = DDS_RETCODE_OK;
  DDSDomainParticipant *   participant = NULL;
  DDS_DynamicDataTypeProperty_t props;
 // int hertz = 100000;
  double sleep_for = sleep_period(hertz) * 1000 * 1000 *1000; //nanoseconds
  DDS_Duration_t loop_period{ 0, (DDS_UnsignedLong)(10*sleep_for)};
  DDS_Duration_t end_wait{ 3,0  };
  const char *topic_name = "PerfHelloWorldTopic";
  SAMPLE = samples * no_readers;
  EXTRA = EXTRA * no_readers;
  long start_time=0 ,  end_time = 0;
  Stats sleep_time(SAMPLE);

  PerfHelloWorldPubListener writerListener;
  participant = DDSDomainParticipantFactory::get_instance()->
  	    create_participant(
               domain_id,DDS_PARTICIPANT_QOS_DEFAULT,
               NULL , DDS_STATUS_MASK_NONE);
  
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

   //Wait for Queuing Service discovery 
     std::cout <<"\nWaiting to discover SharedReaderQueue ...\n";

     while (!listener.foundQS) {
       NDDSUtility::sleep(loop_period);
     }

     std::cout<<"\nSharedReaderQueue discovered...\n";
    int i = 0;
    while(i < (SAMPLE+EXTRA))
     {
       ++i;
       PerfHelloWorld obj;
       obj.messageId = i;
       if(i > EXTRA)
       {
         gettimeofday(&begin,NULL);
         obj.timestamp = begin.tv_sec *1000000 + begin.tv_usec;
       }
      if ( i == (EXTRA +1))
      {
        gettimeofday(&begin,NULL);
        start_time = begin.tv_sec*1000 + begin.tv_usec/1000;

      }
      if( i == (SAMPLE+EXTRA) )
      {
        gettimeofday(&end,NULL);
        end_time = end.tv_sec*1000 + end.tv_usec/1000;

      }
       rc = writer.write(obj);
       if((i % 100000) == 0)
         std::cout <<"Wrote "<< i << "\n";
       if (rc != DDS_RETCODE_OK) {
         std::cerr << "Write error = "
           << reflex::detail::get_readable_retcode(rc)
           << std::endl;
       }
       if( i % 10 == 0)
       {
         struct timeval start, end;
         gettimeofday(&start, NULL);
         NDDSUtility::sleep(loop_period);
         gettimeofday(&end,NULL);
         sleep_time.insert(end-start); // seconds
       }
     }

    NDDSUtility::sleep(end_wait);
    std::cout << "Throughput is "<< ((SAMPLE/no_readers)*1000.0) / ( end_time - start_time);
    std::cout<<"\n sleep histogram is as follows:"<< std::endl;
    sleep_time.sort_keys();
    std::cout << "expected sleep period in usec = " << (10*sleep_for)/( 1000)<< std::endl;
    std::cout << "average = " << sleep_time.average() << std::endl;
    std::cout << "90 percentile = " << sleep_time.percentile(0.90) << std::endl;
    std::cout << "95 percentile = " << sleep_time.percentile(0.95) << std::endl;
    std::cout << "99 percentile = " << sleep_time.percentile(0.99) << std::endl;

}

