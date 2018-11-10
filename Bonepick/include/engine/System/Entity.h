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

#endif
