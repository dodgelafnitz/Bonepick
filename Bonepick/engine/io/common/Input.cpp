#include "engine/io/common/Input.h"

//############################################################################
InputMessage::InputMessage(InputButton button, bool isDown) :
  button(button),
  isDown(isDown)
{}
