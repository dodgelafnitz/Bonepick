#ifndef ENGINE_SYSTEM_ENTITYMANAGER_H
#define ENGINE_SYSTEM_ENTITYMANAGER_H

#include "utility/containers/Array.h"
#include "utility/containers/Optional.h"
#include "utility/containers/SortedArray.h"
#include "utility/containers/Tuple.h"
#include "utility/TemplateTools.h"

//##############################################################################
template <typename T>
class ComponentManager;

template <typename ... Components>
class ComponentArray;

template <typename ... Components>
class EntityManager;

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

  int GetEntityId(T const * component) const;

  void Advance(void);

  void GetComponents(Array<T const *> * components,
    SortedArray<int> const & filterIds) const;

  void GetAllComponents(Array<T const *> * components,
    SortedArray<int> * entityIds) const;

private:
  struct FutureData
  {
    FutureData(int entityId, T const & component);

    int entityId;
    T   component;
  };

  Array<Optional<T>> data_;
  Array<int>         enitityIds_;
  Array<FutureData>  newData_;
  Array<FutureData>  futureData_;
  Array<int>         enititiesToDestroy_;
  Array<int>         emptyComponentSlots_;
  ArrayMap<int, int> componentIds_;
};

//##############################################################################
template <typename ... EntityComponents>
class ComponentArray
{
public:
  SortedArray<int> const & EntityIds(void) const;

  template <typename U>
  Array<U const *> const & Components(void) const;

  template <typename U>
  void AddComponents(ComponentManager<U> const & compMan);

  void FillWithNulls(void);

private:
  void RemoveEntity(int entityIndex);

  template <typename Component, typename ... Remainder>
  void RemoveEntityInternal(int entityIndex,
    TypeList<Component, Remainder...> const &);

  void RemoveEntityInternal(int entityIndex, TypeList<> const &);

  template <typename Component, typename ... Remainder>
  void FillWithNullsInternal(TypeList<Component, Remainder...> const &);

  void FillWithNullsInternal(TypeList<> const &);

  bool                                            initialized_ = false;
  SortedArray<int>                                entityIds_;
  UniqueTuple<Array<EntityComponents const *>...> components_;
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
  int GetEntityId(U const * component) const;

  template <typename U>
  U const & GetComponent(int entityId) const;

  template <typename U>
  void SetComponent(int entityId, U const & component);

  template <typename U>
  U & UpdateComponent(int entityId);

  template <typename ... EntityComponents>
  ComponentArray<Components ...> GetComponents(void) const;

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
  void GetComponentsInternal(ComponentArray<Components ...> * compArray,
    TypeList<Component, Remainder...> const &) const;

  void GetComponentsInternal(ComponentArray<Components ...> *,
    TypeList<> const &) const;

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
  {
    SetComponent(entityId, GetComponent(entityId));
    return UpdateComponent(entityId);
  }
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
int ComponentManager<T>::GetEntityId(T const * component) const
{
  int const entityIndex = data_.GetIndex(component);
  return enitityIds_[entityIndex];
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
    enitityIds_[componentId] = 0;
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
      enitityIds_[componentId] = newData.entityId;

      componentIds_.Emplace(newData.entityId, componentId);

      ASSERT(componentId < data_.Size());
      ASSERT(componentId >= 0);
      ASSERT(data_[componentId].Ptr() == nullptr);

      data_[componentId].Emplace(newData.component);
    }
    else
    {
      componentIds_.Emplace(newData.entityId, data_.Size());
      enitityIds_.EmplaceBack(newData.entityId);
      data_.EmplaceBack(newData.component);
    }
  }

  newData_.Clear();
}

//##############################################################################
template <typename T>
void ComponentManager<T>::GetComponents(Array<T const *> * components,
  SortedArray<int> const & filterIds) const
{
  ASSERT(components);

  components->Clear();
  components->Resize(filterIds.Size());

  int i = 0;
  int j = 0;

  while (i < filterIds.Size() && j < componentIds_.Size())
  {
    if (filterIds[i] < componentIds_[j].key)
      ++i;
    else if (componentIds_[j].key < filterIds[i])
      ++j;
    else
    {
      (*components)[i] = data_[componentIds_[j].value].Ptr();
      ++i;
      ++j;
    }
  }
}

