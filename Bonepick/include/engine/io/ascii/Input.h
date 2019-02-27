#ifndef ENGINE_IO_ASCII_INPUT_H
#define ENGINE_IO_ASCII_INPUT_H

#include "engine/utility/math/Vector.h"
#include "engine/io/common/Input.h"

namespace Ascii
{
  //############################################################################
  InputMessage GetInput(void);

  //############################################################################
  IVec2 GetMousePos(void);
  void Sleep(unsigned milliseconds);
}

#endif
