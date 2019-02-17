#ifndef ENGINE_UTILITY_CONTAINERS_DUMBVARIANT_H
#define ENGINE_UTILITY_CONTAINERS_DUMBVARIANT_H

#include "engine/utility/Typedefs.h"

//##############################################################################
template <int Size>
class DumbVariant
{
public:
  DumbVariant(void) = default;
  DumbVariant(DumbVariant const &) = delete;

  template <typename T, typename ... Params>
  T & Emplace(Params && ... params);

  template <typename T>
  void Clear(void);

  template <typename T>
  T & Get(void);

  template <typename T>
  T const & Get(void) const;

private:
  byte data_[Size];
};

//##############################################################################
template <int Size>
template <typename T, typename ... Params>
T & DumbVariant<Size>::Emplace(Params && ... params)
{
  static_assert(sizeof(T) <= Size);

  new(reinterpret_cast<T *>(data_)) T(std::forward<Params &&>(params) ...);

  return Get<T>();
}

//##############################################################################
template <int Size>
template <typename T>
void DumbVariant<Size>::Clear(void)
{
  static_assert(sizeof(T) <= Size);

  reinterpret_cast<T *>(data_)->~T();
}

//##############################################################################
template <int Size>
template <typename T>
T & DumbVariant<Size>::Get(void)
{
  static_assert(sizeof(T) <= Size);

  return *reinterpret_cast<T *>(data_);
}

//##############################################################################
template <int Size>
template <typename T>
T const & DumbVariant<Size>::Get(void) const
{
  static_assert(sizeof(T) <= Size);

  return *reinterpret_cast<T const *>(data_);
}

#endif
