/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided “as is”, with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <time.h>
#include <memory>
#include "reflex.h"
#include "ndds/cdr/cdr_stream.h"
//#include "ndds/cdr/cdr_stream_impl.h"

#include "shape_type.h"

void delete_entities(DDSDomainParticipant * participant);
std::tuple<DDSDynamicDataWriter *,
           DDSDomainParticipant *> 
create_ddwriter(const char *type_name, 
                const char *topic_name,
                DDSDynamicDataTypeSupport *type_support);

class MyShapesListener : public reflex::GenericDataReaderListener<ShapeType>
{
public:
  void on_data_available(reflex::GenericDataReader<ShapeType> & dr) override
  {
    std::vector<reflex::Sample<ShapeType>> samples;
    dr.take(samples);
    for (auto &ss : samples)
    {
      if (ss.info().valid_data)
      {
        std::cout << "Color = "     << ss->color()     << std::endl
                  << "x = "         << ss->x()         << std::endl
                  << "y = "         << ss->y()         << std::endl
                  << "shapesize = " << ss->shapesize() << std::endl
                  << std::endl;
      }
    }
  }
};

void read_shape_type(int domain_id)
{
  DDSDomainParticipant *   participant =
    DDSDomainParticipantFactory::get_instance()->
    create_participant(
    domain_id,
    DDS_PARTICIPANT_QOS_DEFAULT,
    NULL,   // Listener
    DDS_STATUS_MASK_NONE);

  if (participant == NULL) {
    throw std::runtime_error("Unable to create participant");
  }

  MyShapesListener shapes_listener;
  reflex::GenericDataReader<ShapeType>
    shapes_data_reader(participant, &shapes_listener, "Square");

  std::cout << "Subscribed to Square\n";
  for (;;)
  {
    std::cout << "Polling\n";
    DDS_Duration_t poll_period = { 4, 0 };
    NDDSUtility::sleep(poll_period);
  }
}

template <class Tuple>
void copy(Tuple &t, int x, int y, const std::string &color, int shapesize)
{
  std::get<0>(t) = color;
  std::get<1>(t) = x;
  std::get<2>(t) = y;
  std::get<3>(t) = shapesize;
}

void copy(ShapeType &s, int x, int y, const std::string &color, int shapesize)
{
  s.color() = color;
  s.x() = x;
  s.y() = y;
  s.shapesize() = shapesize;
}

void write_shape_type_extended(int domain_id)
{
  DDS_ReturnCode_t rc;
  DDSDomainParticipant * participant = NULL;
  DDS_Duration_t period{ 0, 100 * 1000 * 1000 };
  const char *topic_name = "Triangle";

  // For interoperability with the ShapesDemo MAX_STRING_SIZE must be 128
  reflex::MAX_STRING_SIZE = 128;

  reflex::SafeTypeCode<DDS_TypeCode>
    shape_ex_tc(reflex::make_typecode<ShapeTypeExtended>());

  reflex::print_IDL(shape_ex_tc.get(), 0);

  participant = DDSDomainParticipantFactory::get_instance()->
    create_participant(
    domain_id,
    DDS_PARTICIPANT_QOS_DEFAULT,
    NULL,   // Listener
    DDS_STATUS_MASK_NONE);
  if (participant == NULL) {
    std::cerr << "! Unable to create DDS domain participant" << std::endl;
    return;
  }

  auto writer = reflex::GenericDataWriter<ShapeTypeExtended>(
                    participant, topic_name, "ShapeType");

  srand((unsigned int) time(NULL));

  int x_max = 200, y_max = 200;
  int x_min = 30, y_min = 30;
  int x_dir = 2, y_dir = 2;

  int x = (rand() % 50) + x_min;
  int y = (rand() % 50) + y_min;

  ShapeTypeExtended shape;
  shape.color() = "RED";
  shape.angle() = 0;
  shape.shapesize() = 30;
  shape.fillKind() = HORIZONTAL_HATCH_FILL;

  std::cout << "Writing "
    << reflex::detail::StructName<ShapeTypeExtended>::get()
    << " topic = "
    << topic_name
    << " (domain = "
    << domain_id
    << ")...\n";

  for (;;)
  {
    if ((x >= x_max) || (x <= x_min))
      x_dir *= -1;
    if ((y >= y_max) || (y <= y_min))
      y_dir *= -1;

    // change app data like usual.
    // tied tuple has references.
    x += x_dir;
    y += y_dir;

    shape.x() = x;
    shape.y() = y;
    rc = writer.write(shape);
    
    if (rc != DDS_RETCODE_OK) {
      std::cerr << "Write error = " << reflex::get_readable_retcode(rc) << std::endl;
      break;
    }
    NDDSUtility::sleep(period);
  }
}

