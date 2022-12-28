#include "EgTypes.h"



ECS_DECLARE(Resolution);
ECS_DECLARE(Position);
ECS_DECLARE(Velocity);
ECS_DECLARE(CropPosition);
ECS_DECLARE(CropSize);
ECS_DECLARE(Uses);
ECS_DECLARE(Open);
ECS_DECLARE(OpenTry);
ECS_DECLARE(OpenError);
ECS_DECLARE(Close);
ECS_DECLARE(CloseTry);
ECS_DECLARE(CloseError);
ECS_DECLARE(Status);
ECS_DECLARE(Action);
ECS_DECLARE(Draw);
ECS_DECLARE(Visualize);
ECS_DECLARE(Window);
ECS_DECLARE(Copy);
ECS_COMPONENT_DECLARE(Vec2i32);
ECS_COMPONENT_DECLARE(Vec2f32);

void EgTypesImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgTypes);
	

    ECS_ENTITY_DEFINE(world, Status, Union);
    ECS_TAG_DEFINE(world, Resolution);
    ECS_TAG_DEFINE(world, Position);
    ECS_TAG_DEFINE(world, Velocity);
    ECS_TAG_DEFINE(world, CropPosition);
    ECS_TAG_DEFINE(world, CropSize);
    //ECS_TAG_DEFINE(world, Uses);
    ECS_TAG_DEFINE(world, Open);
    ECS_TAG_DEFINE(world, OpenTry);
    ECS_TAG_DEFINE(world, OpenError);
    ECS_TAG_DEFINE(world, Close);
    ECS_TAG_DEFINE(world, CloseTry);
    ECS_TAG_DEFINE(world, CloseError);
    ECS_TAG_DEFINE(world, Action);
    ECS_TAG_DEFINE(world, Draw);
    ECS_TAG_DEFINE(world, Visualize);
    ECS_TAG_DEFINE(world, Window);
    ECS_ENTITY_DEFINE(world, Uses, EcsAcyclic);
    ECS_ENTITY_DEFINE(world, Copy, EcsAcyclic);
    ECS_COMPONENT_DEFINE(world, Vec2i32);
    ECS_COMPONENT_DEFINE(world, Vec2f32);

}