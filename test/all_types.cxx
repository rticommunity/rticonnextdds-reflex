/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "all_types.h"
#include <memory>

DDS_DynamicDataTypeProperty_t props;

void test_sparse_type(int)
{
  reflex::SafeTypeCode<DDS_TypeCode>
    stc(reflex::make_typecode<TestSparseType>());

  reflex::print_IDL(stc.get(), 0);

  std::shared_ptr<DDSDynamicDataTypeSupport>
    safe_typeSupport(new DDSDynamicDataTypeSupport(stc.get(), props));

  reflex::SafeDynamicDataInstance ddi1(safe_typeSupport.get());
  reflex::SafeDynamicDataInstance ddi2(safe_typeSupport.get());

  reflex::MultiDimArray<int32_t, 2, 3>::type
    int_array = { { { { 0, 0, 0 } },
    { { 5, 5, 5 } } } };

  TestSparseType sparse1, sparse2;
  std::get<0>(sparse1.member.get_opt_tuple()) =
    std::string("Real-Time Innovations, Inc.");
  std::get<1>(sparse1.member.get_opt_tuple()) = 1.10f;
  std::get<2>(sparse1.member.get_opt_tuple()) = int_array;

  reflex::fill_dd(sparse1, ddi1);

  reflex::extract_dd(ddi1, sparse2);

  reflex::fill_dd(sparse2, ddi2);

  // round-tripping must work!
  assert(ddi1.get()->equal(*ddi2.get()));
    
  //ddi2.get()->print(stdout, 2);
}

void test_optional_type(int domain_id)
{
  reflex::SafeTypeCode<DDS_TypeCode>
    stc(reflex::make_typecode<TestOptionalType>());

  reflex::print_IDL(stc.get(), 0);

  std::shared_ptr<DDSDynamicDataTypeSupport>
    safe_typeSupport(new DDSDynamicDataTypeSupport(stc.get(), props));

  reflex::SafeDynamicDataInstance ddi1(safe_typeSupport.get());
  reflex::SafeDynamicDataInstance ddi2(safe_typeSupport.get());

  TestOptionalType opt1, opt2;
  opt1.oint = 20;

  reflex::fill_dd(opt1, ddi1);

  reflex::extract_dd(ddi1, opt2);

  reflex::fill_dd(opt2, ddi2);

  // round-tripping must work!
  assert(ddi1.get()->equal(*ddi2.get()));

  //ddi2.get()->print(stdout, 2);
}

void test_all_types (int domain_id) 
{
  test_optional_type(domain_id);
  test_sparse_type(domain_id);
}