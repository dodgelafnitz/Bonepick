#include "engine\Utility\Blob.h"

#include <cstdlib>

//##############################################################################
Blob::Blob(unsigned size) :
  data_(new std::vector<char>(size))
{}

//##############################################################################
Blob::Blob(void const * data, unsigned size) :
  data_(new std::vector<char>(reinterpret_cast<char const *>(data),
  reinterpret_cast<char const *>(data) + size))
{}

//##############################################################################
Blob::~Blob(void)
{
  Clear();
}

//##############################################################################
void * Blob::Get(unsigned offset)
{
  ASSERT(offset < data_->size());

  return &*(data_->begin() + offset);
}

//##############################################################################
void const * Blob::Get(unsigned offset) const
{
  return const_cast<Blob *>(this)->Get(offset);
}

//##############################################################################
void Blob::Set(unsigned offset, void const * data, unsigned count)
{
  ASSERT(offset + count <= data_->size());

  std::memcpy(&*(data_->begin() + offset), data, count);
}

//##############################################################################
void Blob::Resize(unsigned size)
{
  data_->resize(size, 0);
}

//##############################################################################
void Blob::Clear(void)
{
  if (data_)
    data_.reset();
}

//##############################################################################
void * Blob::Data(void)
{
  return data_ ? data_->data() : nullptr;
}

//##############################################################################
void const * Blob::Data(void) const
{
  return data_ ? data_->data() : nullptr;
}

//##############################################################################
unsigned Blob::Size(void) const
{
  return data_ ? data_->size() : 0;
}
