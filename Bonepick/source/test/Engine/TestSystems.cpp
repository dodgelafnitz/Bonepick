#include "test/Engine/TestSystems.h"

#include "engine/System/Entity.h"
#include "engine/System/EntityManager.h"
#include "engine/Utility/Debug.h"

namespace
{
  //############################################################################
  void TestEntityManagerSingleComponent(void)
  {
    EntityManager<float> entMan;

    int const ent0   = entMan.AddEntity(2.0f);
    int const ent1   = entMan.AddEntity(3.0f);
    int const nonEnt = (ent0 + 1 == ent1 ? ent1 : ent0) + 1;

    ASSERT(entMan.EntityCount() == 0);

    entMan.Advance();

    ASSERT(entMan.EntityCount() == 2);
    ASSERT(entMan.GetComponent<float>(ent0) == 2.0f);
    ASSERT(entMan.GetComponent<float>(ent1) == 3.0f);

    ASSERT(entMan.ContainsComponent<float>(ent0));
    ASSERT(entMan.ContainsComponent<float>(ent1));
    EXPECT_ERROR(entMan.ContainsComponent<float>(nonEnt););
    EXPECT_ERROR(entMan.GetComponent<float>(nonEnt););

    entMan.SetComponent<float>(ent0, 4.0f);
    entMan.Advance();
    ASSERT(entMan.GetComponent<float>(ent0) == 4.0f);

    entMan.SetComponent<float>(ent0, 2.0f);
    ASSERT(entMan.GetComponent<float>(ent0) == 4.0f);
    EXPECT_ERROR(entMan.SetComponent<float>(ent0, 3.0f););
    entMan.UpdateComponent<float>(ent0) = 3.5f;
    entMan.UpdateComponent<float>(ent1) = 5.5f;

    entMan.Advance();
    ASSERT(entMan.GetComponent<float>(ent0) == 3.5f);
    ASSERT(entMan.GetComponent<float>(ent1) == 5.5f);
  }

  //############################################################################
  void TestEntityManagerMultipleComponents(void)
  {
    EntityManager<float, int, bool> entMan;

    int const ent0 = entMan.AddEntity(4.0f, 2);
    int const ent1 = entMan.AddEntity(false, 2.0f);
    int const ent2 = entMan.AddEntity(7, 3.0f, true);

    ASSERT(entMan.EntityCount() == 0);

    entMan.Advance();

    ASSERT(entMan.EntityCount() == 3);
    ASSERT(entMan.GetComponent<float>(ent0) == 4.0f);

    ASSERT(entMan.ContainsComponent<float>(ent0));
    ASSERT(entMan.ContainsComponent<float>(ent1));
    ASSERT(entMan.ContainsComponent<float>(ent2));
    ASSERT(entMan.ContainsComponent<int>(ent0));
    ASSERT(!entMan.ContainsComponent<int>(ent1));
    ASSERT(entMan.ContainsComponent<int>(ent2));
    ASSERT(!entMan.ContainsComponent<bool>(ent0));
    ASSERT(entMan.ContainsComponent<bool>(ent1));
    ASSERT(entMan.ContainsComponent<bool>(ent2));

    entMan.UpdateComponent<float>(ent1) = 3.5f;
    EXPECT_ERROR(entMan.UpdateComponent<bool>(ent0););

    ASSERT(entMan.GetComponent<float>(ent0) == 4.0f);
    ASSERT(entMan.GetComponent<float>(ent1) == 2.0f);
    ASSERT(entMan.GetComponent<float>(ent2) == 3.0f);
    ASSERT(entMan.GetComponent<int>(ent0) == 2);
    ASSERT(entMan.GetComponent<int>(ent2) == 7);
    ASSERT(entMan.GetComponent<bool>(ent1) == false);
    ASSERT(entMan.GetComponent<bool>(ent2) == true);

    entMan.Advance();

    ASSERT(entMan.GetComponent<float>(ent1) == 3.5f);

    entMan.AddComponent(ent1, 4);
    entMan.RemoveComponent<bool>(ent1);

    entMan.Advance();

    ASSERT(entMan.GetComponent<int>(ent1) == 4);
    ASSERT(!entMan.ContainsComponent<bool>(ent1));
  }

  //############################################################################
  void TestEntityManagerDestruction(void)
  {
    EntityManager<float, int, bool> entMan;

    int const ent0 = entMan.AddEntity(4.0f, 2);
    int const ent1 = entMan.AddEntity(false, 2.0f);
    int const ent2 = entMan.AddEntity(7, 3.0f, true);

    EXPECT_ERROR(entMan.DestroyEntity(ent1););

    entMan.Advance();

    entMan.DestroyEntity(ent1);
    ASSERT(entMan.EntityCount() == 3);

    entMan.Advance();
    ASSERT(entMan.EntityCount() == 2);
    ASSERT(entMan.DoesEntityExist(ent1) == false);
  }

  //############################################################################
  void TestEntityManagerIdGetterHelper(
    EntityManager<float, int, bool> const & entMan)
  {
    Array<int> entityIds;

    for (int i = 0; i < entMan.EntityCount(); ++i)
    {
      ASSERT(!entityIds.Contains(entMan.GetEntityId(i)));
      entityIds.EmplaceBack(entMan.GetEntityId(i));
    }

    ASSERT(entityIds.Size() == entMan.EntityCount());
  }

  //############################################################################
  void TestEntityManagerIdGetter(void)
  {
    EntityManager<float, int, bool> entMan;

    int const ent0 = entMan.AddEntity(4.0f, 2);
    int const ent1 = entMan.AddEntity(false, 2.0f);
    int const ent2 = entMan.AddEntity(7, 3.0f, true);

    entMan.Advance();
    TestEntityManagerIdGetterHelper(entMan);

    entMan.DestroyEntity(ent1);

    entMan.Advance();
    TestEntityManagerIdGetterHelper(entMan);

    int const ent3 = entMan.AddEntity(2.5f, false, 4);

    entMan.Advance();
    TestEntityManagerIdGetterHelper(entMan);

    entMan.DestroyEntity(ent2);
    entMan.DestroyEntity(ent3);
    entMan.DestroyEntity(ent0);

    entMan.Advance();
    TestEntityManagerIdGetterHelper(entMan);
  }

  //############################################################################
  void TestEntityCreation(void)
  {
    EntityManager<float, int> entMan;

    int const entIndex = entMan.AddEntity(2.0f);

    Entity<float, int> ent(entMan, entIndex);

    EXPECT_ERROR(ent.GetComponent<float>(););

    entMan.Advance();

    ASSERT(ent.GetComponent<float>() == 2.0f);
    EXPECT_ERROR(ent.GetComponent<int>(););
  }

  //############################################################################
  void TestEntityManagers(void)
  {
    TestEntityManagerSingleComponent();
    TestEntityManagerMultipleComponents();
    TestEntityManagerDestruction();
  }

  //############################################################################
  void TestEntities(void)
  {
    TestEntityCreation();
  }
}

//##############################################################################
void TestAllSystems(void)
{
  TestEntityManagers();
  TestEntities();
}
