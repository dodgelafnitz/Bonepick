#ifndef ENGINE_UTILITY_MATH_VECTOR_H
#define ENGINE_UTILITY_MATH_VECTOR_H

#include "engine/Utility/Debug.h"
#include "engine/Utility/Typedefs.h"

//##############################################################################
#define DECLARE_BINARY_ASSIGNMENT_OP(op)                            \
  template <typename U, typename UDerived>                          \
  Derived & operator op(VectorBase<U, Size, UDerived> const & vec); \
  Derived & operator op(T const & value);

//##############################################################################
#define DECLARE_BINARY_OP(op)                                           \
  template <typename U, typename UDerived>                              \
  Derived operator op(VectorBase<U, Size, UDerived> const & vec) const; \
  Derived operator op(T const & value) const;

//##############################################################################
#define DECLARE_UNARY_OP(op)       \
  Derived operator op(void) const;

//##############################################################################
#define DECLARE_INCREMENT_PREFIX_OP(op) \
  Derived & operator op(void);

//##############################################################################
#define DECLARE_INCREMENT_POSTFIX_OP(op) \
  Derived operator op(int);

//##############################################################################
#define DEFINE_BINARY_ASSIGNMENT_OP(op)                                 \
  template <typename T, unsigned Size, typename Derived>                \
  template <typename U, typename UDerived>                              \
  Derived & VectorBase<T, Size, Derived>::operator op(                  \
    VectorBase<U, Size, UDerived> const & vec)                          \
  {                                                                     \
    Derived & self = *reinterpret_cast<Derived *>(this);                \
    UDerived const & other = *reinterpret_cast<UDerived const *>(&vec); \
                                                                        \
    for (unsigned i = 0; i < Size; ++i)                                 \
      self[i] op other[i];                                              \
                                                                        \
    return self;                                                        \
  }                                                                     \
                                                                        \
  template <typename T, unsigned Size, typename Derived>                \
  Derived & VectorBase<T, Size, Derived>::operator op(T const & val)    \
  {                                                                     \
    Derived & self = *reinterpret_cast<Derived *>(this);                \
                                                                        \
    for (unsigned i = 0; i < Size; ++i)                                 \
      self[i] op val;                                                   \
                                                                        \
    return self;                                                        \
  }

//##############################################################################
#define DEFINE_BINARY_OP(op)                                             \
  template <typename T, unsigned Size, typename Derived>                 \
  template <typename U, typename UDerived>                               \
  Derived VectorBase<T, Size, Derived>::operator op(                     \
    VectorBase<U, Size, UDerived> const & vec) const                     \
  {                                                                      \
    Derived const & self = *reinterpret_cast<Derived const *>(this);     \
                                                                         \
    return Derived(self).operator op##=(vec);                            \
  }                                                                      \
                                                                         \
  template <typename T, unsigned Size, typename Derived>                 \
  Derived VectorBase<T, Size, Derived>::operator op(T const & val) const \
  {                                                                      \
    Derived const & self = *reinterpret_cast<Derived const *>(this);     \
                                                                         \
    return Derived(self).operator op##=(val);                            \
  }

//##############################################################################
#define DEFINE_UNARY_OP(op)                                     \
  template <typename T, unsigned Size, typename Derived>        \
  Derived VectorBase<T, Size, Derived>::operator op(void) const \
  {                                                             \
    Derived & self = *reinterpret_cast<Derived const *>(this);  \
                                                                \
    Derived newVec;                                             \
    for (unsigned i = 0; i < Size; ++i)                         \
      newVec[i] = op self[i];                                   \
                                                                \
    return newVec;                                              \
  }

//##############################################################################
#define DEFINE_INCREMENT_PREFIX_OP(op)                      \
  template <typename T, unsigned Size, typename Derived>    \
  Derived & VectorBase<T, Size, Derived>::operator op(void) \
  {                                                         \
    Derived & self = *reinterpret_cast<Derived *>(this);    \
                                                            \
    for (unsigned i = 0; i < Size; ++i)                     \
      op self[i];                                           \
                                                            \
    return self;                                            \
  }

