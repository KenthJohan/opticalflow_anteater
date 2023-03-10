#include "EgTensors.h"
#include "EgTypes.h"
#include "tensor.h"
#include <stdio.h>


void Observer_Mat_EcsOnAdd(ecs_iter_t *it)
{
    Tensor2_U8C3 *mat = ecs_field(it, Tensor2_U8C3, 1);
    ecs_os_memset_n(mat, 0, Tensor2_U8C3, it->count);
}

void Observer_Mat_EcsOnSet(ecs_iter_t *it)
{
    Tensor2_U8C3 *mat = ecs_field(it, Tensor2_U8C3, 1);
    for(int i = 0; i < it->count; ++i)
    {
        char const * name = ecs_get_name(it->world, it->entities[i]);
        printf("Observer_Mat_EcsOnSet: %s\n", name);
        mat[i].step[1] = 3; // Three bytes per pixel
        int32_t reqsize = mat[i].shape[0] * mat[i].shape[1] * mat[i].step[1];
        if(reqsize <= 0){continue;}
        if(mat[i].size == reqsize){continue;}

        //printf("%s: Reqsize %i\n", name, reqsize);
        ecs_os_free(mat[i].memory);
        mat[i].memory = ecs_os_malloc(reqsize);
        mat[i].start = mat[i].memory;
        mat[i].size = reqsize;
        mat[i].step[0] = mat[i].shape[1] * mat[i].step[1];

    }
}

void Observer_MatResolution_EcsOnSet(ecs_iter_t *it)
{
    Vec2i32 *res = ecs_field(it, Vec2i32, 1);
    for(int i = 0; i < it->count; ++i)
    {
        Tensor2_U8C3 * mat = ecs_get(it->world, it->entities[i], Tensor2_U8C3);
        mat_allocate(mat, res->x, res->y);
    }
}

void System_Mat_Copy_Instruction(ecs_iter_t *it)
{
    Tensor2_U8C3  * src         = ecs_field(it, Tensor2_U8C3, 1); //Shared
    Tensor2_U8C3  * dst         = ecs_field(it, Tensor2_U8C3, 2); //Shared
    Vec2i32 const * pos_field   = ecs_field(it, Vec2i32, 3); //Shared?
    int             pos_self    = ecs_field_is_self(it, 3); // Shared?
    Vec2i32 const * area_field  = ecs_field(it, Vec2i32, 4); //Shared?
    int             area_self   = ecs_field_is_self(it, 4); // Shared?
    if(src->start == NULL) {return;}
    char const * name1 = ecs_get_name(it->world, ecs_field_src(it, 1));
    char const * name2 = ecs_get_name(it->world, ecs_field_src(it, 2));
    //printf("Copy: %s %s %i\n", name1, name2, it->count);
    for(int i = 0; i < it->count; ++i)
    {
        Vec2i32 const *pos = pos_field + i*pos_self;
        Vec2i32 const *area = area_field + i*area_self;
        mat_copy_region_auto_allocation(src, dst, pos, area);
    }
}









void EgTensorsImport(ecs_world_t *world)
{
    ECS_MODULE(world, EgTensors);
    ECS_IMPORT(world, EgTypes);


    ECS_OBSERVER(world, Observer_MatResolution_EcsOnSet, EcsOnSet, (Vec2i32, eg.types.Resolution), eg.types.Window);
    ECS_OBSERVER(world, Observer_Mat_EcsOnSet, EcsOnSet, Tensor2_U8C3);
    ECS_OBSERVER(world, Observer_Mat_EcsOnAdd, EcsOnAdd, Tensor2_U8C3);

    
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "System_Mat_Copy_Instruction",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .query.filter.instanced = true,
        .query.filter.terms = {
            {.id = ecs_id(Tensor2_U8C3), .inout = EcsIn, .src.flags = EcsUp, .src.trav = CopyFrom},
            {.id = ecs_id(Tensor2_U8C3), .inout = EcsInOut, .src.flags = EcsUp, .src.trav = CopyTo},
            {.id = ecs_pair(ecs_id(Vec2i32), Position), .inout = EcsIn },
            {.id = ecs_pair(ecs_id(Vec2i32), Area), .inout = EcsIn }
        },
        .callback = System_Mat_Copy_Instruction
    });


    //ECS_SYSTEM(world, System_Memory_Copy, EcsOnUpdate, Vec2i32(up(eg.types.Copy), eg.types.Resolution), Memory);

}