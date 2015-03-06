/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_BOUNDED_H
#define RTIREFLEX_BOUNDED_H

#include <vector>
#include <string>

namespace reflex {

  template <class T, size_t Bound>
  struct BoundedRange;

  template <typename T, size_t Bound>
  class Bounded
  {
    T * ptr;

  public:
    Bounded() : ptr(0) {}

    explicit Bounded(T & t)
      : ptr(&t)
    {}

    Bounded(const Bounded & b)
      : ptr(b.ptr)
    {}

    operator T & () const
    {
      if (ptr)
        return *ptr;
      else
        throw std::runtime_error("Bounded<T>: Null pointer");
    }

    Bounded & operator = (T &t)
    {
      ptr = &t;
      return *this;
    }
  };

  template <class Iter, size_t Bound>
  class BoundedViewIterator : public Iter
  {
  public:
    BoundedViewIterator()
      : Iter()
    {}

    explicit BoundedViewIterator(Iter i)
      : Iter(i)
    {}

    BoundedViewIterator(const BoundedViewIterator & bvi)
      : Iter(bvi)
    {}
  };

  template <size_t Bound, class Iter>
  BoundedViewIterator<Iter, Bound> make_bounded_view_iterator(Iter iter)
  {
    return BoundedViewIterator<Iter, Bound>(iter);
  }

} // namespace reflex

#endif // RTIREFLEX_BOUNDED_H

