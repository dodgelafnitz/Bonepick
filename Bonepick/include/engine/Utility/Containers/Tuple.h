#ifndef ENGINE_UTILITY_CONTAINERS_TUPLE_H
#define ENGINE_UTILITY_CONTAINERS_TUPLE_H

#include "engine/Utility/TemplateTools.h"

//##############################################################################
template <typename T, typename ... Remainder>
class UniqueTuple
{
public:
  static_assert(!TypeIsInTypes<T, Remainder...>::value);

  UniqueTuple(void) = default;
  UniqueTuple(UniqueTuple const & typeSet) = default;
  UniqueTuple(UniqueTuple && typeSet) = default;
  UniqueTuple(T const & value, Remainder && ... remainder);
  UniqueTuple(T && value, Remainder && ... remainder);

  UniqueTuple & operator =(UniqueTuple const & typeSet) = default;
  UniqueTuple & operator =(UniqueTuple && typeSet) = default;

  template <typename U>
  std::enable_if_t<!std::is_same_v<T, U>, U> & Get(void);
  template <typename U>
  std::enable_if_t<!std::is_same_v<T, U>, U> const & Get(void) const;

  template <typename U>
  std::enable_if_t<std::is_same_v<T, U>, U> & Get(void);
  template <typename U>
  std::enable_if_t<std::is_same_v<T, U>, U> const & Get(void) const;

  bool operator ==(UniqueTuple const & typeSet) const;
  bool operator !=(UniqueTuple const & typeSet) const;

private:
  T                     value_;
  UniqueTuple<Remainder...> remainder_;
};

//##############################################################################
template <typename T>
class UniqueTuple<T>
{
public:
  UniqueTuple(void) = default;
  UniqueTuple(UniqueTuple const & typeSet) = default;
  UniqueTuple(UniqueTuple && typeSet) = default;
  UniqueTuple(T const & value);
  UniqueTuple(T && value);

  UniqueTuple & operator =(UniqueTuple const & typeSet) = default;
  UniqueTuple & operator =(UniqueTuple && typeSet) = default;

  template <typename U>
  U & Get(void);
  template <typename U>
  U const & Get(void) const;

  bool operator ==(UniqueTuple const & typeSet) const;
  bool operator !=(UniqueTuple const & typeSet) const;

private:
  T value_;
};

//##############################################################################
template <typename T, typename ... Remainder>
UniqueTuple<T, Remainder...>::UniqueTuple(T const & value, Remainder && ... remainder) :
  value_(value), remainder_(remainder...)
{}

//##############################################################################
template <typename T, typename ... Remainder>
UniqueTuple<T, Remainder...>::UniqueTuple(T && value, Remainder && ... remainder) :
  value_(std::move(value)), remainder_(remainder...)
{}

//##############################################################################
template <typename T, typename ... Remainder>
template <typename U>
std::enable_if_t<!std::is_same_v<T, U>, U> & UniqueTuple<T, Remainder...>::Get(
  void)
{
  return remainder_.Get<U>();
}

//##############################################################################
template <typename T, typename ... Remainder>
template <typename U>
std::enable_if_t<!std::is_same_v<T, U>, U> const &
  UniqueTuple<T, Remainder...>::Get(void) const
{
  return remainder_.Get<U>();
}

//##############################################################################
template <typename T, typename ... Remainder>
template <typename U>
std::enable_if_t<std::is_same_v<T, U>, U> & UniqueTuple<T, Remainder...>::Get(
  void)
{
  return value_;
}

//##############################################################################
template <typename T, typename ... Remainder>
template <typename U>
std::enable_if_t<std::is_same_v<T, U>, U> const &
  UniqueTuple<T, Remainder...>::Get(void) const
{
  return value_;
}

//##############################################################################
template <typename T, typename ... Remainder>
bool UniqueTuple<T, Remainder...>::operator ==(UniqueTuple const & typeSet) const
{
  return value_ == typeSet.value_ && remainder_ == typeSet.remainder_;
}

//##############################################################################
template <typename T, typename ... Remainder>
bool UniqueTuple<T, Remainder...>::operator !=(UniqueTuple const & typeSet) const
{
  return !(*this == typeSet);
}

//##############################################################################
template <typename T>
UniqueTuple<T>::UniqueTuple(T const & value) :
  value_(value)
{}

//##############################################################################
template <typename T>
UniqueTuple<T>::UniqueTuple(T && value) :
  value_(std::move(value))
{}

//##############################################################################
template <typename T>
template <typename U>
U & UniqueTuple<T>::Get(void)
{
  static_assert(std::is_same_v<T, U>);
  return value_;
}

//##############################################################################
template <typename T>
template <typename U>
U const & UniqueTuple<T>::Get(void) const
{
  static_assert(std::is_same_v<T, U>);
  return value_;
}

//##############################################################################
template <typename T>
bool UniqueTuple<T>::operator ==(UniqueTuple const & typeSet) const
{
  return value_ == typeSet.value_;
}

//##############################################################################
template <typename T>
bool UniqueTuple<T>::operator !=(UniqueTuple const & typeSet) const
{
  return !(*this == typeSet);
}


#endif
