#ifndef ENGINE_UTILITY_DATALAYOUT_H
#define ENGINE_UTILITY_DATALAYOUT_H

#include <typeindex>

#include "utility/containers/SortedArray.h"
#include "utility/Token.h"

//##############################################################################
class DataLayout
{
public:
  struct LayoutInfo
  {
    LayoutInfo(std::type_index type, unsigned offset);

    std::type_index type;
    unsigned offset;
  };

  void Add(Token key, std::type_index type, unsigned offset);

  bool Contains(Token key) const;
  std::type_index GetType(Token key) const;
  unsigned GetOffset(Token key) const;

  unsigned Size(void) const;

  void Clear(void);

  DataLayout & operator =(DataLayout const & layout) = default;
  DataLayout & operator =(DataLayout && layout) = default;

  ArrayMap<Token, LayoutInfo>::Type * Begin(void);
  ArrayMap<Token, LayoutInfo>::Type * End(void);
  ArrayMap<Token, LayoutInfo>::ConstType * Begin(void) const;
  ArrayMap<Token, LayoutInfo>::ConstType * End(void) const;

  ArrayMap<Token, LayoutInfo>::Type * begin(void);
  ArrayMap<Token, LayoutInfo>::Type * end(void);
  ArrayMap<Token, LayoutInfo>::ConstType * begin(void) const;
  ArrayMap<Token, LayoutInfo>::ConstType * end(void) const;

private:
  ArrayMap<Token, LayoutInfo> layout_;
};

#endif
