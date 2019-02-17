#include "engine/utility/Random.h"

#include <cstdlib>
#include <ctime>

namespace
{
  unsigned storedSeed = 0;
}

//##############################################################################
void InitializeRandom(void)
{
  InitializeRandom(unsigned(std::time(nullptr)));
}

//##############################################################################
void InitializeRandom(unsigned seed)
{
  storedSeed = seed;
  std::srand(unsigned(seed));
}

//##############################################################################
unsigned GetRandomSeed(void)
{
  return storedSeed;
}

//##############################################################################
unsigned GetRandomNumber(unsigned exclusiveMax)
{
  return std::rand() % exclusiveMax;
}

//##############################################################################
float GetRandomNumber(void)
{
  return GetRandomNumber(1319) / 1319.0f;
}

