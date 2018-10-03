#ifndef ENGINE_SYSTEM_ENTITYMANAGER_H
#define ENGINE_SYSTEM_ENTITYMANAGER_H

#include "engine/Utility/Containers/Array.h"
#include "engine/Utility/Containers/Optional.h"
#include "engine/Utility/Containers/TypeSet.h"
#include "engine/Utility/TemplateTools.h"

//##############################################################################
template <typename T>
class EntityManager
{
public:
  EntityManager(void) = default;
  EntityManager(EntityManager const &) = default;
  EntityManager(EntityManager &&) = default;

  ~EntityManager(void) = default;

  void AddEntity(void);
  void AddEntity(T const & component);

  int EntityCount(void) const;

  template <typename U>
  U const & GetComponent(int entityId) const;

  template <typename U>
  void SetComponent(int entityId, U const & component);

  template <typename U>
  void UpdateComponent(int entityId, U const & component);

  template <typename U>
  bool ContainsComponent(int entityId) const;

  void Advance(void);

private:
  struct FutureData
  {
    FutureData(int entityId, T const & component);

    int entityId;
    T   component;
  };

  Array<Optional<T>> data_;
  Array<FutureData>  futureData_;
};

//##############################################################################
//template <typename ... Components>
//class EntityManager
//{
//public:
//  EntityManager(void) = default;
//  EntityManager(EntityManager const &) = default;
//  EntityManager(EntityManager &&) = default;
//
//  ~EntityManager(void) = default;
//
//  void AddEntity(void);
//
//  template <typename ... EntityComponents>
//  void AddEntity(EntityComponents const & ... components);
//
//  int EntityCount(void) const;
//
//  template <typename U>
//  U const & GetComponent(int entityId) const;
//
//  template <typename U>
//  void SetComponent(int entityId, U const & component);
//
//  template <typename U>
//  void UpdateComponent(int entityId, U const & component);
//
//  template <typename U>
//  bool ContainsComponent(int entityId) const;
//
//private:
//  TypeSet<EntityManager<Components>...>
//    componentManagers_;
//
//};

//##############################################################################
template <typename T>
void EntityManager<T>::AddEntity(void)
{
  data_.EmplaceBack();
}

//##############################################################################
template <typename T>
void EntityManager<T>::AddEntity(T const & component)
{
  data_.EmplaceBack(component);
}

//##############################################################################
template <typename T>
int EntityManager<T>::EntityCount(void) const
{
  return data_.Size();
}

//##############################################################################
template <typename T>
template <typename U>
U const & EntityManager<T>::GetComponent(int entityId) const
{
  static_assert(std::is_same_v<T, U>);

  return *data_[entityId];
}

//##############################################################################
template <typename T>
template <typename U>
void EntityManager<T>::SetComponent(int entityId, U const & component)
{
  static_assert(std::is_same_v<T, U>);

  ASSERT(entityId < data_.Size());
  ASSERT(entityId >= 0);

  ASSERT(!futureData_.Contains(
    [&entityId](FutureData const & val)
    {
      return val.entityId == entityId;
    })
  );

  futureData_.EmplaceBack(FutureData(entityId, component));
}

//##############################################################################
template <typename T>
template <typename U>
void EntityManager<T>::UpdateComponent(int entityId, U const & component)
{
  static_assert(std::is_same_v<T, U>);

  ASSERT(entityId < data_.Size());
  ASSERT(entityId >= 0);

  FutureData * futureComponent = futureData_.FindFirst(
    [&entityId](FutureData const & val)
    {
      return val.entityId == entityId;
    });

  if (futureComponent)
    futureComponent->component = component;
  else
    SetComponent<U>(entityId, component);
}

//##############################################################################
template <typename T>
template <typename U>
bool EntityManager<T>::ContainsComponent(int entityId) const
{
  static_assert(std::is_same_v<T, U>);

  ASSERT(entityId < data_.Size());
  ASSERT(entityId >= 0);

  return data_[entityId].Ptr() != nullptr;
}

//##############################################################################
template <typename T>
void EntityManager<T>::Advance(void)
{
  for (FutureData nextData : futureData_)
    data_[nextData.entityId] = nextData.component;

  futureData_.Clear();
}

//##############################################################################
template <typename T>
EntityManager<T>::FutureData::FutureData(int entityId, T const & component) :
  entityId(entityId),
  component(component)
{}

#endif
