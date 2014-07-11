/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
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
#include <ndds/ndds_cpp.h>
#include "enable_if.h"
#include "safe_enum.h"
#include "disc_union.h" 
#include "bounded.h"
#include "common.h"

#define MAKE_SAFETYPECODE_MOVEONLY                        \
private:                                                  \
  SafeTypeCode & operator = (SafeTypeCode &);             \
  SafeTypeCode(SafeTypeCode &);                           \
                                                          \
public:                                                   \
  SafeTypeCode(detail::proxy<SafeTypeCode> p) throw()     \
    : detail::SafeTypeCodeBase(p.factory_,                \
                               p.typecode_,               \
                               p.release_)                \
  {}                                                      \
                                                          \
  SafeTypeCode & operator                                 \
     = (detail::proxy<SafeTypeCode> p) throw()            \
  {                                                       \
    SafeTypeCode(p).swap(*this);                          \
    return *this;                                         \
  }                                                       \
                                                          \
  operator detail::proxy<SafeTypeCode> () throw() {       \
    detail::proxy<SafeTypeCode>                           \
      p(factory_, typecode_, release_);                   \
    typecode_ = 0;                                        \
    return p;                                             \
  }                                                       \
                                                          \
  friend SafeTypeCode move(SafeTypeCode & stc) {          \
    return SafeTypeCode(detail::proxy<SafeTypeCode>(stc));\
  }                                               

#define GET_TYPECODE_DECL(BASIC_TYPE, TYPECODE)           \
const DDS_TypeCode * get_typecode(const BASIC_TYPE *);


namespace reflex {

  namespace detail {

    template <class SafeTypeCode>
    struct proxy 
    {
      DDS_TypeCodeFactory * factory_;
      DDS_TypeCode * typecode_;
      bool release_;

      proxy(
        DDS_TypeCodeFactory * factory,
        DDS_TypeCode * typecode,
        bool release)
         : factory_(factory),
           typecode_(typecode),
           release_(release)
      { }
    };

    class DllExport SafeTypeCodeBase
    {
      SafeTypeCodeBase & operator = (const SafeTypeCodeBase &);
      SafeTypeCodeBase(const SafeTypeCodeBase &);

    protected:
      DDS_TypeCodeFactory * factory_;
      DDS_TypeCode * typecode_;
      bool release_;

      void set_typecode(const DDS_TypeCode * tc);
      void set_typecode(DDS_TypeCode * tc);

      explicit SafeTypeCodeBase(DDS_TypeCode * typecode);
      explicit SafeTypeCodeBase(const DDS_TypeCode * typecode);
      explicit SafeTypeCodeBase(DDS_TypeCodeFactory * factory,
                                DDS_TypeCode * typecode = 0,
                                bool release = true);

      GET_TYPECODE_DECL(reflex::octet_t,    DDS_TK_OCTET)
      GET_TYPECODE_DECL(bool,               DDS_TK_BOOLEAN)
      GET_TYPECODE_DECL(char,               DDS_TK_CHAR)
      GET_TYPECODE_DECL(int8_t,             DDS_TK_CHAR)
      GET_TYPECODE_DECL(int16_t,            DDS_TK_SHORT)
      GET_TYPECODE_DECL(uint16_t,           DDS_TK_USHORT)
      GET_TYPECODE_DECL(int32_t,            DDS_TK_LONG)
      GET_TYPECODE_DECL(uint32_t,           DDS_TK_ULONG)
      GET_TYPECODE_DECL(int64_t,            DDS_TK_LONGLONG)
      GET_TYPECODE_DECL(uint64_t,           DDS_TK_ULONGLONG)

#ifndef RTI_WIN32
      GET_TYPECODE_DECL(char32_t,           DDS_TK_WCHAR)
#endif

      GET_TYPECODE_DECL(float,              DDS_TK_FLOAT)
      GET_TYPECODE_DECL(double,             DDS_TK_DOUBLE)
#if __x86_64__
      GET_TYPECODE_DECL(long long,          DDS_TK_LONGLONG)
      GET_TYPECODE_DECL(unsigned long long, DDS_TK_ULONGLONG)
#endif
      GET_TYPECODE_DECL(long double,        DDS_TK_LONGDOUBLE)

      void create_array_tc(DDS_TypeCode * inner,
      const DDS_UnsignedLongSeq &dims);

      void create_seq_tc(DDS_TypeCode *inner,
                         size_t bound);

