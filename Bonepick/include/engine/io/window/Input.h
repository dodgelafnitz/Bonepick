#ifndef ENGINE_IO_WINDOW_INPUT_H
#define ENGINE_IO_WINDOW_INPUT_H

#include "engine/utility/math/Vector.h"
#include "engine/io/common/Input.h"

//##############################################################################
namespace Window
{
  //############################################################################
  InputMessage GetInput(void);

  //############################################################################
  Vec2 GetMousePos(void);
  void Sleep(unsigned milliseconds);
};

#endif