//##############################################################################
template <typename T>
void ComponentManager<T>::GetAllComponents(Array<T const *> * components,
  SortedArray<int> * entityIds) const
{
  ASSERT(components);
  ASSERT(entityIds);

  components->Clear();
  components->Reserve(componentIds_.Size());
  entityIds->Clear();
  entityIds->Reserve(componentIds_.Size());

  for (int i = 0; i < componentIds_.Size(); ++i)
  {
    entityIds->Emplace(componentIds_[i].key);
    components->EmplaceBack(data_[componentIds_[i].value].Ptr());
  }
}

//##############################################################################
template <typename ... EntityComponents>
SortedArray<int> const & ComponentArray<EntityComponents...>::EntityIds(void)
  const
{
  return entityIds_;
}

//##############################################################################
template <typename ... EntityComponents>
template <typename U>
Array<U const *> const & ComponentArray<EntityComponents...>::Components(void)
  const
{
  return components_.Get<Array<U const *>>();
}

//##############################################################################
template <typename ... EntityComponents>
template <typename U>
void ComponentArray<EntityComponents...>::AddComponents(
  ComponentManager<U> const & compMan)
{
  if (!initialized_)
  {
    compMan.GetAllComponents(&components_.Get<Array<U const *>>(), &entityIds_);
    initialized_ = true;
  }
  else
  {
    Array<U const *> & compArray = components_.Get<Array<U const *>>();
    compMan.GetComponents(&compArray, entityIds_);

    for (int i = entityIds_.Size() - 1; i >= 0; --i)
    {
      if (compArray[i] == nullptr)
        RemoveEntity(i);
    }
  }
}

//##############################################################################
template <typename ... EntityComponents>
void ComponentArray<EntityComponents...>::FillWithNulls(void)
{
  FillWithNullsInternal(TypeList<EntityComponents...>());
}

//##############################################################################
template <typename ... EntityComponents>
void ComponentArray<EntityComponents...>::RemoveEntity(int entityIndex)
{
  entityIds_.Erase(entityIndex);

  RemoveEntityInternal(entityIndex, TypeList<EntityComponents...>());
}

//##############################################################################
template <typename ... EntityComponents>
template <typename Component, typename ... Remainder>
void ComponentArray<EntityComponents...>::RemoveEntityInternal(int entityIndex,
  TypeList<Component, Remainder...> const &)
{
  Array<Component const *> & compArray =
    components_.Get<Array<Component const *>>();

  ASSERT(compArray.Size() > entityIndex || compArray.Empty());

  if (!compArray.Empty())
    compArray.Erase(entityIndex);

  RemoveEntityInternal(entityIndex, TypeList<Remainder...>());
}

//##############################################################################
template <typename ... EntityComponents>
void ComponentArray<EntityComponents...>::RemoveEntityInternal(int,
  TypeList<> const &)
{}

//##############################################################################
template <typename ... EntityComponents>
template <typename Component, typename ... Remainder>
void ComponentArray<EntityComponents...>::FillWithNullsInternal(
  TypeList<Component, Remainder...> const &)
{
  Array<Component const *> & compArray =
    components_.Get<Array<Component const *>>();

  if (compArray.Empty())
    compArray.Resize(entityIds_.Size());

  FillWithNullsInternal(TypeList<Remainder...>());
}

//##############################################################################
template <typename ... Components>
void ComponentArray<Components...>::FillWithNullsInternal(TypeList<> const &)
{}

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
int EntityManager<Components...>::GetEntityId(U const * component) const
{
  return componentManagers_.Get<ComponentManager<U>>().GetEntityId(component);
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
template <typename ... EntityComponents>
ComponentArray<Components ...>
  EntityManager<Components...>::GetComponents(void) const
{
  ComponentArray<Components ...> result;

  GetComponentsInternal(&result, TypeList<EntityComponents...>());

  result.FillWithNulls();

  return result;
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
void EntityManager<Components...>::GetComponentsInternal(
  ComponentArray<Components ...> * compArray,
  TypeList<Component, Remainder...> const &) const
{
  ASSERT(compArray);

  compArray->AddComponents(
    componentManagers_.Get<ComponentManager<Component>>());

  GetComponentsInternal(compArray, TypeList<Remainder...>());
}

//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::GetComponentsInternal(
  ComponentArray<Components ...> *, TypeList<> const &) const
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
