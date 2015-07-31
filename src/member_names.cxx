/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include "reflex/dllexport.h"
#include "reflex/member_names.h"
#include "ndds/ndds_cpp.h"
#include <string>

#if _MSC_VER
#define snprintf sprintf_s
#endif

#ifdef __GNUG__
  #include <cxxabi.h>
#endif 

namespace reflex {

  namespace detail {

    REFLEX_INLINE std::string remove_parenthesis(std::string s)
    {
      std::string::iterator end = s.end();

      if (*--end == ')' && *--end == '(')
        return std::string(s.begin(), end);
      else
        return s;
    }
    REFLEX_INLINE const char * DefaultMemberNames::basename(const char * str)
    {
      if (!str)  return str; // null string
      if (!*str) return str; // empty string

      const char * ptr = str + strlen(str) - 1;
      while ((ptr > str) && (*ptr != ':')) { --ptr; }
      return (ptr == str) ? str : ptr + 1;
    }

    REFLEX_INLINE std::string DefaultMemberNames::type_name(const char * prefix)
    {
      static unsigned int i = 0;
      char buf[32];
      snprintf(buf, 32, "%s%d", prefix, i++);
      return std::string(buf);
    }

    REFLEX_INLINE std::string DefaultMemberNames::demangle(const char* name)
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

    REFLEX_INLINE REFLEX_DLL_EXPORT 
      reflex::codegen::MemberInfo DefaultMemberNames::get_member_info(int i)
    { 
      char member_name[32];
      snprintf(member_name, 32, "m%d", i);
      return reflex::codegen::MemberInfo(member_name, DDS_TYPECODE_NONKEY_REQUIRED_MEMBER);
    }

  } // namespace detail

} // namespace reflex
