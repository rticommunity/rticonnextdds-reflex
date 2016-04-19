#include <iostream>
#include <boost/core/demangle.hpp>

#include "generator.h"
#include "type_generator.h"

#include "reflex.h"

#ifndef RANDOM_SEED
  #define RANDOM_SEED 0xAC0
#endif 

// Clang requires forward declarations for overloaded << operators.
// g++5 does not. Who's correct?

template <class... Args>
std::ostream & operator << (std::ostream & o, const std::tuple<Args...> & tuple);

template <class T>
std::ostream & operator << (std::ostream & o, const std::vector<T> & vector);

template <class T>
std::ostream & operator << (std::ostream & o, const boost::optional<T> & opt);

template <class T, class U>
std::ostream & operator << (std::ostream & o, const std::pair<T, U> & pair);

template <class T, size_t N>
std::ostream & operator << (std::ostream & o, const std::array<T, N> & arr)
{
  for (auto & elem : arr)
    o << elem;

  return o << "\n";
}

template <class T>
std::ostream & operator << (std::ostream & o, const std::vector<T> & vector)
{
  for (const auto & elem : vector)
    o << elem << " ";

  return o;
}

template <class T>
std::ostream & operator << (std::ostream & o, const boost::optional<T> & opt)
{
  if (opt)
    o << opt.get();

  return o;
}

template <class T, class U>
std::ostream & operator << (std::ostream & o, const std::pair<T, U> & pair)
{
  o << "pair.first = " << pair.first << "\n"
    << "pair.second = " << pair.second;

  return o;
}

template <class Tuple, size_t Size>
struct TuplePrinter
{
  static void print(std::ostream & o, const Tuple & tuple)
  {
    TuplePrinter<Tuple, Size-1>::print(o, tuple);
    o << std::get<Size-1>(tuple) << " ";
  }
};

template <class Tuple>
struct TuplePrinter<Tuple, 1>
{
  static void print(std::ostream & o, const Tuple & tuple)
  {
    o << std::get<0>(tuple) << " ";
  }
};

template <class Tuple>
struct TuplePrinter<Tuple, 0>
{
  static void print(std::ostream &, const Tuple &)
  {
    // no-op
  }
};

template <class... Args>
std::ostream & operator << (std::ostream & o, const std::tuple<Args...> & tuple)
{
  TuplePrinter<std::tuple<Args...>, sizeof...(Args)>::print(o, tuple);
  return o;
}

struct ShapeType
{
  int x, y, shapesize;
  std::string color;
};

std::ostream & operator << (std::ostream & o, const ShapeType & shape)
{
  o << "shape.x = "         << shape.x << "\n"
    << "shape.y = "         << shape.y << "\n"
    << "shape.shapesize = " << shape.shapesize << "\n"
    << "shape.color = "     << shape.color << "\n";

  return o;
}

auto test_shape_gen()
{
  auto xgen = gen::make_range_gen(0, 200);
  auto ygen = gen::make_range_gen(0, 200);
  auto sizegen = gen::make_constant_gen(30);
  auto colorgen = gen::make_oneof_gen({ "RED", "GREEN", "BLUE" });

  auto shapegen =
    gen::make_zip_gen(
      [](int x, int y, int size, const char * color) {
          return ShapeType { x, y, size, color };
      }, xgen, ygen, sizegen, colorgen);

  // std::cout << shapegen.generate() << "\n";

  return shapegen;
}

