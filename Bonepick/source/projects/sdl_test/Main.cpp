#include "engine/io/window/Input.h"
#include "engine/io/window/Window.h"

void main(void)
{
  Window::Initialize(
    IVec2(640, 360),
    "Title \xF0\x9F\x99\x80",
    Vec2(10.0f, 10.0f));

  while (true)
  {
    Window::GetInput();
  }
}
