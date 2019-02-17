#ifndef ENGINE_UTILITY_CONTAINERS_TUPLE_H
#define ENGINE_UTILITY_CONTAINERS_TUPLE_H

#include "engine/utility/TemplateTools.h"

#include <tuple>

//##############################################################################
template <typename ... Types>
class Tuple
{
public:
  Tuple(void) = default;
  Tuple(Tuple const & typeSet) = default;
  Tuple(Tuple && typeSet) = default;
  Tuple(Types && ... values);

  Tuple & operator =(Tuple const & typeSet) = default;
  Tuple & operator =(Tuple && typeSet) = default;

  template <typename T>
  T & Get(void);
  template <typename T>
  T const & Get(void) const;

  template <int Index>
  std::tuple_element_t<Index, std::tuple<Types...>> & Get(void);
  template <int Index>
  std::tuple_element_t<Index, std::tuple<Types...>> const & Get(void) const;

  bool operator ==(Tuple const & typeSet) const;
  bool operator !=(Tuple const & typeSet) const;

private:
  std::tuple<Types...> data_;
};

//##############################################################################
template <typename ... Types>
Tuple<Types...>::Tuple(Types && ... data) :
  data_(std::forward<Types &&>(data)...)
{}

//##############################################################################
template <typename ... Types>
template <typename T>
T & Tuple<Types...>::Get(void)
{
  return std::get<T>(data_);
}

//##############################################################################
template <typename ... Types>
template <typename T>
T const & Tuple<Types...>::Get(void) const
{
  return std::get<T>(data_);
}

//##############################################################################
template <typename ... Types>
template <int Index>
std::tuple_element_t<Index, std::tuple<Types...>> & Tuple<Types...>::Get(void)
{
  return std::get<Index>(data_);
}

//##############################################################################
template <typename ... Types>
template <int Index>
std::tuple_element_t<Index, std::tuple<Types...>> const &
  Tuple<Types...>::Get(void) const
{
  return std::get<Index>(data_);
}

//##############################################################################
template <typename ... Types>
bool Tuple<Types...>::operator ==(Tuple const & typeSet) const
{
  return data_ == typeSet.data_;
}

//##############################################################################
template <typename ... Types>
bool Tuple<Types...>::operator !=(Tuple const & typeSet) const
{
  return !(*this == typeSet);
}

//##############################################################################
template <bool Constraint, typename ... Types>
using ConstrainedTuple = std::enable_if_t<Constraint, Tuple<Types...>>;

//##############################################################################
template <typename ... Types>
using UniqueTuple = ConstrainedTuple<TypesAreUnique<Types...>::value, Types...>;

#endif
