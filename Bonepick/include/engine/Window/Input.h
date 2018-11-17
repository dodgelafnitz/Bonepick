#ifndef ENGINE_WINDOW_INPUT_H
#define ENGINE_WINDOW_INPUT_H

#include "engine/Utility/Math/Vector.h"

//##############################################################################
enum InputButton
{
  InputButton0,
  InputButton1,
  InputButton2,
  InputButton3,
  InputButton4,
  InputButton5,
  InputButton6,
  InputButton7,
  InputButton8,
  InputButton9,
  InputButtonA,
  InputButtonB,
  InputButtonC,
  InputButtonD,
  InputButtonE,
  InputButtonF,
  InputButtonG,
  InputButtonH,
  InputButtonI,
  InputButtonJ,
  InputButtonK,
  InputButtonL,
  InputButtonM,
  InputButtonN,
  InputButtonO,
  InputButtonP,
  InputButtonQ,
  InputButtonR,
  InputButtonS,
  InputButtonT,
  InputButtonU,
  InputButtonV,
  InputButtonW,
  InputButtonX,
  InputButtonY,
  InputButtonZ,
  InputButtonSpace,
  InputButtonEscape,
  InputButtonGrave,
  InputButtonTab,
  InputButtonMinus,
  InputButtonEquals,
  InputButtonLeftBracket,
  InputButtonRightBracket,
  InputButtonSemicolon,
  InputButtonApostrophe,
  InputButtonComma,
  InputButtonPeriod,
  InputButtonBackSlash,
  InputButtonForwardSlash,
  InputButtonLeft,
  InputButtonRight,
  InputButtonUp,
  InputButtonDown,
  InputButtonCapsLock,
  InputButtonInsert,
  InputButtonLeftShift,
  InputButtonLeftControl,
  InputButtonRightShift,
  InputButtonRightControl,
  InputButtonAlt,
  InputButtonReturn,
  InputButtonBackspace,
  InputButtonDelete,
  InputButtonF1,
  InputButtonF2,
  InputButtonF3,
  InputButtonF4,
  InputButtonF5,
  InputButtonF6,
  InputButtonF7,
  InputButtonF8,
  InputButtonF9,
  InputButtonF10,
  InputButtonF11,
  InputButtonF12,
  InputButtonLeftMouse,
  InputButtonRightMouse,
  InputButtons
};

//##############################################################################
struct InputMessage
{
  InputMessage(void);
  InputMessage(InputButton button, bool isDown);

  InputButton button;
  bool isDown;
};

//##############################################################################
InputMessage GetInput(void);
IVec2 GetMousePos(void);
void Sleep(unsigned milliseconds);

#endif
