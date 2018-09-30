#include "engine/Utility/DataLayout.h"

#include "engine/Utility/Debug.h"
#include "engine/Utility/TemplateTools.h"

//##############################################################################
DataLayout::LayoutInfo::LayoutInfo(std::type_index type, unsigned offset) :
  type(type), offset(offset)
{}

//##############################################################################
void DataLayout::Add(Token key, std::type_index type, unsigned offset)
{
  ASSERT(layout_.Find(key) == 0);

  layout_.Emplace(key, LayoutInfo(type, offset));

}

//##############################################################################
bool DataLayout::Has(Token key) const
{
  return layout_.Has(key);
}

//##############################################################################
std::type_index DataLayout::GetType(Token key) const
{
  ASSERT(layout_.Has(key));

  return layout_.Find(key)->value.type;
}

//##############################################################################
unsigned DataLayout::GetOffset(Token key) const
{
  ASSERT(layout_.Has(key));

  return layout_.Find(key)->value.offset;
}

//##############################################################################
unsigned DataLayout::Size(void) const
{
  return layout_.Size();
}

//##############################################################################
void DataLayout::Clear(void)
{
  layout_.Clear();
}

//##############################################################################
ArrayMap<Token, DataLayout::LayoutInfo>::Type * DataLayout::Begin(void)
{
  return layout_.Begin();
}

//##############################################################################
ArrayMap<Token, DataLayout::LayoutInfo>::Type * DataLayout::End(void)
{
  return layout_.End();
}

//##############################################################################
ArrayMap<Token, DataLayout::LayoutInfo>::ConstType * DataLayout::Begin(void)
  const
{
  return const_cast<DataLayout *>(this)->Begin();
}

//##############################################################################
ArrayMap<Token, DataLayout::LayoutInfo>::ConstType * DataLayout::End(void) const
{
  return const_cast<DataLayout *>(this)->End();
}

//##############################################################################
ArrayMap<Token, DataLayout::LayoutInfo>::Type * DataLayout::begin(void)
{
  return Begin();
}

//##############################################################################
ArrayMap<Token, DataLayout::LayoutInfo>::Type * DataLayout::end(void)
{
  return End();
}

//##############################################################################
ArrayMap<Token, DataLayout::LayoutInfo>::ConstType * DataLayout::begin(void)
  const
{
  return Begin();
}

//##############################################################################
ArrayMap<Token, DataLayout::LayoutInfo>::ConstType * DataLayout::end(void) const
{
  return End();
}
