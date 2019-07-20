#ifndef ENGINE_UTILITY_TEMPLATETOOLS_H
#define ENGINE_UTILITY_TEMPLATETOOLS_H

#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <type_traits>

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
struct KeyValuePair;

template <typename ... Types>
struct TypeList;

template <typename T, bool AddConst>
struct ConditionalAddConst;

template <typename T, typename ... Args>
struct GetMaxSize;

template <typename T, typename ... Types>
struct TypeIsInTypes;

template <typename T, typename ... Types>
struct TypeIsDecayedTypes;

template <typename ... Types>
struct TypesAreUnique;

template <int Index, typename ... Types>
struct GetNthType;

template <typename TypeSet0, typename TypeSet1>
struct TypeSetUnion;

template <typename TypeSet0, typename TypeSet1>
struct TypeSetIntersection;

template <typename TypeSet0, typename TypeSet1>
struct TypeSetComplement;

template <typename TypeSet0, typename TypeSet1>
struct TypeSetEquality;

template <typename TypeSet0, typename TypeSet1>
struct TypeSetIsSubset;

template <typename TypeList0, typename TypeList1>
struct TypeListConcatination;

template <typename TypeList>
struct TypeListRemoveDuplicates;

//##############################################################################
template <bool Constraint, typename ... Types>
using ConstrainedTypeList = std::enable_if_t<Constraint, TypeList<Types...>>;

template <typename ... Types>
using TypeSet = ConstrainedTypeList<TypesAreUnique<Types...>::value, Types...>;

//##############################################################################
template <typename ... Args>
std::tuple<Args...> VariadicToTuple(Args & ... args);

template <typename T>
std::type_index GetTypeIndex(void);

template <typename T>
T GetLowestHigherExponentOf2(T const & value);

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
struct KeyValuePair
{
  struct SortingPredicate
  {
    bool operator ()(KeyValuePair const & keyVal0, KeyValuePair const & keyVal1)
      const;
    bool operator ()(KeyValuePair const & keyVal, Key const & key) const;
    bool operator ()(Key const & key, KeyValuePair const & keyVal) const;
    bool operator ()(Key const & key0, Key const & key1) const;
  };

  template <typename ... Params>
  KeyValuePair(Key const & key, Params && ... params);

  operator KeyValuePair<Key, Value, Pred, true> & (void);
  operator KeyValuePair<Key, Value, Pred, true> const & (void) const;

  typename ConditionalAddConst<Key, KeyIsConst>::Type key;
  Value value;
};

//##############################################################################
template <typename ... Types>
struct TypeList
{};

//##############################################################################
template <typename T>
struct ConditionalAddConst<T, false>
{
  typedef T Type;
};

//##############################################################################
template <typename T>
struct ConditionalAddConst<T, true>
{
  typedef typename std::add_const<T>::type Type;
};

//##############################################################################
template <typename T, typename ... Args>
struct GetMaxSize
{
  static int const value = sizeof(T) > GetMaxSize<Args...>::value ?
    sizeof(T) : GetMaxSize<Args...>::value;
};

//##############################################################################
template <typename T>
struct GetMaxSize<T>
{
  static int const value = sizeof(T);
};

//##############################################################################
template <typename T>
struct TypeIsInTypes<T>
{
  static bool const value = false;
};

//##############################################################################
template <typename T, typename Type0, typename ... Types>
struct TypeIsInTypes<T, Type0, Types...>
{
  static bool const value = TypeIsInTypes<T, Types...>::value;
};

//##############################################################################
template <typename T, typename ... Types>
struct TypeIsInTypes<T, T, Types...>
{
  static bool const value = true;
};

//##############################################################################
template <typename T, typename Type0>
struct TypeIsInTypes<T, Type0>
{
  static bool const value = false;
};

//##############################################################################
template <typename T, typename ... Types>
struct TypeIsDecayedTypes
{
  static bool const value = false;
};

//##############################################################################
template <typename T>
struct TypeIsDecayedTypes<std::decay_t<T>, T>
{
  static bool const value = true;
};

//##############################################################################
template <typename Type0, typename ... Types>
struct TypesAreUnique<Type0, Types...>
{
  static bool const value =
    TypesAreUnique<Types...>::value &&
    !TypeIsInTypes<Type0, Types...>::value;
};

//##############################################################################
template <>
struct TypesAreUnique<>
{
  static bool const value = true;
};

//##############################################################################
template <typename Type0, typename ... Remainder>
struct GetNthType<0, Type0, Remainder...>
{
  typedef Type0 type;
};

//##############################################################################
template <int Index, typename Type0, typename ... Remainder>
struct GetNthType<Index, Type0, Remainder...>
{
  static_assert(Index != 0);

  typedef typename GetNthType<Index - 1, Remainder...>::type type;
};

//##############################################################################
template <typename TypeSet0, typename TypeSet1>
struct TypeSetUnion
{
  typedef typename TypeListRemoveDuplicates<
    typename TypeListConcatination<TypeSet0, TypeSet1>::type>::type type;
};

//##############################################################################
template <typename ... Types1>
struct TypeSetIntersection<TypeList<>, TypeList<Types1...>>
{
  typedef TypeList<> type;
};

