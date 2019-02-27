#ifndef ENGINE_IO_WINDOW_WINDOW_H
#define ENGINE_IO_WINDOW_WINDOW_H

#include "engine/utility/math/Vector.h"
#include "engine/utility/String.h"

//##############################################################################
namespace Window
{
  //############################################################################
  void Initialize(
    IVec2 const & windowSize,
    String const & title,
    Vec2 const & screenSize);
}

#endif
