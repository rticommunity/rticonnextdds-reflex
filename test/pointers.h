/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef POINTERS_H
#define POINTERS_H

#ifdef RTI_WIN32
#pragma warning(disable: 4351) 
#endif

#include <memory>

#include "reflex.h"


struct pointers
{
  int * int_ptr;
  std::shared_ptr<int> shared_ptr;
  std::unique_ptr<int> unique_ptr;
};

REFLEX_ADAPT_STRUCT(
  pointers,
  (int *, int_ptr)
  (std::shared_ptr<int>, shared_ptr)
  (std::unique_ptr<int>, unique_ptr))
  
#endif // POINTERS_H

