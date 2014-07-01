/*********************************************************************************************
(c) 2005-2014 Copyright, Real-Time Innovations, Inc.  All rights reserved.
RTI grants Licensee a license to use, modify, compile, and create derivative works
of the Software.  Licensee has the right to distribute object form only for use with RTI
products.  The Software is provided “as is”, with no warranty of any type, including
any warranty for fitness for any purpose. RTI is under no obligation to maintain or
support the Software.  RTI shall not be liable for any incidental or consequential
damages arising out of the use or inability to use the software.
**********************************************************************************************/

#ifndef RTIREFLEX_TUPLE_ITERATOR_H
#define RTIREFLEX_TUPLE_ITERATOR_H

#include <iostream>
#include <type_traits>
#include <sstream>

#include <ndds/ndds_cpp.h>

#include "default_member_names.h"
#include "disc_union.h"
#include "dd_extra.h"
#include "enable_if.h"

namespace reflex {

  namespace detail {

    template <class T>
    void set_member_forward(
      DDS_DynamicData & instance,
      const MemberAccess &ma,
      const T & val);

    template <class T>
    void get_member_forward(
      const DDS_DynamicData & instance,
      const MemberAccess &ma,
      T & val);

    template <class T>
    void add_member(
      DDS_TypeCodeFactory * factory,
      DDS_TypeCode * outerTc,
      const char * member_name,
      unsigned char flags,
      int id,
      const T *,
      typename enable_if<is_builtin_array<T>::value>::type * = 0);

    template <class T>
    void add_member(
      DDS_TypeCodeFactory * factory,
      DDS_TypeCode * outerTc,
      const char * member_name,
      unsigned char flags,
      int id,
      const T *,
      typename disable_if<is_builtin_array<T>::value>::type * = 0);

    template <class Case>
    void case_add(
      DDS_TypeCodeFactory * factory,
      const char * member_name,
      DDS_UnionMember & umember,
      typename enable_if<is_builtin_array<typename Case::type>::value>::type * = 0);

    template <class Case>
    void case_add(
      DDS_TypeCodeFactory * factory,
      const char * member_name,
      DDS_UnionMember & umember,
      typename disable_if<is_builtin_array<typename Case::type>::value>::type * = 0);

    template <class T>
    void deleteTc(
      DDS_TypeCodeFactory * factory,
      DDS_TypeCode * tc,
      const T *, // Can't combine para 3 & 4 because SFINAE is non-deducible context.
      typename enable_if<is_primitive<T>::value, T>::type * = 0);

    template <class T>
    void deleteTc(
      DDS_TypeCodeFactory * factory,
      DDS_TypeCode * tc,
      const T *, // Can't combine para 3 & 4 because SFINAE is non-deducible context.
      typename disable_if<is_primitive<T>::value, T>::type * = 0);


    template <class Typelist,
              unsigned int I,
              class Union>
    bool get_union_case(
        const DDS_DynamicData &instance,
        const MemberAccess & ma,
        int discriminator_value,
        Union& val)
    {
        typedef typename At<Typelist, I>::type CaseI;
        if (CaseI::matches(discriminator_value))
        {
          typename CaseI::type * data_ptr = Get<I>(val.get_caseptr_tuple());
          bool data_in_case_ptr = data_ptr ? true : false;

          if (data_in_case_ptr == false)
          {
            // variant may be empty. So inittialize it first. 
            // Otherwise, we will get boost::bad_get exception.
            // Ideally, like boost.optinal we want support for 
            // typed_in_place_factory in boost.variant.
            // but boost.variant does not support it. Sigh!
            // val.get_variant() = boost::in_place<typename CaseI::type>();

            val.get_variant() = CaseI(); // default c-tor
            data_ptr = &(boost::get<CaseI>(val.get_variant()).get());
          }

          if (ma.access_by_id())
            get_member_forward(
                instance,
                MemberAccess::BY_ID(discriminator_value),
                *data_ptr);
          else
          {
            MemberInfo info =
              MemberTraits<typename remove_refs<Typelist>::type, 
                           I>::member_info();

            get_member_forward(
                instance,
                MemberAccess::BY_NAME(info.name.c_str()),
                *data_ptr);
          }
          if (data_in_case_ptr)
            val.set_active_index(I);

          return true;
        }
        return false;
      }

