/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef EVERY_TYPE_H
#define EVERY_TYPE_H

#ifdef RTI_WIN32
#pragma warning(disable: 4351) 
#endif

#include <list>
#include <array>
#include <string>

#include "reflex.h"

typedef reflex::match::Sparse<std::string,
                              float,
                              reflex::match::MultiDimArray<int32_t, 2, 3>::type >
           SparseSFA;

struct TestSparseType
{
  SparseSFA member;
};

RTI_ADAPT_STRUCT(
  TestSparseType,
  (SparseSFA, member))


struct TestOptionalType
{
  boost::optional<int> oint;
};

RTI_ADAPT_STRUCT(
  TestOptionalType,
  (boost::optional<int>, oint))


typedef reflex::match::Bounded<std::string, 128> string128;

struct MyShapeType {
  int32_t x, y, shapesize;
  std::string color_src;
  string128 color;
};

enum MyShapeFillKind {
  SOLID_FILL,
  TRANSPARENT_FILL,
  HORIZONTAL_HATCH_FILL,
  VERTICAL_HATCH_FILL
};

struct MyShapeTypeExtended : MyShapeType {
  MyShapeFillKind fillKind;
  float angle;
};

struct TestOptionalBaseType {
  boost::optional<MyShapeTypeExtended> member;
};

RTI_ADAPT_STRUCT(
  MyShapeType,
  (string128, color, RTI_KEY)
  (int32_t, x)
  (int32_t, y)
  (int32_t, shapesize))

ENUM_DEF(MyShapeFillKind, "MyShapeFillKind", 4)
ENUM_MEMBER_DEF(MyShapeFillKind, 0, "SOLID_FILL", 0)
ENUM_MEMBER_DEF(MyShapeFillKind, 1, "TRANSPARENT_FILL", 1)
ENUM_MEMBER_DEF(MyShapeFillKind, 2, "HORIZONTAL_HATCH_FILL", 2)
ENUM_MEMBER_DEF(MyShapeFillKind, 3, "VERTICAL_HATCH_FILL", 3)

RTI_ADAPT_VALUETYPE(
  MyShapeTypeExtended,
  BASE(MyShapeType),
  (MyShapeFillKind, fillKind)
  (float, angle))

RTI_ADAPT_STRUCT(
  TestOptionalBaseType,
  (boost::optional<MyShapeTypeExtended>, member))

#endif // MANY_MEMBERS_H

