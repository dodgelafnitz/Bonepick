#ifndef ENGINE_UTILITY_CONTAINERS_GRID_H
#define ENGINE_UTILITY_CONTAINERS_GRID_H

#include <algorithm>
#include <vector>

#include "utility/Debug.h"
#include "utility/math/Vector.h"
#include "utility/math/VectorMath.h"
#include "utility/Typedefs.h"

//##############################################################################
template <typename T, typename SizeType = unsigned>
class Grid
{
public:
  typedef bool (*ApplySkipFunction)(T const &);

  Grid(void) = default;
  Grid(Grid const & grid) = default;
  Grid(Grid && grid) = default;
  template <typename U>
  Grid(Grid<U> const & grid);
  Grid(UVec2 const & size);
  Grid(UVec2 const & size, T const & value);
  Grid(UVec2 const & size, T const * buffer);
  Grid(SizeType width, SizeType height);
  Grid(SizeType width, SizeType height, T const & value);
  Grid(SizeType width, SizeType height, T const * buffer);

  Grid & operator = (Grid const & grid) = default;
  Grid & operator = (Grid && grid) = default;

  SizeType GetWidth(void) const;
  SizeType GetHeight(void) const;
  UVec2 GetSize(void) const;
  SizeType GetCount(void) const;

  bool Contains(UVec2 const & pos) const;
  bool Contains(SizeType index) const;
  bool Contains(SizeType xPos, SizeType yPos) const;

  void SetWidth(SizeType width);
  void SetHeight(SizeType height);
  void Resize(UVec2 const & size);
  void Resize(SizeType width, SizeType height);
  void Clear(T const & value);
  void ClearAndResize(UVec2 const & size, T const & value);
  void ClearAndResize(SizeType width, SizeType height, T const & value);

  T GetInterpolatedValue(Vec2 const & pos) const;
  T GetInterpolatedValue(float xPos, float yPos) const;

  void ApplyGrid(Grid const & grid, UVec2 const & pos);
  void ApplyGrid(Grid const & grid, SizeType xPos, SizeType yPos);
  void ApplyGrid(Grid const & grid, UVec2 const & pos,
    ApplySkipFunction skipFunction);
  void ApplyGrid(Grid const & grid, SizeType xPos, SizeType yPos,
    ApplySkipFunction skipFunction);

  Grid GetSubGrid(UVec2 const & pos, UVec2 const & size) const;
  Grid GetSubGrid(UVec2 const & pos, SizeType width, SizeType height) const;
  Grid GetSubGrid(SizeType xPos, SizeType yPos, UVec2 const & size) const;
  Grid GetSubGrid(SizeType xPos, SizeType yPos, SizeType width, SizeType height)
    const;

  UVec2 GetPosFromIndex(SizeType index) const;
  SizeType GetIndexFromPos(UVec2 const & pos) const;
  SizeType GetIndexFromPos(SizeType xPos, SizeType yPos) const;

  T const & GetValue(UVec2 const & pos) const;
  T const & GetValue(SizeType xPos, SizeType yPos) const;
  T const & GetValue(SizeType index) const;
  T & GetValue(UVec2 const & pos);
  T & GetValue(SizeType xPos, SizeType yPos);
  T & GetValue(SizeType index);
  void SetValue(UVec2 const & pos, T const & value);
  void SetValue(SizeType xPos, SizeType yPos, T const & value);
  void SetValue(SizeType index, T const & value);

  T const & operator [](SizeType index) const;
  T & operator [](SizeType index);

  typename std::vector<T>::iterator begin(void);
  typename std::vector<T>::iterator end(void);
  typename std::vector<T>::const_iterator begin(void) const;
  typename std::vector<T>::const_iterator end(void) const;

private:
  UVec2 size_;
  std::vector<T> data_;
};

