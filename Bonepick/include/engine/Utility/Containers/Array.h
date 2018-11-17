#ifndef ENGINE_UTILITY_CONTAINERS_ARRAY_H
#define ENGINE_UTILITY_CONTAINERS_ARRAY_H

#include <initializer_list>
#include <vector>

#include "engine/Utility/Debug.h"
#include "engine/Utility/Typedefs.h"

//##############################################################################
template <typename T, typename SizeType = int>
class Array
{
public:
  Array(void) = default;
  Array(T const & value, SizeType count);
  Array(T const * values, SizeType count);
  Array(std::initializer_list<T> const & initList);
  Array(Array const & arr) = default;
  Array(Array && arr) = default;
  ~Array(void) = default;

  void Reserve(SizeType capacity);
  void Resize(SizeType size);

  void Erase(T const * location);
  void Erase(SizeType index);

  SizeType GetIndex(void const * location) const;

  template <typename ... Params>
  void Emplace(T * location, Params && ... params);

  template <typename ... Params>
  void EmplaceBack(Params && ... params);

  template <typename ... Params>
  void EmplaceFront(Params && ... params);

  T & GetBack(void);
  T & GetFront(void);
  T const & GetBack(void) const;
  T const & GetFront(void) const;

  void PopBack(void);
  void PopFront(void);

  bool Empty(void) const;
  SizeType Size(void) const;

  void Clear(void);

  T * FindFirst(T const & value);
  T * FindLast(T const & value);
  T const * FindFirst(T const & value) const;
  T const * FindLast(T const & value) const;

  bool Contains(T const & value) const;

  template <typename Pred>
  T * FindFirst(Pred const & pred = Pred());
  template <typename Pred>
  T * FindLast(Pred const & pred = Pred());
  template <typename Pred>
  T const * FindFirst(Pred const & pred = Pred()) const;
  template <typename Pred>
  T const * FindLast(Pred const & pred = Pred()) const;

  template <typename Pred>
  bool Contains(Pred const & pred = Pred()) const;

  Array & operator =(Array const & arr) = default;
  Array & operator =(Array && arr) = default;

  bool operator ==(Array const & arr) const;
  bool operator !=(Array const & arr) const;

  T & operator [](SizeType index);
  T const & operator [](SizeType index) const;

  T * Begin(void);
  T * End(void);
  T const * Begin(void) const;
  T const * End(void) const;

private:
  T const * AdjustForBase(void const * location) const;

  std::vector<T> data_;
};

//##############################################################################
template <typename T, typename SizeType>
Array<T, SizeType>::Array(T const & value, SizeType count)
  : data_(count, value)
{}

//##############################################################################
template <typename T, typename SizeType>
Array<T, SizeType>::Array(T const * value, SizeType count)
  : data_(value, value + count)
{}

//##############################################################################
template <typename T, typename SizeType>
Array<T, SizeType>::Array(std::initializer_list<T> const & initList)
  : data_(initList)
{}

//##############################################################################
template <typename T, typename SizeType>
void Array<T, SizeType>::Reserve(SizeType capacity)
{
  data_.reserve(capacity);
}

//##############################################################################
template <typename T, typename SizeType>
void Array<T, SizeType>::Resize(SizeType size)
{
  data_.resize(size);
}

//##############################################################################
template <typename T, typename SizeType>
void Array<T, SizeType>::Erase(T const * value)
{
  Erase(GetIndex(value));
}

//##############################################################################
template <typename T, typename SizeType>
void Array<T, SizeType>::Erase(SizeType index)
{
  ASSERT(!Empty());
  ASSERT(index < Size());
  ASSERT(index >= 0);

  data_.erase(data_.begin() + index);
}

//##############################################################################
template <typename T, typename SizeType>
SizeType Array<T, SizeType>::GetIndex(void const * location) const
{
  T const * locationBase = AdjustForBase(location);

  SizeType const index =
    SizeType(reinterpret_cast<T const *>(locationBase) - Begin());

  ASSERT(index < Size());
  ASSERT(index >= 0);

  return index;
}

//##############################################################################
template <typename T, typename SizeType>
template <typename ... Params>
void Array<T, SizeType>::Emplace(T * location, Params && ... params)
{
  SizeType const index = SizeType(location - Begin());
  ASSERT(index <= Size());
  ASSERT(SizeType(0) <= index);

  data_.emplace(data_.begin() + index, std::forward<Params &&>(params) ...);
}

//##############################################################################
template <typename T, typename SizeType>
template <typename ... Params>
void Array<T, SizeType>::EmplaceBack(Params && ... params)
{
  data_.emplace_back(std::forward<Params &&>(params) ...);
}

//##############################################################################
template <typename T, typename SizeType>
template <typename ... Params>
void Array<T, SizeType>::EmplaceFront(Params && ... params)
{
  data_.emplace_front(std::forward<Params &&>(params) ...);
}

//##############################################################################
template <typename T, typename SizeType>
T & Array<T, SizeType>::GetBack(void)
{
  return data_.back();
}

//##############################################################################
template <typename T, typename SizeType>
T & Array<T, SizeType>::GetFront(void)
{
  return data_.front();
}

//##############################################################################
template <typename T, typename SizeType>
T const & Array<T, SizeType>::GetBack(void) const
{
  return data_.back();
}

//##############################################################################
template <typename T, typename SizeType>
T const & Array<T, SizeType>::GetFront(void) const
{
  return data_.front();
}

