#ifndef ENGINE_WINDOW_INPUT_H
#define ENGINE_WINDOW_INPUT_H

#include "engine/Utility/Math/Vector.h"

//##############################################################################
enum InputButton
{
  IB_0,
  IB_1,
  IB_2,
  IB_3,
  IB_4,
  IB_5,
  IB_6,
  IB_7,
  IB_8,
  IB_9,
  IB_A,
  IB_B,
  IB_C,
  IB_D,
  IB_E,
  IB_F,
  IB_G,
  IB_H,
  IB_I,
  IB_J,
  IB_K,
  IB_L,
  IB_M,
  IB_N,
  IB_O,
  IB_P,
  IB_Q,
  IB_R,
  IB_S,
  IB_T,
  IB_U,
  IB_V,
  IB_W,
  IB_X,
  IB_Y,
  IB_Z,
  IB_Space,
  IB_Escape,
  IB_Grave,
  IB_Tab,
  IB_Minus,
  IB_Equals,
  IB_LeftBracket,
  IB_RightBracket,
  IB_Semicolon,
  IB_Apostrophe,
  IB_Comma,
  IB_Period,
  IB_BackSlash,
  IB_ForwardSlash,
  IB_Left,
  IB_Right,
  IB_Up,
  IB_Down,
  IB_CapsLock,
  IB_Insert,
  IB_LeftShift,
  IB_LeftControl,
  IB_RightShift,
  IB_RightControl,
  IB_Alt,
  IB_Return,
  IB_Backspace,
  IB_Delete,
  IB_F1,
  IB_F2,
  IB_F3,
  IB_F4,
  IB_F5,
  IB_F6,
  IB_F7,
  IB_F8,
  IB_F9,
  IB_F10,
  IB_F11,
  IB_F12,
  IB_LeftMouse,
  IB_RightMouse,
  IB_Count
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
