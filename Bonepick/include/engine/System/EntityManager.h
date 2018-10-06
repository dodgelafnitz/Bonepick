#ifndef ENGINE_SYSTEM_ENTITYMANAGER_H
#define ENGINE_SYSTEM_ENTITYMANAGER_H

#include "engine/Utility/Containers/Array.h"
#include "engine/Utility/Containers/Optional.h"
#include "engine/Utility/Containers/Tuple.h"
#include "engine/Utility/TemplateTools.h"

//##############################################################################
template <typename T>
class ComponentManager
{
public:
  ComponentManager(void) = default;
  ComponentManager(ComponentManager const &) = default;
  ComponentManager(ComponentManager &&) = default;

  ~ComponentManager(void) = default;

  void AddEntity(void);
  void AddEntity(T const & component);

  int EntityCount(void) const;

  T const & GetComponent(int entityId) const;
  void SetComponent(int entityId, T const & component);
  void UpdateComponent(int entityId, T const & component);
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
template <typename ... Components>
class EntityManager
{
public:
  EntityManager(void) = default;
  EntityManager(EntityManager const &) = default;
  EntityManager(EntityManager &&) = default;

  ~EntityManager(void) = default;

  template <typename ... EntityComponents>
  void AddEntity(EntityComponents const & ... components);

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
  template <typename Component, typename ... Remainder>
  void AddComponents(Component const & component,
    Remainder const & ... remainder);

  template <typename Component, typename ... Remainder>
  void AddBlanks(Component const & component,
    Remainder const & ... remainder);

  UniqueTuple<ComponentManager<Components>...> componentManagers_;
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
//  UniqueTuple<EntityManager<Components>...>
//    componentManagers_;
//
//};

//##############################################################################
template <typename T>
void ComponentManager<T>::AddEntity(void)
{
  data_.EmplaceBack();
}

//##############################################################################
template <typename T>
void ComponentManager<T>::AddEntity(T const & component)
{
  data_.EmplaceBack(component);
}

//##############################################################################
template <typename T>
int ComponentManager<T>::EntityCount(void) const
{
  return data_.Size();
}

//##############################################################################
template <typename T>
T const & ComponentManager<T>::GetComponent(int entityId) const
{
  return *data_[entityId];
}

//##############################################################################
template <typename T>
void ComponentManager<T>::SetComponent(int entityId, T const & component)
{
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
void ComponentManager<T>::UpdateComponent(int entityId, T const & component)
{
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
bool ComponentManager<T>::ContainsComponent(int entityId) const
{
  ASSERT(entityId < data_.Size());
  ASSERT(entityId >= 0);

  return data_[entityId].Ptr() != nullptr;
}

//##############################################################################
template <typename T>
void ComponentManager<T>::Advance(void)
{
  for (FutureData nextData : futureData_)
    data_[nextData.entityId] = nextData.component;

  futureData_.Clear();
}

//##############################################################################
template <typename T>
ComponentManager<T>::FutureData::FutureData(int entityId, T const & component) :
  entityId(entityId),
  component(component)
{}

//##############################################################################
template <typename ... Components>
template <typename Component, typename ... Remainder>
void EntityManager<Components>::AddEntity(
  EntityComponents const & ... components)
{
  static_assert(TypeIsInTypes<EntityComponents, Components>::value && ...);

  using 

  AddEntityInternal

}

//##############################################################################
template <typename ... Components>
int EntityManager<Components>::EntityCount(void) const
{
}

//##############################################################################
template <typename ... Components>
template <typename U>
U const & EntityManager<Components>::GetComponent(int entityId) const
{
}

//##############################################################################
template <typename ... Components>
template <typename U>
void EntityManager<Components>::SetComponent(int entityId, U const & component)
{
}

//##############################################################################
template <typename ... Components>
template <typename U>
void EntityManager<Components>::UpdateComponent(
  int entityId, U const & component)
{
}

//##############################################################################
template <typename ... Components>
template <typename U>
bool EntityManager<Components>::ContainsComponent(int entityId) const
{
}

#endif
