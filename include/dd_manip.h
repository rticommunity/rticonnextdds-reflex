/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DD_MANIP_H
#define RTIREFLEX_DD_MANIP_H

#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <cassert>

#include "ndds/ndds_cpp.h"
#include "tuple_iterator.h"
#include "dd_traits.h"
#include "enable_if.h"
#include "safe_enum.h"
#include "dd_extra.h"
#include "bounded.h"
#include "common.h"

#include <boost/fusion/support/is_sequence.hpp>
#include <boost/fusion/include/is_sequence.hpp>

template <class T>
bool do_serialize(T &)
{
  return true;
}

template <class T> // when T is an enum
DDS_ReturnCode_t set_array(DDS_DynamicData &instance,                  
                           const MemberAccess & ma,
                           DDS_UnsignedLong length,                    
                           const T *array) 
{                  
  if(ma.access_by_id())
    return instance.set_long_array(NULL, ma.get_id(), length,             
                                   reinterpret_cast<const DDS_Long *>(array)); 
  else
    return instance.set_long_array(ma.get_name(), 
                                   DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED, 
                                   length,             
                                   reinterpret_cast<const DDS_Long *>(array)); 
}

template <class T> // When T is an enum
void set_memberI_value(DDS_DynamicData & instance,                            
                       const MemberAccess &ma,                                
                       const T & val,
                       typename enable_if<std::is_enum<T>::value>::type *)  
{                                   
  DDS_ReturnCode_t rc;                                                        
  if(ma.access_by_id())                                                       
    rc = instance.set_long(NULL, ma.get_id(), val);                           
  else                                                                        
    rc = instance.set_long(ma.get_name(),  
                           DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,            
                           val);                                              
  if (rc != DDS_RETCODE_OK) {                                                 
    std::cerr << "set_long (for enums) error = " << get_readable_retcode(rc) << "\n";   
    throw 0;                                                                  
  }                                                                           
}

template <class Typelist>
// Typelist could be
// 1. A user-defined struct adapted as a Fusion Sequence
// 2. std::tuple
// 3. std::pair
// The overload is disbaled for user-defined enums.
void set_memberI_value(DDS_DynamicData & instance,
  const MemberAccess & ma,
  const Typelist & val,
  typename disable_if<std::is_enum<Typelist>::value ||
                      is_container<Typelist>::value ||
                      is_range<Typelist>::value>::type * = 0)
{
  DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
  SafeBinder binder(instance, inner, ma);

  typedef
    TypelistIterator < Typelist,
    0,
    Size<Typelist>::value - 1 > TIter;

  TIter::set(inner, MemberAccess::BY_ID(), val);
}


template <class C>
// When C is a container of primitives 
// but not vector<primitives except (bool and enums)>
void set_memberI_value(DDS_DynamicData & instance,
  const MemberAccess &ma,
  const C & val,
  typename enable_if<
    is_container<C>::value &&
    (is_primitive<typename C::value_type>::value ||
     std::is_enum<typename C::value_type>::value)   &&
    (is_vector<C>::value?
     is_bool_or_enum<typename C::value_type>::value : true)>::type * = 0)
/*  typename disable_if<
    !is_container<C>::value ||
    is_tuple<C>::value      ||
    is_tuple<typename C::value_type>::value ||
    is_stdarray<typename C::value_type>::value ||
    is_container<typename C::value_type>::value ||
    boost::fusion::traits::is_sequence<C>::value  ||
    boost::fusion::traits::is_sequence<typename C::value_type>::value ||
    (is_vector<C>::value && 
     !is_bool_or_enum<typename C::value_type>::value) > ::type * = 0)*/
{
  if(do_serialize(val))
  {
    typename DynamicDataSeqTraits<typename C::value_type>::type seq;
    seq.ensure_length(val.size(), val.size());
    
    size_t i = 0;
    for (auto const &elem : val) {
      seq[i] = elem;
      ++i;
    }

    DDS_ReturnCode_t rc = set_sequence(instance, ma, seq);
    if (rc != DDS_RETCODE_OK) {
      std::cerr << "Error setting (bool/enum) seq, error=" << rc << std::endl;
      throw 0;
    }
  }
}

