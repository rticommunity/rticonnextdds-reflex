/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided "as is", with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "darkart.h"
#include "reflex.h"

void write_darkart_type(unsigned domain_id)
{
  DDS_Duration_t period { 0, 1000*1000*1000 };
  DDS_ReturnCode_t rc;
  DDS_DynamicDataTypeProperty_t props;

  darkart::BaselineData vb(1);
  vb[0].found_baseline = true;
  vb[0].mean = 9.99;
  vb[0].variance = 99.99;

  darkart::ChannelInfo vc(2);
  vc[0].label = "FermiLab0";
  vc[1].label = "FermiLab1";

  darkart::EventInfo event;
  event.run_id = 10;

  darkart::PmtInfo sp;
  sp.insert(std::string("PMT"));
  sp.insert(std::string("pmt"));

  darkart::VetoTruth truth;
  truth.hits.resize(1);
  (*begin(truth.hits)).pmt_theta = 5.5;
  truth.sim_event = 5;
  
  darkart::Waveforms vw(2);
  vw[0].sample_rate = 1;
  vw[1].sample_rate = 1;

  darkart::TopLevel toplevel(vb, vc, event, sp, truth, vw);
  
  DDSDomainParticipant *   participant =
    DDSDomainParticipantFactory::get_instance()->
                      create_participant(
                        domain_id,
                        DDS_PARTICIPANT_QOS_DEFAULT,
                        NULL,   // Listener
                        DDS_STATUS_MASK_NONE);

  if (participant == NULL) {
    throw std::runtime_error("write_darkart_type: Unable to create participant");
  }

  /* Option 1: Using darkart::TopLevel */
  reflex::detail::GenericDataWriter<darkart::TopLevel>
    top_level_writer(participant, "DarkartTopLevelTopic", "DarkartTopLevelType");

  std::cout << "Printing IDL\n";
  reflex::detail::print_IDL(top_level_writer.get_typecode(), 0);

  /* Option 2: using tuple */
  auto all = std::tie(vb, vc, event, sp, truth, vw);

  // The following line is to ensure that the library
  // is not making hidden copy-ctor and move-ctor calls.
  // The following line must use copy-constructor.
  // This is the only place in the program that calls them.
  // If they are =deleted, compiler error will arise.
  // darkart::AllTuple a = all; 

  reflex::SafeTypeCode<darkart::AllTuple>
    stc(reflex::make_typecode<darkart::AllTuple>());

  std::shared_ptr<DDSDynamicDataTypeSupport> 
    safe_typeSupport(new DDSDynamicDataTypeSupport(stc.get(), props));

  reflex::AutoDynamicData ddi1(safe_typeSupport.get());
  reflex::write_dynamicdata(all, *ddi1.get());
  std::cout << "Printing Data\n";

  // FIXME: Causes a run-time crash.
  // ddi1.get()->print(stdout, 2);

  reflex::detail::GenericDataWriter<decltype(all)>
    tuple_writer(participant, "DarkartTopic", "DarkartAllTupleType");

  for(;;)
  {
    rc = top_level_writer.write(toplevel);
    if(rc != DDS_RETCODE_OK) {
      std::cerr << "Write error = " 
                << reflex::detail::get_readable_retcode(rc) 
                << std::endl;
      break;
    }

    rc = tuple_writer.write(toplevel);
    if(rc != DDS_RETCODE_OK) {
      std::cerr << "Write error = " 
                << reflex::detail::get_readable_retcode(rc) 
                << std::endl;
      break;
    }

    NDDSUtility::sleep(period);
  }
}

