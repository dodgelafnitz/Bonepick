#ifndef ENGINE_SYSTEM_ENTITY_H
#define ENGINE_SYSTEM_ENTITY_H

#include "engine/System/EntityManager.h"

//##############################################################################
template <typename ... Components>
class Entity
{
public:
  Entity(EntityManager<Components ...> & entityManager, int entityId);

  template <typename T>
  T const & GetComponent(void) const;

  bool operator ==(Entity const & entity) const;
  bool operator !=(Entity const & entity) const;

private:
  EntityManager<Components ...> & entityManager_;
  int                             entityId_;
};

//##############################################################################
template <typename ... Components>
Entity<Components...>::Entity(
  EntityManager<Components ...> & entityManager,
  int entityId) :
  entityManager_(entityManager),
  entityId_(entityId)
{}

//##############################################################################
template <typename ... Components>
template <typename T>
T const & Entity<Components...>::GetComponent(void) const
{
  return entityManager_.GetComponent<T>(entityId_);
}

//##############################################################################
template <typename ... Components>
bool Entity<Components...>::operator ==(Entity const & entity) const
{
  return &entityManager_ == &entity.entityManager_ &&
    entityId_ == entity.entityId_;
}

//##############################################################################
template <typename ... Components>
bool Entity<Components...>::operator !=(Entity const & entity) const
{
  return !operator ==(entity);
}

//##############################################################################
template <typename ... Components>
Entity<Components...> MakeEntity(
  EntityManager<Components ...> & entityManager, int entityId)
{
  return Entity<Components...>(entityManager, entityId);
}

#endif