template <class C>
// When C is a container of user-defined type
void set_memberI_value(DDS_DynamicData & instance,
  const MemberAccess &ma,
  const C & val,
  typename disable_if<!is_container<C>::value ||
                      is_primitive_or_enum<typename C::value_type>::value>::type * = 0)
{
  if(do_serialize(val))
  {
    DDS_DynamicData seq_member(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
    SafeBinder binder(instance, seq_member, ma);

    // set_seq_length seems to avoid the following weird behavior:
    // If you have an outer sequence of size N and each element of
    // the sequence is an empty sequence, the outer sequence in 
    // the dynamic data instance is off by 1. This function seems
    // to handle nested empty sequences.
    // BTW, sequence of strings seem to work fine without magic.
    set_seq_length(seq_member, 
                   val.size(), 
                   is_string<typename C::value_type>::value);
    
    size_t i = 0;
    for (auto const &elem : val)
    {
      set_memberI_value(seq_member, MemberAccess::BY_ID(i + 1), elem);
      ++i;
    }
  }
}

template <class T>
void set_memberI_value(DDS_DynamicData & instance, 
                       const MemberAccess &ma,
                       const std::vector<T> & val,
                       typename enable_if<is_primitive_and_not_bool<T>::value>::type * = 0)
{
  // Sequences of primitive types are loaned and unloaned as an optimization.
  // bools and enums don't use this optimization because vector<bool> storage is
  // 8 bools-per-byte and sizeof enums is not standarized and it is compiler dependent.

  if(!val.empty())
  {
    typename DynamicDataSeqTraits<T>::type seq;
    std::vector<T> & nc_val = const_cast<std::vector<T> &>(val);
    if(seq.loan_contiguous(&nc_val[0], val.size(), val.capacity()) != true)
    {
      std::cerr << "sequence loaning failed.\n";
      throw 0;
    }
    
    DDS_ReturnCode_t rc = set_sequence(instance, ma, seq);
    seq.unloan();
    if (rc != DDS_RETCODE_OK) {
      std::cerr << "Error setting seq, error=" <<  rc << std::endl;
      throw 0;
    }
  }
}

template <class T>
void set_memberI_value(DDS_DynamicData & instance, 
                       const MemberAccess &ma,
                       const Range<T> & range,
                       typename enable_if<is_primitive_or_enum<T>::value>::type * = 0)
{
  typename DynamicDataSeqTraits<T>::type seq;

  size_t size = boost::distance(range);
  seq.ensure_length(size, size);

  size_t i = 0;
  for (auto const &elem : range) 
  {
       seq[i] = elem;
       ++i;
  }

  DDS_ReturnCode_t rc = set_sequence(instance, ma, seq);
  if (rc != DDS_RETCODE_OK) {
    std::cerr << "Error setting seq, error=" <<  rc << std::endl;
    throw 0;
  }
}

template <class T>
void set_memberI_value(DDS_DynamicData & instance, 
                       const MemberAccess &ma,
                       const Range<T> & range,
                       typename disable_if<is_primitive_or_enum<T>::value>::type * = 0)
{
  DDS_DynamicData seq_member(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, seq_member, ma);

  size_t size = boost::distance(range);

  // set_seq_length seems to avoid the following weird behavior:
  // If you have an outer sequence of size N and each element of
  // the sequence is an empty sequence, the outser sequence in 
  // the dynamic data instance is off by 1. This function seems
  // to handle nested empty sequences.
  // BTW, sequence of strings seem to work fine without magic.
  set_seq_length(seq_member, size, is_string<T>::value);

  size_t i = 0;
  for (typename Range<T>::const_iterator iter=boost::begin(range);
       iter != boost::end(range);
       ++iter, ++i) 
  {
    set_memberI_value
      (seq_member, MemberAccess::BY_ID(i+1), *iter);
  }
}

template <class... Args>
void set_memberI_value(DDS_DynamicData & instance,  
                       const MemberAccess & ma,
                       const Sparse<Args...> & val)
{
  DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, inner, ma);

  typedef typename Sparse<Args...>::tuple_type OptTuple;
  typedef 
    TypelistIterator<OptTuple, 
                     0, 
                     Size<OptTuple>::value-1> TIter;

  TIter::set(inner, MemberAccess::BY_ID(), val.get_opt_tuple());
}

