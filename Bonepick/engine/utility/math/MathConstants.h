#ifndef ENGINE_UTILITY_MATH_MATHCONSTANTS_H
#define ENGINE_UTILITY_MATH_MATHCONSTANTS_H

#include "engine/utility/Typedefs.h"

//##############################################################################
float const Pi = 3.14159265358979323846264338328;
float const Epsilon = 0.00001;

//##############################################################################
template <unsigned exponent>
constexpr unsigned Pow(unsigned base)
{
  return Pow<exponent - 1>(base) * base;
}

//##############################################################################
template <>
constexpr unsigned Pow<0>(unsigned)
{
  return 1;
}

#endif
