/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTI_REFLEX_SAFE_DYNAMIC_DATA_H
#define RTI_REFLEX_SAFE_DYNAMIC_DATA_H

#include "reflex/auto_dd.h"

namespace reflex {
  /**
  * @brief Typesafe, exception-safe, poulated DynamicData
  *
  * The objects of SafeDynamicData are guaranteed to contain an initialized
  * and fully populated DynamicData instance of type T. SafeDynamicData
  * is a value-type, which means copying SafeDynamicData would result in a
  * deep copy of the underlying DynamicData instance.
  *
  * @see reflex::write_dynamicdata
  * @see reflex::read_dynamicdata
  * @see reflex::make_dynamicdata
  */
  template <class T>
  class SafeDynamicData : public AutoDynamicData
  {
  public:

    /**
    * @brief Create a new SafeDynamicData object from a source object of type T.
    *
    * @param type_support Type support object that represents T
    * @param src The source object
    * @pre The type support must include the typecode obtained from
    *      reflex::make_typecode for type T.
    * @see reflex::make_dynamicdata
    */
    SafeDynamicData(
      DDSDynamicDataTypeSupport *type_support,
      const T & src)
      : AutoDynamicData(type_support)
    {
      write_dynamicdata(*AutoDynamicData::get(), src);
    }

    /**
    * @brief Swap two SafeDynamicData objects. Does not throw.
    */
    void swap(SafeDynamicData<T> & rhs) throw()
    {
      this->AutoDynamicData::swap(rhs);
    }
  };

  /** @brief Swap the contents of two SafeDynamicData objects. Never throws.
  */
  template <class T>
  void swap(SafeDynamicData<T> & lhs, SafeDynamicData<T> & rhs) throw()
  {
    lhs.swap(rhs);
  }

} // namespace reflex

#endif // RTI_REFLEX_SAFE_DYNAMIC_DATA_H
