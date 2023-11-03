
#include "simple_logger.h"
#include "companion.h"

Entity *companionPlayer = NULL;

void companion_update(Entity *self);

void companion_think(Entity *self);

Entity *companion_new(Vector3D position, Entity *passedPlayer)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no companion for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/kindredHead.model");
    ent->think = companion_think;
    ent->update = companion_update;
    ent->player = false;
    ent->scale = vector3d(5,5,5);
    companionPlayer = passedPlayer;
    vector3d_copy(ent->position,position);
    ent->gravForce = 0;
    ent->type = 0;
    return ent;
}

void companion_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    //entity_gravity(self);
    self->position.x = companionPlayer->position.x -5;
    self->position.y = companionPlayer->position.y -5;
    self->position.z = companionPlayer->position.z + 10;

    self->rotation.x = companionPlayer->rotation.x * -1;
    self->rotation.y = companionPlayer->rotation.y * -1;
    //self->rotation.z += 0.01;
}

void companion_think(Entity *self)
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