//##############################################################################
#define DEFINE_INCREMENT_POSTFIX_OP(op)                  \
  template <typename T, unsigned Size, typename Derived> \
  Derived VectorBase<T, Size, Derived>::operator op(int) \
  {                                                      \
    Derived & self = *reinterpret_cast<Derived *>(this); \
    Derived newVec = self;                               \
                                                         \
    for (unsigned i = 0; i < Size; ++i)                  \
      op self[i];                                        \
                                                         \
    return newVec;                                       \
  }

//##############################################################################
#define DECLARE_COMMON_VECTOR_OPERATIONS(...)  \
  T * begin(void);                             \
  T * end(void);                               \
  T const * begin(void) const;                 \
  T const * end(void) const;

//##############################################################################
#define DEFINE_COMMON_VECTOR_OPERATIONS(size)            \
  template <typename T>                                  \
  T * Vector<T, size>::begin(void)                       \
  {                                                      \
    static_assert(sizeof(Vector) == Size * sizeof(T),    \
      "sizes are mismatched for optimization");          \
                                                         \
    return &x;                                           \
  }                                                      \
                                                         \
  template <typename T>                                  \
  T * Vector<T, size>::end(void)                         \
  {                                                      \
    static_assert(sizeof(Vector) == Size * sizeof(T),    \
      "sizes are mismatched for optimization");          \
                                                         \
    return (&x) + Size;                                  \
  }                                                      \
                                                         \
  template <typename T>                                  \
  T const * Vector<T, size>::begin(void) const           \
  {                                                      \
    return (const_cast<Vector *>(this))->begin();        \
  }                                                      \
                                                         \
  template <typename T>                                  \
  T const * Vector<T, size>::end(void) const             \
  {                                                      \
    return (const_cast<Vector *>(this))->end();          \
  }

//##############################################################################
template <typename T, unsigned Size, typename Derived>
struct VectorBase
{
  static_assert(Size > 1, "Vector size must be greater than 1");

  static unsigned const Size = Size;

  template <typename U, typename UDerived>
  Derived & operator =(VectorBase<U, Size, UDerived> const & vec);

  template <typename U, typename UDerived>
  bool operator ==(VectorBase<U, Size, UDerived> const & vec) const;
  template <typename U, typename UDerived>
  bool operator !=(VectorBase<U, Size, UDerived> const & vec) const;
  template <typename U, typename UDerived>
  bool operator <(VectorBase<U, Size, UDerived> const & vec) const;
  template <typename U, typename UDerived>
  bool operator >(VectorBase<U, Size, UDerived> const & vec) const;
  template <typename U, typename UDerived>
  bool operator <=(VectorBase<U, Size, UDerived> const & vec) const;
  template <typename U, typename UDerived>
  bool operator >=(VectorBase<U, Size, UDerived> const & vec) const;

  T & operator [](unsigned index);
  T const & operator [](unsigned index) const;

  DECLARE_BINARY_ASSIGNMENT_OP(+=)
  DECLARE_BINARY_ASSIGNMENT_OP(-=)
  DECLARE_BINARY_ASSIGNMENT_OP(*=)
  DECLARE_BINARY_ASSIGNMENT_OP(/=)
  DECLARE_BINARY_ASSIGNMENT_OP(%=)
  DECLARE_BINARY_ASSIGNMENT_OP(<<=)
  DECLARE_BINARY_ASSIGNMENT_OP(>>=)
  DECLARE_BINARY_ASSIGNMENT_OP(^=)
  DECLARE_BINARY_ASSIGNMENT_OP(&=)
  DECLARE_BINARY_ASSIGNMENT_OP(|=)

  DECLARE_BINARY_OP(+)
  DECLARE_BINARY_OP(-)
  DECLARE_BINARY_OP(*)
  DECLARE_BINARY_OP(/)
  DECLARE_BINARY_OP(%)
  DECLARE_BINARY_OP(<<)
  DECLARE_BINARY_OP(>>)
  DECLARE_BINARY_OP(^)
  DECLARE_BINARY_OP(&)
  DECLARE_BINARY_OP(|)

