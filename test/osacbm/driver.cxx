#include "gen/StateDetection.hxx"
#include "gen/DAReal.hxx"
#include "gen/listOfDouble.hxx"
#include "gen/DADataSeq.hxx"

#include "reflex.h"

namespace reflex { 
  namespace detail {

template <class T>
struct is_container<xsd::cxx::tree::sequence<T>> : true_type {};

template <class T>
struct container_traits<xsd::cxx::tree::sequence<T>> 
{
  typedef typename xsd::cxx::tree::sequence<T>::value_type value_type; 
  typedef typename xsd::cxx::tree::sequence<T>::iterator iterator; 
};

template <>
struct is_container<osacbm::listOfDouble> : true_type {};

template <>
struct container_traits<osacbm::listOfDouble> 
{
  typedef osacbm::listOfDouble::value_type value_type;
  typedef osacbm::listOfDouble::iterator   iterator;
};

template <>
struct is_container<xml_schema::base64_binary> : true_type {};

template <>
struct container_traits<xml_schema::base64_binary> 
{
  typedef char value_type;
  typedef char * iterator;
};

template <class T>
struct is_optional<xsd::cxx::tree::optional<T>> : true_type {};

template <class T>
struct optional_traits<xsd::cxx::tree::optional<T>> 
{
  typedef T value_type;
};

  } // namespace detail
} // namespace reflex

RTI_ADAPT_STRUCT(
    osacbm::DAReal,
    (osacbm::DAReal::value_type, value())
    );

RTI_ADAPT_STRUCT(
    osacbm::DADataSeq,
    (osacbm::DADataSeq::values_sequence,            values())
    (osacbm::DADataSeq::valuesList_optional,        valuesList())
    (osacbm::DADataSeq::valuesBinary_optional,      valuesBinary())
    (osacbm::DADataSeq::xAxisDeltas_sequence,       xAxisDeltas())
    (osacbm::DADataSeq::xAxisDeltasList_optional,   xAxisDeltasList())
    (osacbm::DADataSeq::xAxisDeltasBinary_optional, xAxisDeltasBinary())
    (osacbm::DADataSeq::xAxisStart_optional,        xAxisStart())
    );

template <class T>
void print_tc()
{
  reflex::SafeTypeCode<DDS_TypeCode>
          stc(reflex::make_typecode<T>());
          
  reflex::print_IDL(stc.get(), 0);
}

int main(void)
{
  print_tc<osacbm::DAReal>();
  print_tc<osacbm::DADataSeq>();

  osacbm::DADataSeq dataseq;
  dataseq.values().push_back(10);
  dataseq.values().push_back(20);
  dataseq.values().push_back(30);

  osacbm::listOfDouble list_double(10, 9.99);
  dataseq.valuesList() = list_double;

  dataseq.xAxisStart(99.99);

  reflex::SafeDynamicData<osacbm::DADataSeq> sdd = 
    reflex::make_dd(dataseq);

  sdd.get()->print(stdout, 2);

  osacbm::DADataSeq dataseq2;

  reflex::extract_dd(*sdd.get(), dataseq2);


  return 0;
}
