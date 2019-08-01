#ifndef ENGINE_UTILITY_BLOB_H
#define ENGINE_UTILITY_BLOB_H

#include <memory>
#include <type_traits>
#include <vector>

#include "utility/Debug.h"
#include "utility/Typedefs.h"

//##############################################################################
class Blob
{
public:
  Blob(void) = default;
  Blob(Blob const & blob) = default;
  Blob(Blob && blob) = default;
  Blob(unsigned size);
  Blob(void const * data, unsigned size);
  ~Blob(void);

  template <typename T>
  T & Get(unsigned offset);

  template <typename T>
  T const & Get(unsigned offset) const;

  void * Get(unsigned offset);
  void const * Get(unsigned offset) const;

  template <typename T>
  void Set(unsigned offset, T const & value);

  void Set(unsigned offset, void const * data, unsigned count);

  void Resize(unsigned size);

  Blob & operator =(Blob const & blob) = default;
  Blob & operator =(Blob && blob) = default;

  void Clear(void);

  void * Data(void);
  void const * Data(void) const;
  unsigned Size(void) const;

private:
  std::shared_ptr<std::vector<char>> data_;
};

//##############################################################################
template <typename T>
T & Blob::Get(unsigned offset)
{
  ASSERT(offset + sizeof(T) <= size_);

  return *reinterpret_cast<T *>(Get(offset));
}

//##############################################################################
template <typename T>
T const & Blob::Get(unsigned offset) const
{
  return const_cast<Blob *>(this)->Get<T>(offset);
}

//##############################################################################
template <typename T>
void Blob::Set(unsigned offset, T const & value)
{
  static_assert(std::is_trivially_destructible<T>::value);

  ASSERT(offset + sizeof(T) <= size_);

  Set(offset, &value, sizeof(value));
}

#endif
