#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_types.h"
#include "gfc_config.h"

#include "world.h"

#include "player.h"
#include "enemy.h"
#include "companion.h"
#include "vase.h"
#include "lamp.h"
#include "firework.h"

/*
typedef struct
{

    Model *worldModel;
    List *spawnList;        //entities to spawn
    List *entityList;       //entities that exist in the world
}World;
*/
Entity *makeEnt(World *self, SJson *spawn)
{
    const char *name = sj_get_string_value(sj_object_get_value(spawn,"name"));
    //slog(name);
    Entity *ent = NULL;
    Vector3D pos;
    if(strcmp(name,"player") == 0)
    {
        sj_value_as_vector3d(sj_object_get_value(spawn,"position"),&pos);
        ent = player_new(pos);
        self->player = ent;
        return ent;
    }
    else if(strcmp(name,"companion") == 0)
    {
        sj_value_as_vector3d(sj_object_get_value(spawn,"position"),&pos);
        ent = companion_new(pos,self->player);
        return ent;
    }
    else if(strcmp(name,"enemy") == 0)
    {
        int type;
        sj_get_integer_value(sj_object_get_value(spawn,"type"),&type);
        sj_value_as_vector3d(sj_object_get_value(spawn,"position"),&pos);
        ent = enemy_new(pos,self->player,type);
        return ent;
    }
    else if(strcmp(name,"vase") == 0)
    {
        Vector3D exit;
        sj_value_as_vector3d(sj_object_get_value(spawn,"position"),&pos);
        sj_value_as_vector3d(sj_object_get_value(spawn,"exit"),&exit);
        ent = vase_new(pos);
        ent->exitPosition=exit;
        return ent;
    }
    else if(strcmp(name,"lamp") == 0)
    {
        sj_value_as_vector3d(sj_object_get_value(spawn,"position"),&pos);
        ent = lamp_new(pos,self->player);
        return ent;
    }
    else if(strcmp(name,"firework") == 0)
    {
        sj_value_as_vector3d(sj_object_get_value(spawn,"position"),&pos);
        ent = firework_new(pos);
        return ent;
    }

    return NULL;
}
World *world_load(char *filename)
{
    SJson *json,*wjson,*spawns;
    World *w = NULL;
    const char *modelName = NULL;
    w = gfc_allocate_array(sizeof(World),1);
    if (w == NULL)
    {
        slog("failed to allocate data for the world");
        return NULL;
    }
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load json file (%s) for the world data",filename);
        free(w);
        return NULL;
    }
    wjson = sj_object_get_value(json,"world");
    if (!wjson)
    {
        slog("failed to find world object in %s world condig",filename);
        free(w);
        sj_free(json);
        return NULL;
    }
    modelName = sj_get_string_value(sj_object_get_value(wjson,"model"));
    if (!modelName)
    {
        slog("world data (%s) has no model",filename);
        sj_free(json);
        return w;
    }
    w->model = gf3d_model_load(modelName);

    spawns = sj_object_get_value(json, "spawns");
    if(!spawns)
    {
        slog("didn't work");
    }
    int count,i;
    count = sj_array_get_count(spawns);
    slog("%i",count);
    for(i = 0; i < count; i++)
    {
        makeEnt(w, sj_array_get_nth(spawns,i));
    }
    
    if(w->player)
    {
        slog("You got a player fool");
    }

    sj_value_as_vector3d(sj_object_get_value(wjson,"scale"),&w->scale);
    sj_value_as_vector3d(sj_object_get_value(wjson,"position"),&w->position);
    sj_value_as_vector3d(sj_object_get_value(wjson,"rotation"),&w->rotation);
    sj_free(json);

    w->color = gfc_color(1,1,1,1);
    return w;
}

void world_draw(World *world)
{
    if (!world)return;
    if (!world->model)return;// no model to draw, do nothing
    gf3d_model_draw(world->model,world->modelMat,gfc_color_to_vector4f(world->color),vector4d(2,2,2,2));
    //gf3d_model_draw_highlight(world->worldModel,world->modelMat,vector4d(1,.5,.1,1));
}

void world_delete(World *world)
{
    if (!world)return;
    gf3d_model_free(world->model);
    free(world);
}

void world_run_updates(World *self)
{
    //self->rotation.z += 0.0001;
    gfc_matrix_identity(self->modelMat);
    
    gfc_matrix_scale(self->modelMat,self->scale);
    gfc_matrix_rotate_by_vector(self->modelMat,self->modelMat,self->rotation);
    gfc_matrix_translate(self->modelMat,self->position);

}

void world_add_entity(World *world,Entity *entity);


/*eol@eof*/