template <class TagType, class... Cases>
void set_memberI_value(DDS_DynamicData & instance,  
                       const MemberAccess & ma,
                       const Union<TagType, Cases...> & val)
{
  DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, inner, ma);

  typedef typename Union<TagType, Cases...>::case_tuple_type CaseTuple;
  typedef TypelistIterator<CaseTuple,
                           0,
                           Size<CaseTuple>::value-1> TIter;

  if(!val.empty())
    TIter::set_union(inner, MemberAccess::BY_ID(), val);
}

template <class T>
uint32_t array_length(const T &)
{
  return dim_list_multiply<typename make_dim_list<T>::type>::value;
}

template <class T, size_t Dim>
void set_memberI_value(
  DDS_DynamicData & instance,  
  const MemberAccess & ma,
  const T (&val)[Dim],
  typename disable_if<is_primitive_or_enum<typename remove_all_extents<T>::type>::value>::type * = 0)
{
  DDS_DynamicData arr_member(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, arr_member, ma);

  typename remove_all_extents<T>::type const * arr =
    reinterpret_cast<typename remove_all_extents<T>::type const *>(&val[0]);

  DDS_UnsignedLong length = array_length(val);
  for(DDS_UnsignedLong i=0;i < length; ++i) 
  {
    set_memberI_value(arr_member, MemberAccess::BY_ID(i+1), arr[i]);
  }
}

template <class T, size_t Dim>
void set_memberI_value(
  DDS_DynamicData & instance, 
  const MemberAccess &ma,
  const T (&val)[Dim],
  typename enable_if<is_primitive_or_enum<typename remove_all_extents<T>::type>::value>::type * = 0)
{
  typename remove_all_extents<T>::type const * arr =
    reinterpret_cast<typename remove_all_extents<T>::type const *>(&val[0]);

  DDS_ReturnCode_t rc = 
    set_array(instance, ma, array_length(val), arr);
  
  if (rc != DDS_RETCODE_OK) {
    std::cerr << "Error setting array, error = " 
              <<  get_readable_retcode(rc) 
              <<  std::endl;
    throw 0;
  }
}

template <class T, size_t Dim>
void set_memberI_value(
  DDS_DynamicData & instance,  
  const MemberAccess & ma,
  const std::array<T, Dim> & val,
  typename disable_if<is_primitive_or_enum<typename remove_all_extents<T>::type>::value>::type * = 0)
{
  DDS_DynamicData arr_member(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, arr_member, ma);

  typename remove_all_extents<T>::type const * arr =
    reinterpret_cast<typename remove_all_extents<T>::type const *>(&val[0]);

  DDS_UnsignedLong length = array_length(val);
  for(DDS_UnsignedLong i=0;i < length; ++i) 
  {
    set_memberI_value(arr_member, MemberAccess::BY_ID(i+1), arr[i]);
  }
}

