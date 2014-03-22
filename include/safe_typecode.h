/*********************************************************************************************
(c) 2005-2013 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

template <class SafeTypeCode>
struct proxy {                                  
  DDS_TypeCodeFactory * factory_;               
  DDS_TypeCode * typecode_;                     
  bool release_;                                
};

#define MAKE_SAFETYPECODE_MOVEONLY                \
private:                                          \
  SafeTypeCode & operator = (SafeTypeCode &);     \
  SafeTypeCode(SafeTypeCode &);                   \
                                                  \
public:                                           \
  SafeTypeCode(proxy<SafeTypeCode> p) throw()     \
    : SafeTypeCodeBase(p.factory_,                \
                       p.typecode_,               \
                       p.release_)                \
  {}                                              \
                                                  \
  SafeTypeCode & operator                         \
     = (proxy<SafeTypeCode> p) throw ()           \
  {                                               \
    SafeTypeCode(p).swap(*this);                  \
    return *this;                                 \
  }                                               \
                                                  \
  operator proxy<SafeTypeCode> () throw() {       \
    proxy<SafeTypeCode>                           \
      p { factory_, typecode_, release_ };        \
    typecode_ = 0;                                \
    return p;                                     \
  }                                               \
                                                  \
  friend SafeTypeCode move(SafeTypeCode & stc) {  \
    return SafeTypeCode(proxy<SafeTypeCode>(stc));\
  }                                               



#define GET_TYPECODE_DECL(BASIC_TYPE, TYPECODE)         \
const DDS_TypeCode * get_typecode(const BASIC_TYPE *);

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

  GET_TYPECODE_DECL(octet_t,            DDS_TK_OCTET)  
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
  void create_enum_tc(const char * name, 
                      const DDS_EnumMemberSeq & enum_seq);
  void create_union_tc(const char * name,
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

template <class T, class = void>
class SafeTypeCode
  : public SafeTypeCodeBase
{
public:
  SafeTypeCode(DDS_TypeCodeFactory * factory) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::set_typecode(
      SafeTypeCodeBase::get_typecode(static_cast<T *>(0)));
  }

  SafeTypeCode(DDS_TypeCodeFactory * factory,
               const char * name) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_struct_tc(name);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <>
class DllExport SafeTypeCode<DDS_TypeCode>
  : public SafeTypeCodeBase
{
public:

  template <class T>
  SafeTypeCode(SafeTypeCode<T> stc)
    : SafeTypeCodeBase(stc.factory_, stc.typecode_, stc.release_)
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
  SafeTypeCode(proxy<SafeTypeCode> p) throw();    
  SafeTypeCode & operator = (proxy<SafeTypeCode> p) throw ();           
  operator proxy<SafeTypeCode> () throw(); 

  friend SafeTypeCode move(SafeTypeCode & stc);   
};

template <class T>
class SafeTypeCode<T, typename enable_if<std::is_enum<T>::value>::type> 
  : public SafeTypeCodeBase
{
  template <size_t I, size_t MAX>
  struct SetEnumMembers 
  {
    static void exec(DDS_EnumMemberSeq & enum_seq,
                     std::vector<MemberInfo> & info_seq)
    {
       info_seq[I] = EnumDef<T>::template EnumMember<I>::info();
       enum_seq[I].name = const_cast<char *>(info_seq[I].name.c_str());
       enum_seq[I].ordinal = info_seq[I].value;
       SetEnumMembers<I+1, MAX>::exec(enum_seq, info_seq);
    }
  };

  template <size_t MAX>
  struct SetEnumMembers<MAX, MAX> 
  {
    static void exec(DDS_EnumMemberSeq & enum_seq,
                     std::vector<MemberInfo> & info_seq)
    {
       info_seq[MAX] = EnumDef<T>::template EnumMember<MAX>::info();
       enum_seq[MAX].name = const_cast<char *>(info_seq[MAX].name.c_str());
       enum_seq[MAX].ordinal = info_seq[MAX].value;
    }
  };    

public:
  SafeTypeCode(DDS_TypeCodeFactory * factory,
               const char * name) 
    : SafeTypeCodeBase(factory)
  {
    // The lifetime of info_seq must be until create_enum_tc is invoked.
    DDS_EnumMemberSeq enum_seq;
    std::vector<MemberInfo> info_seq(EnumDef<T>::size);
    enum_seq.ensure_length(EnumDef<T>::size, EnumDef<T>::size);
    SetEnumMembers<0, EnumDef<T>::size-1>::exec(enum_seq, info_seq);
    SafeTypeCodeBase::create_enum_tc(EnumDef<T>::name(), enum_seq);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <class C> 
class SafeTypeCode<C, typename enable_if<is_container<C>::value>::type> 
  : public SafeTypeCodeBase
{
public:
  SafeTypeCode(DDS_TypeCodeFactory * factory,
               SafeTypeCode<typename C::value_type> & tc) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_seq_tc(tc.get(), MAX_SEQ_SIZE);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <class C, size_t Bound>
class SafeTypeCode<bounded<C, Bound>,
                   typename enable_if<is_container<C>::value>::type> 
  : public SafeTypeCodeBase
{
public:
  SafeTypeCode(DDS_TypeCodeFactory * factory,
               SafeTypeCode<typename C::value_type> & tc) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_seq_tc(tc.get(), Bound);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <class T>
class SafeTypeCode<Range<T>> : public SafeTypeCodeBase
{
public:

  SafeTypeCode(DDS_TypeCodeFactory * factory,
    SafeTypeCode<typename remove_reference<T>::type> & tc)
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_seq_tc(tc.get(), MAX_SEQ_SIZE);
  }

  MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
};

template <class T, size_t Bound>
class SafeTypeCode<BoundedRange<T, Bound>> : public SafeTypeCodeBase
{
public:

  SafeTypeCode(DDS_TypeCodeFactory * factory,
    SafeTypeCode<typename remove_reference<T>::type> & tc)
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_seq_tc(tc.get(), Bound);
  }

  MAKE_SAFETYPECODE_MOVEONLY
    friend class SafeTypeCode<DDS_TypeCode>;
};

template <>
class SafeTypeCode<std::string> : public SafeTypeCodeBase
{
public:
  SafeTypeCode(DDS_TypeCodeFactory * factory) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_string_tc(MAX_STRING_SIZE);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <size_t Bound>
class SafeTypeCode<bounded<std::string, Bound>> : public SafeTypeCodeBase
{
public:
  SafeTypeCode(DDS_TypeCodeFactory * factory) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_string_tc(Bound);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <class... Args>
class SafeTypeCode<std::tuple<Args...>> : public SafeTypeCodeBase
{
public:

  SafeTypeCode(DDS_TypeCodeFactory * factory, 
               const char * name) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_struct_tc(name);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <class... Args>
class SafeTypeCode<Sparse<Args...>> : public SafeTypeCodeBase
{
public:

  SafeTypeCode(DDS_TypeCodeFactory * factory, 
               const char * name) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_sparse_tc(name);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <size_t I, class DimList>
struct CopyDims;

template <size_t I, size_t Head, size_t... Tail>
struct CopyDims <I, dim_list<Head, Tail...>>
{
  static void exec(DDS_UnsignedLongSeq & seq) 
  {
    seq[I] = Head;
    CopyDims<I+1, dim_list<Tail...>>::exec(seq);
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

template <class T, size_t... Dims>
class SafeTypeCode<T, dim_list<Dims...>> 
  : public SafeTypeCodeBase
{
public:
  SafeTypeCode(DDS_TypeCodeFactory * factory,
               SafeTypeCode<T> & tc) 
    : SafeTypeCodeBase(factory)
  {
    DDS_UnsignedLongSeq seq;
    seq.ensure_length(sizeof...(Dims), sizeof...(Dims));
    CopyDims<0, dim_list<Dims...>>::exec(seq);

    SafeTypeCodeBase::create_array_tc(tc.get(), seq);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <class T, size_t Dim>
class SafeTypeCode<std::array<T, Dim>> 
  : public SafeTypeCodeBase
{
public:
  SafeTypeCode(DDS_TypeCodeFactory * factory,
               SafeTypeCode<typename remove_all_extents<T>::type> & tc) 
    : SafeTypeCodeBase(factory)
  {
    DDS_UnsignedLongSeq seq;
    typedef typename make_dim_list<std::array<T,Dim>>::type DimList;
    seq.ensure_length(DimList::size, DimList::size);
    CopyDims<0, DimList>::exec(seq);

    SafeTypeCodeBase::create_array_tc(tc.get(), seq);
  }

  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

template <class Tuple, size_t I, size_t MAX_INDEX, class Case>
struct MatchDefaultCase;

template <class Tuple, size_t I, size_t MAX_INDEX, class T, int Tag, int... Tags>
struct MatchDefaultCase<Tuple, I, MAX_INDEX, Case<T, Tag, Tags...>>
{
  enum { value = 
    MatchDefaultCase<Tuple, I+1, MAX_INDEX, 
                     typename std::tuple_element<I+1, Tuple>::type>::value };
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
  enum { value = 
    MatchDefaultCase<CaseTuple, 
                     1, 
                     Size<CaseTuple>::value-1, 
                     typename std::tuple_element<1, CaseTuple>::type>::value };
};


template <class TagType, class... Cases>
struct SafeTypeCode<Union<TagType, Cases...>>
  : public SafeTypeCodeBase
{
  SafeTypeCode(DDS_TypeCodeFactory * factory,
               const char * name,
               SafeTypeCode<TagType> & discTc,
               DDS_UnionMemberSeq & member_seq) 
    : SafeTypeCodeBase(factory)
  {
    SafeTypeCodeBase::create_union_tc
       (name, 
        discTc.get(),
        DefaultCaseIndex<Union<TagType, Cases...>>::value,
        member_seq);
  }
  
  MAKE_SAFETYPECODE_MOVEONLY
  friend class SafeTypeCode<DDS_TypeCode>;
};

#undef GET_TYPECODE_DECL
#undef MAKE_THIS_CLASS_MOVEONLY

#endif // RTIREFLEX_SAFE_TYPECODE_H

