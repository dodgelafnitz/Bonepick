#ifndef ENGINE_UTILITY_CONTAINERS_VARIANT_H
#define ENGINE_UTILITY_CONTAINERS_VARIANT_H

#include <typeindex>

#include "engine/Utility/Containers/DumbVariant.h"
#include "engine/Utility/Debug.h"
#include "engine/Utility/TemplateTools.h"

//##############################################################################
template <typename ... Types>
class Variant
{
public:
  Variant(void) = default;
  template <typename T>
  Variant(T const & value);
  Variant(Variant const & variant);
  Variant(Variant && variant);
  ~Variant(void);

  template <typename T, typename ... Params>
  T & Emplace(Params && ... params);

  template <typename T>
  bool IsType(void) const;

  template <typename T>
  T & Get(void);

  template <typename T>
  T const & Get(void) const;

  template <typename T>
  T * TryGet(void);

  template <typename T>
  T const * TryGet(void) const;

  void Clear(void);

  Variant & operator =(Variant const & variant);
  Variant & operator =(Variant && variant);

private:
  template <typename T, typename U, typename ... Remaining>
  void ClearHelper(void);
  template <typename T>
  void ClearHelper(void);

  template <typename T, typename U, typename ... Remaining>
  void CopyConstructHelper(Variant const & variant);
  template <typename T>
  void CopyConstructHelper(Variant const & variant);

  template <typename T, typename U, typename ... Remaining>
  void MoveConstructHelper(Variant & variant);
  template <typename T>
  void MoveConstructHelper(Variant & variant);

  template <typename T, typename U, typename ... Remaining>
  void CopyHelper(Variant const & variant);
  template <typename T>
  void CopyHelper(Variant const & variant);

  template <typename T, typename U, typename ... Remaining>
  void MoveHelper(Variant & variant);
  template <typename T>
  void MoveHelper(Variant & variant);

  std::type_index                          type_ = GetTypeIndex<void>();
  DumbVariant<GetMaxSize<Types...>::value> data_;
};

//##############################################################################
template <typename ... Types>
template <typename T>
Variant<Types...>::Variant(T const & value)
{
  static_assert(TypeIsInTypes<T, Types...>::value);

  Emplace<T>(value);
}

//##############################################################################
template <typename ... Types>
Variant<Types...>::Variant(Variant const & variant) :
  type_(variant.type_)
{
  CopyConstructHelper<Types...>(variant);
}

//##############################################################################
template <typename ... Types>
Variant<Types...>::Variant(Variant && variant) :
  type_(variant.type_)
{
  MoveConstructHelper<Types...>(variant);
}

//##############################################################################
template <typename ... Types>
Variant<Types...>::~Variant(void)
{
  Clear();
}

//##############################################################################
template <typename ... Types>
template <typename T, typename ... Params>
T & Variant<Types...>::Emplace(Params && ... params)
{
  static_assert(TypeIsInTypes<T, Types...>::value);

  if (type_ != GetTypeIndex<T>())
  {
    Clear();
    type_ = GetTypeIndex<T>();
    return data_.Emplace<T>(std::forward<Params &&>(params) ...);
  }
  else
    return data_.Get<T>() = T(std::forward<Params &&>(params) ...);
}

//##############################################################################
template <typename ... Types>
template <typename T>
bool Variant<Types...>::IsType(void) const
{
  static_assert(TypeIsInTypes<T, Types...>::value);

  return type_ == GetTypeIndex<T>();
}

//##############################################################################
template <typename ... Types>
template <typename T>
T & Variant<Types...>::Get(void)
{
  ASSERT(type_ == GetTypeIndex<T>());

  return data_.Get<T>();
}

//##############################################################################
template <typename ... Types>
template <typename T>
T const & Variant<Types...>::Get(void) const
{
  ASSERT(type_ == GetTypeIndex<T>());

  return data_.Get<T>();
}

//##############################################################################
template <typename ... Types>
template <typename T>
T * Variant<Types...>::TryGet(void)
{
  if (type_ == GetTypeIndex<T>())
    return &data_.Get<T>();
  else
    return nullptr;
}

//##############################################################################
template <typename ... Types>
template <typename T>
T const * Variant<Types...>::TryGet(void) const
{
  if (type_ == GetTypeIndex<T>())
    return &data_.Get<T>();
  else
    return nullptr;
}