//##############################################################################
template <typename T, typename ... Types0, typename ... Types1>
struct TypeSetIntersection<TypeList<T, Types0...>, TypeList<Types1...>>
{
  typedef std::conditional_t<
    TypeIsInTypes<T, Types1...>::value,
    typename TypeListConcatination<
      TypeList<T>,
      typename TypeSetIntersection<TypeSet<Types0...>, TypeSet<Types1...>>::type
    >::type,
    typename TypeSetIntersection<TypeSet<Types0...>, TypeSet<Types1...>>::type
  > type;
};

//##############################################################################
template <typename ... Types1>
struct TypeSetComplement<TypeList<>, TypeList<Types1...>>
{
  typedef TypeList<> type;
};

//##############################################################################
template <typename T, typename ... Types0, typename ... Types1>
struct TypeSetComplement<TypeList<T, Types0...>, TypeList<Types1...>>
{
  typedef std::conditional_t<
    TypeIsInTypes<T, Types1...>::value,
    typename TypeSetComplement<TypeSet<Types0...>, TypeSet<Types1...>>::type,
    typename TypeListConcatination<
      TypeList<T>,
      typename TypeSetComplement<TypeSet<Types0...>, TypeSet<Types1...>>::type
    >::type
  > type;
};

//##############################################################################
template <typename TypeSet0, typename TypeSet1>
struct TypeSetEquality
{
  static bool const value =
    TypeSetIsSubset<TypeSet0, TypeSet1>::value &&
    TypeSetIsSubset<TypeSet1, TypeSet0>::value;
};

//##############################################################################
template <typename ... Types1>
struct TypeSetIsSubset<TypeList<>, TypeList<Types1...>>
{
  static bool const value = true;
};

//##############################################################################
template <typename T, typename ... Types0, typename ... Types1>
struct TypeSetIsSubset<TypeList<T, Types0...>, TypeList<Types1...>>
{
  static bool const value =
    TypeIsInTypes<T, Types1...>::value &&
    TypeSetIsSubset<TypeList<Types0...>, TypeList<Types1...>>::value;
};

//##############################################################################
template <typename ... Types0, typename ... Types1>
struct TypeListConcatination<TypeList<Types0...>, TypeList<Types1...>>
{
  typedef TypeList<Types0..., Types1...> type;
};

//##############################################################################
template <>
struct TypeListRemoveDuplicates<TypeList<>>
{
  typedef TypeList<> type;
};

//##############################################################################
template <typename T, typename ... Types>
struct TypeListRemoveDuplicates<TypeList<T, Types...>>
{
  typedef std::conditional_t<
    TypeIsInTypes<T, Types...>::value,
    typename TypeListRemoveDuplicates<Types...>::type,
    typename TypeListConcatination<
      TypeList<T>,
      typename TypeListRemoveDuplicates<Types...>::type
    >::type
  > type;
};

//##############################################################################
template <typename ... Args>
std::tuple<Args...> VariadicToTuple(Args & ... args)
{
  return std::forward_as_tuple(args...);
}

//##############################################################################
template <typename T>
std::type_index GetTypeIndex(void)
{
  return std::type_index(typeid(T));
}

//##############################################################################
template <typename T>
T GetLowestHigherExponentOf2(T const & value)
{
  T result = 1;

  while (result < value)
    result <<= 1;

  return result;
}

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
bool KeyValuePair<Key, Value, Pred, KeyIsConst>::SortingPredicate::operator ()(
  KeyValuePair const & keyVal0, KeyValuePair const & keyVal1) const
{
  return Pred()(keyVal0.key, keyVal1.key);
}

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
bool KeyValuePair<Key, Value, Pred, KeyIsConst>::SortingPredicate::operator ()(
  KeyValuePair const & keyVal, Key const & key) const
{
  return Pred()(keyVal.key, key);
}

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
bool KeyValuePair<Key, Value, Pred, KeyIsConst>::SortingPredicate::operator ()(
  Key const & key, KeyValuePair const & keyVal) const
{
  return Pred()(key, keyVal.key);
}

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
bool KeyValuePair<Key, Value, Pred, KeyIsConst>::SortingPredicate::operator ()(
  Key const & key0, Key const & key1) const
{
  return Pred()(key0, key1);
}

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
template <typename ... Params>
KeyValuePair<Key, Value, Pred, KeyIsConst>::KeyValuePair(
  Key const & key, Params && ... params) :
  key(key),
  value(std::forward<Params &&>(params)...)
{}

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
KeyValuePair<Key, Value, Pred, KeyIsConst>::operator
  KeyValuePair<Key, Value, Pred, true> & (void)
{
  return *reinterpret_cast<KeyValuePair<Key, Value, Pred, true> *>(this);
}

//##############################################################################
template <typename Key, typename Value, typename Pred, bool KeyIsConst>
KeyValuePair<Key, Value, Pred, KeyIsConst>::operator
  KeyValuePair<Key, Value, Pred, true> const & (void) const
{
  return *reinterpret_cast<KeyValuePair<Key, Value, Pred, true> const *>(this);
}

#endif