//##############################################################################
template <typename T, typename SizeType>
template <typename U>
Grid<T, SizeType>::Grid(Grid<U> const & grid) :
  size_(size)
{
  SizeType const count = size.x * size.y;

  data_.reserve(count);
  for (SizeType i = 0; i < count; ++i)
    data_.emplace_back(grid.GetValue(i));
}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType>::Grid(UVec2 const & size) :
  size_(size),
  data_(size.x * size.y)
{}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType>::Grid(UVec2 const & size, T const & value) :
  size_(size),
  data_(size.x * size.y, value)
{}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType>::Grid(UVec2 const & size, T const * buffer) :
  size_(size)
{
  SizeType const count = size.x * size.y;

  data_.reserve(count);
  for (SizeType i = 0; i < count; ++i)
    data_.emplace_back(buffer[i]);
}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType>::Grid(SizeType width, SizeType height) :
  Grid(UVec2(width, height))
{}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType>::Grid(SizeType width, SizeType height, T const & value) :
  Grid(UVec2(width, height), value)
{}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType>::Grid(SizeType width, SizeType height, T const * buffer) :
  Grid(UVec2(width, height), buffer)
{}

//##############################################################################
template <typename T, typename SizeType>
SizeType Grid<T, SizeType>::GetWidth(void) const
{
  return size_.x;
}

//##############################################################################
template <typename T, typename SizeType>
SizeType Grid<T, SizeType>::GetHeight(void) const
{
  return size_.y;
}

//##############################################################################
template <typename T, typename SizeType>
UVec2 Grid<T, SizeType>::GetSize(void) const
{
  return size_;
}

//##############################################################################
template <typename T, typename SizeType>
SizeType Grid<T, SizeType>::GetCount(void) const
{
  ASSERT(size_.x * size_.y == data_.size());

  return size_.x * size_.y;
}

//##############################################################################
template <typename T, typename SizeType>
bool Grid<T, SizeType>::Contains(UVec2 const & pos) const
{
  ASSERT(size_.x * size_.y == data_.size());

  return pos.x < size_.x && pos.y < size_.y;
}

//##############################################################################
template <typename T, typename SizeType>
bool Grid<T, SizeType>::Contains(SizeType index) const
{
  return index < GetCount();
}

