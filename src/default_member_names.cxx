/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "default_member_names.h"
#include "ndds/ndds_cpp.h"
#include <string>

#if _MSC_VER
#define snprintf sprintf_s
#endif

#ifdef __GNUG__
  #include <cxxabi.h>
#endif 

const char * DefaultMemberNames::basename(const char * str) 
{
  if(!str)  return str; // null string
  if(!*str) return str; // empty string

  const char * ptr = str + strlen(str) - 1;
  while((ptr > str) && (*ptr != ':')) { --ptr; } 
  return (ptr == str)? str : ptr + 1;
}

std::string DefaultMemberNames::type_name(const char * prefix) 
{
  static unsigned int i = 0;
  char buf[32];
  snprintf(buf, 32, "%s%d", prefix, i++);
  return std::string(buf);
}

std::string DefaultMemberNames::demangle(const char* name) 
{
  #ifdef HAVE_CXA_DEMANGLE
    int status = -4;
    char* result = 
      abi::__cxa_demangle(name, NULL, NULL, &status);

    if(status==0) {
      std::string ret_val(result);
      free(result);
      return ret_val;
    }
    else 
    {
      return DefaultMemberNames::type_name("T");
    }
  #else 
    return DefaultMemberNames::type_name("DefaultTypeName");
  #endif
}

DllExport const MemberInfo DefaultMemberNames::members [] =
  { MemberInfo( "m0",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m1",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m2",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m3",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m4",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m5",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m6",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m7",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m8",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m9",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),

    MemberInfo( "m10",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m11",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m12",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m13",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m14",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m15",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m16",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m17",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m18",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m19",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),

    MemberInfo( "m20",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m21",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m22",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m23",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m24",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m25",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m26",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m27",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m28",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m29",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),

    MemberInfo( "m30",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m31",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m32",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m33",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m34",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m35",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m36",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m37",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m38",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m39",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),

    MemberInfo( "m40",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m41",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m42",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m43",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m44",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m45",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m46",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m47",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m48",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
    MemberInfo( "m49",    DDS_TYPECODE_NONKEY_REQUIRED_MEMBER),
 
    MemberInfo( "m50",   DDS_TYPECODE_NONKEY_REQUIRED_MEMBER) };