      void create_string_tc(size_t bound);
      void create_struct_tc(const char * name);
      void create_value_tc(const char * name,
                           DDS_TypeCode * basetc);

      void create_enum_tc(
              const char * name,
              const DDS_EnumMemberSeq & enum_seq,
              const std::vector<MemberInfo> &);
      void create_union_tc(
             const char * name,
             DDS_TypeCode * discTc,
             DDS_UnsignedLong default_case,
             DDS_UnionMemberSeq & member_seq);

      void create_sparse_tc(const char * name);

      ~SafeTypeCodeBase();

    public:
      DDS_TypeCode * get() const;
      DDS_TypeCode * release();
      void swap(SafeTypeCodeBase & stc) throw();
    };

  } // namespace detail

  template <class T, class = void>
  class SafeTypeCode
    : public detail::SafeTypeCodeBase
  {
  public:
    SafeTypeCode(DDS_TypeCodeFactory * factory)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::set_typecode(
        detail::SafeTypeCodeBase::get_typecode(static_cast<T *>(0)));
    }

    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 const char * name)
      : detail::SafeTypeCodeBase(factory)
    {
        detail::SafeTypeCodeBase::create_struct_tc(name);
    }

    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 const char * name,
                 DDS_TypeCode * basetc)
      : detail::SafeTypeCodeBase(factory)
    {
        detail::SafeTypeCodeBase::create_value_tc(name, basetc);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <>
  class DllExport SafeTypeCode<DDS_TypeCode>
    : public detail::SafeTypeCodeBase
  {
  public:

    template <class T>
    SafeTypeCode(SafeTypeCode<T> stc)
      : detail::SafeTypeCodeBase(stc.factory_, stc.typecode_, stc.release_)
    {
      stc.release();
    }

    SafeTypeCode(DDS_TypeCode * tc);
    SafeTypeCode(const DDS_TypeCode * tc);

    // MAKE_SAFETYPECODE_MOVEONLY declarations below
  private:
    SafeTypeCode & operator = (SafeTypeCode &);
    SafeTypeCode(SafeTypeCode &);

  public:
    SafeTypeCode(detail::proxy<SafeTypeCode> p) throw();
    SafeTypeCode & operator = (detail::proxy<SafeTypeCode> p) throw();
    operator detail::proxy<SafeTypeCode>() throw();

    friend SafeTypeCode move(SafeTypeCode & stc);
  };

  template <class T>
  class SafeTypeCode<T, typename detail::enable_if<
                          std::is_enum<T>::value
                        >::type
                    >
    : public detail::SafeTypeCodeBase
  {
    template <size_t I, size_t MAX>
    struct SetEnumMembers
    {
      static void exec(DDS_EnumMemberSeq & enum_seq,
      std::vector<detail::MemberInfo> & info_seq)
      {
        using namespace detail;

        info_seq[I] = EnumDef<T>::template EnumMember<I>::info();
        enum_seq[I].name = const_cast<char *>(info_seq[I].name.c_str());
        enum_seq[I].ordinal = info_seq[I].value;
        SetEnumMembers<I + 1, MAX>::exec(enum_seq, info_seq);
      }
    };

    template <size_t MAX>
    struct SetEnumMembers<MAX, MAX>
    {
      static void exec(DDS_EnumMemberSeq & enum_seq,
      std::vector<detail::MemberInfo> & info_seq)
      {
        using namespace detail;

        info_seq[MAX] = EnumDef<T>::template EnumMember<MAX>::info();
        enum_seq[MAX].name = const_cast<char *>(info_seq[MAX].name.c_str());
        enum_seq[MAX].ordinal = info_seq[MAX].value;
      }
    };

  public:
    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 const char * name)
      : detail::SafeTypeCodeBase(factory)
    {
      // The lifetime of info_seq must be until create_enum_tc is invoked
      // because enum_seq[i].name is just a pointer to MemberInfo.name strings.
      using namespace detail;

      DDS_EnumMemberSeq enum_seq;
      std::vector<MemberInfo> info_seq(EnumDef<T>::size);
      enum_seq.ensure_length(EnumDef<T>::size, EnumDef<T>::size);
      SetEnumMembers<0, EnumDef<T>::size - 1>::exec(enum_seq, info_seq);
      SafeTypeCodeBase::create_enum_tc(EnumDef<T>::name(), enum_seq, info_seq);
    }

    MAKE_SAFETYPECODE_MOVEONLY
      friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <class C>
  class SafeTypeCode<C, 
                      typename detail::enable_if<
                        detail::is_container<C>::value
                      >::type
                    >
    : public detail::SafeTypeCodeBase
  {
  public:
    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 SafeTypeCode<typename C::value_type> & tc)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::create_seq_tc(tc.get(), MAX_SEQ_SIZE);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <class C, size_t Bound>
  class SafeTypeCode<Bounded<C, Bound>,
                     typename detail::enable_if<
                       detail::is_container<C>::value>::type
                    >
    : public detail::SafeTypeCodeBase
  {
  public:
    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 SafeTypeCode<typename C::value_type> & tc)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::create_seq_tc(tc.get(), Bound);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <class T>
  class SafeTypeCode<Range<T>> : public detail::SafeTypeCodeBase
  {
  public:

    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 SafeTypeCode<typename detail::remove_reference<T>::type> & tc)
      : detail::SafeTypeCodeBase(factory)
    {
        detail::SafeTypeCodeBase::create_seq_tc(tc.get(), MAX_SEQ_SIZE);
      }

    MAKE_SAFETYPECODE_MOVEONLY
      friend class SafeTypeCode<DDS_TypeCode>;
  };