  DECLARE_UNARY_OP(+)
  DECLARE_UNARY_OP(-)
  DECLARE_UNARY_OP(!)
  DECLARE_UNARY_OP(~)

  DECLARE_INCREMENT_PREFIX_OP(++)
  DECLARE_INCREMENT_POSTFIX_OP(++)
  DECLARE_INCREMENT_PREFIX_OP(--)
  DECLARE_INCREMENT_POSTFIX_OP(--)
};

//##############################################################################
template <typename T, unsigned Size>
class Vector : public VectorBase<T, Size, Vector<T, Size>>
{
public:
  Vector(void);

  template <typename ... Args>
  Vector(T const & value, Args const & ... remainder);

  template <typename U, typename UDerived>
  Vector(VectorBase<U, Size, UDerived> const & vec);

  DECLARE_COMMON_VECTOR_OPERATIONS()

private:
  template <typename ... Args>
  void SetInternal(T const & value, Args const & ... remainder);
  void SetInternal(void);

  T data_[Size];
};

//##############################################################################
template <typename T>
struct Vector<T, 2> : public VectorBase<T, 2, Vector<T, 2>>
{
  Vector(void);
  Vector(T const & x, T const & y);

  template <typename U, typename UDerived>
  Vector(VectorBase<U, 2, UDerived> const & vec);

  DECLARE_COMMON_VECTOR_OPERATIONS()

  T x;
  T y;
};

//##############################################################################
template <typename T>
struct Vector<T, 3> : public VectorBase<T, 3, Vector<T, 3>>
{
  Vector(void);
  Vector(T const & x, T const & y, T const & z);

  template <typename U, typename UDerived>
  Vector(VectorBase<U, 3, UDerived> const & vec);

  DECLARE_COMMON_VECTOR_OPERATIONS()

  T x;
  T y;
  T z;
};

//##############################################################################
template <typename T>
struct Vector<T, 4> : public VectorBase<T, 4, Vector<T, 4>>
{
  Vector(void);
  Vector(T const & x, T const & y, T const & z, T const & w);

  template <typename U, typename UDerived>
  Vector(VectorBase<U, 4, UDerived> const & vec);

  DECLARE_COMMON_VECTOR_OPERATIONS()

  T x;
  T y;
  T z;
  T w;
};

//##############################################################################
template <typename T, unsigned Size, typename Derived>
template <typename U, typename UDerived>
Derived & VectorBase<T, Size, Derived>::operator =(
  VectorBase<U, Size, UDerived> const & vec)
{
  Derived & self = *reinterpret_cast<Derived *>(this);
  UDerived const & other = *reinterpret_cast<UDerived const *>(&vec);

  for (unsigned i = 0; i < Size; ++i)
    self[i] = other[i];

  return self;
}

//##############################################################################
template <typename T, unsigned Size, typename Derived>
template <typename U, typename UDerived>
bool VectorBase<T, Size, Derived>::operator ==(
  VectorBase<U, Size, UDerived> const & vec) const
{
  Derived const & self = *reinterpret_cast<Derived const *>(this);
  UDerived const & other = *reinterpret_cast<UDerived const *>(&vec);

  for (unsigned i = 0; i < Size; ++i)
  {
    if (self[i] != other[i])
      return false;
  }

  return true;
}

//##############################################################################
template <typename T, unsigned Size, typename Derived>
template <typename U, typename UDerived>
bool VectorBase<T, Size, Derived>::operator !=(
  VectorBase<U, Size, UDerived> const & vec) const
{
  return !(*this == vec);
}

//##############################################################################
template <typename T, unsigned Size, typename Derived>
template <typename U, typename UDerived>
bool VectorBase<T, Size, Derived>::operator <(
  VectorBase<U, Size, UDerived> const & vec) const
{
  Derived const & self = *reinterpret_cast<Derived const *>(this);
  UDerived const & other = *reinterpret_cast<UDerived const *>(&vec);

  for (unsigned i = 0; i < Size; ++i)
  {
    if (self[i] < other[i])
      return true;
    else if (other[i] < self[i])
      return false;
  }

  return false;
}

