#include "test/Engine/TestSystems.h"

#include "engine/System/EntityManager.h"
#include "engine/Utility/Debug.h"

namespace
{
  //############################################################################
  void TestEntityManagerSingleComponent(void)
  {
    EntityManager<float> entMan;

    entMan.AddEntity();
    entMan.AddEntity(3.0f);

    ASSERT(entMan.EntityCount() == 2);
    ASSERT(entMan.GetComponent<float>(1) == 3.0f);

    ASSERT(!entMan.ContainsComponent<float>(0));
    ASSERT(entMan.ContainsComponent<float>(1));
    EXPECT_ERROR(entMan.ContainsComponent<float>(2););

    EXPECT_ERROR(entMan.GetComponent<float>(0););
    EXPECT_ERROR(entMan.GetComponent<float>(3););

    entMan.SetComponent<float>(0, 4.0f);
    entMan.Advance();
    ASSERT(entMan.GetComponent<float>(0) == 4.0f);

    entMan.SetComponent<float>(0, 2.0f);
    ASSERT(entMan.GetComponent<float>(0) == 4.0f);
    EXPECT_ERROR(entMan.SetComponent<float>(0, 3.0f););
    entMan.UpdateComponent<float>(0, 3.5f);
    entMan.UpdateComponent<float>(1, 5.5f);

    entMan.Advance();
    ASSERT(entMan.GetComponent<float>(0) == 3.5f);
    ASSERT(entMan.GetComponent<float>(1) == 5.5f);
  }

  //############################################################################
  void TestEntityManagerMultipleComponents(void)
  {
    //EntityManager<float, int, bool> entMan;
    //
    //entMan.AddEntity(4.0f, 2);
    //entMan.AddEntity(false, 2.0f);
    //entMan.AddEntity(7, 3.0f, true);


  }

  //############################################################################
  void TestEntityManagers(void)
  {
    TestEntityManagerSingleComponent();
    TestEntityManagerMultipleComponents();
  }
}

//##############################################################################
void TestAllSystems(void)
{
  TestEntityManagers();
}
