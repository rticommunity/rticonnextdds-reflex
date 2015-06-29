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
//#include "reflex/enable_if.h"
//#include "reflex/disc_union.h" 
//#include "reflex/bounded.h"
/*
#define MAKE_SAFETYPECODE_MOVEONLY                        \
private:                                                  \
  InnerTypeCode & operator = (InnerTypeCode &);             \
  InnerTypeCode(InnerTypeCode &);                           \
                                                          \
public:                                                   \
  InnerTypeCode(detail::proxy<InnerTypeCode> p) throw()     \
    : InnerTypeCodeBase(p.factory_,                \
                        p.typecode_,               \
                        p.release_)                \
  {}                                                      \
                                                          \
  InnerTypeCode & operator                                 \
     = (proxy<InnerTypeCode> p) throw()            \
  {                                                       \
    InnerTypeCode(p).swap(*this);                          \
    return *this;                                         \
  }                                                       \
                                                          \
  operator proxy<InnerTypeCode> () throw() {       \
    proxy<InnerTypeCode>                           \
      p(factory_, typecode_, release_);                   \
    typecode_ = 0;                                        \
    return p;                                             \
  }                                                       \
                                                          \
  friend InnerTypeCode move(InnerTypeCode & stc) {          \
    return InnerTypeCode(proxy<InnerTypeCode>(stc));\
  }                                               

#define GET_TYPECODE_DECL(BASIC_TYPE, TYPECODE)           \
const DDS_TypeCode * get_typecode(const BASIC_TYPE *);


namespace reflex {

  namespace detail {

    template <class InnerTypeCode>
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

    class REFLEX_DLL_EXPORT InnerTypeCodeBase
    {
      InnerTypeCodeBase & operator = (const InnerTypeCodeBase &);
      InnerTypeCodeBase(const InnerTypeCodeBase &);

    protected:
      DDS_TypeCodeFactory * factory_;
      DDS_TypeCode * typecode_;
      bool release_;

      void set_typecode(const DDS_TypeCode * tc);
      void set_typecode(DDS_TypeCode * tc);

      explicit InnerTypeCodeBase(DDS_TypeCode * typecode);
      explicit InnerTypeCodeBase(const DDS_TypeCode * typecode);
      explicit InnerTypeCodeBase(DDS_TypeCodeFactory * factory,
        DDS_TypeCode * typecode = 0,
        bool release = true);

        GET_TYPECODE_DECL(match::octet_t, DDS_TK_OCTET)
        GET_TYPECODE_DECL(bool, DDS_TK_BOOLEAN)
        GET_TYPECODE_DECL(char, DDS_TK_CHAR)
        GET_TYPECODE_DECL(int8_t, DDS_TK_CHAR)
        GET_TYPECODE_DECL(int16_t, DDS_TK_SHORT)
        GET_TYPECODE_DECL(uint16_t, DDS_TK_USHORT)
        GET_TYPECODE_DECL(int32_t, DDS_TK_LONG)
        GET_TYPECODE_DECL(uint32_t, DDS_TK_ULONG)
        GET_TYPECODE_DECL(int64_t, DDS_TK_LONGLONG)
        GET_TYPECODE_DECL(uint64_t, DDS_TK_ULONGLONG)

#ifndef RTI_WIN32
        GET_TYPECODE_DECL(char32_t,           DDS_TK_WCHAR)
#endif

        GET_TYPECODE_DECL(float, DDS_TK_FLOAT)
        GET_TYPECODE_DECL(double, DDS_TK_DOUBLE)
#if __x86_64__
        GET_TYPECODE_DECL(long long,          DDS_TK_LONGLONG)
        GET_TYPECODE_DECL(unsigned long long, DDS_TK_ULONGLONG)
#endif
        GET_TYPECODE_DECL(long double, DDS_TK_LONGDOUBLE)

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

      ~InnerTypeCodeBase();

    public:
      DDS_TypeCode * get();
      const DDS_TypeCode * get() const;
      DDS_TypeCode * release();
      void swap(InnerTypeCodeBase & stc) throw();
    };

    template <class T, class = void>
    class InnerTypeCode
      : public detail::InnerTypeCodeBase
    {
    public:
      InnerTypeCode(DDS_TypeCodeFactory * factory)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::set_typecode(
            detail::InnerTypeCodeBase::get_typecode(static_cast<T *>(0)));
        }

      InnerTypeCode(DDS_TypeCodeFactory * factory,
        const char * name)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_struct_tc(name);
        }

      InnerTypeCode(DDS_TypeCodeFactory * factory,
        const char * name,
        DDS_TypeCode * basetc)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_value_tc(name, basetc);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <>
    class REFLEX_DLL_EXPORT InnerTypeCode<DDS_TypeCode>
      : public InnerTypeCodeBase
    {
    public:

      template <class T>
      InnerTypeCode(InnerTypeCode<T> stc)
        : detail::InnerTypeCodeBase(stc.factory_, stc.typecode_, stc.release_)
      {
          stc.release();
        }

      InnerTypeCode(DDS_TypeCode * tc);
      InnerTypeCode(const DDS_TypeCode * tc);

      // MAKE_SAFETYPECODE_MOVEONLY declarations below
    private:
      InnerTypeCode & operator = (InnerTypeCode &);
      InnerTypeCode(InnerTypeCode &);

    public:
      InnerTypeCode(proxy<InnerTypeCode> p) throw();
      InnerTypeCode & operator = (proxy<InnerTypeCode> p) throw();
      operator proxy<InnerTypeCode>() throw();

      friend InnerTypeCode move(InnerTypeCode & stc);
    };

    template <class T>
    class InnerTypeCode<T, typename detail::enable_if<
      detail::is_enum<T>::value
    >::type
    >
    : public detail::InnerTypeCodeBase
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
      InnerTypeCode(DDS_TypeCodeFactory * factory,
        const char * name)
        : detail::InnerTypeCodeBase(factory)
      {
          // The lifetime of info_seq must be until create_enum_tc is invoked
          // because enum_seq[i].name is just a pointer to MemberInfo.name strings.
          using namespace detail;

          DDS_EnumMemberSeq enum_seq;
          std::vector<MemberInfo> info_seq(EnumDef<T>::size);
          enum_seq.ensure_length(EnumDef<T>::size, EnumDef<T>::size);
          SetEnumMembers<0, EnumDef<T>::size - 1>::exec(enum_seq, info_seq);
          InnerTypeCodeBase::create_enum_tc(EnumDef<T>::name(), enum_seq, info_seq);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class C>
    class InnerTypeCode<C,
      typename detail::enable_if<
      detail::is_container<C>::value
      >::type
    >
    : public detail::InnerTypeCodeBase
    {
    public:
      InnerTypeCode(DDS_TypeCodeFactory * factory,
        InnerTypeCode<typename detail::container_traits<C>::value_type> & tc)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_seq_tc(
            tc.get(),
            detail::static_container_bound<C>::value);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class C, size_t Bound>
    class InnerTypeCode<match::Bounded<C, Bound>,
      typename detail::enable_if<
      detail::is_container<C>::value>::type
    >
    : public detail::InnerTypeCodeBase
    {
    public:
      InnerTypeCode(DDS_TypeCodeFactory * factory,
        InnerTypeCode<typename detail::container_traits<C>::value_type> & tc)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_seq_tc(tc.get(), Bound);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class T>
    class InnerTypeCode<match::Range<T>> : public detail::InnerTypeCodeBase
    {
    public:

      InnerTypeCode(DDS_TypeCodeFactory * factory,
        InnerTypeCode<typename detail::remove_reference<T>::type> & tc)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_seq_tc(
            tc.get(),
            detail::static_container_bound<match::Range<T>>::value);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class Opt>
    class InnerTypeCode<Opt,
      typename detail::enable_if<
      detail::is_optional<Opt>::value >::type
    >
    : public detail::InnerTypeCodeBase
    {
      static Opt * opt;
      typedef
        typename detail::remove_const<
        typename detail::remove_reference<decltype(**opt)
        >::type
        >::type
        HeadT;

      InnerTypeCode<HeadT> innerTc;

    private:
      InnerTypeCode & operator = (InnerTypeCode &);
      InnerTypeCode(InnerTypeCode &);

    public:

      InnerTypeCode(DDS_TypeCodeFactory * factory,
        InnerTypeCode<HeadT> tc)
        : detail::InnerTypeCodeBase(factory, 0, false),
        innerTc(move(tc))
      { }

      InnerTypeCode(detail::proxy<InnerTypeCode> p) throw()
        : detail::InnerTypeCodeBase(p.factory_,
        0,
        false),
        innerTc(detail::proxy<InnerTypeCode<HeadT>>
        (p.factory_, p.typecode_, p.release_))
      {}

      operator detail::proxy<InnerTypeCode>() throw()
      {
        detail::proxy<InnerTypeCode<HeadT>> inner_proxy = innerTc;
        detail::proxy<InnerTypeCode> proxy(
          inner_proxy.factory_,
          inner_proxy.typecode_,
          inner_proxy.release_);
        return proxy;
      }

    public:
      const DDS_TypeCode * get() const {
        return innerTc.get();
      }

      DDS_TypeCode * get() {
        return innerTc.get();
      }

      DDS_TypeCode * release() {
        return innerTc.release();
      }

      void swap(InnerTypeCode & stc) throw() {
        this->innerTc.swap(stc.innerTc);
      }

      InnerTypeCode & operator
        = (detail::proxy<InnerTypeCode> p) throw()
      {
        InnerTypeCode(p).swap(*this);
        return *this;
      }

      friend InnerTypeCode move(InnerTypeCode & stc)
      {
        return InnerTypeCode(detail::proxy<InnerTypeCode>(stc));
      }

      friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class T, size_t Bound>
    class InnerTypeCode<match::BoundedRange<T, Bound>> : public detail::InnerTypeCodeBase
    {
    public:

      InnerTypeCode(DDS_TypeCodeFactory * factory,
        InnerTypeCode<typename detail::remove_reference<T>::type> & tc)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_seq_tc(tc.get(), Bound);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class Str>
    class InnerTypeCode<Str,
      typename detail::enable_if<
      detail::is_string<Str>::value >::type
    >
    : public detail::InnerTypeCodeBase
    {
    public:
      InnerTypeCode(DDS_TypeCodeFactory * factory)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_string_tc(
            detail::static_string_bound<Str>::value);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <size_t Bound>
    class InnerTypeCode<match::Bounded<std::string, Bound>> : public detail::InnerTypeCodeBase
    {
    public:
      InnerTypeCode(DDS_TypeCodeFactory * factory)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_string_tc(Bound);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class... Args>
    class InnerTypeCode<std::tuple<Args...>> : public detail::InnerTypeCodeBase
    {
    public:

      InnerTypeCode(DDS_TypeCodeFactory * factory,
        const char * name)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_struct_tc(name);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class... Args>
    class InnerTypeCode<match::Sparse<Args...>> : public detail::InnerTypeCodeBase
    {
    public:

      InnerTypeCode(DDS_TypeCodeFactory * factory,
        const char * name)
        : detail::InnerTypeCodeBase(factory)
      {
          detail::InnerTypeCodeBase::create_sparse_tc(name);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
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
    class InnerTypeCode<T, detail::dim_list<Dims...>>
      : public detail::InnerTypeCodeBase
    {
    public:
      InnerTypeCode(DDS_TypeCodeFactory * factory,
        InnerTypeCode<T> & tc)
        : detail::InnerTypeCodeBase(factory)
      {
          DDS_UnsignedLongSeq seq;
          seq.ensure_length(sizeof...(Dims), sizeof...(Dims));
          detail::CopyDims<0, detail::dim_list<Dims...>>::exec(seq);

          detail::InnerTypeCodeBase::create_array_tc(tc.get(), seq);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    template <class T, size_t Dim>
    class InnerTypeCode<std::array<T, Dim>>
      : public detail::InnerTypeCodeBase
    {
    public:
      InnerTypeCode(DDS_TypeCodeFactory * factory,
        InnerTypeCode<typename
        detail::remove_all_extents<T>::type> & tc)
        : detail::InnerTypeCodeBase(factory)
      {
          DDS_UnsignedLongSeq seq;
          typedef typename
            detail::make_dim_list<std::array<T, Dim>>::type DimList;
          seq.ensure_length(DimList::size, DimList::size);
          detail::CopyDims<0, DimList>::exec(seq);

          detail::InnerTypeCodeBase::create_array_tc(tc.get(), seq);
        }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

    namespace detail {

      template <class Tuple, size_t I, size_t MAX_INDEX, class Case>
      struct MatchDefaultCase;

      template <class Tuple, size_t I, size_t MAX_INDEX, class T, int Tag, int... Tags>
      struct MatchDefaultCase<Tuple, I, MAX_INDEX, match::Case<T, Tag, Tags...>>
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
      struct MatchDefaultCase<Tuple, MAX_INDEX, MAX_INDEX, match::Case<T, Tag, Tags...>>
      {
        enum { value = -1 };
      };

      template <class Tuple, size_t I, size_t MAX_INDEX, class T>
      struct MatchDefaultCase<Tuple, I, MAX_INDEX, match::Case<T>>
      {
        enum { value = I };
      };

      template <class Tuple, size_t MAX_INDEX, class T>
      struct MatchDefaultCase<Tuple, MAX_INDEX, MAX_INDEX, match::Case<T>>
      {
        enum { value = MAX_INDEX };
      };

      template <class TagType, class... Cases>
      struct DefaultCaseIndex;

      template <class TagType, class... Cases>
      struct DefaultCaseIndex<match::Union<TagType, Cases...>>
      {
        typedef typename match::Union<TagType, Cases...>::case_tuple_type CaseTuple;
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
    struct InnerTypeCode<match::Union<TagType, Cases...>>
      : public detail::InnerTypeCodeBase
    {
      InnerTypeCode(DDS_TypeCodeFactory * factory,
      const char * name,
      InnerTypeCode<TagType> & discTc,
      DDS_UnionMemberSeq & member_seq)
      : detail::InnerTypeCodeBase(factory)
      {
        detail::InnerTypeCodeBase::create_union_tc(
          name,
          discTc.get(),
          detail::DefaultCaseIndex<match::Union<TagType, Cases...>>::value,
          member_seq);
      }

      MAKE_SAFETYPECODE_MOVEONLY
        friend class InnerTypeCode<DDS_TypeCode>;
    };

  } // namespace detail
} // namespace reflex

#undef GET_TYPECODE_DECL
#undef MAKE_SAFETYPECODE_MOVEONLY

#ifndef REFLEX_NO_HEADER_ONLY
#include "reflex/../../src/safe_typecode.cxx"
#endif
*/
namespace reflex {

