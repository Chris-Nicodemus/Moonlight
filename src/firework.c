#include "simple_logger.h"
#include "firework.h"

float radius = 200;
uint32_t stunDuration = 2000;
void firework_update(Entity *self);

void firework_think(Entity *self);

Entity *firework_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no firework for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/firework.model");
    ent->think = firework_think;
    ent->update = firework_update;
    vector3d_copy(ent->position,position);
    ent->gravForce=-0.05;
    ent->type = 0;
    ent->firework = true;
    ent->used = false;

    ent->bounds = gfc_box(ent->position.x,ent->position.y,ent->position.z,2,1,2);
    ent->roundBounds = gfc_sphere(0,0,0,0);
    //ent->bounds = gfc_box(0,0,0,0,0,0);
    ent->scale = vector3d(5,5,5);

    return ent;
}

void firework_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    entity_gravity(self);
    self->bounds = gfc_box(self->position.x,self->position.y,self->position.z,2,1,2);
    //self->rotation.z += 0.01;
}

void firework_think(Entity *self)
{
    if (!self)return;
    
    if(self->used && self->fireworkExplosion < SDL_GetTicks())
    {
        entity_stars(self,radius,stunDuration);
        entity_free(self);
    }
}