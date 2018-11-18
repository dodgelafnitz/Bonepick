#include "engine/System/EntityManager.h"
#include "engine/Utility/Math/Vector.h"
#include "engine/Utility/Math/VectorMath.h"
#include "engine/Window/Graphics.h"
#include "engine/Window/Input.h"

#include <algorithm>

namespace
{
  struct LinkData
  {
    LinkData(int previous) :
      prevLinkedId(previous),
      nextLinkedId(0)
    {}

    int prevLinkedId;
    int nextLinkedId;
  };

  struct TransformationData
  {
    Vec2 position;
  };

  struct PhysicsData
  {
    Vec2 lastPos;
  };

  struct ControlData
  {
  };

  using ChainManager =
    EntityManager<
      LinkData,
      TransformationData,
      PhysicsData,
      ControlData
    >;

  Vec2 ImageToWorldPos(AsciiImage const & image, IVec2 const & pos)
  {
    return Vec2(
      (float(pos.x) / image.GetWidth()) * 2.0f - 1.0f,
      (float(pos.y) / image.GetHeight()) * -2.0f + 1.0f
    );
  }

  IVec2 WorldToImagePos(AsciiImage const & image, Vec2 const & pos)
  {
    return IVec2(
      int((pos.x + 1.0f) * 0.5f * image.GetWidth() + 0.5f),
      int((pos.y - 1.0f) * -0.5f * image.GetHeight() + 0.5f)
    );
  }

  void Nop(void) {}
}

