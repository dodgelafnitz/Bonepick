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

  void AddComponent(void);
  void AddComponent(T const & component);

  int ComponentCount(void) const;

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
  Array<Optional<T>> newData_;
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

  void AddComponents(void);

  template <typename Component, typename ... Remainder>
  void AddBlanks(TypeList<Component, Remainder...> const &);

  void AddBlanks(TypeList<> const &);

  template <typename Component, typename ... Remainder>
  void AdvanceInternal(TypeList<Component, Remainder...> const &);

  void AdvanceInternal(TypeList<> const &);

  UniqueTuple<ComponentManager<Components>...> componentManagers_;
};

//##############################################################################
template <typename T>
void ComponentManager<T>::AddComponent(void)
{
  newData_.EmplaceBack();
}

//##############################################################################
template <typename T>
void ComponentManager<T>::AddComponent(T const & component)
{
  newData_.EmplaceBack(component);
}

//##############################################################################
template <typename T>
int ComponentManager<T>::ComponentCount(void) const
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
    SetComponent(entityId, component);
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

  for (Optional<T> nextData : newData_)
    data_.EmplaceBack(nextData);

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
void EntityManager<Components...>::AddEntity(
  EntityComponents const & ... components)
{
  static_assert(TypeSetIsSubset<
    TypeSet<EntityComponents...>,
    TypeSet<Components...>
  >::value);

  using EntityComponentSet = TypeSet<EntityComponents...>;
  using ComponentSet       = TypeSet<Components...>;

  using EmptyCompSet =
    TypeSetComplement<ComponentSet, EntityComponentSet>::type;

  AddComponents(components...);
  AddBlanks(EmptyCompSet());
}

//##############################################################################
template <typename ... Components>
int EntityManager<Components...>::EntityCount(void) const
{
  if constexpr (sizeof...(Components) == 0)
    return 0;
  else
    return componentManagers_.Get<0>().ComponentCount();
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
void EntityManager<Components...>::UpdateComponent(
  int entityId, U const & component)
{
  componentManagers_.Get<ComponentManager<U>>().UpdateComponent(
    entityId, component);
}

//##############################################################################
template <typename ... Components>
template <typename U>
bool EntityManager<Components...>::ContainsComponent(int entityId) const
{
  return
    componentManagers_.Get<ComponentManager<U>>().ContainsComponent(entityId);
}

//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::Advance()
{
  AdvanceInternal(TypeSet<Components...>());
}

//##############################################################################
template <typename ... Components>
template <typename Component, typename ... Remainder>
void EntityManager<Components...>::AddComponents(Component const & component,
  Remainder const & ... remainder)
{
  componentManagers_.Get<ComponentManager<Component>>().AddComponent(component);

  AddComponents(remainder...);
}

//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::AddComponents(void)
{}

//##############################################################################
template <typename ... Components>
template <typename Component, typename ... Remainder>
void EntityManager<Components...>::AddBlanks(
  TypeList<Component, Remainder...> const &)
{
  componentManagers_.Get<ComponentManager<Component>>().AddComponent();

  AddBlanks(TypeSet<Remainder...>());
}

//##############################################################################
template <typename ... Components>
void EntityManager<Components...>::AddBlanks(TypeList<> const &)
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

#endif
