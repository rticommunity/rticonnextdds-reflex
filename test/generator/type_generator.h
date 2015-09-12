#ifndef RTI_TYPE_GENERATOR_H
#define RTI_TYPE_GENERATOR_H

#include <type_traits>

#define TYPE_MAP(Idx, Type)   \
  template <uint16_t seed>    \
  struct TypeMap<Idx, seed> { \
    typedef Type type;        \
  };

namespace typegen {

  template <uint16_t seed, uint16_t Size = 24>
  struct RandomTuple;

  namespace detail {

    constexpr uint16_t LFSR_bit(uint16_t prev)
    {
      /* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
      return ((prev >> 0) ^ (prev >> 2) ^ (prev >> 3) ^ (prev >> 5)) & 1;
    }

    // Compile-time Linear Feedback Shift Register (LFSR) random number generator
    constexpr uint16_t LFSR(uint16_t prev)
    {
      return (prev >> 1) | (LFSR_bit(prev) << 15);
    }

    template <size_t lfsr, size_t... values>
    struct oneof
    {
      constexpr static size_t val[sizeof...(values)] = { values... };
      constexpr static size_t value = val[LFSR(lfsr) % sizeof...(values)];
    };

    template <uint16_t I, uint16_t seed>
    struct TypeMap;

    constexpr uint16_t MAP_SIZE = 19;

    TYPE_MAP(0,  bool);
    TYPE_MAP(1,  char);
    TYPE_MAP(2,  int8_t);
    TYPE_MAP(3,  int16_t);
    TYPE_MAP(4,  int32_t);
    TYPE_MAP(5,  int64_t);
    TYPE_MAP(6,  uint8_t);
    TYPE_MAP(7,  uint16_t);
    TYPE_MAP(8,  uint32_t);
    TYPE_MAP(9,  uint64_t);
    TYPE_MAP(10, float);
    TYPE_MAP(11, double);
    TYPE_MAP(12, long double);

    // sequence generator
    template <uint16_t lfsr>
    struct TypeMap<13, lfsr>
    {
      // avoid sequence<shared_ptr<T>> = 16
      // avoid sequence<raw pointer = 17
      // avoid sequence<optional<T>> = 18
      constexpr static size_t selection = LFSR(lfsr) % 16; 
      typedef std::vector<typename TypeMap<selection, LFSR(lfsr)>::type> type;
    };

    // array generator
    template <uint16_t lfsr>
    struct TypeMap<14, lfsr>
    {
      // avoid array<optional<T>> = 18
      // avoid array<raw pointer> = 18
      // avoid array<shared_ptr<T>> = 16
      constexpr static size_t selection = LFSR(lfsr) % 16; 
      typedef std::array<typename TypeMap<selection, LFSR(lfsr)>::type,
                         (LFSR(lfsr) % 10) + 1> type;
    };

    // tuple generator
    template <uint16_t lfsr>
    struct TypeMap<15, lfsr>
    {
      constexpr static size_t tuplesize = (LFSR(lfsr) % 18) + 1;
      typedef typename RandomTuple<LFSR(lfsr), tuplesize>::type type;
    };

    // shared_ptr generator
    template <uint16_t lfsr>
    struct TypeMap<16, lfsr>
    {
      // avoid shared_ptr<shared_ptr<T>> = 16
      // avoid shared_ptr<raw pointer> = 17
      // avoid shared_ptr<optional<T>> = 18
      constexpr static size_t selection = LFSR(lfsr) % 16; 
      typedef std::shared_ptr<typename TypeMap<selection, LFSR(lfsr)>::type> type;
    };

    // raw pointer generator
    template <uint16_t lfsr>
    struct TypeMap<17, lfsr>
    {
      // avoid raw pointer to shared_ptr<T> = 16
      // avoid raw pointer to raw pointer = 17
      // avoid raw pointer to optional<T> = 18
      constexpr static size_t selection = LFSR(lfsr) % 16; 
      typedef typename std::add_pointer<typename TypeMap<selection, LFSR(lfsr)>::type>::type type;
    };

    // optional generator. 
    template <uint16_t lfsr>
    struct TypeMap<18, lfsr>
    {
      // avoid optional<shared_ptr<T>> = 16
      // avoid optional<raw pointer> = 17
      // avoid optional<optional<T>> = 18
      constexpr static size_t selection = LFSR(lfsr) % 16; 
      typedef boost::optional<typename TypeMap<selection, LFSR(lfsr)>::type> type;
    };

    template <class Head, class... Tail>
    struct tuple_cons;

    template <class Head, class... Tail>
    struct tuple_cons<Head, std::tuple<Tail...>>
    {
      typedef std::tuple<Head, Tail...> type;
    };

    template <class T>
    struct identity
    {
      typedef T type;
    };

    template <bool, class T, class U>
    struct if_
    {
      typedef typename T::type type;
    };

    template <class T, class U>
    struct if_<false, T, U>
    {
      typedef typename U::type type;
    };

    template <class Elem, class... Tail>
    struct contains;

    template <class Elem, class Head, class... Tail>
    struct contains<Elem, std::tuple<Head, Tail...>>
    {
      constexpr static bool value = contains<Elem, std::tuple<Tail...>>::value;
    };

    template <class Elem, class... Tail>
    struct contains<Elem, std::tuple<Elem, Tail...>>
    {
      constexpr static bool value = true;
    };

    template <class Elem>
    struct contains<Elem, std::tuple<>>
    {
      constexpr static bool value = false;
    };

    template<size_t lfsr, class... Args>
    struct NextUniqueType;

    template<size_t lfsr, class... Args>
    struct NextUniqueType<lfsr, std::tuple<Args...>>
    {
      typedef typename TypeMap<LFSR(lfsr) % MAP_SIZE, LFSR(lfsr)>::type Next;
      typedef typename if_<contains<Next, std::tuple<Args...>>::value,
                           NextUniqueType<LFSR(lfsr), std::tuple<Args...>>,
                           identity<Next>>::type type;
                           
    };

    template <size_t size, uint16_t lfsr>
    struct RandomTupleImpl
    {
      typedef typename RandomTupleImpl<size - 1, LFSR(lfsr)>::type Tuple;
      typedef typename NextUniqueType<LFSR(lfsr), Tuple>::type Next;
      // For non-unique types, uncomment the following.
      //typedef typename TypeMap<LFSR(lfsr) % MAP_SIZE, LFSR(lfsr)>::type Next;
      typedef typename tuple_cons<Next, Tuple>::type type;
    };

    template <uint16_t lfsr>
    struct RandomTupleImpl<0, lfsr>
    {
      typedef std::tuple<typename TypeMap<LFSR(lfsr) % MAP_SIZE, LFSR(lfsr)>::type> type;
    };

  } // namespace detail

  template <uint16_t seed, uint16_t Size>
  struct RandomTuple
  {
    typedef typename detail::RandomTupleImpl<Size - 1, detail::LFSR(seed)>::type type;
  };

  template <uint16_t seed>
  struct RandomTuple<seed, 0>
  {
    // Size must be > 0
  };

  template <uint16_t Size>
  struct RandomTuple<0, Size>
  {
    // LSFR seed must be non-zero
  };

} // namespace typegen

#undef TYPE_MAP

#endif // RTI_TYPE_GENERATOR_H

