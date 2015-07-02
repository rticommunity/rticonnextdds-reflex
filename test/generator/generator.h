#ifndef RTI_GENERATOR_H
#define RTI_GENERATOR_H

#include <string>
#include <cstdlib>
#include <vector>
#include <list>
#include <array>
#include <ctime>
#include <limits>

#include <boost/optional.hpp>

namespace gen {

  constexpr unsigned int DEFAULT_MAX_STR_LEN = 100;
  constexpr unsigned int DEFAULT_MAX_SEQ_LEN = 100;

#ifdef RTI_WIN32
  void initialize(unsigned int seed = 0)
  {
    if (seed == 0)
      seed = (unsigned) time(NULL);
    
    srand(seed);
  }

  int32_t random_int32()
  {
    return rand();
  }

#else
  void initialize(unsigned int seed = 0)
  {
    if (seed == 0)
      seed = (unsigned) time(NULL);

    srandom(seed);
  }

  int random_int32()
  {
    return static_cast<int>(random());
  }
#endif

  template <class GenFunc>
  auto make_gen_from(GenFunc&& func);

  template <class T>
  struct GenFactory
  {
    static auto make();
  };

  template <class T, class GenFunc>
  class Gen : GenFunc
  {
  public:
    typedef T value_type;

    explicit Gen(GenFunc func)
      : GenFunc(std::move(func))
    { }

    T generate()
    {
      return GenFunc::operator()();
    }

    template <class Func>
    auto map(Func&& func) const
    {
      return make_gen_from(
                [self = *this,
                 func = std::forward<Func>(func)]() mutable {
                    return func(self.generate());
                });
    }

    template <class Zipper, class... GenList>
    auto zip_with(Zipper&& func, GenList&&... genlist) const
    {
      return make_gen_from(
        [self = *this,
         genlist...,
         func = std::forward<Zipper>(func)]() mutable {
            return func(self.generate(), genlist.generate()...);
      });
    }

    template <class UGen>
    auto amb(UGen&& ugen) const
    {
      return make_gen_from(
         [self = *this,
          ugen = std::forward<UGen>(ugen)]() mutable {
             return (random_int32() % 2) ? self.generate() : ugen.generate();
      });
    }

  };

  template <class GenFunc>
  auto make_gen_from(GenFunc&& func)
  {
    return Gen<decltype(func()), GenFunc>(std::forward<GenFunc>(func));
  }

  template <class T>
  auto make_single_gen(T&& t)
  {
    return make_gen_from([t = std::forward<T>(t)]() { return t; });
  }

  template <class Integer>
  auto make_range_gen(Integer lo, Integer hi)
  {
    return make_gen_from([lo, hi]() { 
      return static_cast<Integer>(lo + random_int32() % (hi - lo)); 
    });
  }

  auto make_printable_gen()
  {
    return make_gen_from([]() {
      auto space = 32;
      auto tilde = 126;
      return static_cast<char>(space + random_int32() % (tilde + 1 - space));
    });
  }

  auto make_ascii_gen()
  {
    return make_gen_from([]() {
              return static_cast<char>(random_int32() % 128);
           });
  }

  auto make_lowercase_gen()
  {
    return make_gen_from([]() {
      return static_cast<char>('a' + random_int32() % ('z' + 1 - 'a'));
    });
  }

  auto make_uppercase_gen()
  {
    return make_gen_from([]() {
      return static_cast<char>('A' + random_int32() % ('Z' + 1 - 'A'));
    });
  }

  auto make_alpha_gen()
  {
    return make_gen_from([]() {
      return (random_int32() % 2) ?
        static_cast<char>('a' + random_int32() % ('z' + 1 - 'a')) :
        static_cast<char>('A' + random_int32() % ('Z' + 1 - 'A'));
    });
  }

  auto make_digit_gen()
  {
    return make_gen_from([]() {
      return static_cast<char>('0' + random_int32() % ('9' + 1 - '0'));
    });
  }

  auto make_alphanum_gen()
  {
    return make_gen_from([]() {
      if (random_int32() % 2)
      {
        return (random_int32() % 2) ?
          static_cast<char>('a' + random_int32() % ('z' + 1 - 'a')) :
          static_cast<char>('A' + random_int32() % ('Z' + 1 - 'A'));
      }
      else
        return static_cast<char>('0' + random_int32() % ('9' + 1 - '0'));
    });
  }

  template <class CharGen>
  auto make_string_gen(CharGen&& chargen = make_printable_gen(), 
                       unsigned int maxlen = DEFAULT_MAX_STR_LEN, 
                       bool possibly_empty = false)
  {
    return make_gen_from(
      [chargen=std::forward<CharGen>(chargen),
       maxlen,
       possibly_empty]() mutable {
        std::string str;
        int length =
          possibly_empty ? (random_int32() % (maxlen+1)) :
                           (random_int32() % maxlen) + 1;
        str.reserve(length);
        for (int i = 0; i < length; ++i)
          str.push_back(chargen.generate());
        
        return str;
    });
  }

