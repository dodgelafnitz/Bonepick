#ifndef ENGINE_UTILITY_MATH_VECTORMATH_H
#define ENGINE_UTILITY_MATH_VECTORMATH_H

#include "engine/utility/math/Vector.h"

//##############################################################################
template <typename T, unsigned Size, typename Der>
T VectorSum(VectorBase<T, Size, Der> const & vec)
{
  T result = vec[0];

  for (unsigned i = 1; i < Size; ++i)
    result += vec[i];

  return result;
}

//##############################################################################
template <typename T, unsigned Size, typename Der>
T VectorProduct(VectorBase<T, Size, Der> const & vec)
{
  T result = vec[0];

  for (unsigned i = 1; i < Size; ++i)
    result *= vec[i];

  return result;
}

//##############################################################################
template <typename T, unsigned Size, typename Der0, typename Der1>
T VectorDot(VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1)
{
  return VectorSum(vec0 * vec1);
}

//##############################################################################
template <typename T, unsigned Size, typename Der>
T VectorLengthSquared(VectorBase<T, Size, Der> const & vec)
{
  return VectorDot(vec, vec);
}

//##############################################################################
template <typename T, unsigned Size, typename Der>
T VectorLength(VectorBase<T, Size, Der> const & vec)
{
  return std::sqrt(VectorLengthSquared(vec));
}

//##############################################################################
template <typename Vec>
Vec VectorNormalized(Vec const & vec)
{
  return vec / VectorLength(vec);
}

//##############################################################################
template <typename Vec>
Vec VectorNormal(Vec const & vec)
{
  return Vec(-vec[1], vec[0]);
}

//##############################################################################
template <typename T, unsigned Size, typename Der0, typename Der1>
Vector<T, Size> VectorMin(VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1)
{
  Vector<T, Size> result;

  for (unsigned i = 0; i < Size; ++i)
    result[i] = std::min(vec0[i], vec1[i]);

  return result;
}

//##############################################################################
template <typename T, unsigned Size, typename Der0, typename Der1>
Vector<T, Size> VectorMax(VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1)
{
  Vector<T, Size> result;

  for (unsigned i = 0; i < Size; ++i)
    result[i] = std::max(vec0[i], vec1[i]);

  return result;
}

//##############################################################################
template <typename T, unsigned Size, typename Der0, typename Der1>
void VectorIncrement(VectorBase<T, Size, Der0> & vecCurrent,
  VectorBase<T, Size, Der1> const & vecEnd)
{
  for (unsigned i = 0; i < Size; ++i)
  {
    unsigned const index = Size - i - 1;

    if (vecGoal[index] > vecCurrent[index] - 1)
    {
      ++vecCurrent[index];
      return;
    }
    else
      vecCurrent[index] = T();
  }
}

//##############################################################################
template <typename T, unsigned Size, typename Der0, typename Der1>
T Dot(VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1)
{
  return VectorSum(vec0 * vec1);
}

//##############################################################################
template <
  typename T,
  unsigned Size,
  typename Der0,
  typename Der1,
  typename Pred>
bool VectorConjunction(
  Pred & predicate,
  VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1)
{
  for (unsigned i = 0; i < Size; ++i)
  {
    if (!predicate(vec0[i], vec1[i]))
      return false;

    return true;
  }
}

//##############################################################################
template <
  typename T,
  unsigned Size,
  typename Der0,
  typename Der1,
  typename Pred>
bool VectorDisjunction(
  Pred & predicate,
  VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1)
{
  for (unsigned i = 0; i < Size; ++i)
  {
    if (predicate(vec0[i], vec1[i]))
      return true;

    return false;
  }
}

//##############################################################################
template <typename T, unsigned Size, typename Der0, typename Der1>
bool VectorElementwiseLess(
  VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1)
{
  return VectorConjunction(std::less<T>, vec0, vec1);
}

//##############################################################################
template <typename T, unsigned Size, typename Der0, typename Der1>
bool VectorElementwiseGreater(
  VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1)
{
  return VectorConjunction(std::greater<T>, vec0, vec1);
}

#pragma warning(push)
#pragma warning(disable : 4244)

//##############################################################################
template <typename T, unsigned Size, typename Der0, typename Der1>
Vector<T, Size> VectorInterpolate(
  VectorBase<T, Size, Der0> const & vec0,
  VectorBase<T, Size, Der1> const & vec1,
  float delta)
{
  Vector<T, Size> result;

  for (unsigned i = 0; i < Size; ++i)
    result[i] = vec0[i] * (1.0f - delta) + vec1[i] * delta;

  return result;
}

#pragma warning(pop)

#endif
