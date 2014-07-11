/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
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

typedef reflex::Sparse<std::string,
                       float,
                       reflex::MultiDimArray<int32_t, 2, 3>::type >
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

#endif // MANY_MEMBERS_H