//##############################################################################
template <typename ... Types>
void Variant<Types...>::Clear(void)
{
  ClearHelper<Types...>();
}

//##############################################################################
template <typename ... Types>
Variant<Types...> & Variant<Types...>::operator =(Variant const & variant)
{
  if (this == &variant)
    return *this;

  if (type_ == variant.type_)
    CopyHelper<Types...>(variant);
  else
  {
    Clear();
    CopyConstructHelper<Types...>(variant);
  }

  return *this;
}

//##############################################################################
template <typename ... Types>
Variant<Types...> & Variant<Types...>::operator =(Variant && variant)
{
  if (this == &variant)
    return *this;

  if (type_ == variant.type_)
    MoveHelper<Types...>(variant);
  else
  {
    Clear();
    MoveConstructHelper<Types...>(variant);
  }

  return *this;
}

//##############################################################################
template <typename ... Types>
template <typename T, typename U, typename ... Remaining>
void Variant<Types...>::CopyConstructHelper(Variant const & variant)
{
  if (variant.type_ == GetTypeIndex<T>())
  {
    data_.Emplace<T>(variant.Get<T>());
    type_ = variant.type_;
  }
  else
    CopyConstructHelper<U, Remaining...>(variant);
}

//##############################################################################
template <typename ... Types>
template <typename T>
void Variant<Types...>::CopyConstructHelper(Variant const & variant)
{
  if (variant.type_ == GetTypeIndex<T>())
  {
    data_.Emplace<T>(variant.Get<T>());
    type_ = variant.type_;
  }
}

//##############################################################################
template <typename ... Types>
template <typename T, typename U, typename ... Remaining>
void Variant<Types...>::MoveConstructHelper(Variant & variant)
{
  if (type_ == GetTypeIndex<T>())
  {
    data_.Emplace<T>(std::move(variant.Get<T>()));
    type_ = variant.type_;
  }
  else
    MoveConstructHelper<U, Remaining...>(variant);
}

//##############################################################################
template <typename ... Types>
template <typename T>
void Variant<Types...>::MoveConstructHelper(Variant & variant)
{
  if (type_ == GetTypeIndex<T>())
  {
    data_.Emplace<T>(std::move(variant.Get<T>()));
    type_ = variant.type_;
  }
}

//##############################################################################
template <typename ... Types>
template <typename T, typename U, typename ... Remaining>
void Variant<Types...>::ClearHelper(void)
{
  if (type_ == GetTypeIndex<T>())
    data_.Clear<T>();
  else
    ClearHelper<U, Remaining...>();
}

//##############################################################################
template <typename ... Types>
template <typename T>
void Variant<Types...>::ClearHelper(void)
{
  if (type_ == GetTypeIndex<T>())
  {
    data_.Clear<T>();
    type_ = GetTypeIndex<void>();
  }
}

//##############################################################################
template <typename ... Types>
template <typename T, typename U, typename ... Remaining>
void Variant<Types...>::CopyHelper(Variant const & variant)
{
  if (type_ == GetTypeIndex<T>())
  {
    data_.Get<T>() = variant.Get<T>();
    type_ = variant.type_;
  }
  else
    CopyHelper<U, Remaining...>(variant);
}

//##############################################################################
template <typename ... Types>
template <typename T>
void Variant<Types...>::CopyHelper(Variant const & variant)
{
  if (type_ == GetTypeIndex<T>())
  {
    data_.Get<T>() = variant.Get<T>();
    type_ = variant.type_;
  }
}

//##############################################################################
template <typename ... Types>
template <typename T, typename U, typename ... Remaining>
void Variant<Types...>::MoveHelper(Variant & variant)
{
  if (type_ == GetTypeIndex<T>())
  {
    data_.Get<T>() = std::move(variant.Get<T>());
    type_ = variant.type_;
  }
  else
    MoveHelper<U, Remaining...>(variant);
}

//##############################################################################
template <typename ... Types>
template <typename T>
void Variant<Types...>::MoveHelper(Variant & variant)
{
  if (type_ == GetTypeIndex<T>())
  {
    data_.Get<T>() = std::move(variant.Get<T>());
    type_ = variant.type_;
  }
}

#endif
