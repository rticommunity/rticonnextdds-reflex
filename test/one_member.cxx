/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided “as is”, with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "one_member.h"
#include <memory>

void write_one_member(int) 
{
    reflex::SafeTypeCode<DDS_TypeCode> 
      stc(reflex::make_typecode<one_member>());
    
    reflex::print_IDL(stc.get(), 0);

    DDS_DynamicDataTypeProperty_t props;
    
    std::shared_ptr<DDSDynamicDataTypeSupport> 
       safe_typeSupport(new DDSDynamicDataTypeSupport(stc.get(), props));
  
    reflex::SafeDynamicDataInstance ddi1(safe_typeSupport.get());
    reflex::SafeDynamicDataInstance ddi2(safe_typeSupport.get());

    reflex::MultiDimArray<int32_t,2,3>::type 
      int_array = { { { { 0,0,0 } }, 
                      { { 5,5,5 } } } };
    
    one_member sparse;
    std::get<0>(sparse.member.get_opt_tuple()) = 
      std::string("Real-Time Innovations, Inc.");
    std::get<1>(sparse.member.get_opt_tuple()) = 1.10f;
    std::get<2>(sparse.member.get_opt_tuple()) = int_array;

    reflex::fill_dd(sparse, ddi1);
    ddi1.get()->print(stdout, 2);
}