  namespace detail {
      REFLEX_DLL_EXPORT const char *get_readable_ex_code(DDS_ExceptionCode_t ex);
      REFLEX_DLL_EXPORT void check_exception_code(const char * message, DDS_ExceptionCode_t ex);
  }

  template <class T>
  class SafeTypeCode
  {
    DDS_TypeCodeFactory * factory_;
    DDS_TypeCode * typecode_;
    bool is_primitive_;

  public:
    explicit SafeTypeCode(DDS_TypeCode * tc)
      : factory_(DDS_TypeCodeFactory::get_instance()),
        typecode_(tc),
        is_primitive_(false)
    { }

    SafeTypeCode(DDS_TypeCodeFactory * fact,
                 DDS_TypeCode * tc)
      : factory_(fact),
        typecode_(tc),
        is_primitive_(false)
    { }

    SafeTypeCode(DDS_TypeCodeFactory * fact,
                 const DDS_TypeCode * tc)
      : factory_(fact),
        typecode_(const_cast<DDS_TypeCode *>(tc)),
        is_primitive_(true)
    { }

    SafeTypeCode(const SafeTypeCode & safetc)
      : factory_(safetc.factory_)
    {
      if (safetc.is_primitive_)
      {
        typecode_ = safetc.typecode_;
        is_primitive_ = true;
      }
      else
      {
        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
        typecode_ = factory_->clone_tc(safetc.typecode_, ex);
        is_primitive_ = false;
        detail::check_exception_code(
          "SafeTypeCode::SafeTypeCode: Unable to clone typecode, error = ",
          ex);
      }
    }