//##############################################################################
template <typename T, typename SizeType>
void Array<T, SizeType>::PopBack(void)
{
  if (!Empty())
    data_.pop_back();
}

//##############################################################################
template <typename T, typename SizeType>
void Array<T, SizeType>::PopFront(void)
{
  if (!Empty())
    data_.pop_front();
}

//##############################################################################
template <typename T, typename SizeType>
bool Array<T, SizeType>::Empty(void) const
{
  return data_.empty();
}

//##############################################################################
template <typename T, typename SizeType>
SizeType Array<T, SizeType>::Size(void) const
{
  return SizeType(data_.size());
}

//##############################################################################
template <typename T, typename SizeType>
bool Array<T, SizeType>::operator ==(Array const & arr) const
{
  return data_ == arr.data_;
}

//##############################################################################
template <typename T, typename SizeType>
bool Array<T, SizeType>::operator !=(Array const & arr) const
{
  return !(*this == arr);
}

//##############################################################################
template <typename T, typename SizeType>
T & Array<T, SizeType>::operator [](SizeType index)
{
  ASSERT(Size() > index);
  ASSERT(index >= SizeType(0));

  return data_[index];
}

//##############################################################################
template <typename T, typename SizeType>
T const & Array<T, SizeType>::operator [](SizeType index) const
{
  ASSERT(Size() > index);
  ASSERT(index >= SizeType(0));

  return data_[index];
}

//##############################################################################
template <typename T, typename SizeType>
void Array<T, SizeType>::Clear(void)
{
  data_.clear();
}

//##############################################################################
template <typename T, typename SizeType>
T * Array<T, SizeType>::FindFirst(T const & value)
{
  for (T & element : *this)
  {
    if (element == value)
      return &element;
  }

  return nullptr;
}

//##############################################################################
template <typename T, typename SizeType>
T * Array<T, SizeType>::FindLast(T const & value)
{
  for (int i = data_.size() - 1; i >= 0; --i)
  {
    if (data_[i] == value)
      return = &data_[i];
  }

  return nullptr;
}

//##############################################################################
template <typename T, typename SizeType>
T const * Array<T, SizeType>::FindFirst(T const & value) const
{
  return const_cast<Array<T, SizeType> *>(this)->FindFirst(value);
}

//##############################################################################
template <typename T, typename SizeType>
T const * Array<T, SizeType>::FindLast(T const & value) const
{
  return const_cast<Array<T, SizeType> *>(this)->FindLast(value);
}

//##############################################################################
template <typename T, typename SizeType>
bool Array<T, SizeType>::Contains(T const & value) const
{
  return FindFirst(value) != nullptr;
}

//##############################################################################
template <typename T, typename SizeType>
template <typename Pred>
T * Array<T, SizeType>::FindFirst(Pred const & pred)
{
  for (T & element : *this)
  {
    if (pred(element))
      return &element;
  }

  return nullptr;
}

//##############################################################################
template <typename T, typename SizeType>
template <typename Pred>
T * Array<T, SizeType>::FindLast(Pred const & pred)
{
  for (int i = data_.size() - 1; i >= 0; --i)
  {
    if (pred(data_[i]))
      return &data_[i];
  }

  return nullptr;
}

//##############################################################################
template <typename T, typename SizeType>
template <typename Pred>
T const * Array<T, SizeType>::FindFirst(Pred const & pred) const
{
  return const_cast<Array<T, SizeType> *>(this)->FindFirst<Pred>(pred);
}

//##############################################################################
template <typename T, typename SizeType>
template <typename Pred>
T const * Array<T, SizeType>::FindLast(Pred const & pred) const
{
  return const_cast<Array<T, SizeType> *>(this)->FindLast<Pred>(pred);
}

//##############################################################################
template <typename T, typename SizeType>
template <typename Pred>
bool Array<T, SizeType>::Contains(Pred const & pred) const
{
  return FindFirst<Pred>(pred) != nullptr;
}

//##############################################################################
template <typename T, typename SizeType>
T * Array<T, SizeType>::Begin(void)
{
  return data_.data();
}

//##############################################################################
template <typename T, typename SizeType>
T * Array<T, SizeType>::End(void)
{
  return Begin() + Size();
}

//##############################################################################
template <typename T, typename SizeType>
T const * Array<T, SizeType>::Begin(void) const
{
  return data_.data();

}

//##############################################################################
template <typename T, typename SizeType>
T const * Array<T, SizeType>::End(void) const
{
  return Begin() + Size();
}

//##############################################################################
template <typename T, typename SizeType>
T const * Array<T, SizeType>::AdjustForBase(void const * location) const
{
  static_assert(sizeof(std::size_t) == sizeof(void const *));

  std::size_t const baseValue     = reinterpret_cast<std::size_t>(Begin());
  std::size_t const locationValue = reinterpret_cast<std::size_t>(location);

  std::size_t const offset = locationValue - baseValue;
  return Begin() + (offset / sizeof(T));
}

namespace std
{
  //############################################################################
  template <typename T, typename SizeType>
  T * begin(Array<T, SizeType> & arr)
  {
    return arr.Begin();
  }

  //############################################################################
  template <typename T, typename SizeType>
  T * end(Array<T, SizeType> & arr)
  {
    return arr.End();
  }

  //############################################################################
  template <typename T, typename SizeType>
  T * begin(Array<T, SizeType> const & arr)
  {
    return arr.Begin();
  }

  //############################################################################
  template <typename T, typename SizeType>
  T * end(Array<T, SizeType> const & arr)
  {
    return arr.End();
  }
}

#endif
