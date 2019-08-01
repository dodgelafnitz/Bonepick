#ifndef ENGINE_UTILITY_RANDOM_H
#define ENGINE_UTILITY_RANDOM_H

#include "utility/Typedefs.h"

void InitializeRandom(void);
void InitializeRandom(unsigned seed);

unsigned GetRandomSeed(void);

unsigned GetRandomNumber(unsigned exclusiveMax);
float GetRandomNumber(void);

#endif
