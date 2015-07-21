/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_AUTO_DD_H
#define RTIREFLEX_AUTO_DD_H

#include "reflex/dllexport.h"

class DDSDynamicDataTypeSupport;
struct DDS_DynamicData;

namespace reflex {

  /**
  * @brief Automatically manages the memory of a DynamicData instance. 
  * 
  * AutoDynamicData behaves like a value-type and therefore copying
  * an AutoDynamicData instance makes a deep copy of the underlying
  * DDS_DynamicData instance. AutoDynamicData is exception-safe but
  * not type-safe. For type-safety, see reflex::SafeDynamicData.
  *
  * AutoDynamiData may or may not contain meaningful data. It always
  * contains a properly initialized (not necessarily populated) 
  * DynamicData instance.
  */
  class REFLEX_DLL_EXPORT AutoDynamicData
  {
  private:
    DDSDynamicDataTypeSupport * type_support_;
    DDS_DynamicData * instance_;

    /**
     * Create an empty DynamicData instance from the given type-support.
     */
    static DDS_DynamicData * init(DDSDynamicDataTypeSupport * ts);

  public:
    /**
     * @brief Create an initialized but unpopulated DynamicData instance
     *        from the given type-support. 
     * 
     *        The typeSupport object is not owned by AutoDynamicData.
     */
    AutoDynamicData(DDSDynamicDataTypeSupport * typeSupport);

    /**
    * @brief Create a deep copy 
    */
    AutoDynamicData(const AutoDynamicData &);

    /**
    * @brief Deep copy of the contents of src in to this
    */
    AutoDynamicData & operator = (const AutoDynamicData &src);

    /**
    * @brief Destructor
    */
    ~AutoDynamicData();

#ifdef REFLEX_HAS_RVALUE_REF
    /**@brief Moves the ownership of DynamicData from src to a newly
    *         created AutoDynamicData object.
    *
    *         The src object no longer owns the DynamicData instance.
    */
    AutoDynamicData(AutoDynamicData && src);

    /**@brief Moves the ownership of DynamicData from src to this.
    *
    *         Deletes the existing DynamicData instance in this.
    */
    AutoDynamicData & operator = (AutoDynamicData && src);
#endif 

    /**
    * @brief Swap two AutoDynamicData objects. Does not throw.
    */
    void swap(AutoDynamicData &) throw();

    /**
    * @brief Return the underlying DynamicData instance.
    */
    DDS_DynamicData * get();

    /**
    * @brief Return the underlying DynamicData instance.
    */
    const DDS_DynamicData * get() const;
  };

  /**
  * @brief Swap the contents of two AutoDynamicData instances. Never throws.
  */
  void swap(AutoDynamicData & lhs, AutoDynamicData & rhs) throw();

} // namespace reflex

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/auto_dd.cxx"
#endif

#endif // RTIREFLEX_AUTO_DD_H