//##############################################################################
template <typename T, typename SizeType>
bool Grid<T, SizeType>::Contains(SizeType xPos, SizeType yPos) const
{
  return Contains(UVec2(xPos, yPos));
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::SetWidth(SizeType width)
{
  Resize(width, size_.y);
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::SetHeight(SizeType height)
{
  Resize(size_.x, height);
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::Resize(UVec2 const & size)
{
  if (size.x == size_.x && size.y == size_.y)
    return;

  SizeType const copyWidth = std::min(size.x, size_.x);
  SizeType const copyHeight = std::min(size.y, size_.y);

  Grid<T, SizeType> newGrid(size.x, size.y);
  for (SizeType i = 0; i < copyHeight; ++i)
  {
    for (SizeType j = 0; j < copyWidth; ++j)
    {
      SizeType const index = i * size_.x + j;
      SizeType const newIndex = i * size.x + j;

      newGrid.data_[newIndex] = data_[index];
    }
  }

  *this = newGrid;
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::Resize(SizeType width, SizeType height)
{
  Resize(UVec2(width, height));
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::Clear(T const & value)
{
  for (T & data : data_)
    data = value;
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::ClearAndResize(UVec2 const & size, T const & value)
{
  if (size.x == size_.x && size.y == size_.y)
  {
    Clear(value);
    return;
  }

  size_ = size;
  data_.resize(size_.x * size_.y, value);
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::ClearAndResize(SizeType width, SizeType height, T const & value)
{
  ClearAndResize(UVec2(width, height), value);
}

//##############################################################################
template <typename T, typename SizeType>
T Grid<T, SizeType>::GetInterpolatedValue(Vec2 const & pos) const
{
  return GetInterpolatedValue(pos.x, pos.y);
}

//#pragma warning(push)
//#pragma warning(disable : 4244)

//##############################################################################
template <typename T, typename SizeType>
T Grid<T, SizeType>::GetInterpolatedValue(float xPos, float yPos) const
{
  ASSERT(xPos <= float(GetWidth() - 1));
  ASSERT(yPos <= float(GetHeight() - 1));
  ASSERT(xPos >= 0.0f);
  ASSERT(yPos >= 0.0f);

  SizeType xIndices[2] =
  {
    SizeType(xPos),
    xPos == SizeType(xPos) ? SizeType(xPos) : SizeType(xPos) + 1
  };

  float xDelta = xPos - xIndices[0];

  SizeType yIndices[2] =
  {
    SizeType(yPos),
    yPos == SizeType(yPos) ? SizeType(yPos) : SizeType(yPos) + 1
  };

  float yDelta = yPos - yIndices[0];

  T const * values[] =
  {
    &GetValue(xIndices[0], yIndices[0]),
    &GetValue(xIndices[0], yIndices[1]),
    &GetValue(xIndices[1], yIndices[0]),
    &GetValue(xIndices[1], yIndices[1])
  };

  T weight0 = *values[0];
  T weight1 = *values[1];
  T weight2 = *values[2];
  T weight3 = *values[3];

  T topAvg = VectorInterpolate(weight0, weight2, xDelta);
  T bottomAvg = VectorInterpolate(weight1, weight3, xDelta);

  T avg = VectorInterpolate(topAvg, bottomAvg, yDelta);

  return avg;
}

//#pragma warning(pop)

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::ApplyGrid(Grid const & grid, UVec2 const & pos)
{
  ApplyGrid(grid, pos, nullptr);
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::ApplyGrid(Grid const & grid, SizeType xPos, SizeType yPos)
{
  ApplyGrid(grid, UVec2(xPos, yPos), nullptr);
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::ApplyGrid(Grid const & grid, UVec2 const & pos,
  ApplySkipFunction skipFunction)
{
  UVec2 applySize = grid.size_;
  UVec2 applyPos = pos;

  ASSERT(applyPos.x < size_.x);
  ASSERT(applyPos.y < size_.y);
  ASSERT(applyPos.x + applySize.x <= size_.x);
  ASSERT(applyPos.y + applySize.y <= size_.y);

  for (SizeType i = 0; i < applySize.y; ++i)
  {
    for (SizeType j = 0; j < applySize.x; ++j)
    {
      UVec2 const currentOtherPos(j, i);
      T const & value = grid.GetValue(currentOtherPos);

      if (skipFunction == nullptr || !skipFunction(value))
      {
        UVec2 const currentPos(applyPos.x + j, applyPos.y + i);
        SetValue(currentPos, value);
      }
    }
  }
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::ApplyGrid(Grid const & grid, SizeType xPos, SizeType yPos,
  ApplySkipFunction skipFunction)
{
  ApplyGrid(grid, UVec2(xPos, yPos), skipFunction);
}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType> Grid<T, SizeType>::GetSubGrid(UVec2 const & pos, UVec2 const & size) const
{
  UVec2 subPos = pos;
  UVec2 subSize = size;

  ASSERT(subPos.x < size_.x);
  ASSERT(subPos.y < size_.y);
  ASSERT(subPos.x + subSize.x <= size_.x);
  ASSERT(subPos.y + subSize.y <= size_.y);

  Grid subGrid(subSize);

  for (SizeType i = 0; i < subSize.y; ++i)
  {
    for (SizeType j = 0; j < subSize.x; ++j)
    {
      UVec2 const currentPos(subPos.x + j, subPos.y + i);
      UVec2 const currentSubPos(j, i);

      subGrid.SetValue(currentSubPos, GetValue(currentPos));
    }
  }

  return subGrid;
}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType> Grid<T, SizeType>::GetSubGrid(UVec2 const & pos, SizeType width, SizeType height)
  const
{
  return GetSubGrid(pos, UVec2(width, height));
}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType> Grid<T, SizeType>::GetSubGrid(SizeType xPos, SizeType yPos, UVec2 const & size)
  const
{
  return GetSubGrid(UVec2(xPos, yPos), size);
}

//##############################################################################
template <typename T, typename SizeType>
Grid<T, SizeType> Grid<T, SizeType>::GetSubGrid(SizeType xPos, SizeType yPos, SizeType width,
  SizeType height) const
{
  return GetSubGrid(UVec2(xPos, yPos), UVec2(width, height));
}

//##############################################################################
template <typename T, typename SizeType>
UVec2 Grid<T, SizeType>::GetPosFromIndex(SizeType index) const
{
  ASSERT(size_.x * size_.y == data_.size());
  ASSERT(index < data_.size());

  return UVec2(index % size_.x, index / size_.x);
}

//##############################################################################
template <typename T, typename SizeType>
SizeType Grid<T, SizeType>::GetIndexFromPos(UVec2 const & pos) const
{
  return GetIndexFromPos(pos.x, pos.y);
}

//##############################################################################
template <typename T, typename SizeType>
SizeType Grid<T, SizeType>::GetIndexFromPos(SizeType xPos, SizeType yPos) const
{
  ASSERT(x < size_.x);
  ASSERT(y < size_.y);

  return y * size_.x + x;
}

//##############################################################################
template <typename T, typename SizeType>
T const & Grid<T, SizeType>::GetValue(UVec2 const & pos) const
{
  return const_cast<Grid<T, SizeType> *>(this)->GetValue(pos);
}

//##############################################################################
template <typename T, typename SizeType>
T const & Grid<T, SizeType>::GetValue(SizeType xPos, SizeType yPos) const
{
  return const_cast<Grid<T, SizeType> *>(this)->GetValue(xPos, yPos);
}

//##############################################################################
template <typename T, typename SizeType>
T const & Grid<T, SizeType>::GetValue(SizeType index) const
{
  return const_cast<Grid<T, SizeType> *>(this)->GetValue(index);
}

//##############################################################################
template <typename T, typename SizeType>
T & Grid<T, SizeType>::GetValue(UVec2 const & pos)
{
  return GetValue(pos.x, pos.y);
}

//##############################################################################
template <typename T, typename SizeType>
T & Grid<T, SizeType>::GetValue(SizeType xPos, SizeType yPos)
{
  ASSERT(xPos < size_.x);
  ASSERT(yPos < size_.y);

  return data_[yPos * size_.x + xPos];
}

//##############################################################################
template <typename T, typename SizeType>
T & Grid<T, SizeType>::GetValue(SizeType index)
{
  ASSERT(size_.x * size_.y == data_.size());
  ASSERT(index < data_.size());

  return data_[index];
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::SetValue(UVec2 const & pos, T const & value)
{
  GetValue(pos) = value;
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::SetValue(SizeType xPos, SizeType yPos, T const & value)
{
  GetValue(xPos, yPos) = value;
}

//##############################################################################
template <typename T, typename SizeType>
void Grid<T, SizeType>::SetValue(SizeType index, T const & value)
{
  GetValue(index) = value;
}

//##############################################################################
template <typename T, typename SizeType>
T const & Grid<T, SizeType>::operator [](SizeType index) const
{
  return GetValue(index);
}

//##############################################################################
template <typename T, typename SizeType>
T & Grid<T, SizeType>::operator [](SizeType index)
{
  return GetValue(index);
}

//##############################################################################
template <typename T, typename SizeType>
typename std::vector<T>::iterator Grid<T, SizeType>::begin(void)
{
  return data_.begin();
}

//##############################################################################
template <typename T, typename SizeType>
typename std::vector<T>::iterator Grid<T, SizeType>::end(void)
{
  return data_.end();
}

//##############################################################################
template <typename T, typename SizeType>
typename std::vector<T>::const_iterator Grid<T, SizeType>::begin(void) const
{
  return data_.begin();
}

//##############################################################################
template <typename T, typename SizeType>
typename std::vector<T>::const_iterator Grid<T, SizeType>::end(void) const
{
  return data_.end();
}

#endif
