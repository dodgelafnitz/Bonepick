#ifndef ENGINE_UTILITY_CONTAINERS_OPTIONAL_H
#define ENGINE_UTILITY_CONTAINERS_OPTIONAL_H

#include <optional>
#include <type_traits>

#include "engine/Utility/Debug.h"
#include "engine/Utility/TemplateTools.h"

//##############################################################################
template <typename T>
class Optional
{
public:
  Optional(void) = default;

  Optional(Optional const & optional) = default;
  Optional(Optional && optional) = default;

  template <typename... Params, typename = std::enable_if_t<
    !TypeIsDecayedTypes<Optional<T>, Params...>::value>>
  Optional(Params && ... params);

  ~Optional(void) = default;

  T & operator *(void);
  T const & operator *(void) const;

  T * operator ->(void);
  T const * operator ->(void) const;

  Optional & operator =(Optional const & optional);
  Optional & operator =(Optional && optional);

  template <typename ... Params>
  T & Emplace(Params && ... params);

  T * Ptr(void);
  T const * Ptr(void) const;

  void Clear(void);

private:
  std::optional<T> data_;
};

//##############################################################################
template <typename T>
template <typename... Params, typename>
Optional<T>::Optional(Params && ... params) :
  data_(std::in_place_t(), std::forward<Params &&>(params)...)
{}

//##############################################################################
template <typename T>
T & Optional<T>::operator *(void)
{
  ASSERT(data_.has_value());
  return data_.value();
}

//##############################################################################
template <typename T>
T const & Optional<T>::operator *(void) const
{
  ASSERT(data_.has_value());
  return data_.value();
}

//##############################################################################
template <typename T>
T * Optional<T>::operator ->(void)
{
  ASSERT(data_.has_value());
  return &data_.value();
}

//##############################################################################
template <typename T>
T const * Optional<T>::operator ->(void) const
{
  ASSERT(data_.has_value());
  return &data_.value();
}

//##############################################################################
template <typename T>
Optional<T> & Optional<T>::operator =(Optional const & optional)
{
  data_ = optional.data_;
  return *this;
}

//##############################################################################
template <typename T>
Optional<T> & Optional<T>::operator =(Optional && optional)
{
  data_ = std::move(optional.data_);
  return *this;
}

//##############################################################################
template <typename T>
template <typename ... Params>
T & Optional<T>::Emplace(Params && ... params)
{
  if (data_.has_value())
    data_.value() = T(std::forward<Params &&>(params)...);
  else
    data_.emplace(std::forward<Params &&>(params)...);

  return data_.value();
}

//##############################################################################
template <typename T>
T * Optional<T>::Ptr(void)
{
  if (data_.has_value())
    return &data_.value();
  else
    return nullptr;
}

//##############################################################################
template <typename T>
T const * Optional<T>::Ptr(void) const
{
  if (data_.has_value())
    return &data_.value();
  else
    return nullptr;
}

//##############################################################################
template <typename T>
void Optional<T>::Clear(void)
{
  data_.reset();
}

#endif
