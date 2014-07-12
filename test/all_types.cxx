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

template <class T>
void round_trip_test(int domain, const T & source, T & dest)
{
  DDS_DynamicDataTypeProperty_t props;

  reflex::SafeTypeCode<DDS_TypeCode>
    stc(reflex::make_typecode<T>());

  reflex::print_IDL(stc.get(), 0);

  std::shared_ptr<DDSDynamicDataTypeSupport>
    safe_typeSupport(new DDSDynamicDataTypeSupport(stc.get(), props));

  reflex::SafeDynamicDataInstance src_dd(safe_typeSupport.get());
  reflex::SafeDynamicDataInstance dest_dd(safe_typeSupport.get());

  reflex::fill_dd(source, src_dd);

  reflex::extract_dd(src_dd, dest);

  reflex::fill_dd(dest, dest_dd);

  // round-tripping must work!
  assert(src_dd.get()->equal(*dest_dd.get()));

#ifndef RTI_WIN32    
  ddi2.get()->print(stdout, 2);
#endif
}

void test_sparse_type(int domain)
{
  reflex::MultiDimArray<int32_t, 2, 3>::type
    int_array = { { { { 0, 0, 0 } },
                    { { 5, 5, 5 } } } };

  TestSparseType sparse, sparse_dest;
  std::get<0>(sparse.member.get_opt_tuple()) =
    std::string("Real-Time Innovations, Inc.");
  std::get<1>(sparse.member.get_opt_tuple()) = 1.10f;
  std::get<2>(sparse.member.get_opt_tuple()) = int_array;

  round_trip_test(domain, sparse, sparse_dest);
}

void test_optional_type(int domain)
{
  TestOptionalType opt, opt_dest;
  opt.oint = 20;
  round_trip_test(domain, opt, opt_dest);
}

void test_optional_base_type(int domain)
{
  ShapeTypeExtended ex_shape;
  ex_shape.x = 10;
  ex_shape.y = 20;
  ex_shape.color_src = "BLUE";
  ex_shape.color = ex_shape.color_src;
  ex_shape.shapesize = 30;
  ex_shape.angle = 90.0;
  ex_shape.fillKind = VERTICAL_HATCH_FILL;

  ShapeTypeExtended shape_dest;
  TestOptionalBaseType opt_dest;
  /* Provide space for receive side Bounded<std::string,128>.*/
  shape_dest.color = shape_dest.color_src;
  opt_dest.member = shape_dest;

  TestOptionalBaseType opt_src;
  opt_src.member = ex_shape;
  round_trip_test(domain, opt_src, opt_dest);
}

void test_all_types (int domain_id) 
{
  test_optional_type(domain_id);
  test_sparse_type(domain_id);
  test_optional_base_type(domain_id);
}