//##############################################################################
template <typename T, unsigned Size, typename Derived>
template <typename U, typename UDerived>
bool VectorBase<T, Size, Derived>::operator >(
  VectorBase<U, Size, UDerived> const & vec) const
{
  return vec < *this;
}

//##############################################################################
template <typename T, unsigned Size, typename Derived>
template <typename U, typename UDerived>
bool VectorBase<T, Size, Derived>::operator <=(
  VectorBase<U, Size, UDerived> const & vec) const
{
  return !(vec < *this);
}

//##############################################################################
template <typename T, unsigned Size, typename Derived>
template <typename U, typename UDerived>
bool VectorBase<T, Size, Derived>::operator >=(
  VectorBase<U, Size, UDerived> const & vec) const
{
  return !(*this < vec);
}

//##############################################################################
template <typename T, unsigned Size, typename Derived>
T & VectorBase<T, Size, Derived>::operator [](unsigned index)
{
  Derived & self = *reinterpret_cast<Derived *>(this);

  static_assert(sizeof(Derived) == Size * sizeof(T),
    "sizes are mismatched for optimization");

  ASSERT(index < Size);

  return *(self.begin() + index);
}

//##############################################################################
template <typename T, unsigned Size, typename Derived>
T const & VectorBase<T, Size, Derived>::operator [](unsigned index) const
{
  Derived const & self = *reinterpret_cast<Derived const *>(this);

  static_assert(sizeof(Derived) == Size * sizeof(T),
    "sizes are mismatched for optimization");

  ASSERT(index < Size);

  return *(self.begin() + index);
}

//##############################################################################
DEFINE_BINARY_ASSIGNMENT_OP(+=)
DEFINE_BINARY_ASSIGNMENT_OP(-=)
DEFINE_BINARY_ASSIGNMENT_OP(*=)
DEFINE_BINARY_ASSIGNMENT_OP(/=)
DEFINE_BINARY_ASSIGNMENT_OP(%=)
DEFINE_BINARY_ASSIGNMENT_OP(<<=)
DEFINE_BINARY_ASSIGNMENT_OP(>>=)
DEFINE_BINARY_ASSIGNMENT_OP(^=)
DEFINE_BINARY_ASSIGNMENT_OP(&=)
DEFINE_BINARY_ASSIGNMENT_OP(|=)

//##############################################################################
DEFINE_BINARY_OP(+)
DEFINE_BINARY_OP(-)
DEFINE_BINARY_OP(*)
DEFINE_BINARY_OP(/)
DEFINE_BINARY_OP(%)
DEFINE_BINARY_OP(<<)
DEFINE_BINARY_OP(>>)
DEFINE_BINARY_OP(^)
DEFINE_BINARY_OP(&)
DEFINE_BINARY_OP(|)

//##############################################################################
DEFINE_UNARY_OP(+)
DEFINE_UNARY_OP(-)
DEFINE_UNARY_OP(!)
DEFINE_UNARY_OP(~)

//##############################################################################
DEFINE_INCREMENT_PREFIX_OP(++)
DEFINE_INCREMENT_POSTFIX_OP(++)
DEFINE_INCREMENT_PREFIX_OP(--)
DEFINE_INCREMENT_POSTFIX_OP(--)

//##############################################################################
template <typename T>
Vector<T, 2>::Vector(void) :
  x(),
  y()
{}

//##############################################################################
template <typename T>
Vector<T, 2>::Vector(T const & x, T const & y) :
  x(x),
  y(y)
{}

//##############################################################################
template <typename T>
template <typename U, typename UDerived>
Vector<T, 2>::Vector(VectorBase<U, 2, UDerived> const & vec) :
  x(T(vec[0])),
  y(T(vec[1]))
{}

//##############################################################################
template <typename T>
Vector<T, 3>::Vector(void) :
  x(),
  y(),
  z()
{}

//##############################################################################
template <typename T>
Vector<T, 3>::Vector(T const & x, T const & y, T const & z) :
  x(x),
  y(y),
  z(z)
{}