void test_generators(void)
{
  gen::initialize();

  auto strgen =
    gen::make_string_gen(gen::make_printable_gen());

  //std::cout << "size of strgen = " << sizeof(strgen) << "\n"
  //          << "string = " << strgen.generate() << "\n";

  auto vecgen =
    //gen::make_seq_gen<std::vector>(gen::GenFactory<int>::make(), 5, true);
    gen::GenFactory<std::vector<int>>::make(gen::GenFactory<int>::make(), 5, true);

  // std::cout << "vector = " << vecgen.generate() << "\n";

  auto optgen = gen::make_optional_gen(strgen);
  (void) optgen;
  // std::cout << "optional string = " << optgen.generate() << "\n";

  auto pairgen = gen::make_pair_gen(strgen, vecgen);
  (void) pairgen;
  // std::cout << pairgen.generate() << "\n";

  auto tuplegen = gen::make_composed_gen(strgen, vecgen);
  (void) tuplegen;
  // std::cout << tuplegen.generate() << "\n";

  auto shapegen = test_shape_gen();

  auto arraygen = gen::make_array_gen(shapegen, gen::dim_list<2, 2>());
  // std::cout << arraygen.generate() << "\n";

  auto inordergen = 
    gen::make_inorder_gen({ 10, 20 });

  assert(inordergen.generate() == 10);
  assert(inordergen.generate() == 20);

  auto concatgen = 
    gen::make_inorder_gen({ 10, 20 })
       .concat(gen::make_inorder_gen({ 30 }));

  assert(concatgen.generate() == 10);
  assert(concatgen.generate() == 20);
  assert(concatgen.generate() == 30);

  auto v1 = gen::make_stepper_gen().take(5).to_vector();
  std::vector<int> v2 { 0, 1, 2, 3, 4 };
  assert(v1 == v2);

  // std::cout << "All assertions satisfied\n";
}

template <class Tuple, size_t I>
struct tuple_iterator;

struct iterate_overload_helper
{
  template <class... Args, class ActionFunc>
  static void recurse(std::tuple<Args...> & tuple, ActionFunc act)
  {
    tuple_iterator<std::tuple<Args...>, 
                   std::tuple_size<std::tuple<Args...>>::value-1>
                     ::iterate(tuple, act);
  }

  template <class T, class ActionFunc>
  static void recurse(std::vector<T> & vec, ActionFunc act) 
  { 
    for(auto & elm: vec)
      recurse(elm, act);
  }

  template <class ActionFunc>
  static void recurse(std::vector<bool> & vec, ActionFunc act) 
  { 
    // No-op
  }

  template <class T, size_t N, class ActionFunc>
  static void recurse(std::array<T, N> & arr, ActionFunc act) 
  { 
    for(auto & elm: arr)
      recurse(elm, act);
  }

  template <class T, class ActionFunc>
  static void recurse(std::shared_ptr<T> & sh_ptr, ActionFunc act) 
  { 
    if(sh_ptr)
      recurse(*sh_ptr, act);
  }

  template <class T, class ActionFunc>
  static void recurse(boost::optional<T> & op, ActionFunc act) 
  { 
    if(op)
      recurse(*op, act);
  }

  template <class T, class ActionFunc>
  static void recurse(T *t, ActionFunc act,
      typename reflex::meta::disable_if<reflex::type_traits::is_primitive<T>::value>::type * = 0) 
  { 
    if(t)
      recurse(*t, act);
  }

  template <class T, class ActionFunc>
  static void recurse(T *, ActionFunc,
      typename reflex::meta::enable_if<reflex::type_traits::is_primitive<T>::value>::type * = 0) 
  { 
    // No-op
  }

  template <class T, class ActionFunc>
  static void recurse(T, ActionFunc,
      typename reflex::meta::enable_if<reflex::type_traits::is_primitive<T>::value>::type * = 0) 
  { 
    // No-op
  }
};

struct compare_overload_helper
{
  template <class... Args>
  static bool recurse_compare(
      const std::tuple<Args...> & t1, 
      const std::tuple<Args...> & t2)
  {
    return tuple_iterator<std::tuple<Args...>, 
                          std::tuple_size<std::tuple<Args...>>::value-1>
                           ::compare(t1, t2);
  }

  template <class T>
  static bool recurse_compare(const std::vector<T> & v1, const std::vector<T> & v2) 
  { 
    if(v1.size() == v2.size())
    {
      bool result = true;
      for(unsigned int i = 0;(i < v1.size()) && result; i++)
        result = result && recurse_compare(v1[i], v2[i]);
      
      return result;
    }
    else
      return false;
  }

  static bool recurse_compare(const std::vector<bool> & v1, const std::vector<bool> & v2) 
  { 
    return v1==v2;
  }

