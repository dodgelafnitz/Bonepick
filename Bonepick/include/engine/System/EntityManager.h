#ifndef ENGINE_SYSTEM_ENTITYMANAGER_H
#define ENGINE_SYSTEM_ENTITYMANAGER_H

#include "engine/Utility/Containers/Array.h"
#include "engine/Utility/Containers/Optional.h"
#include "engine/Utility/Containers/SortedArray.h"
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

  void AddComponent(int entityId, T const & component);

  T const & GetComponent(int entityId) const;
  void SetComponent(int entityId, T const & component);
  T & UpdateComponent(int entityId);
  void DestroyComponent(int entityId);
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
  Array<FutureData>  newData_;
  Array<FutureData>  futureData_;
  Array<int>         enititiesToDestroy_;
  Array<int>         emptyComponentSlots_;
  ArrayMap<int, int> componentIds_;
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
  int AddEntity(EntityComponents const & ... components);

  template <typename U>
  void AddComponent(int entityId, U const & component);

  template <typename U>
  void RemoveComponent(int entityId);

  int EntityCount(void) const;
  int GetEntityId(int entityIndex) const;

  template <typename U>
  U const & GetComponent(int entityId) const;

  template <typename U>
  void SetComponent(int entityId, U const & component);

  template <typename U>
  U & UpdateComponent(int entityId);

  void DestroyEntity(int entityId);

  template <typename U>
  bool ContainsComponent(int entityId) const;

  bool DoesEntityExist(int entityId) const;

  void Advance(void);

private:
  int GetNewEntityId(void);

  template <typename Component, typename ... Remainder>
  void AddComponents(int entityId, Component const & component,
    Remainder const & ... remainder);

  void AddComponents(int);

  template <typename Component, typename ... Remainder>
  void AdvanceInternal(TypeList<Component, Remainder...> const &);

  void AdvanceInternal(TypeList<> const &);

  template <typename Component, typename ... Remainder>
  void DestroyInternal(int entityId, TypeList<Component, Remainder...> const &);

  void DestroyInternal(int, TypeList<> const &);

  UniqueTuple<ComponentManager<Components>...> componentManagers_;
  SortedArray<int>                             entityIds_;
  Array<int>                                   newEntityIds_;
  int                                          nextEntityId_       = 1;
  Array<int>                                   enititiesToDestroy_;
};

//##############################################################################
template <typename T>
void ComponentManager<T>::AddComponent(int entityId, T const & component)
{
  ASSERT(!componentIds_.Contains(entityId));
  newData_.EmplaceBack(entityId, component);
}

//##############################################################################
template <typename T>
T const & ComponentManager<T>::GetComponent(int entityId) const
{
  ASSERT(componentIds_.Contains(entityId));

  return *data_[componentIds_.Find(entityId)->value];
}