//##############################################################################
template <typename T>
template <typename U, typename UDerived>
Vector<T, 3>::Vector(VectorBase<U, 3, UDerived> const & vec) :
  x(T(vec[0])),
  y(T(vec[1])),
  z(T(vec[2]))
{}

//##############################################################################
template <typename T>
Vector<T, 4>::Vector(void) :
  x(),
  y(),
  z(),
  w()
{}

//##############################################################################
template <typename T>
Vector<T, 4>::Vector(T const & x, T const & y, T const & z, T const & w) :
  x(x),
  y(y),
  z(z),
  w(w)
{}

//##############################################################################
template <typename T>
template <typename U, typename UDerived>
Vector<T, 4>::Vector(VectorBase<U, 4, UDerived> const & vec) :
  x(T(vec[0])),
  y(T(vec[1])),
  z(T(vec[2])),
  w(T(vec[3]))
{}

//##############################################################################
template <typename T, unsigned Size>
Vector<T, Size>::Vector(void)
{
  for (unsigned i = 0; i < Size; ++i)
    data_[i] = T();
}

//##############################################################################
template <typename T, unsigned Size>
template <typename ... Args>
Vector<T, Size>::Vector(T const & value, Args const & ... remainder)
{
  static_assert(sizeof...(remainder) + 1 == Size,
    "incorrect number of values passed to Vector ctor");

  SetInternal(value, remainder...);
}

//##############################################################################
template <typename T, unsigned Size>
template <typename U, typename UDerived>
Vector<T, Size>::Vector(VectorBase<U, Size, UDerived> const & vec)
{
  for (unsigned i = 0; i < Size; ++i)
    data_[i] = T(vec[i]);
}

//##############################################################################
DEFINE_COMMON_VECTOR_OPERATIONS(2)
DEFINE_COMMON_VECTOR_OPERATIONS(3)
DEFINE_COMMON_VECTOR_OPERATIONS(4)

//##############################################################################
template <typename T, unsigned Size>
T * Vector<T, Size>::begin(void)
{
  static_assert(sizeof(Vector) == Size * sizeof(T),
    "sizes are mismatched for optimization");

  return data_;
}

//##############################################################################
template <typename T, unsigned Size>
T * Vector<T, Size>::end(void)
{
  static_assert(sizeof(Vector) == Size * sizeof(T),
    "sizes are mismatched for optimization");

  return &(data_[0]) + Size;
}

//##############################################################################
template <typename T, unsigned Size>
T const * Vector<T, Size>::begin(void) const
{
  return (const_cast<Vector *>(this))->begin();
}

//##############################################################################
template <typename T, unsigned Size>
T const * Vector<T, Size>::end(void) const
{
  return (const_cast<Vector *>(this))->end();
}

//##############################################################################
template <typename T, unsigned Size>
template <typename ... Args>
void Vector<T, Size>::SetInternal(T const & value, Args const & ... remainder)
{
  data_[Size - 1 - sizeof...(remainder)] = value;
  SetInternal(remainder...);
}

//##############################################################################
template <typename T, unsigned Size>
void Vector<T, Size>::SetInternal(void)
{}

//##############################################################################
#undef DECLARE_BINARY_ASSIGNMENT_OP
#undef DECLARE_BINARY_OP
#undef DECLARE_UNARY_OP
#undef DECLARE_INCREMENT_PREFIX_OP
#undef DECLARE_INCREMENT_POSTFIX_OP
#undef DEFINE_BINARY_ASSIGNMENT_OP
#undef DEFINE_BINARY_OP
#undef DEFINE_UNARY_OP
#undef DEFINE_INCREMENT_PREFIX_OP
#undef DEFINE_INCREMENT_POSTFIX_OP

typedef Vector<unsigned, 2> UVec2;
typedef Vector<unsigned, 3> UVec3;
typedef Vector<unsigned, 4> UVec4;
typedef Vector<int, 2> IVec2;
typedef Vector<int, 3> IVec3;
typedef Vector<int, 4> IVec4;
typedef Vector<float, 2> Vec2;
typedef Vector<float, 3> Vec3;
typedef Vector<float, 4> Vec4;

#endif
