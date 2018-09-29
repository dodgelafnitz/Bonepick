#include "engine/Utility/String.h"

#include <algorithm>

#include "engine/Utility/Debug.h"

//tHis fUncTion oR varIable mAy bE uNsafE
#pragma warning(disable : 4996)

//##############################################################################
String::String(void) :
  type_(StringTypeInternal)
{
  str_.inner[0] = '\0';
}

//##############################################################################
String::String(String const & str)
{
  switch (str.type_)
  {
  case StringTypeOwned:
  case StringTypeExternal:
    str_.outer = str.str_.outer;
    type_ = StringTypeExternal;
    break;
  case StringTypeInternal:
    std::strncpy(str_.inner, str.Ptr(), StringDataSize);
    type_ = StringTypeInternal;
    break;
  };
}

//##############################################################################
String::String(String && str)
{
  switch (str.type_)
  {
  case StringTypeOwned:
  case StringTypeExternal:
    str_.outer = str.str_.outer;
    break;
  case StringTypeInternal:
    std::strncpy(str_.inner, str.Ptr(), StringDataSize);
    break;
  };

  type_ = str.type_;
  str.str_.inner[0] = '\0';
  str.type_ = StringTypeInternal;
}

//##############################################################################
String::String(char const * str) :
  type_(StringTypeExternal)
{
  str_.outer.ptr = str;
  str_.outer.size = std::strlen(str);
}

//##############################################################################
String::String(char const * str, int size) :
  type_(StringTypeExternal)
{
  str_.outer.ptr = str;

  int length;
  for (length = 0; length < size; ++length)
  {
    if (str[length] == '\0')
      break;
  }
  str_.outer.size = length;
}

//##############################################################################
String::String(std::string const & str) :
  type_(StringTypeExternal)
{
  str_.outer.ptr = str.c_str();
  str_.outer.size = str.size();
}

//##############################################################################
String::~String(void)
{
  Clear();
}

//##############################################################################
void String::Append(String const & str)
{
  switch (type_)
  {
  case StringTypeInternal:
    if (Size() + str.Size() < StringDataSize - 1)
    {
      int const size = Size();

      std::strncpy(str_.inner + size, str.Ptr(), str.Size());
      str_.inner[size + str.Size()] = '\0';
      break;
    }
  case StringTypeOwned:
  case StringTypeExternal:
    char * newData = new char[Size() + str.Size() + 1];
    std::strncpy(newData, Ptr(), Size());
    std::strncpy(newData + Size(), str.Ptr(), str.Size());
    newData[Size() + str.Size()] = '\0';

    char const * oldData = Ptr();
    str_.outer.ptr = newData;
    str_.outer.size = str_.outer.size + str.Size();

    if (type_ == StringTypeOwned)
      delete[] oldData;
    else
      type_ = StringTypeOwned;

    break;
  }
}

//##############################################################################
void String::Append(char character)
{
  Append(String(&character, 1));
}

//##############################################################################
void String::Clear(void)
{
  if (type_ == StringTypeOwned)
    delete[] str_.outer.ptr;

  str_.inner[0] = '\0';
  type_ = StringTypeInternal;
}

//##############################################################################
int String::Size(void) const
{
  return type_ == StringTypeInternal ?
    std::strlen(str_.inner) : str_.outer.size;
}

//##############################################################################
bool String::Empty(void) const
{
  return Size() == 0;
}

//##############################################################################
String String::Substring(int index, int size) const
{
  return String(Ptr() + index, size);
}

//##############################################################################
String & String::MakeOwner(void)
{
  if (type_ == StringTypeExternal)
  {
    if (Size() < StringDataSize - 1)
    {
      std::strncpy(str_.inner, Ptr(), Size());
      str_.inner[Size()] = '\0';
      type_ = StringTypeInternal;
    }
    else
    {
      char * newData = new char[Size() + 1];
      std::strncpy(newData, Ptr(), Size());
      newData[Size()] = '\0';
      str_.outer.ptr = newData;
      type_ = StringTypeOwned;
    }
  }

  return *this;
}

//##############################################################################
String & String::MakeNullTerminated(void)
{
  if (!IsNullTerminated())
    MakeOwner();

  return *this;
}

//##############################################################################
bool String::IsNullTerminated(void) const
{
  return type_ == StringTypeExternal ? Ptr()[Size()] == '\0' : true;
}

//##############################################################################
String & String::operator +=(String const & str)
{
  Append(str);
  return *this;
}

//##############################################################################
String & String::operator +=(char character)
{
  Append(character);
  return *this;
}

//##############################################################################
String String::operator +(String const & str) const
{
  return String(*this) += str;
}

//##############################################################################
String String::operator +(char character) const
{
  return String(*this) += character;
}

//##############################################################################
bool String::operator ==(String const & str) const
{
  if (this == &str)
    return true;

  if (Size() != str.Size())
    return false;

  return std::strncmp(Ptr(), str.Ptr(), Size()) == 0;
}

//##############################################################################
bool String::operator !=(String const & str) const
{
  return !(*this == str);
}

//##############################################################################
bool String::operator <=(String const & str) const
{
  return !(str < *this);
}

//##############################################################################
bool String::operator >=(String const & str) const
{
  return !(*this < str);
}

//##############################################################################
bool String::operator <(String const & str) const
{
  int const size = std::min(Size(), str.Size());
  int const cmp  = std::strncmp(Ptr(), str.Ptr(), size);

  return cmp == 0 ? Size() < str.Size() : cmp < 0;
}

//##############################################################################
bool String::operator >(String const & str) const
{
  return str < *this;
}

//##############################################################################
String & String::operator =(String const & str)
{
  if (this == &str)
    return *this;

  Clear();

  type_ = str.type_ == StringTypeInternal ?
    StringTypeInternal : StringTypeExternal;
  str_ = str.str_;

  return *this;
}

//##############################################################################
String & String::operator =(String && str)
{
  if (this == &str)
    return *this;

  Clear();

  type_ = str.type_;
  str_ = str.str_;

  str.type_ = StringTypeInternal;
  str.str_.inner[0] = '\0';

  return *this;
}

//##############################################################################
char String::operator [](int index) const
{
  ASSERT(index >= 0);
  ASSERT(index < Size());

  return Ptr()[index];
}

//##############################################################################
char const * String::Ptr(void) const
{
  return type_ == StringTypeInternal ? str_.inner : str_.outer.ptr;
}

//##############################################################################
char String::Front(void) const
{
  ASSERT(Size() != 0);

  return *Ptr();
}

//##############################################################################
char String::Back(void) const
{
  ASSERT(Size() != 0);

  return Ptr()[Size() - 1];
}

//##############################################################################
char const * String::Begin(void) const
{
  return Ptr();
}

//##############################################################################
char const * String::End(void) const
{
  return Ptr() + Size();
}

namespace std
{
  //############################################################################
  char const * begin(String const & string)
  {
    return string.Begin();
  }

  //############################################################################
  char const * end(String const & string)
  {
    return string.End();
  }
}