template <class T, size_t Dim>
void set_memberI_value(
  DDS_DynamicData & instance, 
  const MemberAccess &ma,
  const std::array<T, Dim> & val,
  typename enable_if<is_primitive_or_enum<typename remove_all_extents<T>::type>::value>::type * = 0)
{
  typename remove_all_extents<T>::type const * arr =
    reinterpret_cast<typename remove_all_extents<T>::type const *>(&val[0]);

  DDS_ReturnCode_t rc = 
    set_array(instance, ma, array_length(val), arr);
  
  if (rc != DDS_RETCODE_OK) {
    std::cerr << "Error setting array, error=" 
              << get_readable_retcode(rc)  
              << std::endl;
    throw 0;
  }
}

#ifdef RTI_WIN32
template <typename... T>
void set_memberI_value(DDS_DynamicData & instance,
                       const MemberAccess &ma,
                       const boost::optional<T...> & opt)
#else
template <typename T>
void set_memberI_value(DDS_DynamicData & instance,
                       const MemberAccess &ma,
                       const boost::optional<T> & opt)
#endif
{
  if(opt.is_initialized())
  {
    set_memberI_value(instance, ma, *opt.get_ptr());
  }
}

template <class T, size_t Bound>
void set_memberI_value(DDS_DynamicData & instance,                            
                       const MemberAccess &ma,                                
	                     const bounded<T, Bound> & val)  
{
  set_memberI_value(instance, ma, static_cast<const T &>(val));
}


/***************************************************/

template <class T> // when T is an enum
DDS_ReturnCode_t get_array(const DDS_DynamicData & instance,         
                           T *array,                        
                           DDS_UnsignedLong *length,                 
                           const MemberAccess &ma) 
{ 
  if(ma.access_by_id())
    return instance.get_long_array(reinterpret_cast<DDS_Long *>(array),      
                                   length, NULL, ma.get_id());          
  else
    return instance.get_long_array(reinterpret_cast<DDS_Long *>(array),      
                                   length, 
                                   ma.get_name(), 
                                   DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);          
}

template <class T> // When T is an enum                                                         
void get_memberI_value(const DDS_DynamicData & instance,                           
                       const MemberAccess &ma,                                     
                       T & val,
                       typename enable_if<std::is_enum<T>::value>::type *)
{                                         
  DDS_ReturnCode_t rc;                                                             
  DDS_Long out;                                                                    

  const char * member_name = 
    ma.access_by_id()? NULL : ma.get_name();  
  int id = ma.access_by_id()?                                          
           ma.get_id() : DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED;       
    
  rc = instance.get_long(out, member_name, id);                
                                                                                   
  if (rc != DDS_RETCODE_OK) {                                                      
    std::cerr << "get_long (for enums) error" <<  rc << std::endl;                           
    throw 0;                                                                       
  }                                                                                
  val = static_cast<T>(out); // cast required for enums
}

template <class Typelist>
// Typelist could be
// 1. A user-defined struct adapted as a Fusion Sequence
// 2. std::tuple
// 3. std::pair
// The overload is disbaled for user-defined enums.
void get_memberI_value(const DDS_DynamicData & instance,
  const MemberAccess &ma,
  Typelist & val,
  typename disable_if<std::is_enum<Typelist>::value ||
                      is_container<Typelist>::value ||
                      is_range<Typelist>::value> ::type * = 0)
{
  typedef
    TypelistIterator < Typelist,
    0,
    Size<Typelist>::value - 1 > TIter;

  DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
  SafeBinder binder(instance, inner, ma);

  TIter::get(inner, MemberAccess::BY_ID(), val);
}

template <class T, class Alloc>
void right_size(std::vector<T, Alloc> & v, size_t size)
{
  v.resize(size);
}

template <class T, class Alloc>
void right_size(std::list<T, Alloc> & l, size_t size)
{
  l.resize(size);
}

template <class T, class Comp, class Alloc>
void right_size(std::set<T, Comp, Alloc> & s, size_t)
{
  // sets have no resize function.
  // You have clear it and then insert from scratch.
  s.clear();
}

template <class Key, class T, class Comp, class Alloc>
void right_size(std::map<Key, T, Comp, Alloc> & m, size_t)
{
  // maps have no resize function.
  // You have clear it and then insert from scratch.
  m.clear();
}

