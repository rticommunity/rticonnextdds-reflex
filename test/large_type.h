/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided "as is", with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_LARGE_TYPE_H
#define RTIREFLEX_LARGE_TYPE_H

#ifdef RTI_WIN32
#pragma warning(disable: 4351) 
#endif

#include "reflex.h"

enum Color { red, green, blue };

REFLEX_ENUM_DEF_CUSTOM(Color, "Color", 3)  

REFLEX_ENUM_MEMBER_DEF_CUSTOM(Color, 0, "RED",   0)
REFLEX_ENUM_MEMBER_DEF_CUSTOM(Color, 1, "GREEN", 1)
REFLEX_ENUM_MEMBER_DEF_CUSTOM(Color, 2, "BLUE",  2)

namespace custom
{
  typedef std::tuple<int32_t, std::tuple<std::string>> TupleIntString;
}

REFLEX_STRUCT_NAME_DEF_CUSTOM(custom::TupleIntString, "TIString");

REFLEX_STRUCT_MEMBER_DEF_CUSTOM(custom::TupleIntString, 0, "custom_integer",
                                DDS_TYPECODE_NONKEY_REQUIRED_MEMBER);

REFLEX_STRUCT_MEMBER_DEF_CUSTOM(custom::TupleIntString, 1, "custom_tuple",
                                DDS_TYPECODE_NONKEY_REQUIRED_MEMBER);

namespace Zip {
    typedef std::map<std::string, int32_t> Map;
}

#endif // RTIREFLEX_LARGE_TYPE_H

