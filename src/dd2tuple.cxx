/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <iostream>
#include "ndds/ndds_cpp.h"
#include "reflex/dd2tuple.h"

namespace reflex {

  DDS_DynamicData * SafeDynamicDataInstance::init(DDSDynamicDataTypeSupport * ts)
  {
    if (!ts) 
    {
      throw std::runtime_error("SafeDynamicDataInstance::init NULL typeSupport");
    }
    DDS_DynamicData * instance = ts->create_data();
    if (!instance) 
    {
      throw std::runtime_error(
        "SafeDynamicDataInstance::init DynamicDataTypeSupport::create_data failed");
    }
    return instance;
  }

  SafeDynamicDataInstance::SafeDynamicDataInstance(
    DDSDynamicDataTypeSupport * typeSupport)
    : type_support_(typeSupport),
      instance_(0)
  {
    instance_ = init(type_support_);
  }

  SafeDynamicDataInstance::SafeDynamicDataInstance(
      const SafeDynamicDataInstance & sddi)
    : type_support_(sddi.type_support_),
      instance_(0)
  {
    instance_ = init(type_support_);
    instance_->copy(*sddi.instance_);
  }

  SafeDynamicDataInstance & SafeDynamicDataInstance::operator = (
      const SafeDynamicDataInstance & rhs)
  {
    SafeDynamicDataInstance(rhs).swap(*this);
    return *this;
  }

  void SafeDynamicDataInstance::swap(SafeDynamicDataInstance & other) throw()
  {
    std::swap(type_support_, other.type_support_);
    std::swap(instance_, other.instance_);
  }

  SafeDynamicDataInstance::~SafeDynamicDataInstance()
  {
    if (instance_ != NULL) {
      DDS_ReturnCode_t rc = type_support_->delete_data(instance_);
      if (rc != DDS_RETCODE_OK) 
      {
        std::cerr << "~SafeDynamicDataInstance: Unable to delete instance data, error = "
                  << get_readable_retcode(rc) << std::endl;
      }
      instance_ = NULL;
    }
  }

  DDS_DynamicData * SafeDynamicDataInstance::get() {
    return instance_;
  }

  const DDS_DynamicData * SafeDynamicDataInstance::get() const {
    return instance_;
  }

} // namespace reflex