template <class Seq, class C>
void copy_primitive_seq(
  const Seq & seq,
  size_t count,
  C & c,
  typename disable_if<is_stdset<C>::value>::type * = 0)
{
  typename C::iterator iter = begin(c);
  for (size_t i = 0; i < count; ++i, ++iter)
  {
    // Cast required for enums
    // Possible extra copy of primitives due 
    // to cast to value (as opposed to a reference)
    *iter = static_cast<typename C::value_type>(seq[i]);
  }
}

template <class Seq, class T, class Key, class Alloc>
void copy_primitive_seq(
  const Seq & seq,
  size_t count,
  std::set<T, Key, Alloc> & s)
{
  for (size_t i = 0; i < count; ++i)
  {
    // Cast required for enums
    // Possible extra copy of primitives due 
    // to cast to value (as opposed to reference)
    s.insert(static_cast<
      typename std::set<T, Key, Alloc>::value_type>(seq[i]));
  }
}

template <class C>
// When C is a std container of primitives
// but not vector<primitives except (bool and enums)>
void get_memberI_value(
  const DDS_DynamicData & instance,
  const MemberAccess &ma,
  C & val,
  typename enable_if <
  is_container<C>::value &&
  (is_primitive<typename C::value_type>::value ||
  std::is_enum<typename C::value_type>::value) &&
  (is_vector<C>::value ?
  is_bool_or_enum<typename C::value_type>::value : true) > ::type * = 0)
/*  typename disable_if<
  !is_container<C>::value ||
  is_tuple<C>::value      ||
  is_tuple<typename C::value_type>::value ||
  is_stdarray<typename C::value_type>::value ||
  is_container<typename C::value_type>::value ||
  boost::fusion::traits::is_sequence<C>::value  ||
  boost::fusion::traits::is_sequence<typename C::value_type>::value ||
  (is_vector<C>::value &&
  !is_bool_or_enum<typename C::value_type>::value) > ::type * = 0)*/
{
  DDS_DynamicDataMemberInfo seq_info;

  const char * member_name =
    ma.access_by_id() ? NULL : ma.get_name();
  int id = ma.access_by_id() ?
    ma.get_id() : DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED;

  instance.get_member_info(seq_info, member_name, id);
  right_size(val, seq_info.element_count); // shrink/grow as needed.

  if (seq_info.element_count > 0)
  {
    typename DynamicDataSeqTraits<typename C::value_type>::type seq;
    seq.ensure_length(seq_info.element_count,
                      seq_info.element_count);

    DDS_ReturnCode_t rc = get_sequence(instance, seq, ma);
    if (rc != DDS_RETCODE_OK) {
      std::cerr << "get_sequence error = " << rc << std::endl;
      throw 0;
    }

    copy_primitive_seq(seq, seq_info.element_count, val);
  }
}

template <class Seq, class C>
// When C is a container of user-defined type (a.k.a. fusion sequence)
void copy_complex_seq(
  const Seq & seq,
  size_t count,
  C & c,
  typename disable_if<is_stdset<C>::value>::type * = 0)
{
  if (count > 0)
  {
#ifdef _DEBUG
    assert(c.size() == count); // size() is O(1) even for std::list in C++11
#endif
    size_t i = 0;
    for (auto &elem : c) // runs from 0 to count-1
    {
      get_memberI_value(seq, MemberAccess::BY_ID(i + 1), elem);
      ++i;
    }
  }
}

template <class Seq, class Key, class T, class Comp, class Alloc>
// When C is a map (each value_type is a std::pair<const Key, T>)
void copy_complex_seq(
  const Seq & seq,
  size_t count,
  std::map<Key, T, Comp, Alloc> & map)
{
  if (count > 0)
  {
    size_t i = 0;
    for (size_t i = 0; i < count; ++i)
    {
      typedef typename std::map<Key, T, Comp, Alloc>::value_type PairType;
      std::pair<typename std::remove_const<typename PairType::first_type>::type,
                typename PairType::second_type> temp;
      get_memberI_value(seq, MemberAccess::BY_ID(i + 1), temp);
      map.insert(std::move(temp));
    }
  }
}

