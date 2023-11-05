#include <stdlib.h>
#include <string.h>

#include "simple_logger.h"

#include "entity.h"

/*typedef struct
{
    Entity *entity_list;
    Uint32  entity_count;
    
}EntityManager;*/

EntityManager entity_manager = {0};

void entity_system_close()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        entity_free(&entity_manager.entity_list[i]);        
    }
    free(entity_manager.entity_list);
    memset(&entity_manager,0,sizeof(EntityManager));
    slog("entity_system closed");
}

void entity_system_init(Uint32 maxEntities)
{
    entity_manager.entity_list = gfc_allocate_array(sizeof(Entity),maxEntities);
    if (entity_manager.entity_list == NULL)
    {
        slog("failed to allocate entity list, cannot allocate ZERO entities");
        return;
    }
    entity_manager.entity_count = maxEntities;
    atexit(entity_system_close);
    slog("entity_system initialized");
}

Entity *entity_new()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet, so we can!
        {
            entity_manager.entity_list[i]._inuse = 1;
            gfc_matrix_identity(entity_manager.entity_list[i].modelMat);
            entity_manager.entity_list[i].scale.x = 1;
            entity_manager.entity_list[i].scale.y = 1;
            entity_manager.entity_list[i].scale.z = 1;
            
            entity_manager.entity_list[i].color = gfc_color8(255,255,255,255);
            entity_manager.entity_list[i].selectedColor = gfc_color8(255,255,255,255);
/*            if(!entity_manager.entity_list[i].gravForce)
            {
                entity_manager.entity_list[i].gravForce = -0.05;
            }*/
            
            return &entity_manager.entity_list[i];
        }
    }
    slog("entity_new: no free space in the entity list");
    return NULL;
}

void entity_free(Entity *self)
{
    if (!self)return;
    //MUST DESTROY
    if(self->type == 2 && self->owner)
    {
        self->owner->flameNum = self->owner->flameNum - 1;
    }
    gf3d_model_free(self->model);
    memset(self,0,sizeof(Entity));
}


void entity_draw(Entity *self)
{
    if (!self)return;
    if (self->hidden)return;
    gf3d_model_draw(self->model,self->modelMat,gfc_color_to_vector4f(self->color),vector4d(1,1,1,1));
    if (self->selected)
    {
        gf3d_model_draw_highlight(
            self->model,
            self->modelMat,
            gfc_color_to_vector4f(self->selectedColor));
    }
}

void entity_draw_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_draw(&entity_manager.entity_list[i]);
    }
}

void entity_think(Entity *self)
{
    if (!self)return;
    if (self->think)self->think(self);
}

void entity_think_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_think(&entity_manager.entity_list[i]);
    }
}


void entity_update(Entity *self)
{
    if (!self)return;
    // HANDLE ALL COMMON UPDATE STUFF
    
    vector3d_add(self->position,self->position,self->velocity);
    vector3d_add(self->velocity,self->acceleration,self->velocity);
    
    gfc_matrix_identity(self->modelMat);
    
    gfc_matrix_scale(self->modelMat,self->scale);
    gfc_matrix_rotate_by_vector(self->modelMat,self->modelMat,self->rotation);
    gfc_matrix_translate(self->modelMat,self->position);
    
    if (self->update)self->update(self);
}

void entity_update_all()
{
    int i;
    for (i = 0; i < entity_manager.entity_count; i++)
    {
        if (!entity_manager.entity_list[i]._inuse)// not used yet
        {
            continue;// skip this iteration of the loop
        }
        entity_update(&entity_manager.entity_list[i]);
    }
}

void entity_gravity(Entity *self)
{
    if(!self)
    {
        slog("gravity failed cuz no self");
    }
    
    self->position.z = self->position.z + self->gravForce;
    

    if(self->position.z < 0)
    {
        self->position.z = 0;
    }
}


Bool entity_checkBox(Entity *self, Box bounds)
{
    if(!self)
    {
        slog("entity checkbox messed up because of self problem");
        return false;
    }
    int i;
    for(i = 0; i < entity_manager.entity_count; i++)
    {
        //slog("%i",i);
        if(!entity_manager.entity_list[i]._inuse)
        {
            continue;
        }

        if(self==&entity_manager.entity_list[i])
        {
        //    slog("skipping");
            continue;
        }

        if(entity_manager.entity_list[i].bounds.h == 0)
        {
            continue;
        }

        if(gfc_box_overlap(bounds, entity_manager.entity_list[i].bounds))
        {
        //    slog("returning true");
            return true;
        }
    }
    return false;
}

