/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided "as is", with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTI_REFLEX_TYPE_LIBRARY_H
#define RTI_REFLEX_TYPE_LIBRARY_H

#include <typeindex>
#include <unordered_map>
#include <memory>
#include <vector>

#include "reflex/reflex_fwd.h"

#include <ndds/ndds_cpp.h>

namespace reflex
{
  namespace detail
  {
    class TypeInfoContainerBase
    {
    public:
      virtual ~TypeInfoContainerBase();
    };

    template <class T>
    class TypeInfoContainer : public TypeInfoContainerBase
    {
      SafeTypeCode<T> _typecode;
      DDSDynamicDataTypeSupport * _type_support;
      bool _own_type_support;

    public:
      TypeInfoContainer(SafeTypeCode<T> tc,
                        DDSDynamicDataTypeSupport * support,
                        bool own)
        : _typecode(std::move(tc)),
          _type_support(support),
          _own_type_support(own)
      { }

      const SafeTypeCode<T> & get_safetypecode() const
      {
        return _typecode;
      }

      const DDSDynamicDataTypeSupport * get_type_support() const
      {
        return _type_support;
      }

      TypeInfoContainer & set_safetypecode(const SafeTypeCode<T> & tc)
      {
        _typecode = tc;
        return *this;
      }

      TypeInfoContainer & set_type_support(DDSDynamicDataTypeSupport * support,
                                           bool own)
      {
        if (_own_type_support)
          delete _type_support;

        _type_support = support;
        _own_type_support = own;
        
        return *this;
      }

      ~TypeInfoContainer()
      {
        if (_own_type_support)
          delete _type_support;
      }

    };
  } // namespace detail

  class TypeLibrary
  {
    typedef
      std::unordered_map < std::type_index, std::shared_ptr<detail::TypeInfoContainerBase> >
        TypeMap;

    DDS_DynamicDataTypeProperty_t _default_props;
    TypeMap _typemap;
    std::vector<TypeLibrary> _nested;

  public:

    explicit TypeLibrary(const DDS_DynamicDataTypeProperty_t & props);

    template <class T>
    TypeLibrary & put(DDSDynamicDataTypeSupport * support)
    {
      _typemap[typeid(T)] =
        std::make_shared<detail::TypeInfoContainer<T>>(reflex::make_typecode<T>(), support, false);

      return *this;
    }

    template <class T>
    TypeLibrary & put()
    {
      reflex::make_typecode<T> stc(reflex::make_typecode<T>());

      _typemap[typeid(T)] =
        std::make_shared<detail::TypeInfoContainer<T>>(
          detail::TypeInfoContainer<T>(std::move(stc),
                                       new DDSDynamicDataTypeSupport(stc.get(), _default_props),
                                       true));

      return *this;
    }

    template <class T>
    const detail::TypeInfoContainer<T> & get() const
    {
      TypeMap::const_iterator got = _typemap.find(typeid(T));
      
      if (got == _typemap.end())
        throw std::runtime_error("Non-existent type");
      else
        return *dynamic_cast<detail::TypeInfoContainer<T> *>(got->second.get());
    }

    std::vector<TypeLibrary> & nested() 
    {
      return _nested;
    }
  };

  class GlobalTypeLibrary
  {
  public:
    static TypeLibrary & instance(
      const DDS_DynamicDataTypeProperty_t & props
        = DDS_DynamicDataTypeProperty_t());
  };

} // namespace reflex


#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/type_library.cxx"
#endif


#endif // RTI_REFLEX_TYPE_LIBRARY_H
