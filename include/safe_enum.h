/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_SAFE_ENUM_H
#define RTIREFLEX_SAFE_ENUM_H

namespace reflex {

  template<typename def, typename inner = typename def::type>
  class safe_enum : public def
  {
    typedef inner type;
    inner val;

  public:

    explicit safe_enum(type v) : val(v) {}
    inner underlying() const { return val; }

    friend bool operator == (
      const safe_enum & lhs, 
      const safe_enum & rhs) 
    { return lhs.val == rhs.val; }

    friend bool operator != (
      const safe_enum & lhs, 
      const safe_enum & rhs) 
    { return lhs.val != rhs.val; }
    
    friend bool operator <  (
      const safe_enum & lhs, 
      const safe_enum & rhs) 
    { return lhs.val <  rhs.val; }
    
    friend bool operator <= (
      const safe_enum & lhs, 
      const safe_enum & rhs) 
    { return lhs.val <= rhs.val; }
    
    friend bool operator >(
      const safe_enum & lhs, 
      const safe_enum & rhs) 
    { return lhs.val >  rhs.val; }
    
    friend bool operator >= (
      const safe_enum & lhs, 
      const safe_enum & rhs) 
    { return lhs.val >= rhs.val; }

  };

} // namespace reflex

#endif // RTIREFLEX_SAFE_ENUM_H

