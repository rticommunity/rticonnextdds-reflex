#ifndef RTI_TYPE_GENERATOR_H
#define RTI_TYPE_GENERATOR_H

#define TYPE_MAP(Idx, Type)   \
  template <uint16_t seed>    \
  struct TypeMap<Idx, seed> { \
    typedef Type type;        \
  };

namespace typegen {

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

    template <uint16_t I, uint16_t seed>
    struct TypeMap;

    constexpr uint16_t MAP_SIZE = 15;

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

    // optional generator
    template <uint16_t lfsr>
    struct TypeMap<13, lfsr>
    {
      typedef boost::optional<typename TypeMap<LFSR(lfsr) % 13, LFSR(lfsr)>::type> type;
    };

    // sequence generator
    template <uint16_t lfsr>
    struct TypeMap<14, lfsr>
    {
      //typedef std::vector<typename TypeMap<LFSR(lfsr) % 13, LFSR(lfsr)>::type> type;
      typedef std::vector<long double> type;
    };

    template <class Head, class... Tail>
    struct tuple_cons;

    template <class Head, class... Tail>
    struct tuple_cons<Head, std::tuple<Tail...>>
    {
      typedef std::tuple<Head, Tail...> type;
    };

    template <size_t size, uint16_t lfsr>
    struct RandomTupleImpl
    {
      typedef typename TypeMap<LFSR(lfsr) % MAP_SIZE, LFSR(lfsr)>::type Next;
      typedef typename
        tuple_cons<Next, typename RandomTupleImpl<size - 1, LFSR(lfsr)>::type>::type type;
    };

    template <uint16_t lfsr>
    struct RandomTupleImpl<0, lfsr>
    {
      typedef std::tuple<typename TypeMap<LFSR(lfsr) % MAP_SIZE, LFSR(lfsr)>::type> type;
    };

  } // namespace detail

  template <uint16_t seed, uint16_t Size = 16>
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

