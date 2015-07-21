/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTI_REFLEX_TYPE_INFO_H
#define RTI_REFLEX_TYPE_INFO_H

#include <memory>

#include "reflex/safe_typecode.h"


namespace reflex
{
  template <class T>
  class SafeDynamicData;

  namespace detail {
    class empty_deleter
    {
    public:
      void operator () (DDSDynamicDataTypeSupport *) { }
    };
  }

  /** 
  * @brief  Manage type-specific context, such as typecode, type-support, and 
  *         dynamic data properties.
  *         
  *         This class manages the type related context necessary to create
  *         dynamic data objects from a user-defined type. It is a container
  *         of the typecode, type-support for type T. It also encapsulates
  *         the DynamicDataType properties used to create dynamic data objects.
  *         Instances of the TypeManager class are immutable. 
  */

  template <class T>
  class TypeManager
  {
    DDS_DynamicDataTypeProperty_t _props;
    SafeTypeCode<T> _safe_typecode;
    std::shared_ptr<DDSDynamicDataTypeSupport> _type_support;

  public:
    /**
    * @brief  Create a type manager that uses the argument properties to create
    *         all the DynamicData objects.
    * @param  props Properties that govern DynamicData objects
    */
    explicit TypeManager(const DDS_DynamicDataTypeProperty_t & props =
                           DDS_DynamicDataTypeProperty_t())
      : _props(props),
      _safe_typecode(reflex::make_typecode<T>()),
      _type_support(std::make_shared<DDSDynamicDataTypeSupport>(_safe_typecode.get(), _props))
    {  }

    /**
    * @brief  Create a type manager that uses the argument DynamicDataTypeSupport 
    *         to create all the DynamicData objects.
    * @param  support An instance of DynamicDataTypeSupport. 
    */

    TypeManager(DDSDynamicDataTypeSupport * support)
      : _props(),
        _safe_typecode(static_cast<const DDSDynamicDataTypeSupport *>(support->get_data_type())),
        _type_support(support, detail::empty_deleter())
    {  }

    /**
    * Creates a type-safe, exception-safe instance of SafeDynamicData from a source object.
    * This function combines reflex::make_typecode and reflex::write_dynamicdata into one.
    * @param src The source object of type T
    * @return Type-safe, exception-safe instance of SafeDynamicData
    * @see reflex::make_typecode
    * @see reflex::write_dynamicdata
    * @pre Type T must be adapted using the RTI_ADAPT_STURCT macro.
    */
    SafeDynamicData<T> create_dynamicdata(const T & src) const
    {
      return SafeDynamicData<T>(_type_support, src);
    }

    /**
    * Creates a instance of T from a SafeDynamicData object.
    * @param src The source SafeDynamicData object
    * @return An instance of type T
    * @see reflex::read_dynamicdata
    * @pre Type T must be default constructible and adapted using the RTI_ADAPT_STURCT macro.
    */
    T create_instance(const SafeDynamicData<T> & src) const
    {
      T t;
      reflex::read_dynamicdata(t, src);
      return t;
    }

    /**
    * Return the underlying DynamicData properties.
    */
    const DDS_DynamicDataTypeProperty_t & get_properties() const
    {
      return _props;
    }

    /**
    * Return the underlying SafeTypeCode object.
    */
    const SafeTypeCode<T> & get_safe_typecode() const
    {
      return _safe_typecode;
    }

    /**
    * Return the underlying DynamicDataTypeSupport.
    */
    DDSDynamicDataTypeSupport * get_type_support() const
    {
      return _type_support;
    }

  };

} // namespace reflex


#endif // RTI_REFLEX_TYPE_INFO_H

