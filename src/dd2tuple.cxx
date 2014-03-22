/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <iostream>
#include "ndds/ndds_cpp.h"
#include "dd2tuple.h"

SafeDynamicDataInstance::SafeDynamicDataInstance(
    DDSDynamicDataTypeSupport * typeSupport)
	: typeSupport_(typeSupport),
	  instance_(0)
{ 
  if(!typeSupport_) {
    std::cerr << "NULL TypeSupport\n";
    throw 0;
  }
  instance_ = typeSupport_->create_data();
  if(!instance_) {
    std::cerr << "DynamicDataTypeSupport::create_data failed\n";
    throw 0;
  }
}

SafeDynamicDataInstance::~SafeDynamicDataInstance() 
{
  if (instance_ != NULL) {
    DDS_ReturnCode_t rc = typeSupport_->delete_data(instance_);
    if (rc != DDS_RETCODE_OK) {
	      std::cerr << "! Unable to delete instance data: " 
                  << rc << std::endl;
    }
    instance_ = NULL;
  }
}

DDS_DynamicData * SafeDynamicDataInstance::get() {
  return instance_;
}

