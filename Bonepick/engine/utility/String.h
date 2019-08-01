#ifndef ENGINE_UTILITIES_STRING_H
#define ENGINE_UTILITIES_STRING_H

#include <string>

#include "utility/Typedefs.h"

//##############################################################################
static int const StringDataSize = sizeof(char const *) + sizeof(int);

//##############################################################################
class String
{
public:
  String(void);
  String(String const & str);
  String(String && str);
  String(char const * str);
  String(char const * str, int size);
  String(std::string const & str);
  ~String(void);

  void Append(String const & str);
  void Append(char character);

  void Clear(void);

  int Size(void) const;
  bool Empty(void) const;

  String Substring(int index, int size) const;

  String & MakeOwner(void);
  String & MakeNullTerminated(void);
  bool IsNullTerminated(void) const;

  String & operator +=(String const & str);
  String & operator +=(char character);

  String operator +(String const & str) const;
  String operator +(char character) const;

  bool operator ==(String const & str) const;
  bool operator !=(String const & str) const;
  bool operator <=(String const & str) const;
  bool operator >=(String const & str) const;
  bool operator <(String const & str) const;
  bool operator >(String const & str) const;

  String & operator =(String const & str);
  String & operator =(String && str);

  char operator [](int index) const;
  char const * Ptr(void) const;

  char Front(void) const;
  char Back(void) const;

  char const * Begin(void) const;
  char const * End(void) const;

private:
  enum StringType
  {
    StringTypeOwned,
    StringTypeExternal,
    StringTypeInternal,
    StringTypes
  };

  StringType type_;

  union
  {
    struct
    {
      char const * ptr;
      int size;
    } outer;
    char inner[StringDataSize];
  } str_;

};

//##############################################################################
namespace std
{
  char const * begin(String const & string);
  char const * end(String const & string);
}

#endif