template <class Seq, class T, class Key, class Alloc>
// When set is a container of user-defined type (a.k.a. fusion sequence)
void copy_complex_seq(
  const Seq & seq,
  size_t count,
  std::set<T, Key, Alloc> & s)
{
  if (count > 0)
  {
    for (size_t i = 0; i < count; ++i)
    {
      // set iterator refer to const elements only
      // Therefore, you have to create a temporary, fill it in,
      // and insert in the set.
      T temp {}; // default-initialize
      get_memberI_value(seq, MemberAccess::BY_ID(i + 1), temp);
      s.insert(std::move(temp));
    }
  }
}

template <class C>
// When C is a container of user-defined type (a.k.a. fusion sequence)
void get_memberI_value(
  const DDS_DynamicData & instance,
  const MemberAccess &ma,
  C & val,
  typename disable_if<!is_container<C>::value || 
                      is_primitive_or_enum<typename C::value_type>::value>::type * = 0)
{
  DDS_DynamicData seq_member(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT);
  SafeBinder binder(instance, seq_member, ma);

  DDS_DynamicDataInfo seq_info;
  seq_member.get_info(seq_info);

  right_size(val, seq_info.member_count);
  copy_complex_seq(seq_member, seq_info.member_count, val);
}


template <class T>
void get_memberI_value(const DDS_DynamicData & instance, 
                       const MemberAccess &ma, 
                       std::vector<T> & val,
                       typename enable_if<is_primitive_and_not_bool<T>::value>::type * = 0)
{
  // Sequences of primitive types are loaned and unloaned as an optimization.
  // bools and enums don;t use this optimization because vector<bool> storage is
  // 8 bools-per-byte and sizeof enums is not standarized and it is compiler dependent.

  DDS_DynamicDataMemberInfo seq_info;

  const char * member_name = 
    ma.access_by_id()? NULL : ma.get_name();  
  int id = ma.access_by_id()?                                          
           ma.get_id() : DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED;       

  instance.get_member_info(seq_info, member_name, id);
  right_size(val, seq_info.element_count); 

  if (seq_info.element_count > 0)
  {
    typename DynamicDataSeqTraits<T>::type seq;
    if(seq.loan_contiguous(&val[0], val.size(), val.capacity()) != true)
    {
      std::cerr << "sequence loaning failed.\n";
      throw 0;
    }

    DDS_ReturnCode_t rc = get_sequence(instance, seq, ma);
    seq.unloan();
    if (rc != DDS_RETCODE_OK) {
      std::cerr << "get_sequence error = " << rc << std::endl;
      throw 0;
    }
  }
}

template <class T, size_t Bound>
void get_memberI_value(const DDS_DynamicData & instance, 
                       const MemberAccess &ma, 
                       bounded<T, Bound> & val)
{
  get_memberI_value(instance, ma, static_cast<T &>(val));
}

