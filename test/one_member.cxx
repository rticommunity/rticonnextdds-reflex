/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided "as is", with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "one_member.h"
#include <memory>

bool operator == (const one_member & lhs, const one_member & rhs)
{
  return lhs.member == rhs.member;
}

void write_one_member(int) 
{
    reflex::SafeTypeCode<one_member> 
      stc(reflex::make_typecode<one_member>());
    
    reflex::detail::print_IDL(stc.get(), 0);

    DDS_DynamicDataTypeProperty_t props;
    
    std::shared_ptr<DDSDynamicDataTypeSupport> 
       safe_typeSupport(new DDSDynamicDataTypeSupport(stc.get(), props));
  
    reflex::AutoDynamicData d1(safe_typeSupport.get());
    reflex::AutoDynamicData d2(safe_typeSupport.get());

    one_member x;
    x.member = 10000000000000000L;

    one_member y;
    y.member = 10000000000000000L;

    reflex::write_dynamicdata(d1, x);
    reflex::read_dynamicdata(y, d1);
    std::cout << "one_member same = " << std::boolalpha << (x == y) << "\n";

    d1.get()->print(stdout, 2);
}

