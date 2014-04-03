/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <ndds/ndds_cpp.h>
#include <set>
#include <string>
#include "safe_typecode.h"
#include "dllexport.h"

DllExport  
SafeTypeCode<std::string> DECLSPEC 
get_typecode(DDS_TypeCodeFactory * factory,
             const std::string *)
{
  return SafeTypeCode<std::string>(factory);
}

static void print_recursive_IDL(const DDS_TypeCode * tc,
                                DDS_UnsignedLong indent,
                                std::set<std::string> & visited)
{
  DDS_ExceptionCode_t ex;
  DDS_TCKind kind = tc->kind(ex);
  if((kind == DDS_TK_STRUCT)  || 
     (kind == DDS_TK_UNION)   ||
     (kind == DDS_TK_SPARSE))
  {
    size_t count = tc->member_count(ex);
    for(size_t i=0;i < count; ++i)
    {
      print_recursive_IDL(tc->member_type(i, ex), indent+1, visited);
    }
  }
  if((kind == DDS_TK_SEQUENCE) ||
     (kind == DDS_TK_ARRAY))
  {
    print_recursive_IDL(tc->content_type(ex), indent+1, visited);
  }

  if((kind == DDS_TK_STRUCT)   || 
     (kind == DDS_TK_UNION)    ||
     (kind == DDS_TK_ENUM)     || 
     (kind == DDS_TK_SEQUENCE) || 
     (kind == DDS_TK_ARRAY)    ||
     (kind == DDS_TK_SPARSE))
  {
    if(tc->name(ex) != NULL)
    { 
      std::string name(tc->name(ex));
      if(visited.find(name) == visited.end())
      {
        tc->print_IDL(indent, ex);
        printf("\n");
        visited.insert(name);
      }
    }
  }
}

DllExport void DECLSPEC print_IDL(const DDS_TypeCode *tc,
                                  DDS_UnsignedLong indent)
{
  std::set<std::string> visited;
  print_recursive_IDL(tc, indent, visited);
}
