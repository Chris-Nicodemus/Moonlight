#include "simple_logger.h"
#include "door.h"

char *model;
void door_update(Entity *self);

void door_think(Entity *self);

Entity *door_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no door for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/door.model");
    model = "models/door.model";
    ent->think = door_think;
    ent->update = door_update;
    vector3d_copy(ent->position,position);
    ent->gravForce=-0.05;
    ent->type = 0;
    ent->door = true;
    ent->unlocked = false;
    ent->rotation.z = GFC_HALF_PI;

    //ent->bounds = gfc_box(ent->position.x-0.2,ent->position.y-0.2,ent->position.z,2,1,2);
    //ent->roundBounds = gfc_sphere(ent->position.x,ent->position.y,ent->position.z, 4);
    ent->bounds = gfc_box(ent->position.x,ent->position.y,ent->position.z,5,5,1);
    ent->scale = vector3d(5, 5, 5);

    return ent;
}

void door_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    entity_gravity(self);
    self->bounds = gfc_box(self->position.x,self->position.y,self->position.z,5,5,1);
    
    if(self->open && strcmp(model,"models/door.model") == 0)
    {
        self->model = gf3d_model_load("models/doorOpen.model");
        model = "models/doorOpen.model";
        slog("door was opened!");
    }

    if(!self->open && strcmp(model,"models/doorOpen.model") == 0)
    {
        self->model = gf3d_model_load("models/door.model");
        model = "models/door.model";
        slog("door was closed!");
    }
}

void door_think(Entity *self)
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