Bool entity_checkSphere(Entity *self, Vector3D point)
{
    if(!self)
    {
        slog("entity checksphere messed up cuz of self problem");
    }
    int i;
    for(i = 0; i < entity_manager.entity_count; i++)
    {
        //slog("%i",i);
        if(!entity_manager.entity_list[i]._inuse)
        {
            continue;
        }

        if(self==&entity_manager.entity_list[i])
        {
        //    slog("skipping");
            continue;
        }

        if(entity_manager.entity_list[i].roundBounds.r == 0)
        {
            continue;
        }

        if(gfc_point_in_sphere(point, entity_manager.entity_list[i].roundBounds))
        {
            if(self->player && entity_manager.entity_list[i].lamp)
            {
                continue;
            }
            return true;
        }
    }
    return false;
}

void entity_highlight(Entity *self, Entity *exclude, float radius)
{
    if(!self || !exclude)
    {
        slog("entity highlight messed up cuz of self or exclude problem");
        return;
    }

    int i;
    for(i = 0; i < entity_manager.entity_count; i++)
    {
        if(!entity_manager.entity_list[i]._inuse)
        {
            continue;
        }

        if(self==&entity_manager.entity_list[i] || exclude==&entity_manager.entity_list[i])
        {
        //    slog("skipping");
            continue;
        }

        if(vector3d_magnitude_between(self->position,entity_manager.entity_list[i].position) <= radius)
        {
            entity_manager.entity_list[i].selected = 1;
        }
    }
}

void entity_unhighlight()
{
    int i;
    for(i = 0; i < entity_manager.entity_count; i++)
    {
        if(!entity_manager.entity_list[i]._inuse)
        {
            continue;
        }

        entity_manager.entity_list[i].selected = 0;
    }
}

Entity *entity_find_item(Entity* self, float radius)
{
    if(!self)
    {
        slog("item find did not work because of problem with self");
        return NULL;
    }
    Entity* ent = NULL;
    float entDist = radius;
    float currentDist;
    int i;
    for(i = 0; i < entity_manager.entity_count; i++)
    {
        if(!entity_manager.entity_list[i]._inuse)
        {
            continue;
        }

        //skip if not ent type we lookin for
        if(!entity_manager.entity_list[i].vase)
        {
            continue;
        }

        currentDist = vector3d_magnitude_between(self->position,entity_manager.entity_list[i].position);
        if( currentDist <= radius)
        {
            //first case
            if(ent == NULL)
            {
                ent = &entity_manager.entity_list[i];
                entDist = currentDist;
            }
            else if (currentDist < entDist)
            {
                ent = &entity_manager.entity_list[i];
                entDist = currentDist;
            }
        }
    }

    return ent;
}

void entity_stars(Entity* self, float radius, uint32_t stunDuration)
{
    if(!self)
    {
        slog("entity stars failed cuz no player");
    }

    Entity *toKill[entity_manager.entity_count];
    int toKillLength = 0;

    int i;
    for(i = 0; i < entity_manager.entity_count; i++)
    {
        if(!entity_manager.entity_list[i]._inuse)
        {
            continue;
        }

        //skip if too far
        if(vector3d_magnitude_between(self->position,entity_manager.entity_list[i].position) > radius)
        {
            continue;
        }

        switch(entity_manager.entity_list[i].type)
        {
            case 0:
                continue;
                break;
            case 1:
                entity_manager.entity_list[i].stunDuration = SDL_GetTicks() + stunDuration;
                entity_manager.entity_list[i].stunned = true;
                break;
            case 2:
                toKill[toKillLength] = &entity_manager.entity_list[i];
                toKillLength++;
                break;
            case 3:
                entity_manager.entity_list[i].stunDuration = SDL_GetTicks() + stunDuration;
                entity_manager.entity_list[i].stunned = true;
                break;
            default:
                slog("U messed up the switch for entity stars, punk");
        }
    }

    i = toKillLength - 1;
    for( i = toKillLength - 1; i >= 0; i--)
    {
        entity_free(toKill[i]);
    }
}

void entity_shatter(Entity *self, float radius)
{
    if(!self)
    {
        slog("entity stars failed cuz no player");
    }

    Entity *toKill[entity_manager.entity_count];
    int toKillLength = 0;

    int i;
    for(i = 0; i < entity_manager.entity_count; i++)
    {
        if(!entity_manager.entity_list[i]._inuse)
        {
            continue;
        }

        //skip if too far
        if(vector3d_magnitude_between(self->position,entity_manager.entity_list[i].position) > radius)
        {
            continue;
        }

        if(entity_manager.entity_list[i].type > 0)
        {
            //slog("This is happening");
            toKill[toKillLength] = &entity_manager.entity_list[i];
            //slog("test %f",toKill[toKillLength]->position.x);
            toKillLength++;
        }
    }

    i = toKillLength - 1;
    for( i = toKillLength - 1; i >= 0; i--)
    {
        entity_free(toKill[i]);
    }
}
/*eol@eof*/