  template <template <class, class> class Container, class ElemGen>
  auto make_seq_gen(ElemGen&& elemgen, 
                    unsigned int maxlen = DEFAULT_MAX_SEQ_LEN,
                    bool possibly_empty = false)
  {
    return make_gen_from([elemgen=std::forward<ElemGen>(elemgen),
                          maxlen,
                          possibly_empty]() mutable 
    {
      typedef typename std::remove_reference<ElemGen>::type ElemGenT;
      typedef typename ElemGenT::value_type ElemType;
      Container<ElemType, std::allocator<ElemType>> container;

      int length =
        possibly_empty ? (random_int32() % (maxlen + 1)) :
                         (random_int32() % maxlen) + 1;
      container.reserve(length);
      for (int i = 0; i < length; ++i)
        container.push_back(elemgen.generate());

      return container;
    });
  }

  template <class Zipper, class... GenList>
  auto make_zip_gen(Zipper&& func, GenList&&... genlist)
  {
    return make_gen_from(
      [genlist...,
      func = std::forward<Zipper>(func)]() mutable {
      return func(genlist.generate()...);
    });
  }

  template<size_t... Dims>
  struct dim_list {
    enum { size = sizeof...(Dims) };
  };

  namespace detail {

    template <class ElemGen, size_t DimListSize, class DimList>
    struct ArrayGen;

    template <class ElemGen, size_t DimListSize, size_t Head, size_t... Tail>
    struct ArrayGen<ElemGen, DimListSize, dim_list<Head, Tail...>>
    {
      static auto make(ElemGen & elemgen)
      {
        auto innergen =
          ArrayGen<ElemGen, DimListSize - 1, dim_list<Tail...>>::make(elemgen);
        
        return make_gen_from([innergen=std::move(innergen)]() mutable {

          std::array<decltype(innergen.generate()), Head> arr;
          for (auto & elem : arr)
            elem = innergen.generate();

          return arr;
        });
      }
    };
    
    template <class ElemGen, size_t Dim>
    struct ArrayGen<ElemGen, 1, dim_list<Dim>>
    {
      static auto make(ElemGen & elemgen)
      {
        return make_gen_from([elemgen]() mutable {
          typedef typename std::remove_reference<ElemGen>::type ElemGenT;
          std::array<typename ElemGenT::value_type, Dim> arr;
          for (auto & elem : arr)
            elem = elemgen.generate();

          return arr;
        });
      }
    };

    template <class... Args>
    struct TupleGen;

    template <class... Args>
    struct TupleGen<std::tuple<Args...>>
    {
      static auto make()
      {
        return make_zip_gen([](auto... args) {
          return std::make_tuple(args...);
        }, GenFactory<Args>::make()...);
      }
    };

  } // anonymous namespace

  template <class ElemGen, class DimList>
  auto make_array_gen(ElemGen&& elemgen, DimList dimlist)
  {
    typedef typename std::remove_reference<ElemGen>::type ElemGenT;
    return detail::ArrayGen<ElemGenT, DimList::size, DimList>::make(elemgen);
  }

  template <class ElemGen>
  auto make_optional_gen(ElemGen&& elemgen)
  {
    typedef typename std::remove_reference<ElemGen>::type ElemGenT;
    typedef typename boost::optional<typename ElemGenT::value_type> Opt;

    return make_gen_from([elemgen=std::forward<ElemGen>(elemgen)]() mutable {
      return (random_int32() % 2) ? Opt() : Opt(elemgen.generate());
    });
  }

  template <class T>
  auto make_oneof_gen(std::initializer_list<T> list)
  {
    std::vector<T> options(list);
    return make_gen_from([options = std::move(options)]() {
      return *(options.begin() + random_int32() % options.size());
    });
  }

  template <class T>
  auto make_oneof_gen(std::vector<T> options)
  {
    return make_gen_from([options = std::move(options)]() {
      return *(options.begin() + random_int32() % options.size());
    });
  }

  template <class UGen, class VGen>
  auto make_pair_gen(UGen&& ugen, VGen&& vgen)
  {
    return make_gen_from(
      [ugen = std::forward<UGen>(ugen),
       vgen = std::forward<VGen>(vgen)]() mutable {
          return std::make_pair(ugen.generate(), vgen.generate());
    });
  }

  template <class... GenList>
  auto make_composed_gen(GenList&&... genlist)
  {
    return make_gen_from(
      [genlist...]() mutable {
      return std::make_tuple(genlist.generate()...);
    });
  }

  template <class Tuple>
  auto make_tuple_gen()
  {
    return detail::TupleGen<Tuple>::make();
  }

} // namespace gen

#include "gen_factory.h"

#endif // RTI_GENERATOR_H


