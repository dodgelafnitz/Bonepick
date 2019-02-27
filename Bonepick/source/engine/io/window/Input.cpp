#include "engine/io/window/Input.h"

#include "external/SDL/SDL.h"

namespace Window
{
  //############################################################################
  InputMessage GetInput(void)
  {
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
      //todo
    }

    return InputMessage();
  }
}
