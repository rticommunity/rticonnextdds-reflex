/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DD2TUPLE_H
#define RTIREFLEX_DD2TUPLE_H

#include <ndds/ndds_cpp.h>
#include <tuple>
#include <vector>

#include "reflex/dllexport.h"
#include "reflex/dd_manip.h"
#include "reflex/typecode_manip.h"
#include "reflex/memberwise.h"
#include "reflex/default_member_names.h"
#include "reflex/bounded.h"

#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/include/size.hpp>

namespace reflex {

  namespace detail {

    template <class T, class U>
    struct MembersInBasesImpl;

    template <class T>
    struct MembersInBasesImpl<T, false_type>
    {
      enum { value = 0 };
    };

    template <class T>
    struct MembersInBasesImpl<T, true_type>
    {
      typedef typename InheritanceTraits<T>::basetype  Base;
      typedef typename InheritanceTraits<Base>::has_base BaseHasBase;

      enum {
        value = MembersInBasesImpl<Base, BaseHasBase>::value +
        Size<Base>::value
      };
    };

    template <class T>
    struct MembersInBases
    {
      typedef typename InheritanceTraits<T>::has_base HasBase;
      enum { value = MembersInBasesImpl<T, HasBase>::value };
    };


    template <class T>
    void fill_dd_impl(
      const T & data,
      DDS_DynamicData &instance,
      false_type /* T has no base */)
    {
      typedef detail::TypelistIterator<
                T,
                0,
                detail::Size<T>::value - 1> TIter;
      
      TIter::set(
            instance, 
            detail::MemberAccess::BY_ID(1), 
            data);
    }

    template <class T>
    void fill_dd_impl(
      const T & data,
      DDS_DynamicData &instance,
      true_type /* T has a base */)
    {
      typedef typename InheritanceTraits<T>::basetype Base;

      fill_dd_impl(
        static_cast<const Base &>(data),
        instance,
        typename detail::InheritanceTraits<Base>::has_base());

      typedef detail::TypelistIterator<
                  T,
                  0,
                  detail::Size<T>::value - 1> TIter;
      
      TIter::set(
            instance, 
            detail::MemberAccess::BY_ID(MembersInBases<T>::value + 1), 
            data);
    }

    template <class T>
    void extract_dd_impl(
      const DDS_DynamicData & instance,
      T & data,
      false_type /* T has no base*/)
    {
      typedef detail::TypelistIterator<
        T,
        0,
        detail::Size<T>::value - 1> TIter;

      TIter::get(
        instance,
        detail::MemberAccess::BY_ID(1),
        data);
    }

    template <class T>
    void extract_dd_impl(
      const DDS_DynamicData & instance,
      T & data,
      true_type /* T has a base*/)
    {
      typedef typename InheritanceTraits<T>::basetype Base;

      extract_dd_impl(
        instance,
        static_cast<Base &>(data),
        typename InheritanceTraits<Base>::has_base());

      typedef detail::TypelistIterator<
        T,
        0,
        detail::Size<T>::value - 1> TIter;

      TIter::get(
        instance,
        detail::MemberAccess::BY_ID(MembersInBases<T>::value + 1),
        data);
    }

  } // namespace detail

  class DllExport AutoDynamicData
  {
  protected:
    DDSDynamicDataTypeSupport * type_support_;
    DDS_DynamicData * instance_;

   static DDS_DynamicData * init(DDSDynamicDataTypeSupport * ts);

  public:
    AutoDynamicData(DDSDynamicDataTypeSupport * typeSupport);
    AutoDynamicData(const AutoDynamicData &);

    AutoDynamicData & operator = (const AutoDynamicData &);

    ~AutoDynamicData();

    void swap(AutoDynamicData &) throw();
    DDS_DynamicData * get();
    const DDS_DynamicData * get() const;
  };

  template <class T>
  class SafeDynamicData : public AutoDynamicData
  {
    public:
      SafeDynamicData(
          DDSDynamicDataTypeSupport *type_support,
          const T & src)
        : AutoDynamicData(type_support)
      {
        fill_dd(src, *instance_);
      }

      explicit SafeDynamicData(
          DDSDynamicDataTypeSupport *type_support)
        : AutoDynamicData(type_support)
      { }

      SafeDynamicData & operator = (const T & src)
      {
        fill_dd(src, *instance_);
        return *this;
      }
  };

  template <class T>
  void fill_dd(const T & data, DDS_DynamicData &instance)
  {
    detail::fill_dd_impl(
      data,
      instance,
      typename detail::InheritanceTraits<T>::has_base());
  }

  template <class T>
  void fill_dd(const T & data, AutoDynamicData &instance)
  {
    detail::fill_dd_impl(
      data,
      *instance.get(),
      typename detail::InheritanceTraits<T>::has_base());
  }

  template <class T>
  SafeDynamicData<T> make_dd(const T & src)
  {
    static reflex::SafeTypeCode<DDS_TypeCode> 
              stc(reflex::make_typecode<T>());
      
    static DDS_DynamicDataTypeProperty_t props;
              
    static DDSDynamicDataTypeSupport * type_support =  
             new DDSDynamicDataTypeSupport(stc.get(), props);
      
    return reflex::SafeDynamicData<T>(type_support, src);
  }
  
  template <class T>
  SafeTypeCode<T> make_typecode(const char * name /* default 0 */)
  {
    SafeTypeCode<T> aggregateTc =
      detail::make_typecode_impl<T>(
        name,
        typename detail::InheritanceTraits<T>::has_base());
    
    return move(aggregateTc);
  }

  template <class T>
  void extract_dd(const DDS_DynamicData & instance, T & data)
  {
    detail::extract_dd_impl(
      instance,
      data,
      typename detail::InheritanceTraits<T>::has_base());
  }

  template <class T>
  void extract_dd(const AutoDynamicData & instance, T & data)
  {
    detail::extract_dd_impl(
      *instance.get(),
      data,
      typename detail::InheritanceTraits<T>::has_base());
  }


  template <class Tuple>
  void tuple2dd(const Tuple & tuple, DDS_DynamicData &instance)
  {
    fill_dd(tuple, instance);
  }

  template <class Tuple>
  void dd2tuple(const DDS_DynamicData & instance, Tuple & tuple)
  {
    extract_dd(instance, tuple);
  }

  template <class Tuple>
  SafeTypeCode<Tuple> tuple2typecode(const char * name = 0)
  {
    return make_typecode<Tuple>(name);
  }

} // namespace reflex

#endif //  RTIREFLEX_DD2TUPLE_H