    SafeTypeCode & operator = (const SafeTypeCode & rhs)
    {
      SafeTypeCode(rhs).swap(*this);
      return *this;
    }

#ifdef REFLEX_HAS_RVALUE_REF

    SafeTypeCode(SafeTypeCode && stc)
      : factory_(stc.factory_),
        typecode_(stc.typecode_),
        is_primitive_(stc.is_primitive_)
    {
      stc.factory_ = 0;
      stc.typecode_ = 0;
    }
   
    SafeTypeCode & operator = (SafeTypeCode && rhs)
    {
      this->swap(rhs);
      return *this;
    }

#endif

    ~SafeTypeCode()
    {
        DDS_ExceptionCode_t ex = DDS_NO_EXCEPTION_CODE;
        if (factory_ && typecode_ && !is_primitive_)
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

    DDS_TypeCode * get()
    {
      return typecode_;
    }
  
    const DDS_TypeCode * get() const
    {
      return typecode_;
    }

    DDS_TypeCode * release()
    {
      DDS_TypeCode * ret = typecode_;
      typecode_ = 0;
      return ret;
    }
  
    void swap(SafeTypeCode & stc) throw()
    {
      std::swap(factory_, stc.factory_);
      std::swap(typecode_, stc.typecode_);
      std::swap(is_primitive_, stc.is_primitive_);
    }
    /*
    static Safe create_array_tc(DDS_TypeCode * inner,
      const DDS_UnsignedLongSeq &dims)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * arrTc =
        factory_->create_array_tc(dims, inner, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_array_tc: Unable to create array typecode, error = ",
        ex);

      set_typecode(arrTc);
    }

    REFLEX_INLINE void InnerTypeCodeBase::create_seq_tc(DDS_TypeCode *inner,
      size_t bound)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * seqTc =
        factory_->create_sequence_tc(bound, inner, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_seq_tc: Unable to create sequence typecode, error = ",
        ex);

      set_typecode(seqTc);
    }

    REFLEX_INLINE void InnerTypeCodeBase::create_string_tc(size_t bound)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * stringTc =
        factory_->create_string_tc(bound, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_string_tc: Unable to create string typecode, error = ",
        ex);

      set_typecode(stringTc);
    }

    REFLEX_INLINE void InnerTypeCodeBase::create_struct_tc(const char * name)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode *structTc =
        factory_->create_struct_tc(name,
        DDS_StructMemberSeq(), ex);

      check_exception_code(
        "InnerTypeCodeBase::create_struct_tc: Unable to create struct typecode, error = ",
        ex);

      set_typecode(structTc);
    }

    REFLEX_INLINE void InnerTypeCodeBase::create_value_tc(
      const char * name,
      DDS_TypeCode * basetc)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode *valueTc =
        factory_->create_value_tc(
        name,
        DDS_EXTENSIBLE_EXTENSIBILITY,
        DDS_VM_NONE,
        basetc,
        DDS_ValueMemberSeq(),
        ex);

      check_exception_code(
        "InnerTypeCodeBase::create_value_tc: Unable to create valuetype typecode, error = ",
        ex);

      set_typecode(valueTc);
    }

    REFLEX_INLINE void InnerTypeCodeBase::create_enum_tc(const char * name,
      const DDS_EnumMemberSeq & enum_seq,
      const std::vector<MemberInfo> &)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * enumTc = factory_->create_enum_tc(name, enum_seq, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_enum_tc: Unable to create enum typecode, error = ",
        ex);

      set_typecode(enumTc);
    }

    REFLEX_INLINE void InnerTypeCodeBase::create_union_tc(const char * name,
      DDS_TypeCode * discTc,
      DDS_UnsignedLong default_case,
      DDS_UnionMemberSeq & member_seq)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode * unionTc =
        factory_->create_union_tc(name,
        discTc,
        default_case,
        member_seq,
        ex);

      check_exception_code(
        "InnerTypeCodeBase::create_union_tc: Unable to create union typecode, error = ",
        ex);

      set_typecode(unionTc);
    }

    REFLEX_INLINE void InnerTypeCodeBase::create_sparse_tc(const char * name)
    {
      DDS_ExceptionCode_t ex;
      DDS_TypeCode *sparseTc =
        factory_->create_sparse_tc(name, DDS_VM_NONE, NULL, ex);

      check_exception_code(
        "InnerTypeCodeBase::create_sparse_tc: Unable to create sparse typecode, error = ",
        ex);

      set_typecode(sparseTc);
    }
    */

  };

  template <class T>
  void swap(SafeTypeCode<T> & lhs, SafeTypeCode<T> & rhs) throw()
  {
    lhs.swap(rhs);
  }


} // namespace reflex


#endif // RTIREFLEX_SAFE_TYPECODE_H

