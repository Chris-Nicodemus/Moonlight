#include "simple_logger.h"
#include "lamp.h"

uint32_t lampInterval = 100;

void lamp_update(Entity *self);

void lamp_think(Entity *self);

Entity *lamp_new(Vector3D position, Entity *player)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no lamp for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/lamp.model");
    ent->think = lamp_think;
    ent->update = lamp_update;
    vector3d_copy(ent->position,position);
    ent->gravForce=-0.05;
    ent->type = 0;
    ent->vase = false;
    ent->playerEnt = player;
    ent->lamp = true;
    ent->lampNextMana = 0;


    //ent->bounds = gfc_box(ent->position.x-0.2,ent->position.y-0.2,ent->position.z,2,1,2);
    ent->roundBounds = gfc_sphere(ent->position.x,ent->position.y,ent->position.z, 12);
    ent->bounds = gfc_box(0,0,0,0,0,0);
    ent->scale = vector3d(2,2,2);

    return ent;
}

void lamp_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    entity_gravity(self);
    self->roundBounds = gfc_sphere(self->position.x,self->position.y,self->position.z,12);

    if(self->playerEnt->mana < self->playerEnt->manaMax && gfc_point_in_sphere(self->playerEnt->position,self->roundBounds) && SDL_GetTicks() > self->lampNextMana)
    {
        self->playerEnt->mana = self->playerEnt->mana + 1;
        self->lampNextMana = SDL_GetTicks() + lampInterval;
    }

    //self->rotation.z += 0.01;
}

void lamp_think(Entity *self)
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

/*eol@eof*/