template <class T>
// If you are populating a range it must be sufficient in capacity
// You probably also need to "mark" the range to be erased to
// separate the old elements from the new one. 
void get_memberI_value(const DDS_DynamicData & instance, 
                       const MemberAccess &ma, 
                       Range<T> & range,
                       typename enable_if<is_primitive_or_enum<T>::value>::type * = 0)
{
  typename DynamicDataSeqTraits<T>::type seq;
  DDS_DynamicDataMemberInfo seq_info;

  const char * member_name = 
    ma.access_by_id()? NULL : ma.get_name();  
  int id = ma.access_by_id()?                                          
           ma.get_id() : DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED;       

  instance.get_member_info(seq_info, member_name, id);
  seq.ensure_length(seq_info.element_count, 
                    seq_info.element_count);
  
  if (seq_info.element_count > 0)
  {
    DDS_ReturnCode_t rc = get_sequence(instance, seq, ma);
    if (rc != DDS_RETCODE_OK) {
      std::cerr << "get_sequence error = " << rc << std::endl;
      throw 0;
    }

    if(boost::distance(range) < seq_info.element_count) {
      std::cerr << "Insufficient range\n";
      throw 0;
    }

    typename Range<T>::iterator iter = boost::begin(range);
    for (size_t i = 0; i < seq_info.element_count; ++i, ++iter)
    {
      // Possible extra copy of the primitives
      *iter = static_cast<T>(seq[i]); // cast required for enums
    }
  }
}

template <class T>
void get_memberI_value(const DDS_DynamicData & instance, 
                       const MemberAccess &ma, 
                       Range<T> & range,
                       typename disable_if<is_primitive_or_enum<T>::value>::type * = 0)
{
  DDS_DynamicData seq_member(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, seq_member, ma);

  DDS_DynamicDataInfo seq_info;
  seq_member.get_info(seq_info);

  if(boost::distance(range) < seq_info.member_count) {
    std::cerr << "Insufficient range\n";
    throw 0;
  }

  typename Range<T>::iterator iter = boost::begin(range);
  for(size_t i=0; i < seq_info.member_count; ++i, ++iter)
  {
    get_memberI_value(seq_member, MemberAccess::BY_ID(i+1), *iter);
  }
}

template <class... Args>
void get_memberI_value(const DDS_DynamicData & instance,  
                       const MemberAccess &ma, 
                       Sparse<Args...> & val)
{
  typedef typename Sparse<Args...>::tuple_type OptTuple;
  typedef 
    TypelistIterator<OptTuple, 
                     0, 
                     Size<OptTuple>::value-1> TIter;
  DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, inner, ma);

  TIter::get(inner, MemberAccess::BY_ID(), val.get_opt_tuple());
}

template <class T, size_t Dim>
void get_memberI_value(
  const DDS_DynamicData & instance,  
  const MemberAccess & ma,
  T(&val)[Dim],
  typename disable_if<is_primitive_or_enum<typename remove_all_extents<T>::type>::value>::type * = 0)
{
  DDS_DynamicData arr_member(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, arr_member, ma);

  DDS_UnsignedLong length = array_length(val);

  typename remove_all_extents<T>::type * arr =
    reinterpret_cast<typename remove_all_extents<T>::type *>(&val[0]);

  for(DDS_UnsignedLong i=0;i < length; ++i)
  {
    get_memberI_value(arr_member, MemberAccess::BY_ID(i+1), arr[i]);
  }
}

template <class T, size_t Dim>
void get_memberI_value(
  const DDS_DynamicData & instance,  
  const MemberAccess & ma,
  T(&val)[Dim],
  typename enable_if<is_primitive_or_enum<typename remove_all_extents<T>::type>::value>::type * = 0)
{
  DDS_UnsignedLong length = array_length(val);

  typename remove_all_extents<T>::type * arr =
    reinterpret_cast<typename remove_all_extents<T>::type *>(&val[0]);

  DDS_ReturnCode_t rc = get_array(instance, arr, &length, ma);
  if(rc != DDS_RETCODE_OK) {
     std::cerr << "get_array error = " << rc << std::endl;
     throw 0;
  }  
}

template <class T, size_t Dim>
void get_memberI_value(
  const DDS_DynamicData & instance,  
  const MemberAccess & ma,
  std::array<T, Dim> &val,
  typename disable_if<is_primitive_or_enum<typename remove_all_extents<T>::type>::value>::type * = 0)
{
  DDS_DynamicData arr_member(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, arr_member, ma);

  DDS_UnsignedLong length = array_length(val);

  typename remove_all_extents<T>::type * arr =
    reinterpret_cast<typename remove_all_extents<T>::type *>(&val[0]);

  for(DDS_UnsignedLong i=0;i < length; ++i)
  {
    get_memberI_value(arr_member, MemberAccess::BY_ID(i+1), arr[i]);
  }
}

