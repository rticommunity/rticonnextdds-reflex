#ifndef GEN_FACTORY_H
#define GEN_FACTORY_H

namespace gen {

  template <>
  auto GenFactory<bool>::make()
  {
    return make_gen_from([]() {
      return (random_int32() % 2)? true : false;
    });
  }

  template <>
  auto GenFactory<char>::make()
  {
    return make_printable_gen();
  }

  template <>
  auto GenFactory<int8_t>::make()
  {
    return make_gen_from([]() {
      return static_cast<int8_t>(random_int32() % std::numeric_limits<int8_t>::max());
    });
  }

  template <>
  auto GenFactory<int16_t>::make()
  {
    return make_gen_from([]() {
      return static_cast<int16_t>(random_int32() % std::numeric_limits<int16_t>::max());
    });
  }

  template <>
  auto GenFactory<int32_t>::make()
  {
    return make_gen_from([]() { 
      return static_cast<int32_t>(random_int32()); 
    });
  }

  template <>
  auto GenFactory<int64_t>::make()
  {
    return make_gen_from([]() {
      return (static_cast<int64_t>(random_int32()) << 32) + random_int32();
    });
  }

  template <>
  auto GenFactory<uint8_t>::make()
  {
    return make_gen_from([]() {
      return static_cast<uint8_t>(random_int32() % std::numeric_limits<uint8_t>::max());
    });
  }

  template <>
  auto GenFactory<uint16_t>::make()
  {
    return make_gen_from([]() {
      return static_cast<uint16_t>(random_int32() % std::numeric_limits<uint16_t>::max());
    });
  }

  template <>
  auto GenFactory<uint32_t>::make()
  {
    return make_gen_from([]() {
      return static_cast<uint32_t>(random_int32());
    });
  }

  template <>
  auto GenFactory<uint64_t>::make()
  {
    return make_gen_from([]() {
      return (static_cast<uint64_t>(random_int32()) << 32) + random_int32();
    });
  }

  template <>
  auto GenFactory<float>::make()
  {
    return make_gen_from([]() {
      return  static_cast<float>(random_int32()) / random_int32();
      //std::numeric_limits<int32_t>::max() *
      //std::numeric_limits<float>::max();
    });
  }

  template <>
  auto GenFactory<double>::make()
  {
    return make_gen_from([]() {
      return  static_cast<double>((random_int32()) /
        std::numeric_limits<int32_t>::max() *
        std::numeric_limits<double>::max());
    });
  }

  template <>
  auto GenFactory<long double>::make()
  {
    return make_gen_from([]() {
      return  static_cast<long double>(random_int32()) /
        std::numeric_limits<int32_t>::max() *
        std::numeric_limits<long double>::max();
    });
  }

  template <class T>
  struct GenFactory<std::vector<T>>
  {
    template <class TGen>
    static auto make(TGen&& tgen,
                     unsigned int maxlen = DEFAULT_MAX_SEQ_LEN,
                     bool possibly_empty = false)
    {
      typedef typename std::remove_reference<TGen>::type::value_type TGenType;
      static_assert(std::is_same<T, TGenType>::value, "Types don't match");
      return make_seq_gen<std::vector>(std::forward<TGen>(tgen), maxlen, possibly_empty);
    }

    static auto make()
    {
      return make_seq_gen<std::vector>(GenFactory<T>::make());
    }

  };

  template <class T>
  struct GenFactory<std::list<T>>
  {
    template <class TGen>
    static auto make(TGen&& tgen,
                     unsigned int maxlen = DEFAULT_MAX_SEQ_LEN,
                     bool possibly_empty = false)
    {
      typedef typename std::remove_reference<TGen>::type::value_type TGenType;
      static_assert(std::is_same<T, TGenType>::value, "Types don't match");
      return make_seq_gen<std::list>(std::forward<TGen>(tgen), maxlen, possibly_empty);
    }

    static auto make()
    {
      return make_seq_gen<std::list>(GenFactory<T>::make());
    }

  };

  template <class T>
  struct GenFactory<boost::optional<T>>
  {
    static auto make()
    {
      return make_optional_gen(GenFactory<T>::make());
    }

    template <class TGen>
    static auto make(TGen&& tgen)
    {
      return make_optional_gen(std::forward<TGen>(tgen));
    }
  };

  template <class T, size_t Dim>
  struct GenFactory<std::array<T, Dim>>
  {
    static auto make()
    {
      return make_array_gen(GenFactory<T>::make(), dim_list<Dim>());
    }

    template <class TGen>
    static auto make(TGen&& tgen)
    {
      return make_array_gen(std::forward<TGen>(tgen), dim_list<Dim>());
    }
  };

  template <class T>
  struct GenFactory<std::shared_ptr<T>>
  {
    static auto make()
    {
      auto tgen = GenFactory<T>::make();
      return make_gen_from([tgen]() mutable {
            return std::make_shared<T>(tgen.generate());
          });
    }
  };

  template <class T>
  struct GenFactory<T *>
  {
    static auto make()
    {
      auto tgen = GenFactory<T>::make();
      return make_gen_from([tgen]() mutable {
            return new T(tgen.generate());
          });
    }
  };

  template <class... Args>
  struct GenFactory<std::tuple<Args...>>
  {
    static auto make()
    {
      return make_tuple_gen<std::tuple<Args...>>();
    }
  };

  template <>
  struct GenFactory<std::string>
  {
    auto make()
    {
      return make_string_gen(make_printable_gen(), 256);
    }
  };

} // namespace gen

#endif // GEN_FACTORY_H
