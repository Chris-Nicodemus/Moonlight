#include "simple_logger.h"
#include "enemy.h"

void enemy_update(Entity *self);

void enemy_think(Entity *self);

Entity *enemy_new(Vector3D pos, int enemyType)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no enemy for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/kindred.model");
    ent->think = enemy_think;
    ent->update = enemy_update;
    vector3d_copy(ent->position,pos);
    return ent;
}

void enemy_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    //self->rotation.z += 0.01;
}

void enemy_think(Entity *self)
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