  template <class T, size_t N>
  static bool recurse_compare(const std::array<T, N> & a1, const std::array<T, N> & a2) 
  { 
    bool result = true;
    for(unsigned int i = 0;(i < N) && result; i++)
      result = result && recurse_compare(a1[i], a2[i]);
      
    return result;
  }

  template <class T>
  static bool recurse_compare(const T &t1, const T &t2,
      typename reflex::meta::enable_if<
        reflex::type_traits::is_smart_ptr<T>::value ||
        reflex::type_traits::is_optional<T>::value  ||
        reflex::type_traits::is_pointer<T>::value>::type * = 0)
  { 
    if(t1 && t2)
      return recurse_compare(*t1, *t2);
    else if(!t1 && !t2)
      return true;
    else
      return false;
  }

  template <class T>
  static bool recurse_compare(T t1, T t2,
      typename reflex::meta::enable_if<reflex::type_traits::is_primitive<T>::value>::type * = 0) 
  { 
    return t1==t2;
  }
};

template <class Tuple, size_t I>
struct tuple_iterator
{
  template <class ActionFunc>
  static void iterate(Tuple &t, ActionFunc act)
  {
    act(std::get<I>(t));
    iterate_overload_helper::recurse(std::get<I>(t), act);
    tuple_iterator<Tuple, I-1>::iterate(t, act);
  }

  static bool compare(const Tuple &t1, const Tuple &t2)
  {
    return compare_overload_helper::recurse_compare(std::get<I>(t1), std::get<I>(t2)) && 
           tuple_iterator<Tuple, I-1>::compare(t1, t2);
  }
};

template <class Tuple>
struct tuple_iterator<Tuple, 0>
{
  template <class ActionFunc>
  static void iterate(Tuple &t, ActionFunc act)
  {
    act(std::get<0>(t));
    iterate_overload_helper::recurse(std::get<0>(t), act);
  }

  static bool compare(const Tuple &t1, const Tuple &t2)
  {
    return compare_overload_helper::recurse_compare(std::get<0>(t1), std::get<0>(t2));
  }
};

struct allocator
{
  template <class T>
  void operator () (T &) const {}

  template <class T>
  void operator () (T* &t) const { t = new T(); }
};

struct deallocator
{
  template <class T>
  void operator () (T &) const {}

  template <class T>
  void operator () (T* &t) const { delete t; }
};

template <class Tuple>
void allocate_pointers(Tuple &t)
{
  iterate_overload_helper::recurse(t, allocator());
}

template <class Tuple>
void deallocate_pointers(Tuple &t)
{
  iterate_overload_helper::recurse(t, deallocator());
}

template <class Tuple>
bool compare_tuples(Tuple &t1, Tuple &t2)
{
  return compare_overload_helper::recurse_compare(t1, t2);
}

template <class Tuple>
bool test_roundtrip_property()
{
  std::cout << "Tuple = \n" << boost::core::demangle(typeid(Tuple).name()) << "\n";
  std::cout << "Size of tuple = " << sizeof(Tuple) << "\n";

  auto generator = gen::make_tuple_gen<Tuple>();
  Tuple d1 = generator.generate(); // allocates raw pointers.

  reflex::TypeManager<Tuple> tm;
  reflex::SafeDynamicData<Tuple> safedd = tm.create_dynamicdata(d1);
  reflex::detail::print_IDL(safedd.get()->get_type(), 2);
  safedd.get()->print(stdout, 2);

  Tuple d2;
  allocate_pointers(d2);
  reflex::read_dynamicdata(d2, safedd);

  bool is_same = compare_tuples(d1, d2);
  std::cout << "roundtrip successful = " << std::boolalpha << is_same << "\n";
  //assert(is_same);

  deallocate_pointers(d1);
  deallocate_pointers(d2);

  return true;
}

int main(void)
{
  test_generators();
  
  std::cout << "RANDOM SEED = " << RANDOM_SEED << std::endl;
  test_roundtrip_property<typegen::RandomTuple<RANDOM_SEED>::type>();
  
}