void write_shape_type(int domain_id)
{
  DDS_ReturnCode_t         rc;
  DDSDomainParticipant *   participant = NULL;
  DDSDynamicDataWriter *ddWriter = NULL;
  DDS_DynamicDataTypeProperty_t props;
  DDS_Duration_t period{ 0, 100 * 1000 * 1000 };

  // For interoperability with the ShapesDemo MAX_STRING_SIZE must be 128
  reflex::MAX_STRING_SIZE = 128;

  int32_t x = 0, y = 0, shapesize = 30;
  std::string color = "BLUE";
  const char *topic_name = "Square";

  // Use only one of the following.
  auto t1 = std::tie(color, x, y, shapesize);
  //auto t1 = ShapeType(color, x, y, shapesize);

  typedef reflex::detail::remove_refs<decltype(t1)>::type Tuple;
  reflex::SafeTypeCode<DDS_TypeCode> 
    stc(reflex::tuple2typecode<Tuple>());
  
  reflex::print_IDL(stc.get(), 0);
  
  std::shared_ptr<DDSDynamicDataTypeSupport> 
    safe_typeSupport(new DDSDynamicDataTypeSupport(stc.get(), props));

  reflex::SafeDynamicDataInstance ddi1(safe_typeSupport.get());
  reflex::SafeDynamicDataInstance ddi2(safe_typeSupport.get());
  
  // multi-value assignment using tie
  std::tie(ddWriter, participant) = 
    create_ddwriter(
      reflex::detail::StructName<Tuple>::get().c_str(), 
      topic_name, 
      safe_typeSupport.get());

  if(ddWriter == NULL)
    return;

  std::cout << "Writing " 
            << reflex::detail::StructName<Tuple>::get()
            << " topic = " 
            << topic_name
            << " (domain = " 
            << domain_id 
            << ")...\n";

  srand((unsigned int) time(NULL));

  int x_max=200, y_max=200;
  int x_min=30, y_min=30;
  int x_dir=2, y_dir=2;

  x = (rand() % 50)+x_min;
  y = (rand() % 50)+y_min;
  
  Tuple t2; 

  for(;;)
  {
    if((x >= x_max) || (x <= x_min)) 
       x_dir *= -1;
    if((y >= y_max) || (y <= y_min))
       y_dir *= -1;

    // change app data like usual.
    // tied tuple has references.
    x += x_dir;
    y += y_dir;  

    copy(t1, x, y, color, shapesize);

    // write the values in a dynamic data instance.
    reflex::tuple2dd(t1, *ddi1.get());

    // print if you like
    //ddi1.get()->print(stdout, 2);


    // read the dynamic data instance back 
    // in a different tuple
    reflex::dd2tuple(*ddi1.get(), t2);

    // write the second tuple again in a
    // different dynamic data instance.
    reflex::tuple2dd(t2, *ddi2.get());

    // print if you like
    // ddi2.get()->print(stdout, 2);

    // round-tripping must work!
    assert(ddi1.get()->equal(*ddi2.get()));

    // write it for fun!
    rc = ddWriter->write(*ddi2.get(), DDS_HANDLE_NIL);
    if(rc != DDS_RETCODE_OK) {
      std::cerr << "Write error = " << reflex::get_readable_retcode(rc) << std::endl;
      break;
    }
    NDDSUtility::sleep(period);
  }
  
  delete_entities(participant);
}

#ifdef USE_SHAPES_TUPLE

class MyShapesTupleListener : public GenericDataReaderListener<ShapeTypeTuple>
{
public:
  void on_data_available(GenericDataReader<ShapeTypeTuple> & dr) override
  {
    std::vector<ShapeTypeTuple> samples = dr.take();
    for (auto const &elem : samples)
    {
      std::cout << "Color = " << std::get<0>(elem) << std::endl;
      std::cout << "x = " << std::get<1>(elem) << std::endl;
      std::cout << "y = " << std::get<2>(elem) << std::endl;
      std::cout << "shapesize = " << std::get<3>(elem) << std::endl;
      std::cout << std::endl;
    }
  }
};

MyShapesTupleListener shapes_tuple_listener;
GenericDataReader<ShapeTypeTuple>
shapes_tuple_data_reader(participant, &shapes_tuple_listener, "Square");

#endif // USE_SHAPES_TUPLE
