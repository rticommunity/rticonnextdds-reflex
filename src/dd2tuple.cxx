/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#include <iostream>
#include "ndds/ndds_cpp.h"
#include "reflex/dllexport.h"
#include "reflex/dd2tuple.h"

namespace reflex {

  REFLEX_INLINE DDS_DynamicData * AutoDynamicData::init(DDSDynamicDataTypeSupport * ts)
  {
    if (!ts) 
    {
      throw std::runtime_error("AutoDynamicData::init NULL typeSupport");
    }
    DDS_DynamicData * instance = ts->create_data();
    if (!instance) 
    {
      throw std::runtime_error(
        "AutoDynamicData::init DynamicDataTypeSupport::create_data failed");
    }
    return instance;
  }

  REFLEX_INLINE AutoDynamicData::AutoDynamicData(
    DDSDynamicDataTypeSupport * typeSupport)
    : type_support_(typeSupport),
      instance_(0)
  {
    instance_ = init(type_support_);
  }

  REFLEX_INLINE AutoDynamicData::AutoDynamicData(
      const AutoDynamicData & sddi)
    : type_support_(sddi.type_support_),
      instance_(0)
  {
    instance_ = init(type_support_);
    instance_->copy(*sddi.instance_);
  }

  REFLEX_INLINE AutoDynamicData & AutoDynamicData::operator = (
      const AutoDynamicData & rhs)
  {
    AutoDynamicData(rhs).swap(*this);
    return *this;
  }

  REFLEX_INLINE void AutoDynamicData::swap(AutoDynamicData & other) throw()
  {
    std::swap(type_support_, other.type_support_);
    std::swap(instance_, other.instance_);
  }

  REFLEX_INLINE AutoDynamicData::~AutoDynamicData()
  {
    if (instance_ != NULL) {
      DDS_ReturnCode_t rc = type_support_->delete_data(instance_);
      if (rc != DDS_RETCODE_OK) 
      {
        std::cerr << "~AutoDynamicData: Unable to delete instance data, error = "
                  << detail::get_readable_retcode(rc) << std::endl;
      }
      instance_ = NULL;
    }
  }

  REFLEX_INLINE DDS_DynamicData * AutoDynamicData::get() {
    return instance_;
  }

  REFLEX_INLINE const DDS_DynamicData * AutoDynamicData::get() const {
    return instance_;
  }

} // namespace reflex
