#if DEBUG_ENABLED

#include <cstdarg>
#include <vector>

#include "engine/Utility/Debug.h"
#include "engine/Utility/Typedefs.h"

//tHis fUncTion oR varIable mAy bE uNsafE
#pragma warning(disable : 4996)

namespace
{
  //############################################################################
  bool expectError = false;
}

//##############################################################################
bool TestIsExpectingError(void)
{
  return expectError;
}

//##############################################################################
void TestSetExpectError(bool expect)
{
  expectError = expect;
}

//##############################################################################
TestExpectedError::TestExpectedError(char const * str, ...)
{
  //This should never happen, cannot use ERROR here due to this check though.
  ON_DEBUG(
    if (expectError == false)
      DEBUG_BREAK();
  )

  va_list args;
  va_start(args, str);

  unsigned const characters = std::vsnprintf(nullptr, 0, str, args);
  std::vector<char> buffer(characters + 1);
  std::vsprintf(buffer.data(), str, args);

  va_end(args);

  error.assign(buffer.data());

  TestSetExpectError(false);
}

//##############################################################################
AssertionError::AssertionError(char const * str, ...)
{
  va_list args;
  va_start(args, str);

  unsigned const characters = std::vsnprintf(nullptr, 0, str, args);
  std::vector<char> buffer(characters + 1);
  std::vsprintf(buffer.data(), str, args);

  va_end(args);

  error.assign(buffer.data());
}

#endif