    template <class Typelist,
              unsigned int I,
              class Union>
    bool set_union_case(
        DDS_DynamicData &instance,
        const MemberAccess & ma,
        const Union& val)
    {
        // Note that CaseTypelist is off by one w.r.t the variant.
        if ((I == val.get_active_index()) ||
          (I == val.get_variant().which() - 1))
        {
          typedef typename At<Typelist, I>::type CaseI;
          typename CaseI::type const * data_ptr =
            Get<I>(val.get_caseptr_tuple()) ? :
            &(boost::get<CaseI>(val.get_variant()).get());

          if (ma.access_by_id())
            set_member_forward(
                       instance,
                       MemberAccess::BY_ID(CaseI::discriminator),
                       *data_ptr);
          else
          {
            MemberInfo info =
              MemberTraits<typename remove_refs<Typelist>::type, I>::member_info();

            set_member_forward(
                       instance, 
                       MemberAccess::BY_NAME(info.name.c_str()), 
                       *data_ptr);
          }

          return true;
        }
        return false;
      }

    template <class Typelist,
              unsigned int I,
              unsigned int MAX_INDEX>
    struct TypelistIterator
    {
      typedef TypelistIterator<
                 Typelist,
                 I + 1,
                 MAX_INDEX> Next;

      static void set(
          DDS_DynamicData & instance,
          const MemberAccess &ma,
          const Typelist & tuple)
      {
#ifdef _DEBUG
        unsigned i = I;
#endif

        if (ma.access_by_id())
          set_member_forward(
                     instance, 
                     MemberAccess::BY_ID(I + 1), 
                     Get<I>(tuple));
        else
        {
          MemberInfo info =
            MemberTraits<typename remove_refs<Typelist>::type, I>::member_info();

          set_member_forward(
                     instance,
                     MemberAccess::BY_NAME(info.name.c_str()),
                     Get<I>(tuple));
        }
        Next::set(instance, ma, tuple);
      }

      static void get(
        const DDS_DynamicData & instance,
        const MemberAccess &ma,
        Typelist & tuple)
      {
#ifdef _DEBUG
        unsigned i = I;
#endif
        if (ma.access_by_id())
          get_member_forward(
              instance,
              MemberAccess::BY_ID(I + 1),
              Get<I>(tuple));
        else
        {
          MemberInfo info =
            MemberTraits<typename remove_refs<Typelist>::type,
                         I>::member_info();

          get_member_forward(
              instance,
              MemberAccess::BY_NAME(info.name.c_str()),
              Get<I>(tuple));
        }
        Next::get(instance, ma, tuple);
      }

      static void add(
        DDS_TypeCodeFactory * factory,
        DDS_TypeCode * outer_structTc)
      {
#ifdef _DEBUG
        unsigned i = I;
#endif

        typedef typename At<Typelist, I>::type Inner;
        typedef typename remove_reference<Inner>::type InnerNoRef;

        MemberInfo info =
          MemberTraits<typename remove_refs<Typelist>::type, 
                       I
                      >::member_info();

        add_member(factory,
                   outer_structTc,
                   info.name.c_str(),
                   info.value,
                   I + 1,
                   static_cast<InnerNoRef *>(0));

        Next::add(factory, outer_structTc);
      }

      static void add_union_member(
        DDS_TypeCodeFactory * factory,
        DDS_UnionMemberSeq & seq)
      {
        typedef typename At<Typelist, I>::type CaseI;

        MemberInfo info =
          MemberTraits<typename remove_refs<Typelist>::type, 
                       I
                      >::member_info();

        case_add<CaseI>(factory, info.name.c_str(), seq[I]);

        Next::add_union_member(factory, seq);
      }

      static void delete_typecodes(
        DDS_TypeCodeFactory * factory,
        DDS_UnionMemberSeq & seq)
      {
        typedef typename At<Typelist, I>::type Case;
        typedef typename remove_reference<typename Case::type>::type CaseTypeNoRef;
        deleteTc(factory, const_cast<DDS_TypeCode *>(seq[I].type),
          static_cast<CaseTypeNoRef *>(0));
        Next::delete_typecodes(factory, seq);
      }

      template <class Union>
      static void set_union(
        DDS_DynamicData &instance,
        const MemberAccess & ma,
        const Union& val)
      {
        if (!set_union_case<Typelist, I>(instance, ma, val))
          Next::set_union(instance, ma, val);
      }

