#ifndef ENGINE_UTILITY_CONTAINERS_TYPESET_H
#define ENGINE_UTILITY_CONTAINERS_TYPESET_H

#include "engine/Utility/TemplateTools.h"

//##############################################################################
template <typename T, typename ... Remainder>
class TypeSet
{
public:
  static_assert(!TypeIsInTypes<T, Remainder...>::value);

  TypeSet(void) = default;
  TypeSet(TypeSet const & typeSet) = default;
  TypeSet(TypeSet && typeSet) = default;
  TypeSet(T const & value, Remainder && ... remainder);
  TypeSet(T && value, Remainder && ... remainder);

  TypeSet & operator =(TypeSet const & typeSet) = default;
  TypeSet & operator =(TypeSet && typeSet) = default;

  template <typename U>
  std::enable_if_t<!std::is_same_v<T, U>, U> & Get(void);
  template <typename U>
  std::enable_if_t<!std::is_same_v<T, U>, U> const & Get(void) const;

  template <typename U>
  std::enable_if_t<std::is_same_v<T, U>, U> & Get(void);
  template <typename U>
  std::enable_if_t<std::is_same_v<T, U>, U> const & Get(void) const;

  bool operator ==(TypeSet const & typeSet) const;
  bool operator !=(TypeSet const & typeSet) const;

private:
  T                     value_;
  TypeSet<Remainder...> remainder_;
};

//##############################################################################
template <typename T>
class TypeSet<T>
{
public:
  TypeSet(void) = default;
  TypeSet(TypeSet const & typeSet) = default;
  TypeSet(TypeSet && typeSet) = default;
  TypeSet(T const & value);
  TypeSet(T && value);

  TypeSet & operator =(TypeSet const & typeSet) = default;
  TypeSet & operator =(TypeSet && typeSet) = default;

  template <typename U>
  U & Get(void);
  template <typename U>
  U const & Get(void) const;

  bool operator ==(TypeSet const & typeSet) const;
  bool operator !=(TypeSet const & typeSet) const;

private:
  T value_;
};

//##############################################################################
template <typename T, typename ... Remainder>
TypeSet<T, Remainder...>::TypeSet(T const & value, Remainder && ... remainder) :
  value_(value), remainder_(remainder...)
{}

//##############################################################################
template <typename T, typename ... Remainder>
TypeSet<T, Remainder...>::TypeSet(T && value, Remainder && ... remainder) :
  value_(std::move(value)), remainder_(remainder...)
{}

//##############################################################################
template <typename T, typename ... Remainder>
template <typename U>
std::enable_if_t<!std::is_same_v<T, U>, U> & TypeSet<T, Remainder...>::Get(
  void)
{
  return remainder_.Get<U>();
}

//##############################################################################
template <typename T, typename ... Remainder>
template <typename U>
std::enable_if_t<!std::is_same_v<T, U>, U> const &
  TypeSet<T, Remainder...>::Get(void) const
{
  return remainder_.Get<U>();
}

//##############################################################################
template <typename T, typename ... Remainder>
template <typename U>
std::enable_if_t<std::is_same_v<T, U>, U> & TypeSet<T, Remainder...>::Get(
  void)
{
  return value_;
}

//##############################################################################
template <typename T, typename ... Remainder>
template <typename U>
std::enable_if_t<std::is_same_v<T, U>, U> const &
  TypeSet<T, Remainder...>::Get(void) const
{
  return value_;
}

//##############################################################################
template <typename T, typename ... Remainder>
bool TypeSet<T, Remainder...>::operator ==(TypeSet const & typeSet) const
{
  return value_ == typeSet.value_ && remainder_ == typeSet.remainder_;
}

//##############################################################################
template <typename T, typename ... Remainder>
bool TypeSet<T, Remainder...>::operator !=(TypeSet const & typeSet) const
{
  return !(*this == typeSet);
}

//##############################################################################
template <typename T>
TypeSet<T>::TypeSet(T const & value) :
  value_(value)
{}

//##############################################################################
template <typename T>
TypeSet<T>::TypeSet(T && value) :
  value_(std::move(value))
{}

//##############################################################################
template <typename T>
template <typename U>
U & TypeSet<T>::Get(void)
{
  static_assert(std::is_same_v<T, U>);
  return value_;
}

//##############################################################################
template <typename T>
template <typename U>
U const & TypeSet<T>::Get(void) const
{
  static_assert(std::is_same_v<T, U>);
  return value_;
}

//##############################################################################
template <typename T>
bool TypeSet<T>::operator ==(TypeSet const & typeSet) const
{
  return value_ == typeSet.value_;
}

//##############################################################################
template <typename T>
bool TypeSet<T>::operator !=(TypeSet const & typeSet) const
{
  return !(*this == typeSet);
}


#endif
