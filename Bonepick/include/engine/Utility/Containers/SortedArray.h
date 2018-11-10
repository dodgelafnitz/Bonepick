#ifndef ENGINE_UTILITY_CONTAINERS_SortedArrayBase_H
#define ENGINE_UTILITY_CONTAINERS_SortedArrayBase_H

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>

#include "engine/Utility/Containers/Array.h"
#include "engine/Utility/Debug.h"
#include "engine/Utility/TemplateTools.h"
#include "engine/Utility/Typedefs.h"

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
class SortedArrayBase;

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
class SortedArrayBase
{
public:
  typedef typename ConditionalAddConst<UseType, AlwaysConst>::Type Type;
  typedef typename std::add_const<UseType>::type ConstType;

  SortedArrayBase(void) = default;
  SortedArrayBase(SortedArrayBase const & arr) = default;
  SortedArrayBase(SortedArrayBase && arr) = default;
  SortedArrayBase(std::initializer_list<StoreType> const & init);

  void Reserve(SizeType index);

  template <typename ... Params>
  void Emplace(Params && ... params);

  bool Empty(void) const;
  SizeType Size(void) const;

  void Erase(SizeType index);
  void Erase(Type * location);

  void Clear(void);

  template <typename U>
  Type * Find(U const & key);
  template <typename U>
  ConstType * Find(U const & key) const;

  SizeType GetIndex(ConstType * location) const;

  template <typename U>
  bool Contains(U const & key) const;

  SortedArrayBase & operator =(SortedArrayBase const & arr) = default;
  SortedArrayBase & operator =(SortedArrayBase && arr) = default;

  Type & operator [](SizeType index);
  ConstType & operator [](SizeType index) const;

  Type * Begin(void);
  Type * End(void);
  ConstType * Begin(void) const;
  ConstType * End(void) const;

private:
  template <typename U, typename V>
  bool AreEqual(U const & key0, V const & key1);
  template <typename U>
  SizeType FindDesiredLocation(U const & key) const;

  Array<StoreType, SizeType> data_;
};

//##############################################################################
template <typename Key, typename Value, typename Pred = std::less<Key>,
  typename SizeType = int>
using ArrayMap = SortedArrayBase<KeyValuePair<Key, Value, Pred, false>,
  KeyValuePair<Key, Value, Pred, true>,
  typename KeyValuePair<Key, Value, Pred, false>::SortingPredicate,
  false, SizeType>;

//##############################################################################
template <typename Key, typename Pred = std::less<Key>,
  typename SizeType = int>
using SortedArray = SortedArrayBase<Key, Key, Pred, true, SizeType>;

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  SortedArrayBase(std::initializer_list<StoreType> const & init)
{
  for (auto && value : init)
    Emplace(value);
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
void SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Reserve(
  SizeType capacity)
{
  data_.Reserve(capacity);
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
template <typename ... Params>
void SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Emplace(
  Params && ... params)
{
  StoreType value(std::forward<Params &&>(params)...);

  SizeType const index = FindDesiredLocation(value);

  ASSERT(index == data_.Size() || Pred()(value, data_[index]),
    "Value is already in the sorted array");

  data_.Emplace(data_.Begin() + index, std::move(value));
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
SizeType
  SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Size(void)
  const
{
  return SizeType(data_.Size());
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
bool
  SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Empty(void)
  const
{
  return data_.Size() == SizeType(0);
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
void SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Erase(
  SizeType index)
{
  data_.Erase(index);
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
void SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Erase(
  Type * location)
{
  data_.Erase(location);
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
void
  SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Clear(void)
{
  return data_.Clear();
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
template <typename U>
typename SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  Type * SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  Find(U const & key)
{
  SizeType index = FindDesiredLocation(key);

  if (index < Size() && AreEqual(data_[index], key))
    return &static_cast<UseType &>(data_[index]);
  else
    return nullptr;
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
template <typename U>
typename SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  ConstType * SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  Find(U const & key) const
{
  return const_cast<SortedArrayBase *>(this)->Find(key);
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
SizeType SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  GetIndex(ConstType * location) const
{
  return data_.GetIndex(location);
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
template <typename U>
bool SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Contains(
  U const & key) const
{
  return Find(key) != nullptr;
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
typename SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  Type & SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  operator [](SizeType index)
{
  ASSERT(index < Size());

  return data_[index];
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
typename SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  ConstType & SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  operator [](SizeType index) const
{
  ASSERT(index < Size());

  return data_[index];
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
typename SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  Type * SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  Begin(void)
{
  return &static_cast<UseType &>(*data_.Begin());
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
typename SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  Type * SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  End(void)
{
  return Begin() + Size();
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
typename SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  ConstType * SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  Begin(void) const
{
  return const_cast<SortedArrayBase *>(this)->Begin();
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
typename SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  ConstType * SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  End(void) const
{
  return const_cast<SortedArrayBase *>(this)->End();
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
template <typename U, typename V>
bool SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::AreEqual(
  U const & key0, V const & key1)
{
  Pred pred;

  return !pred(key0, key1) && !pred(key1, key0);
}

//##############################################################################
template <typename StoreType, typename UseType, typename Pred, bool AlwaysConst,
  typename SizeType>
template <typename U>
SizeType SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
  FindDesiredLocation(U const & key) const
{
  SizeType index = 0;
  SizeType delta = GetLowestHigherExponentOf2(Size());

  while (delta != 0)
  {
    SizeType nextIndex = index + delta;
    if (nextIndex <= Size() && Pred()(data_[nextIndex - 1], key))
      index += delta;

    delta >>= 1;
  }

  return index;
}

namespace std
{
  //############################################################################
  template <typename StoreType, typename UseType, typename Pred,
    bool AlwaysConst, typename SizeType>
  typename
    SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Type *
    begin(
    SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType> & arr)
  {
    return arr.Begin();
  }
  
  //############################################################################
  template <typename StoreType, typename UseType, typename Pred,
    bool AlwaysConst, typename SizeType>
  typename
    SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
    ConstType * begin(
    SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType> const &
    arr)
  {
    return arr.Begin();
  }

  //############################################################################
  template <typename StoreType, typename UseType, typename Pred,
    bool AlwaysConst, typename SizeType>
  typename
    SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::Type *
    end(
    SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType> & arr)
  {
    return arr.End();
  }
  
  //############################################################################
  template <typename StoreType, typename UseType, typename Pred,
    bool AlwaysConst, typename SizeType>
  typename
    SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType>::
    ConstType * end(
    SortedArrayBase<StoreType, UseType, Pred, AlwaysConst, SizeType> const &
    arr)
  {
    return arr.End();
  }
}

#endif
