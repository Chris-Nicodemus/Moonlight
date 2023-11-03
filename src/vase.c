#include "simple_logger.h"
#include "vase.h"


void vase_update(Entity *self);

void vase_think(Entity *self);

Entity *vase_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no vase for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/vase.model");
    ent->think = vase_think;
    ent->update = vase_update;
    vector3d_copy(ent->position,position);
    ent->gravForce=-0.05;
    ent->type = 0;

    //ent->bounds = gfc_box(ent->position.x-0.2,ent->position.y-0.2,ent->position.z,2,1,2);
    ent->roundBounds = gfc_sphere(ent->position.x,ent->position.y,ent->position.z, 6);
    ent->bounds = gfc_box(0,0,0,0,0,0);
    ent->scale = vector3d(0.2,0.2,0.2);

    return ent;
}

void vase_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    entity_gravity(self);
    self->roundBounds = gfc_sphere(self->position.x,self->position.y,self->position.z,2);
    //self->rotation.z += 0.01;
}

void vase_think(Entity *self)
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