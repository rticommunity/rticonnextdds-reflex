/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_H
#define RTIREFLEX_H

#include <ndds/ndds_cpp.h>
#include <tuple>
#include <vector>

#include "reflex/sample.h"
#include "reflex/auto_dd.h"
#include "reflex/dllexport.h"
#include "reflex/dd_manip.h"
#include "reflex/typecode_manip.h"
#include "reflex/member_names.h"
#include "reflex/memberwise.h"
#include "reflex/bounded.h"
#include "reflex/generic_dr.h"
#include "reflex/generic_dw.h"
#include "reflex/safe_dynamicdata.h"
#include "reflex/type_manager.h"
#include "reflex/reflex_fwd.h"

#include <boost/fusion/sequence/intrinsic/size.hpp>
#include <boost/fusion/include/size.hpp>

/** @file
*   @brief The top-level file that must be #included to use RefleX.
*/

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
    void write_dynamicdata_impl(
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
    void write_dynamicdata_impl(
      const T & data,
      DDS_DynamicData &instance,
      true_type /* T has a base */)
    {
      typedef typename InheritanceTraits<T>::basetype Base;

      write_dynamicdata_impl(
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
    void read_dynamicdata_impl(
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
    void read_dynamicdata_impl(
      const DDS_DynamicData & instance,
      T & data,
      true_type /* T has a base*/)
    {
      typedef typename InheritanceTraits<T>::basetype Base;

      read_dynamicdata_impl(
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

  template <class T>
  void write_dynamicdata(DDS_DynamicData &dest, const T & src)
  {
    detail::write_dynamicdata_impl(
      src,
      dest,
      typename detail::InheritanceTraits<T>::has_base());
  }

  template <class T>
  void write_dynamicdata(AutoDynamicData &dest, const T & src)
  {
    detail::write_dynamicdata_impl(
      src,
      *dest.get(),
      typename detail::InheritanceTraits<T>::has_base());
  }

  template <class T>
  SafeTypeCode<T> make_typecode(const char * name /* default 0 */)
  {
    SafeTypeCode<T> aggregateTc =
      detail::Struct_TC_Helper::get_typecode_struct<T>(
        name,
        typename detail::InheritanceTraits<T>::has_base());

    return aggregateTc;
  }

  template <class T>
  void read_dynamicdata(T & dest, const DDS_DynamicData & src)
  {
    detail::read_dynamicdata_impl(
      src,
      dest,
      typename detail::InheritanceTraits<T>::has_base());
  }

  template <class T>
  void read_dynamicdata(T & dest, const AutoDynamicData & src)
  {
    detail::read_dynamicdata_impl(
      *src.get(),
      dest,
      typename detail::InheritanceTraits<T>::has_base());
  }

} // namespace reflex



#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/reflex.cxx"
#endif


#endif // RTIREFLEX_H