//##############################################################################
template <typename T>
void ComponentManager<T>::SetComponent(int entityId, T const & component)
{
  ASSERT(componentIds_.Contains(entityId));
  ON_DEBUG(int const componentId = componentIds_.Find(entityId)->value;)

  ASSERT(componentId < data_.Size());
  ASSERT(componentId >= 0);

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
T & ComponentManager<T>::UpdateComponent(int entityId)
{
  ASSERT(componentIds_.Contains(entityId));
  ON_DEBUG(int const componentId = componentIds_.Find(entityId)->value;)

  ASSERT(componentId < data_.Size());
  ASSERT(componentId >= 0);

  FutureData * futureComponent = futureData_.FindFirst(
    [&entityId](FutureData const & val)
    {
      return val.entityId == entityId;
    });

  if (futureComponent)
    return futureComponent->component;
  else if constexpr(std::is_default_constructible_v<T>)
  {
    SetComponent(entityId, T());
    ASSERT(futureData_[futureData_.Size() - 1].entityId == entityId);
    return futureData_[futureData_.Size() - 1].component;
  }
  else
    ERROR_LOG("Can't construct a default object to return.");
}

//##############################################################################
template <typename T>
void ComponentManager<T>::DestroyComponent(int entityId)
{
  if (componentIds_.Contains(entityId))
  {
    ON_DEBUG(int const componentId = componentIds_.Find(entityId)->value;)

    ASSERT(componentId < data_.Size());
    ASSERT(componentId >= 0);

    if (!enititiesToDestroy_.Contains(entityId))
      enititiesToDestroy_.EmplaceBack(entityId);
  }
}

//##############################################################################
template <typename T>
bool ComponentManager<T>::ContainsComponent(int entityId) const
{
  return componentIds_.Contains(entityId);
}

//##############################################################################
template <typename T>
void ComponentManager<T>::Advance(void)
{
  for (FutureData nextData : futureData_)
  {
    ASSERT(componentIds_.Contains(nextData.entityId));
    int const componentId = componentIds_.Find(nextData.entityId)->value;
    data_[componentId] = nextData.component;
  }

  futureData_.Clear();

  for (int entityId : enititiesToDestroy_)
  {
    ASSERT(componentIds_.Contains(entityId));
    auto * component = componentIds_.Find(entityId);
    int const componentId = component->value;
    data_[componentId].Clear();
    emptyComponentSlots_.EmplaceBack(componentId);
    componentIds_.Erase(component);
  }

  enititiesToDestroy_.Clear();

  for (FutureData newData : newData_)
  {
    if (!emptyComponentSlots_.Empty())
    {
      int const componentId =
        emptyComponentSlots_[emptyComponentSlots_.Size() - 1];
      emptyComponentSlots_.PopBack();

      componentIds_.Emplace(newData.entityId, componentId);

      ASSERT(componentId < data_.Size());
      ASSERT(componentId >= 0);
      ASSERT(data_[componentId].Ptr() == nullptr);

      data_[componentId].Emplace(newData.component);
    }
    else
    {
      componentIds_.Emplace(newData.entityId, data_.Size());
      data_.EmplaceBack(newData.component);
    }
  }

  newData_.Clear();
}

//##############################################################################
template <typename T>
ComponentManager<T>::FutureData::FutureData(int entityId, T const & component) :
  entityId(entityId),
  component(component)
{}

//##############################################################################
template <typename ... Components>
template <typename ... EntityComponents>
int EntityManager<Components...>::AddEntity(
  EntityComponents const & ... components)
{
  static_assert(TypeSetIsSubset<
    TypeSet<EntityComponents...>,
    TypeSet<Components...>
  >::value);

  static_assert(sizeof...(EntityComponents) > 0);

  int const entityId = GetNewEntityId();

  AddComponents(entityId, components...);

  return entityId;
}

//##############################################################################
template <typename ... Components>
template <typename U>
void EntityManager<Components...>::AddComponent(int entityId,
  U const & component)
{
  AddComponents(entityId, component);
}

//##############################################################################
template <typename ... Components>
template <typename U>
void EntityManager<Components...>::RemoveComponent(int entityId)
{
  DestroyInternal(entityId, TypeList<U>());
}

//##############################################################################
template <typename ... Components>
int EntityManager<Components...>::EntityCount(void) const
{
  return entityIds_.Size();
}

//##############################################################################
template <typename ... Components>
int EntityManager<Components...>::GetEntityId(int entityIndex) const
{
  ASSERT(entityIndex < entityIds_.Size());
  ASSERT(entityIndex >= 0);

  return entityIds_[entityIndex];
}

//##############################################################################
template <typename ... Components>
template <typename U>
U const & EntityManager<Components...>::GetComponent(int entityId) const
{
  return componentManagers_.Get<ComponentManager<U>>().GetComponent(entityId);
}

//##############################################################################
template <typename ... Components>
template <typename U>
void
  EntityManager<Components...>::SetComponent(int entityId, U const & component)
{
  componentManagers_.Get<ComponentManager<U>>().SetComponent(
    entityId, component);
}

//##############################################################################
template <typename ... Components>
template <typename U>
U & EntityManager<Components...>::UpdateComponent(int entityId)
{
  return
    componentManagers_.Get<ComponentManager<U>>().UpdateComponent(entityId);
}


//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::DestroyEntity(int entityId)
{
  ASSERT(entityIds_.Contains(entityId));

  if (!enititiesToDestroy_.Contains(entityId))
  {
    enititiesToDestroy_.EmplaceBack(entityId);

    DestroyInternal(entityId, TypeSet<Components...>());
  }
}

//##############################################################################
template <typename ... Components>
template <typename U>
bool EntityManager<Components...>::ContainsComponent(int entityId) const
{
  ASSERT(entityIds_.Contains(entityId));

  return
    componentManagers_.Get<ComponentManager<U>>().ContainsComponent(entityId);
}

//##############################################################################
template <typename ... Components>
bool EntityManager<Components...>::DoesEntityExist(int entityId) const
{
  return entityIds_.Contains(entityId);
}

//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::Advance()
{
  for (int entityId : enititiesToDestroy_)
  {
    ASSERT(entityIds_.Contains(entityId));
    entityIds_.Erase(entityIds_.Find(entityId));
  }

  enititiesToDestroy_.Clear();

  entityIds_.Reserve(entityIds_.Size() + newEntityIds_.Size());

  for (int entityId : newEntityIds_)
    entityIds_.Emplace(entityId);

  newEntityIds_.Clear();

  AdvanceInternal(TypeSet<Components...>());
}

//##############################################################################
template <typename ... Components>
int EntityManager<Components...>::GetNewEntityId(void)
{
  if (nextEntityId_ == 0)
    ++nextEntityId_;

  newEntityIds_.EmplaceBack(nextEntityId_);

  return nextEntityId_++;
}

//##############################################################################
template <typename ... Components>
template <typename Component, typename ... Remainder>
void EntityManager<Components...>::AddComponents(int entityId,
  Component const & component, Remainder const & ... remainder)
{
  componentManagers_.Get<ComponentManager<Component>>().AddComponent(entityId,
    component);

  AddComponents(entityId, remainder...);
}

//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::AddComponents(int)
{}

//##############################################################################
template <typename ... Components>
template <typename Component, typename ... Remainder>
void EntityManager<Components...>::AdvanceInternal(
  TypeList<Component, Remainder...> const &)
{
  componentManagers_.Get<ComponentManager<Component>>().Advance();

  AdvanceInternal(TypeSet<Remainder...>());
}

//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::AdvanceInternal(TypeList<> const &)
{}

//##############################################################################
template <typename ... Components>
template <typename Component, typename ... Remainder>
void EntityManager<Components...>::DestroyInternal(int entityId,
  TypeList<Component, Remainder...> const &)
{
  componentManagers_.Get<ComponentManager<Component>>().DestroyComponent(
    entityId);

  DestroyInternal(entityId, TypeSet<Remainder...>());
}

//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::DestroyInternal(int, TypeList<> const &)
{}

#endif
