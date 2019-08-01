#include <Windows.h>
#undef ERROR
#undef min

#include "io/ascii/Input.h"

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
      return InputButton0;
    case '1':
      return InputButton1;
    case '2':
      return InputButton2;
    case '3':
      return InputButton3;
    case '4':
      return InputButton4;
    case '5':
      return InputButton5;
    case '6':
      return InputButton6;
    case '7':
      return InputButton7;
    case '8':
      return InputButton8;
    case '9':
      return InputButton9;
    case 'A':
      return InputButtonA;
    case 'B':
      return InputButtonB;
    case 'C':
      return InputButtonC;
    case 'D':
      return InputButtonD;
    case 'E':
      return InputButtonE;
    case 'F':
      return InputButtonF;
    case 'G':
      return InputButtonG;
    case 'H':
      return InputButtonH;
    case 'I':
      return InputButtonI;
    case 'J':
      return InputButtonJ;
    case 'K':
      return InputButtonK;
    case 'L':
      return InputButtonL;
    case 'M':
      return InputButtonM;
    case 'N':
      return InputButtonN;
    case 'O':
      return InputButtonO;
    case 'P':
      return InputButtonP;
    case 'Q':
      return InputButtonQ;
    case 'R':
      return InputButtonR;
    case 'S':
      return InputButtonS;
    case 'T':
      return InputButtonT;
    case 'U':
      return InputButtonU;
    case 'V':
      return InputButtonV;
    case 'W':
      return InputButtonW;
    case 'X':
      return InputButtonX;
    case 'Y':
      return InputButtonY;
    case 'Z':
      return InputButtonZ;
    case VK_SPACE:
      return InputButtonSpace;
    case VK_ESCAPE:
      return InputButtonEscape;
    case VK_OEM_3:
      return InputButtonGrave;
    case VK_TAB:
      return InputButtonTab;
    case VK_OEM_MINUS:
      return InputButtonMinus;
    case VK_ADD:
      return InputButtonEquals;
    case VK_OEM_4:
      return InputButtonLeftBracket;
    case VK_OEM_6:
      return InputButtonRightBracket;
    case VK_OEM_1:
      return InputButtonSemicolon;
    case VK_OEM_7:
      return InputButtonApostrophe;
    case VK_OEM_COMMA:
      return InputButtonComma;
    case VK_OEM_PERIOD:
      return InputButtonPeriod;
    case VK_OEM_5:
      return InputButtonBackSlash;
    case VK_OEM_2:
      return InputButtonForwardSlash;
    case VK_LEFT:
      return InputButtonLeft;
    case VK_RIGHT:
      return InputButtonRight;
    case VK_UP:
      return InputButtonUp;
    case VK_DOWN:
      return InputButtonDown;
    case VK_CAPITAL:
      return InputButtonCapsLock;
    case VK_INSERT:
      return InputButtonInsert;
    case VK_LSHIFT:
      return InputButtonLeftShift;
    case VK_LCONTROL:
      return InputButtonLeftControl;
    case VK_RSHIFT:
      return InputButtonRightShift;
    case VK_RCONTROL:
      return InputButtonRightControl;
    case VK_MENU:
      return InputButtonAlt;
    case VK_RETURN:
      return InputButtonReturn;
    case VK_BACK:
      return InputButtonBackspace;
    case VK_DELETE:
      return InputButtonDelete;
    case VK_F1:
      return InputButtonF1;
    case VK_F2:
      return InputButtonF2;
    case VK_F3:
      return InputButtonF3;
    case VK_F4:
      return InputButtonF4;
    case VK_F5:
      return InputButtonF5;
    case VK_F6:
      return InputButtonF6;
    case VK_F7:
      return InputButtonF7;
    case VK_F8:
      return InputButtonF8;
    case VK_F9:
      return InputButtonF9;
    case VK_F10:
      return InputButtonF10;
    case VK_F11:
      return InputButtonF11;
    case VK_F12:
      return InputButtonF12;
    case VK_LBUTTON:
      return InputButtonLeftMouse;
    case VK_RBUTTON:
      return InputButtonRightMouse;
    default:
      return InputButtons;
    };
  }
}

namespace Ascii
{
  //############################################################################
  InputMessage GetInput(void)
  {
    RefreshInputs();

    if (messages.empty())
      return InputMessage();

    InputMessage const result = messages.front();
    messages.pop();

    return result;
  }

  //############################################################################
  void Sleep(unsigned milliseconds)
  {
    ::Sleep(DWORD(milliseconds));
  }

  //############################################################################
  IVec2 GetMousePos(void)
  {
    RefreshInputs();

    return mousePos;
  }
}
