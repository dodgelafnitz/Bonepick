#include <Windows.h>
#undef ERROR
#undef min

#include "engine/Window/Input.h"

#include <algorithm>
#include <queue>

namespace
{
  //############################################################################
  void RefreshInputs(void);
  InputButton GetButtonFromVirtualKey(WORD virtualKey);

  //############################################################################
  unsigned const MaxRecordsPerRead = 64;

  //############################################################################
  std::queue<InputMessage> messages;
  IVec2 mousePos;

  //############################################################################
  void RefreshInputs(void)
  {
    HANDLE consoleHandle = GetStdHandle(STD_INPUT_HANDLE);

    DWORD recordsLeft;
    GetNumberOfConsoleInputEvents(consoleHandle, &recordsLeft);

    if (recordsLeft == 0)
      return;

    do
    {
      INPUT_RECORD records[MaxRecordsPerRead];
      DWORD recordsRead = 0;

      DWORD const recordsToRead =
        std::min(DWORD(MaxRecordsPerRead), recordsLeft);

      ReadConsoleInput(consoleHandle, records, recordsToRead, &recordsRead);

      for (unsigned i = 0; i < recordsRead; ++i)
      {
        INPUT_RECORD const & record = records[i];

        if (record.EventType == KEY_EVENT)
        {
          if (record.Event.KeyEvent.wRepeatCount > 1)
            continue;

          InputButton button = GetButtonFromVirtualKey(
            record.Event.KeyEvent.wVirtualKeyCode);

          bool isDown = record.Event.KeyEvent.bKeyDown ? true : false;

          messages.emplace(button, isDown);
        }
        else if (record.EventType == MOUSE_EVENT)
        {
          mousePos.x = record.Event.MouseEvent.dwMousePosition.X;
          mousePos.y = record.Event.MouseEvent.dwMousePosition.Y;
        }

      }

      recordsLeft -= recordsRead;
    }
    while (recordsLeft);
  }

  //############################################################################
  InputButton GetButtonFromVirtualKey(WORD virtualKey)
  {
    switch (virtualKey)
    {
    case '0':
      return IB_0;
    case '1':
      return IB_1;
    case '2':
      return IB_2;
    case '3':
      return IB_3;
    case '4':
      return IB_4;
    case '5':
      return IB_5;
    case '6':
      return IB_6;
    case '7':
      return IB_7;
    case '8':
      return IB_8;
    case '9':
      return IB_9;
    case 'A':
      return IB_A;
    case 'B':
      return IB_B;
    case 'C':
      return IB_C;
    case 'D':
      return IB_D;
    case 'E':
      return IB_E;
    case 'F':
      return IB_F;
    case 'G':
      return IB_G;
    case 'H':
      return IB_H;
    case 'I':
      return IB_I;
    case 'J':
      return IB_J;
    case 'K':
      return IB_K;
    case 'L':
      return IB_L;
    case 'M':
      return IB_M;
    case 'N':
      return IB_N;
    case 'O':
      return IB_O;
    case 'P':
      return IB_P;
    case 'Q':
      return IB_Q;
    case 'R':
      return IB_R;
    case 'S':
      return IB_S;
    case 'T':
      return IB_T;
    case 'U':
      return IB_U;
    case 'V':
      return IB_V;
    case 'W':
      return IB_W;
    case 'X':
      return IB_X;
    case 'Y':
      return IB_Y;
    case 'Z':
      return IB_Z;
    case VK_SPACE:
      return IB_Space;
    case VK_ESCAPE:
      return IB_Escape;
    case VK_OEM_3:
      return IB_Grave;
    case VK_TAB:
      return IB_Tab;
    case VK_OEM_MINUS:
      return IB_Minus;
    case VK_ADD:
      return IB_Equals;
    case VK_OEM_4:
      return IB_LeftBracket;
    case VK_OEM_6:
      return IB_RightBracket;
    case VK_OEM_1:
      return IB_Semicolon;
    case VK_OEM_7:
      return IB_Apostrophe;
    case VK_OEM_COMMA:
      return IB_Comma;
    case VK_OEM_PERIOD:
      return IB_Period;
    case VK_OEM_5:
      return IB_BackSlash;
    case VK_OEM_2:
      return IB_ForwardSlash;
    case VK_LEFT:
      return IB_Left;
    case VK_RIGHT:
      return IB_Right;
    case VK_UP:
      return IB_Up;
    case VK_DOWN:
      return IB_Down;
    case VK_CAPITAL:
      return IB_CapsLock;
    case VK_INSERT:
      return IB_Insert;
    case VK_LSHIFT:
      return IB_LeftShift;
    case VK_LCONTROL:
      return IB_LeftControl;
    case VK_RSHIFT:
      return IB_RightShift;
    case VK_RCONTROL:
      return IB_RightControl;
    case VK_MENU:
      return IB_Alt;
    case VK_RETURN:
      return IB_Return;
    case VK_BACK:
      return IB_Backspace;
    case VK_DELETE:
      return IB_Delete;
    case VK_F1:
      return IB_F1;
    case VK_F2:
      return IB_F2;
    case VK_F3:
      return IB_F3;
    case VK_F4:
      return IB_F4;
    case VK_F5:
      return IB_F5;
    case VK_F6:
      return IB_F6;
    case VK_F7:
      return IB_F7;
    case VK_F8:
      return IB_F8;
    case VK_F9:
      return IB_F9;
    case VK_F10:
      return IB_F10;
    case VK_F11:
      return IB_F11;
    case VK_F12:
      return IB_F12;
    case VK_LBUTTON:
      return IB_LeftMouse;
    case VK_RBUTTON:
      return IB_RightMouse;
    default:
      return IB_Count;
    };
  }
}

//##############################################################################
InputMessage::InputMessage(void) :
  button(IB_Count),
  isDown(false)
{}

//##############################################################################
InputMessage::InputMessage(InputButton button, bool isDown) :
  button(button),
  isDown(isDown)
{}

//##############################################################################
InputMessage GetInput(void)
{
  RefreshInputs();

  if (messages.empty())
    return InputMessage();

  InputMessage const result = messages.front();
  messages.pop();

  return result;
}

//##############################################################################
void Sleep(unsigned milliseconds)
{
  Sleep(DWORD(milliseconds));
}

//##############################################################################
IVec2 GetMousePos(void)
{
  RefreshInputs();

  return mousePos;
}
