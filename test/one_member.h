/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.    	                             
RTI grants Licensee a license to use, modify, compile, and create derivative works 
of the Software.  Licensee has the right to distribute object form only for use with RTI 
products.  The Software is provided "as is", with no warranty of any type, including 
any warranty for fitness for any purpose. RTI is under no obligation to maintain or 
support the Software.  RTI shall not be liable for any incidental or consequential 
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef MANY_MEMBERS_H
#define MANY_MEMBERS_H

#ifdef RTI_WIN32
#pragma warning(disable: 4351) 
#endif

#include <list>
#include <array>
#include <string>
#include <memory>

#include "reflex.h"


struct one_member
{
  unsigned long long member;
};

REFLEX_ADAPT_STRUCT(
    one_member,
    (unsigned long long, member))

#endif // MANY_MEMBERS_H