template <class T, size_t Dim>
void get_memberI_value(
  const DDS_DynamicData & instance,  
  const MemberAccess & ma,
  std::array<T, Dim> & val,
  typename enable_if<is_primitive_or_enum<typename remove_all_extents<T>::type>::value>::type * = 0)
{
  DDS_UnsignedLong length = array_length(val);

  typename remove_all_extents<T>::type * arr =
    reinterpret_cast<typename remove_all_extents<T>::type *>(&val[0]);

  DDS_ReturnCode_t rc = get_array(instance, arr, &length, ma);
  if(rc != DDS_RETCODE_OK) {
     std::cerr << "get_array error = " << rc << std::endl;
     throw 0;
  }  
}

template <class TagType, class... Cases>
void get_memberI_value(const DDS_DynamicData & instance,  
                       const MemberAccess & ma,
                       Union<TagType, Cases...> & val)
{
  DDS_DynamicData inner(NULL, DDS_DYNAMIC_DATA_PROPERTY_DEFAULT); 
  SafeBinder binder(instance, inner, ma);

  typedef typename Union<TagType, Cases...>::case_tuple_type CaseTuple;
  typedef TypelistIterator<CaseTuple,
                           0,
                           Size<CaseTuple>::value-1> TIter;

 	DDS_DynamicDataMemberInfo member_info;
  inner.get_member_info_by_index(member_info, 0 /* discriminator id */);

  // member_info.member_id is a DDS_Long. It seems when the discriminator 
  // typecode is DDS_TK_SHORT or DDS_TK_CHAR, i.e., anything smaller than 
  // DDS_Long and it is default = -1, a conversion error occurs making 
  // the member_id = 255 or 65535 instead of -1. Hence the static_cast.

  int discriminator_value = static_cast<TagType>(member_info.member_id);

  TIter::get_union(inner, MemberAccess::BY_ID(), discriminator_value, val);
}

#ifdef RTI_WIN32
template <typename... T>
void get_memberI_value(const DDS_DynamicData & instance,  
                       const MemberAccess &ma, 
                       boost::optional<T...> & opt)
#else
template <typename T>
void get_memberI_value(const DDS_DynamicData & instance,
                       const MemberAccess &ma,
                       boost::optional<T> & opt)
#endif
{
  const char * member_name 
    = ma.access_by_id()? NULL : ma.get_name();

  int id = ma.access_by_id()? 
           ma.get_id() : DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED;

  if(instance.member_exists(member_name, id))
  {
#ifdef RTI_WIN32
    if(!opt.is_initialized())
       opt = boost::in_place<T...>();
#else
    if (!opt.is_initialized())
      opt = boost::in_place<T>();
#endif

    get_memberI_value(instance, ma, *opt.get_ptr());
  }
}

template <class T>
Range<typename T::reference> make_range(T &t)
{
  return Range<typename T::reference>(t);
}

template <class Iter>
Range<typename Iter::reference> make_range(Iter begin, Iter end)
{
  // iterators are always pass-by-value.
  return boost::make_iterator_range(begin, end);
}

template <size_t Bound, class Iter>
BoundedRange<typename Iter::reference, Bound> make_bounded_range(Iter begin, Iter end)
{
  // iterators are always pass-by-value.
  return boost::make_iterator_range(make_bounded_view_iterator<Bound>(begin), 
                                    make_bounded_view_iterator<Bound>(end));
}

template <size_t Bound, class T>
BoundedRange<typename T::reference, Bound> make_bounded_range(T &t)
{
  return make_bounded_range<Bound>(t.begin(), t.end());
}


#endif //  RTIREFLEX_DD_MANIP_H

