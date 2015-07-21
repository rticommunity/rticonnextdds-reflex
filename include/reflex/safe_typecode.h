/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_SAFE_TYPECODE_H
#define RTIREFLEX_SAFE_TYPECODE_H

#include <tuple>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <boost/type_traits.hpp>

#include <ndds/ndds_cpp.h>
#include "reflex/dllexport.h"

namespace reflex {

  namespace detail {
      REFLEX_DLL_EXPORT const char *get_readable_ex_code(DDS_ExceptionCode_t ex);
      REFLEX_DLL_EXPORT void check_exception_code(const char * message, DDS_ExceptionCode_t ex);
  }

  /**
  * @brief A type-safe, exception-safe wrapper for DDS 
  *        <a href="http://community.rti.com/rti-doc/510/ndds/doc/html/api_cpp/structDDS__TypeCode.html"> 
  *        TypeCode</a>. 
  *
  *        The SafeTypeCode may or may not own the underlying TypeCode object. 
  *        It distinguishes between owned vs. not-owned via const-ness of 
  *        the argument TypeCode. const TypeCode objects are not owned whereas
  *        non-const TypeCode objects are owned by the SafeTypeCode instance.
  *        Note that TypeCode for all fundamental types are const and therefore
  *        SafeTypeCode does not assume ownership of them. 
  *
  *        Copying SafeTypeCode object may make a deep copy depending upon
  *        whether the argument TypeCode was const or not.
  */
  template <class T>
  class SafeTypeCode
  {
    DDS_TypeCodeFactory * factory_;
    DDS_TypeCode * typecode_;
    bool is_shared_;

  public:

    /** @brief Construct an SafeTypeCode object that owns the TypeCode
    *
    *          Use default TypeCodeFactory. I.e., DDS_TypeCodeFactory::get_instance()
    */
    explicit SafeTypeCode(DDS_TypeCode * tc)
      : factory_(DDS_TypeCodeFactory::get_instance()),
        typecode_(tc),
        is_shared_(false)
    { }

    /** @brief Construct an SafeTypeCode object that does not own the TypeCode
    *
    *          Use default TypeCodeFactory. I.e., DDS_TypeCodeFactory::get_instance()
    */
    explicit SafeTypeCode(const DDS_TypeCode * tc)
      : factory_(DDS_TypeCodeFactory::get_instance()),
      typecode_(tc),
      is_shared_(true)
    { }

    /** @brief Construct an SafeTypeCode object that owns the TypeCode
    */
    SafeTypeCode(DDS_TypeCodeFactory * fact,
                 DDS_TypeCode * tc)
      : factory_(fact),
        typecode_(tc),
        is_shared_(false)
    { }

    /** @brief Construct an SafeTypeCode object that does not own the TypeCode
    */
    SafeTypeCode(DDS_TypeCodeFactory * fact,
                 const DDS_TypeCode * tc)
      : factory_(fact),
        typecode_(const_cast<DDS_TypeCode *>(tc)),
        is_shared_(true)
    { }

    /**@brief Create a copy of the argument SafeTypeCode
    *    
    *         The copy is shallow if the argument SafeTypeCode
    *         was constructed using a const TypeCode.
    *         Makes a deep copy otherwise.
    */
    SafeTypeCode(const SafeTypeCode & safetc)
      : factory_(safetc.factory_)
    {
      if (safetc.is_shared_)
      {
        typecode_ = safetc.typecode_;
        is_shared_ = true;
      }
      else
      {
        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
        typecode_ = factory_->clone_tc(safetc.typecode_, ex);
        is_shared_ = false;
        detail::check_exception_code(
          "SafeTypeCode::SafeTypeCode: Unable to clone typecode, error = ",
          ex);
      }
    }

    /**@brief Create a copy of the argument SafeTypeCode
    *
    *         The copy is shallow if the argument SafeTypeCode
    *         was constructed using a const TypeCode.
    *         Makes a deep copy otherwise.
    */
    SafeTypeCode & operator = (const SafeTypeCode & rhs)
    {
      SafeTypeCode(rhs).swap(*this);
      return *this;
    }

#ifdef REFLEX_HAS_RVALUE_REF

    /**@brief Moves the ownership of the TypeCode from src to a newly
    *         created SafeTypeCode object. The src typecode
    *         is set to NULL.     
    */
    SafeTypeCode(SafeTypeCode && src) 
      : factory_(src.factory_),
        typecode_(src.typecode_),
        is_shared_(src.is_shared_)
    {
      src.factory_ = 0;
      src.typecode_ = 0;
    }

    /**@brief Moves the ownership of the TypeCode from src to this 
    *         object. Deletes the existing TypeCode if owned. The 
    *         src typecode is set to NULL.
    */
    SafeTypeCode & operator = (SafeTypeCode && src)
    {
      this->swap(src);
      return *this;
    }

#endif

    /**@brief Deletes the underlying TypeCode if the object was
    *         was constructed using a non-const TypeCode.
    */
    ~SafeTypeCode()
    {
        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
        if (factory_ && typecode_ && !is_shared_)
        {
          factory_->delete_tc(typecode_, ex);

          if (ex != DDS_NO_EXCEPTION_CODE)
          {
            std::cerr << "~SafeTypeCode: Unable to delete typecode, error = "
                      << detail::get_readable_ex_code(ex) 
                      << std::endl;
            // Do not throw.
          }
        }
    }

    /** @brief Get the underlying TypeCode 
    */

    DDS_TypeCode * get()
    {
      return typecode_;
    }

    /** @brief Get the underlying TypeCode
    */
    const DDS_TypeCode * get() const
    {
      return typecode_;
    }

    /** @brief Release the ownership of the underlying TypeCode.
    *          The SafeTypeCode object remains usable.
    */
    DDS_TypeCode * release()
    {
      is_shared_ = true;
      return typecode_;
    }

    /** @brief Swap the contents of two SafeTypeCode objects. Never throws.
    */
    void swap(SafeTypeCode & stc) throw()
    {
      std::swap(factory_, stc.factory_);
      std::swap(typecode_, stc.typecode_);
      std::swap(is_shared_, stc.is_shared_);
    }
  };

  /** @brief Swap the contents of two SafeTypeCode objects. Never throws.
  */
  template <class T>
  void swap(SafeTypeCode<T> & lhs, SafeTypeCode<T> & rhs) throw()
  {
    lhs.swap(rhs);
  }


} // namespace reflex


#endif // RTIREFLEX_SAFE_TYPECODE_H

