#include "simple_logger.h"
#include "key.h"

void key_update(Entity *self);
void key_think(Entity *self);

Bool up = true;
uint32_t transInterval = 10;
uint32_t trans = 0;
Entity *key_new(Vector3D position, Entity *player)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no key for you!");
        return NULL;
    }

    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,0);
    ent->model = gf3d_model_load("models/key.model");
    ent->think = key_think;
    ent->update = key_update;
    vector3d_copy(ent->position,position);
    ent->gravForce= 0;
    ent->type = 0;
    ent->vase = false;
    ent->playerEnt = player;
    ent->key = true;
    ent->playerIn = false;


    //ent->bounds = gfc_box(ent->position.x-0.2,ent->position.y-0.2,ent->position.z,2,1,2);
    ent->roundBounds = gfc_sphere(ent->position.x,ent->position.y,ent->position.z, 12);
    ent->bounds = gfc_box(0,0,0,0,0,0);
    ent->scale = vector3d(7,7,7);
    ent->rotation.y = GFC_HALF_PI * 0.5;
    ent->rotation.x = GFC_HALF_PI * 0.5;

    return ent;
}

void key_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    //entity_gravity(self);
    self->rotation.z = self->rotation.z +0.002;

    //slog("%f",self->position.z);
    if(up && (self->position.z + 0.002) > 12.0)
    {
        up = false;
    }
    if(!up && (self->position.z - 0.002) < 8.0)
    {
        up = true;
    }

    if(up)
    {
        self->position.z = self->position.z + 0.002;
    }
    else
    {
        self->position.z = self->position.z - 0.002;
    }
    self->roundBounds = gfc_sphere(self->position.x,self->position.y,self->position.z,12);

    if(self->playerEnt->invisible && self->color.a < 255 && SDL_GetTicks() > trans)
    {
        trans = SDL_GetTicks() + transInterval;
        self->color.a = self->color.a + 1;
    }
    else if(!self->playerEnt->invisible && self->color.a > 0 && SDL_GetTicks() > trans)
    {
        trans = SDL_GetTicks() + transInterval;
        self->color.a = self->color.a - 1;
    }
}

void key_think(Entity *self)
{
    if (!self)return;
    switch(self->state)
    {
        case ES_idle:
            //look for player
            break;
        case ES_hunt:
            // set move towards player
            break;
        case ES_dead:
            // remove myself from the system
            break;
        case ES_attack:
            // run through attack animation / deal damage
            break;
    }
}