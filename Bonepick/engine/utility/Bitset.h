#ifndef ENGINE_UTILITY_BITSET_H
#define ENGINE_UTILITY_BITSET_H

#include <initializer_list>

#include "utility/containers/Array.h"
#include "utility/Typedefs.h"

//##############################################################################
class Bitset
{
public:
  Bitset(void);
  Bitset(Bitset const & bitset);
  Bitset(Bitset && bitset);
  Bitset(std::initializer_list<int> const & indices);
  ~Bitset(void);

  void Set(int index);
  void Unset(int index);

  bool Get(int index) const;
  bool operator [](int index) const;

  void Clear(void);

  Bitset & operator =(Bitset const & bitset);
  Bitset & operator =(Bitset && bitset);

  bool operator ==(Bitset const & bitset) const;
  bool operator !=(Bitset const & bitset) const;

private:
  unsigned * data_;
  int size_;
};

#endif
