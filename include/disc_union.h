/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_DISC_UNION_H
#define RTIREFLEX_DISC_UNION_H

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/utility/typed_in_place_factory.hpp> 
#include <tuple>
#include <typeinfo>
#include "tuple_iterator.h"
#include "default_member_names.h"

template <class... T>
struct Sparse
{
  typedef std::tuple<boost::optional<T>...> tuple_type;
  typedef std::tuple<T...> raw_tuple_type;

  const tuple_type & get_opt_tuple() const {
    return val_;
  }

  tuple_type & get_opt_tuple() {
    return val_;
  }

  friend bool operator == (const Sparse & lhs, const Sparse & rhs) {
    return lhs.val_ == rhs.val_;
  }

private:
  tuple_type val_;
};

template <int... N>
struct Match;

template <int Head, int... Tail> 
struct Match<Head, Tail...> 
{
  static bool exec(int discriminator) {
    return (Head == discriminator)? 
             true : Match<Tail...>::exec(discriminator);
  } 
};

template <int Last> 
struct Match<Last> 
{
  static bool exec(int discriminator) {
    return (Last == discriminator);
  } 
};

template <> 
struct Match<> 
{
  static bool exec(int discriminator) {
    return (-1 == discriminator);
  } 
};

template <class Case>
struct case_discriminator;

template <class T, int... Tags>
class Case;

template <class T, int Head, int... Tail>
struct case_discriminator<Case<T, Head, Tail...>>
{
  // Take the first value out of many possible values
  enum { value = Head };
};

template <class T>
struct case_discriminator<Case<T>>
{
  enum { value = -1 };
};

template <class T, int... Tags>
class Case 
{
  T val_;

public:
  typedef T * case_ptr_type;

  typedef T type;
  enum { discriminator = case_discriminator<Case>::value };
  
  static bool matches(int discriminator) 
  {
    return Match<Tags...>::exec(discriminator);
  }

  Case() : val_() {}

  explicit Case(const T &t) 
    : val_(t)
  {}

  Case(const Case &c)
    : val_(c.val_)
  {}

  Case & operator = (const Case & rhs) {
    val_ = rhs.val_;
    return *this;
  }

  const T & get() const { return val_; }
  T & get() { return val_; }

  friend bool operator == (const Case & lhs, const Case & rhs) {
    return lhs.val_ == rhs.val_;
  }
};

template <class TagType, class... Cases>
struct Union
{
  typedef void * Blank;
  typedef TagType tag_type;
  typedef std::tuple<Cases...> case_tuple_type;
  typedef boost::variant<Blank, Cases...> variant_type;
  typedef std::tuple<typename Cases::case_ptr_type...> caseptr_tuple_type;
  
  Union() 
   : var_(), 
     caseptr_tuple_(), 
     active_index_(-1) 
  {}

  Union(const Union & other)
   : var_(other.var_),
     caseptr_tuple_(other.caseptr_tuple_),
     active_index_(other.active_index_)
  {}
  
  Union & operator=(const Union & rhs)
  {
    Union(rhs).swap(*this);
    return *this;
  }

  Union(const variant_type &v)
   : var_(v) 
  {}
  
  Union & operator=(const variant_type &lhs)
  {
    var_ = lhs;
    return *this;
  }

  template <typename CaseType, int... Tags> 
  Union (const Case<CaseType, Tags...> &c)
    : var_(c)
  {}

  template <typename CaseType, int... Tags> 
  Union & operator=(const Case<CaseType, Tags...> &rhs)
  {
    Union(rhs).swap(*this);
    return *this;
  }
        
  template <class Tag, class... C>
  Union(const Union<Tag, C...> & u)
    : var_(u)
  {}

  template <typename Tag, class... C> 
  Union & operator=(const Union<Tag, C...> &rhs)
  {
    Union(rhs).swap(*this);
    return *this;
  }
  
  variant_type & get_variant() 
  {
    return var_;
  }

  const variant_type & get_variant() const
  {
    return var_;
  }

  void set_active_index(int i) 
  {
    if(i == -1) 
      reset_caseptr_tuple();
    else {
      active_index_ = i;
    }
  }

  void reset_caseptr_tuple() 
  {
    active_index_ = -1;
  }

  void set_caseptr_tuple_for_writing(const caseptr_tuple_type & caseptrs, int ai) 
  {
    caseptr_tuple_ = caseptrs;
    set_active_index(ai);
  }

  void set_caseptr_tuple_for_reading(const caseptr_tuple_type & caseptrs) 
  {
    caseptr_tuple_ = caseptrs;
  }

  const caseptr_tuple_type & get_caseptr_tuple() const
  {
    return caseptr_tuple_;
  }

  caseptr_tuple_type & get_caseptr_tuple() 
  {
    return caseptr_tuple_;
  }

  void swap(Union & other)
  {
    var_.swap(other.var_);
    caseptr_tuple_.swap(other.caseptr_tuple_);
    std::swap(active_index_,other.active_index_);
  }

  int get_active_index() const
  {
    return active_index_;
  }

  bool empty() const 
  {
    return (active_index_ == -1) &&
           (var_.which()  == 0);
  }

  const std::type_info & type() const
  {
    return var_.type();
  }

  bool operator==(const Union &rhs) const
  {
    return var_ == rhs.var_;
  } 
 
  template<typename U> 
  bool operator==(const U &rhs) const
  {
    return var_ == rhs;
  }

  bool operator<(const Union &rhs) const
  { 
    return var_ < rhs.var_;
  }
 
  template<typename U> 
  bool operator<(const U &rhs) const
  {
    return var_ < rhs;
  }

private:
  variant_type var_;
  caseptr_tuple_type caseptr_tuple_;
  int active_index_;
};

template <class... T>
std::tuple<T*...> make_caseptr_tuple(T&... args)
{
  return make_tuple((&args)...);
}

#endif // RTIREFLEX_DISC_UNION_H

