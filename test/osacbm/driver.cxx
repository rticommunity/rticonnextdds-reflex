#include "gen/StateDetection.hxx"
#include "gen/DAReal.hxx"
#include "reflex.h"

RTI_ADAPT_STRUCT(
    osacbm::DAReal,
    (::xml_schema::double_, value())
    );

int main(void)
{
  reflex::SafeTypeCode<DDS_TypeCode>
          stc(reflex::make_typecode<osacbm::DAReal>());
          
  reflex::print_IDL(stc.get(), 0);

  return 0;
}
