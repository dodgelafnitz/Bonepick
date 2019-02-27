#include "engine/io/window/Window.h"

#include "external/SDL/SDL.h"

//##############################################################################
namespace Window
{
  //############################################################################
  void Initialize(
    IVec2 const & windowSize,
    String const & title,
    Vec2 const & screenSize)
  {
    SDL_Init(SDL_INIT_EVERYTHING);

    String titleOwned = title;
    titleOwned.MakeNullTerminated();

    SDL_CreateWindow(
      titleOwned.Ptr(),
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      windowSize.x,
      windowSize.y,
      0 /*flags*/);
  }

}