//##############################################################################
int main(void)
{
  int const   linkCount        = 5;
  int const   screenWidth      = 100;
  int const   screenHeight     = 100;
  int const   linkCalculations = 1;
  float const linkDist         = 0.3f;
  float const linkDistSqrd     = linkDist * linkDist;
  float const pullStrength     = 0.9f;
  float const dragStrength     = 0.01f;
  float const gravity          = 0.01f;
  bool const  attachBackEnd    = false;
  float const maxSpeed         = 0.1f;

  ChainManager manager;
  bool ending = false;

  int const mouseId =
    manager.AddEntity(
      TransformationData(),
      ControlData()
    );

  int previousLink = 0;
  for (int i = 0; i < linkCount; ++i)
  {
    int const link =
      manager.AddEntity(
        LinkData(previousLink ? previousLink : mouseId),
        TransformationData(),
        PhysicsData()
      );

    if (previousLink)
      manager.UpdateComponent<LinkData>(previousLink).nextLinkedId = link;

    previousLink = link;

    //Can't yet update components that aren't gettable
    manager.Advance();
  }

  if (attachBackEnd)
  {
    manager.AddEntity(
      TransformationData()
    );

    manager.Advance();
  }

  Color palette[NumberOfColors] =
  {
    Color(0, 0, 0),
    Color(255, 255, 255),
  };

  SetColorPalette(palette, NumberOfColors, 0);

  while (!ending)
  {
    //Input!
    while (true)
    {
      InputMessage msg = GetInput();

      switch (msg.button)
      {
      case InputButtonEscape:
        ending = true;
        break;
      }

      if (msg.button == InputButtons)
        break;
    }

    AsciiImageData clearValue(' ', 1, 0);
    AsciiImage screen(screenWidth, screenHeight, clearValue);

    Array<int> entityIds;

    //Might want a way to do this without looping (at least externally)
    for (int i = 0; i < manager.EntityCount(); ++i)
    {
      int const entityId = manager.GetEntityId(i);
      entityIds.EmplaceBack(entityId);
    }

    //Update mouse entities
    auto const mouse = manager.GetComponents<ControlData, TransformationData>();

    for (int i = 0; i < mouse.EntityIds().Size(); ++i)
    {
      TransformationData mouseTransform;
      mouseTransform.position = ImageToWorldPos(screen, GetMousePos());

      manager.SetComponent<TransformationData>(
        mouse.EntityIds()[i], mouseTransform);
    }
    manager.Advance();

    //Apply motion
    auto const motion =
      manager.GetComponents<PhysicsData, TransformationData>();

    for (int i = 0; i < motion.EntityIds().Size(); ++i)
    {
      TransformationData const & transform =
        *motion.Components<TransformationData>()[i];

      PhysicsData const & physics =
        *motion.Components<PhysicsData>()[i];

      TransformationData entityTransform;
      entityTransform.position = (transform.position +
        (transform.position - physics.lastPos)) * (1.0f - dragStrength) +
        transform.position * dragStrength;

      if (VectorLengthSquared(entityTransform.position - transform.position) >
        maxSpeed * maxSpeed)
      {
        Vec2 const dirFromRoot =
          VectorNormalized(entityTransform.position - transform.position);

        entityTransform.position = transform.position + dirFromRoot * maxSpeed;
      }

      PhysicsData entityPhysics;
      entityPhysics.lastPos = transform.position;

      manager.SetComponent<TransformationData>(motion.EntityIds()[i],
        entityTransform);
      manager.SetComponent<PhysicsData>(motion.EntityIds()[i], entityPhysics);
    }
    manager.Advance();

    //Apply gravity
    for (int i = 0; i < motion.EntityIds().Size(); ++i)
    {
      TransformationData const & transform =
        *motion.Components<TransformationData>()[i];

      TransformationData entityTransform;
      entityTransform.position = transform.position + Vec2(0.0f, -gravity);

      manager.SetComponent<TransformationData>(motion.EntityIds()[i],
        entityTransform);
    }
    manager.Advance();

    //Apply chain links
    auto const links =
      manager.GetComponents<LinkData, TransformationData>();

    for (int i = 0; i < linkCalculations; ++i)
    {
      for (int j = 0; j < links.EntityIds().Size(); ++j)
      {
        TransformationData const & transform =
          *links.Components<TransformationData>()[j];

        LinkData const & linkData = *links.Components<LinkData>()[j];

        {
          int linked[2] =
          {
            linkData.prevLinkedId,
            linkData.nextLinkedId
          };

          if (!linked[0])
            std::swap(linked[0], linked[1]);

          if (!linked[0])
            continue;

          if (!linked[1] && !attachBackEnd)
          {
            Vec2 const & linkPos =
              manager.GetComponent<TransformationData>(linked[0]).position;

            Vec2 const diffFromLink = transform.position - linkPos;

            if (VectorLengthSquared(diffFromLink) > linkDistSqrd)
            {
              Vec2 const dirFromLink = VectorNormalized(diffFromLink);

              TransformationData entityTransform;
              entityTransform.position =
                (linkPos + dirFromLink * linkDist) * pullStrength +
                transform.position * (1.0f - pullStrength);

              manager.SetComponent<TransformationData>(links.EntityIds()[j],
                entityTransform);
            }
          }
          else
          {
            Vec2 const linkPos[2] =
            {
              manager.GetComponent<TransformationData>(linked[0]).position,
              linked[1] ?
                manager.GetComponent<TransformationData>(linked[1]).position :
                Vec2()
            };

            Vec2 const diffFromLink[2] =
            {
              transform.position - linkPos[0],
              transform.position - linkPos[1],
            };

            bool const pull[2] =
            {
              VectorLengthSquared(diffFromLink[0]) > linkDistSqrd,
              VectorLengthSquared(diffFromLink[1]) > linkDistSqrd,
            };

            bool const bothPull =
              (VectorLengthSquared(linkPos[0] - linkPos[1]) >
                linkDistSqrd * 4.0f) || (pull[0] && pull[1]);

            if (bothPull)
            {
              Vec2 const targetPos = (linkPos[0] + linkPos[1]) * 0.5f;
              TransformationData entityTransform;
              entityTransform.position =
                targetPos * pullStrength +
                transform.position * (1.0f - pullStrength);

              manager.SetComponent<TransformationData>(links.EntityIds()[j],
                entityTransform);
            }
            else if (pull[0] || pull[1])
            {
              int const pulledLink = pull[0] ? 0 : 1;
              Vec2 const dirFromLink =
                VectorNormalized(diffFromLink[pulledLink]);

              TransformationData entityTransform;
              entityTransform.position =
                (linkPos[pulledLink] + dirFromLink * linkDist) *
                pullStrength + transform.position * (1.0f - pullStrength);

              manager.SetComponent<TransformationData>(links.EntityIds()[j],
                entityTransform);
            }
          }
        }
      }
      manager.Advance();
    }

    //Draw
    auto const transforms =
      manager.GetComponents<TransformationData>();

    for (int i = 0; i < transforms.EntityIds().Size(); ++i)
    {
      TransformationData const & transform =
        *transforms.Components<TransformationData>()[i];

      IVec2 const screenPos = WorldToImagePos(screen, transform.position);

      if (screenPos.x >= 0 && screenPos.x < screen.GetWidth() &&
        screenPos.y >= 0 && screenPos.y < screen.GetHeight())
      {
        char printVal = 'A' + i;

        if (printVal > 'Z')
          printVal = '#';

        screen.SetValue(screenPos, AsciiImageData(printVal, 1, 0));
      }
    }

    screen.Render();
    Sleep(1);
  }

  return 0;
}
