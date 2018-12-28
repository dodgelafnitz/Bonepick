#ifndef ENGINE_UI_WIDGETS_H
#define ENGINE_UI_WIDGETS_H

#include "engine/Utility/Containers/External.h"
#include "engine/Utility/Math/Vector.h"
#include "engine/Window/Graphics.h"
#include "engine/Window/Input.h"

//##############################################################################
class IWidget;
class WidgetField;
class WidgetList;
class WidgetGrid;
class WidgetTextInput;
class WidgetText;
class WidgetScroll;
class WidgetButton;
class WidgetRadial;
class WidgetFlag;
class WidgetSlider;
class WidgetSeperator;

//##############################################################################
class IWidget
{
public:
  virtual ~IWidget(void) = default;

  virtual bool IsResizeable(void) const;
  virtual void Resize(IVec2 const & size);
  virtual IVec2 GetSize(void) const;

  virtual AsciiImage GetContent(void) const;

  virtual bool OnFocus(bool hasFocus, IVec2 const & cursorPos) const;
  virtual bool OnButtonPress(
    bool hasFocus,
    IVec2 const & cursorPos,
    InputButton button,
    bool buttonDown) const;

  virtual bool OnHover(bool hasFocus, IVec2 const & cursorPos) const;

  WeakExternal<IWidget const> GetParent(void) const;
  virtual WeakExternal<IWidget> GetParent(void) = 0;

  WeakExternal<IWidget const> GetRoot(void) const;
  WeakExternal<IWidget> GetRoot(void);
};

#endif