      template <class Union>
      static void get_union(
        const DDS_DynamicData &instance,
        const MemberAccess & ma,
        int discriminator_value,
        Union& val)
      {
        if (!get_union_case<Typelist, I>(instance, ma, discriminator_value, val))
          Next::get_union(instance, ma, discriminator_value, val);
      }
    };

    template <class Typelist,
              unsigned int MAX_INDEX>
    struct TypelistIterator<Typelist, MAX_INDEX, MAX_INDEX>
    {
      static void set(
          DDS_DynamicData & instance,
          const MemberAccess &ma,
          const Typelist & tuple)
      {
#ifdef _DEBUG
        unsigned max = MAX_INDEX;
#endif

        if (ma.access_by_id())
          set_member_forward(
                     instance,
                     MemberAccess::BY_ID(MAX_INDEX + 1),
                     Get<MAX_INDEX>(tuple));
        else
        {
          MemberInfo info =
            MemberTraits<typename remove_refs<Typelist>::type, 
                         MAX_INDEX>::member_info();

          set_member_forward(
                     instance,
                     MemberAccess::BY_NAME(info.name.c_str()),
                     Get<MAX_INDEX>(tuple));
        }
      }

      static void get(const DDS_DynamicData & instance,
                      const MemberAccess &ma,
                      Typelist & tuple)
      {
#ifdef _DEBUG
        unsigned max = MAX_INDEX;
#endif

        if (ma.access_by_id())
          get_member_forward(
              instance,
              MemberAccess::BY_ID(MAX_INDEX + 1),
              Get<MAX_INDEX>(tuple));
        else
        {
          MemberInfo info =
            MemberTraits<typename remove_refs<Typelist>::type, 
                         MAX_INDEX>::member_info();

          get_member_forward(
              instance,
              MemberAccess::BY_NAME(info.name.c_str()),
              Get<MAX_INDEX>(tuple));
        }
      }

      static void add(
        DDS_TypeCodeFactory * factory,
        DDS_TypeCode * outer_structTc)
      {
#ifdef _DEBUG
        unsigned max = MAX_INDEX;
#endif

        typedef typename At<Typelist, MAX_INDEX>::type Inner;
        typedef typename remove_reference<Inner>::type InnerNoRef;

        MemberInfo info =
          MemberTraits<typename remove_refs<Typelist>::type, 
                       MAX_INDEX>::member_info();

        add_member(factory,
                   outer_structTc,
                   info.name.c_str(),
                   info.value,
                   MAX_INDEX + 1,
                   static_cast<InnerNoRef *>(0));
      }

      static void add_union_member(
        DDS_TypeCodeFactory * factory,
        DDS_UnionMemberSeq & seq)
      {
        typedef typename At<Typelist, MAX_INDEX>::type CaseI;
        MemberInfo info =
          MemberTraits<typename remove_refs<Typelist>::type, 
                       MAX_INDEX>::member_info();

        case_add<CaseI>(factory, info.name.c_str(), seq[MAX_INDEX]);
      }

      static void delete_typecodes(
        DDS_TypeCodeFactory * factory,
        DDS_UnionMemberSeq & seq)
      {
        typedef typename At<Typelist, MAX_INDEX>::type Case;
        typedef typename remove_reference<typename Case::type>::type CaseTypeNoRef;
        deleteTc(factory, 
                 const_cast<DDS_TypeCode *>(seq[MAX_INDEX].type),
                 static_cast<CaseTypeNoRef *>(0));
      }

      template <class Union>
      static void set_union(
        DDS_DynamicData &instance,
        const MemberAccess & ma,
        const Union& val)
      {
        if (!set_union_case<Typelist, MAX_INDEX>(instance, ma, val))
        {
          throw std::runtime_error("set_union: Union was empty");
        }
      }

      template <class Union>
      static void get_union(
        const DDS_DynamicData &instance,
        const MemberAccess & ma,
        int discriminator_value,
        Union& val)
      {
        if (!get_union_case<Typelist, MAX_INDEX>(
                instance, 
                ma, 
                discriminator_value, 
                val))
        {
          std::stringstream stream;
          stream << "get_union: No match for the discriminator. "
                 << "Did you forget to set a union member? "
                 << "Discriminator value = "
                 << discriminator_value;
          throw std::runtime_error(stream.str());
        }
      }
    };

  } // namespace detail

} // namespace reflex

#endif // RTIREFLEX_TUPLE_ITERATOR_H

