#ifndef ENGINE_UTILITY_DEBUG_H
#define ENGINE_UTILITY_DEBUG_H

#include <cstdio>
#include <string>

#include "engine/utility/Macros.h"

//conditional expression can be constant if it wants to be
#pragma warning(disable : 4127)

#if MODE == DEBUG_MODE
#define DEBUG_BREAK(...) __debugbreak()
#define ON_DEBUG(...) __VA_ARGS__
#else
#define DEBUG_BREAK(...)
#define ON_DEBUG(...)
#endif

#if DEBUG_ENABLED

//##############################################################################
bool TestIsExpectingError(void);
void TestSetExpectError(bool expect);

//##############################################################################
struct TestExpectedError
{
  TestExpectedError(char const * str, ...);

  std::string error;
};

//##############################################################################
struct AssertionError
{
  AssertionError(char const * str, ...);

  std::string error;
};

//##############################################################################
#define EXPECT_ERROR(...)             \
  do                                  \
  {                                   \
    bool testGotError = false;        \
    TestSetExpectError(true);         \
                                      \
    try                               \
    {                                 \
      __VA_ARGS__                     \
    }                                 \
    catch (TestExpectedError const &) \
    {                                 \
      testGotError = true;            \
    }                                 \
                                      \
    if (!testGotError)                \
    {                                 \
      TestSetExpectError(false);      \
      ERROR("error expecter");        \
    }                                 \
  }                                   \
  while (false)

//##############################################################################
#define DEBUG_LOG(...)                 \
  do                                   \
  {                                    \
    std::fprintf(stderr, __VA_ARGS__); \
    std::fprintf(stderr, "\n");        \
  }                                    \
  while (false)

//##############################################################################
#define ERROR_LOG(...)                      \
  do                                        \
  {                                         \
    if (TestIsExpectingError())             \
      throw TestExpectedError(__VA_ARGS__); \
    DEBUG_LOG(__VA_ARGS__);                 \
    DEBUG_BREAK();                          \
  }                                         \
  while (false)

//##############################################################################
#define ERROR(...)                     \
  do                                   \
  {                                    \
    ERROR_LOG(__VA_ARGS__);            \
    throw AssertionError(__VA_ARGS__); \
  }                                    \
  while (false)

//##############################################################################
#define ASSERT_ACTUAL(condition, ...)    \
  do                                     \
  {                                      \
    if (!(condition))                    \
    {                                    \
      ERROR_LOG(__VA_ARGS__);            \
      throw AssertionError(__VA_ARGS__); \
    }                                    \
  }                                      \
  while (false)

//##############################################################################
#define ASSERT(...) EXPAND(MACRO_NAME_HAS_EXTRA_ARGS(ASSERT_, __VA_ARGS__)(__VA_ARGS__))
#define ASSERT_SINGLE(condition) ASSERT_ACTUAL(condition, "%s", #condition)
#define ASSERT_MULTI(condition, ...) ASSERT_ACTUAL(condition, __VA_ARGS__)

#else

//I don't care if I no longer care about a parameter
#pragma warning(disable : 4189)

//##############################################################################
#define EXPECT_ERROR(...)
#define DEBUG_LOG(...)
#define ERROR_LOG(...)
#define ERROR(...)
#define ASSERT(...)

#endif

#endif
