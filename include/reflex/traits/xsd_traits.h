#ifndef REFLEX_XSD_TRAITS_H
#define REFLEX_XSD_TRAITS_H

/** 
 * @file Reusable traits for adapting xsdcxx generated code to RefleX.
 *       This file must be included after including xsdcxx generated files
 *       and reflex.h
 */

namespace reflex {
  namespace type_traits {

    template <class T>
    struct is_container<xsd::cxx::tree::sequence<T>> : reflex::meta::true_type{};

    template <>
    struct is_container<osacbm::listOfDouble> : reflex::meta::true_type{};

    template <>
    struct is_container<xml_schema::base64_binary> : reflex::meta::true_type{};

    template <>
    struct container_traits<xml_schema::base64_binary>
    {
      typedef char value_type;
      typedef char * iterator;
    };

    template <class T>
    struct is_optional<xsd::cxx::tree::optional<T>> : reflex::meta::true_type{};

    template <class T>
    struct optional_traits<xsd::cxx::tree::optional<T>>
    {
      typedef T value_type;
    };

    template <>
    struct is_string<xml_schema::string> : reflex::meta::true_type{};

  } // namespace type_traits 
} // namespace reflex

#endif // REFLEX_XSD_TRAITS_H
