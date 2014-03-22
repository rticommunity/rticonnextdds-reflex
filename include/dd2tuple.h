/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DD2TUPLE_H
#define RTIREFLEX_DD2TUPLE_H

#include <ndds/ndds_cpp.h>
#include <tuple>
#include <vector>

#include "dllexport.h"
#include "dd_manip.h"
#include "typecode_manip.h"
#include "memberwise.h"
#include "default_member_names.h"
#include "bounded.h"

#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/include/size.hpp>

template <class T>
void FillDD(const T & data, DDS_DynamicData &instance)
{
  MemberAccess nested_ma = MemberAccess::BY_ID(); 

  TypelistIterator<T, 
                   0, 
                   Size<T>::value-1>::set(instance, nested_ma, data);
}

template <class T>
SafeTypeCode<T> MakeTypecode (const char * name = 0)
{
  DDS_TypeCodeFactory * factory = 
    DDS_TypeCodeFactory::get_instance();
  
  std::string type_name_string = 
    StructName<typename remove_refs<T>::type>::get();

  const char * type_name = name? name : type_name_string.c_str();

  SafeTypeCode<T> structTc(factory, type_name);

  TypelistIterator<T, 
                   0, 
                   Size<T>::value-1>::add(factory, structTc.get());
  
  return move(structTc);
}

template <class T>
void ExtractDD(const DDS_DynamicData & instance, T & data)
{	
  MemberAccess nested_ma = MemberAccess::BY_ID();

  TypelistIterator<T, 
                   0, 
                   Size<T>::value-1>::get(instance, nested_ma, data);
}


template <class Tuple>
void Tuple2DD(const Tuple & tuple, DDS_DynamicData &instance)
{
  FillDD<Tuple>(tuple, instance);  
}

template <class Tuple>
void DD2Tuple(const DDS_DynamicData & instance, Tuple & tuple)
{	
  ExtractDD<Tuple>(instance, tuple);
}

template <class Tuple>
SafeTypeCode<Tuple> Tuple2Typecode (const char * name = 0)
{
  return MakeTypecode<Tuple>(name);
}

class DllExport SafeDynamicDataInstance
{
  DDSDynamicDataTypeSupport * typeSupport_;
  DDS_DynamicData * instance_;

  SafeDynamicDataInstance(const SafeDynamicDataInstance &);
  SafeDynamicDataInstance & operator = (const SafeDynamicDataInstance &);

public:
  SafeDynamicDataInstance(DDSDynamicDataTypeSupport * typeSupport);
  ~SafeDynamicDataInstance();
  DDS_DynamicData * get();
};

#endif //  RTIREFLEX_DD2TUPLE_H

