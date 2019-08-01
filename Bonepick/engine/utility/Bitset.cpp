#include "utility/Bitset.h"

#include <algorithm>
#include <cstring>

//##############################################################################
namespace
{
  //############################################################################
  int const BitsPerByte = 8;

  //############################################################################
  int GetValueIndexForBitIndex(int index)
  {
    ASSERT(index >= 0);

    return index / (BitsPerByte * sizeof(unsigned));
  }

  //############################################################################
  int GetValueCountForBitIndex(int index)
  {
    return GetValueIndexForBitIndex(index) + 1;
  }

  //############################################################################
  int GetValueCountForBitCount(int bitCount)
  {
    return bitCount ? GetValueCountForBitIndex(bitCount - 1) : 0;
  }

  //############################################################################
  int GetValueShiftForBitIndex(int index)
  {
    ASSERT(index >= 0);

    return index % (BitsPerByte * sizeof(unsigned));
  }
}

//##############################################################################
Bitset::Bitset(void) :
  data_(nullptr), size_(0)
{}

//##############################################################################
Bitset::Bitset(Bitset const & bitset) :
  data_(nullptr), size_(bitset.size_)
{
  data_ = new unsigned[size_];

  std::memcpy(data_, bitset.data_, size_ * sizeof(unsigned));
}

//##############################################################################
Bitset::Bitset(Bitset && bitset) :
  data_(bitset.data_), size_(bitset.size_)
{
  bitset.data_ = nullptr;
  bitset.size_ = 0;
}

//##############################################################################
Bitset::Bitset(std::initializer_list<int> const & indices) :
  data_(nullptr), size_(0)
{
  for (int const & index : indices)
    Set(index);
}

//##############################################################################
Bitset::~Bitset(void)
{
  Clear();
}

//##############################################################################
void Bitset::Set(int index)
{
  ASSERT(index >= 0);

  int const size = GetValueCountForBitIndex(index);

  if (size > size_)
  {
    unsigned * newData = new unsigned[size];

    std::memcpy(newData, data_, size_ * sizeof(unsigned));
    std::memset(newData + size_, 0, (size - size_) * sizeof(unsigned));

    if (data_)
      delete[] data_;

    size_ = size;
    data_ = newData;
  }

  data_[GetValueIndexForBitIndex(index)] |=
    (unsigned(1) << GetValueShiftForBitIndex(index));
}

//##############################################################################
void Bitset::Unset(int index)
{
  ASSERT(index >= 0);

  int const size = GetValueCountForBitIndex(index);

  if (size > size_)
    return;

  data_[GetValueIndexForBitIndex(index)] &=
    ~(unsigned(1) << GetValueShiftForBitIndex(index));
}

//##############################################################################
bool Bitset::Get(int index) const
{
  ASSERT(index >= 0);

  int const size = GetValueCountForBitIndex(index);

  if (size > size_)
    return false;

  return data_[GetValueIndexForBitIndex(index)] &
    (unsigned(1) << GetValueShiftForBitIndex(index));
}

//##############################################################################
bool Bitset::operator [](int index) const
{
  return Get(index);
}

//##############################################################################
void Bitset::Clear(void)
{
  if (data_)
    delete[] data_;

  data_ = nullptr;
  size_ = 0;
}

//##############################################################################
Bitset & Bitset::operator =(Bitset const & bitset)
{
  if (this == &bitset)
    return *this;

  Clear();

  size_ = bitset.size_;
  data_ = new unsigned[size_];

  std::memcpy(data_, bitset.data_, size_ * sizeof(unsigned));

  return *this;
}

//##############################################################################
Bitset & Bitset::operator =(Bitset && bitset)
{
  if (this == &bitset)
    return *this;

  Clear();

  size_ = bitset.size_;
  data_ = bitset.data_;

  bitset.size_ = 0;
  bitset.data_ = nullptr;

  return *this;
}

//##############################################################################
bool Bitset::operator ==(Bitset const & bitset) const
{
  if (this == &bitset)
    return true;

  int const lowSize = std::min(size_, bitset.size_);

  if (std::memcmp(data_, bitset.data_, lowSize * sizeof(unsigned)) != 0)
    return false;

  int const highSize = std::max(size_, bitset.size_);

  if (highSize != lowSize)
  {
    unsigned const * checkData = size_ < bitset.size_ ?
      bitset.data_ : data_;

    for (int i = lowSize; i != highSize; ++i)
    {
      if (checkData[i] != 0)
        return false;
    }
  }

  return true;
}

//##############################################################################
bool Bitset::operator !=(Bitset const & bitset) const
{
  return !(*this == bitset);
}