#ifdef RTI_WIN32
  template <class... T>
  class SafeTypeCode<boost::optional<T...>> : public detail::SafeTypeCodeBase
  {
     typedef typename detail::PackHead<T...>::type HeadT;
#else
  template <class T>
  class SafeTypeCode<boost::optional<T>> : public detail::SafeTypeCodeBase
  {
    typedef T HeadT;
#endif 
    SafeTypeCode<HeadT> innerTc;

  private:
    SafeTypeCode & operator = (SafeTypeCode &);
    SafeTypeCode(SafeTypeCode &);

  public:

    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 SafeTypeCode<HeadT> innertc)
      : detail::SafeTypeCodeBase(factory),
        innerTc(move(innertc))
    {  }

    SafeTypeCode(detail::proxy<SafeTypeCode> p) throw()
      : detail::SafeTypeCodeBase(p.factory_,
                                 p.typecode_,
                                 p.release_),
        innerTc(detail::proxy<SafeTypeCode<HeadT>>
               (p.factory_, p.typecode_, p.release_))
    {}

    operator detail::proxy<SafeTypeCode>() throw() 
    { 
      detail::proxy<SafeTypeCode<HeadT>> inner_proxy = innerTc;
      detail::proxy<SafeTypeCode> proxy(
        inner_proxy.factory_,
        inner_proxy.typecode_,
        inner_proxy.release_);
      return proxy;
    }

  public:
    DDS_TypeCode * get() const {
      return innerTc.get();
    }

    DDS_TypeCode * release() {
      return innerTc.release();
    }

    void swap(SafeTypeCode & stc) throw() {
      this->innerTc.swap(stc.innerTc);
    }

    SafeTypeCode & operator
      = (detail::proxy<SafeTypeCode> p) throw()
    {
      SafeTypeCode(p).swap(*this);
      return *this;
    }

    friend SafeTypeCode move(SafeTypeCode & stc) 
    {
      return SafeTypeCode(detail::proxy<SafeTypeCode>(stc));
    }

    friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <class T, size_t Bound>
  class SafeTypeCode<BoundedRange<T, Bound>> : public detail::SafeTypeCodeBase
  {
  public:

    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 SafeTypeCode<typename detail::remove_reference<T>::type> & tc)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::create_seq_tc(tc.get(), Bound);
    }

    MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <>
  class SafeTypeCode<std::string> : public detail::SafeTypeCodeBase
  {
  public:
    SafeTypeCode(DDS_TypeCodeFactory * factory)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::create_string_tc(MAX_STRING_SIZE);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <size_t Bound>
  class SafeTypeCode<Bounded<std::string, Bound>> : public detail::SafeTypeCodeBase
  {
  public:
    SafeTypeCode(DDS_TypeCodeFactory * factory)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::create_string_tc(Bound);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <class... Args>
  class SafeTypeCode<std::tuple<Args...>> : public detail::SafeTypeCodeBase
  {
  public:

    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 const char * name)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::create_struct_tc(name);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <class... Args>
  class SafeTypeCode<Sparse<Args...>> : public detail::SafeTypeCodeBase
  {
  public:

    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 const char * name)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::create_sparse_tc(name);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  namespace detail {

    template <size_t I, class DimList>
    struct CopyDims;

    template <size_t I, size_t Head, size_t... Tail>
    struct CopyDims <I, dim_list<Head, Tail...>>
    {
      static void exec(DDS_UnsignedLongSeq & seq)
      {
        seq[I] = Head;
        detail::CopyDims<I + 1, dim_list<Tail...>>::exec(seq);
      }
    };

    template <size_t I, size_t Last>
    struct CopyDims<I, dim_list<Last>>
    {
      static void exec(DDS_UnsignedLongSeq & seq)
      {
        seq[I] = Last;
      }
    };

  } // namespace detail

  template <class T, size_t... Dims>
  class SafeTypeCode<T, detail::dim_list<Dims...>>
    : public detail::SafeTypeCodeBase
  {
  public:
    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 SafeTypeCode<T> & tc)
      : detail::SafeTypeCodeBase(factory)
    {
      DDS_UnsignedLongSeq seq;
      seq.ensure_length(sizeof...(Dims), sizeof...(Dims));
      detail::CopyDims<0, detail::dim_list<Dims...>>::exec(seq);

      detail::SafeTypeCodeBase::create_array_tc(tc.get(), seq);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  template <class T, size_t Dim>
  class SafeTypeCode<std::array<T, Dim>>
    : public detail::SafeTypeCodeBase
  {
  public:
    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 SafeTypeCode<typename 
                              detail::remove_all_extents<T>::type> & tc)
      : detail::SafeTypeCodeBase(factory)
    {
      DDS_UnsignedLongSeq seq;
      typedef typename 
        detail::make_dim_list<std::array<T, Dim>>::type DimList;
      seq.ensure_length(DimList::size, DimList::size);
      detail::CopyDims<0, DimList>::exec(seq);

      detail::SafeTypeCodeBase::create_array_tc(tc.get(), seq);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

  namespace detail {

    template <class Tuple, size_t I, size_t MAX_INDEX, class Case>
    struct MatchDefaultCase;

    template <class Tuple, size_t I, size_t MAX_INDEX, class T, int Tag, int... Tags>
    struct MatchDefaultCase<Tuple, I, MAX_INDEX, Case<T, Tag, Tags...>>
    {
      enum {
        value =
          detail::MatchDefaultCase<Tuple,
                                   I + 1,
                                   MAX_INDEX,
                                   typename std::tuple_element<I + 1, Tuple>::type
                                  >::value
      };
    };

    template <class Tuple, size_t MAX_INDEX, class T, int Tag, int... Tags>
    struct MatchDefaultCase<Tuple, MAX_INDEX, MAX_INDEX, Case<T, Tag, Tags...>>
    {
      enum { value = -1 };
    };

    template <class Tuple, size_t I, size_t MAX_INDEX, class T>
    struct MatchDefaultCase<Tuple, I, MAX_INDEX, Case<T>>
    {
      enum { value = I };
    };

    template <class Tuple, size_t MAX_INDEX, class T>
    struct MatchDefaultCase<Tuple, MAX_INDEX, MAX_INDEX, Case<T>>
    {
      enum { value = MAX_INDEX };
    };

    template <class TagType, class... Cases>
    struct DefaultCaseIndex;

    template <class TagType, class... Cases>
    struct DefaultCaseIndex<Union<TagType, Cases...>>
    {
      typedef typename Union<TagType, Cases...>::case_tuple_type CaseTuple;
      enum {
        value =
         MatchDefaultCase<CaseTuple,
                          1,
                          Size<CaseTuple>::value - 1,
                          typename std::tuple_element<1, CaseTuple>::type>::value
      };
    };

  } // namespace detail

  template <class TagType, class... Cases>
  struct SafeTypeCode<Union<TagType, Cases...>>
    : public detail::SafeTypeCodeBase
  {
    SafeTypeCode(DDS_TypeCodeFactory * factory,
                 const char * name,
                 SafeTypeCode<TagType> & discTc,
                 DDS_UnionMemberSeq & member_seq)
      : detail::SafeTypeCodeBase(factory)
    {
      detail::SafeTypeCodeBase::create_union_tc(
          name,
          discTc.get(),
          detail::DefaultCaseIndex<Union<TagType, Cases...>>::value,
          member_seq);
    }

    MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
  };

} // namespace reflex

#undef GET_TYPECODE_DECL
#undef MAKE_THIS_CLASS_MOVEONLY

#endif // RTIREFLEX_SAFE_TYPECODE_H

