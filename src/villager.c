#include "simple_logger.h"
#include "villager.h"

extern Bool dialog;
extern char* dialogText;
extern Bool hasKey;
char *firstStep = "Psst! Hey! If you\'re looking for the key, I saw the Mother hide it UNDER THE LAMP using magic!";
char *secondStep = "Second step not implemented";
void villager_update(Entity *self);

void villager_think(Entity *self);

Entity *villager_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no villager for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    ent->model = gf3d_model_load("models/villager.model");
    ent->think = villager_think;
    ent->update = villager_update;
    vector3d_copy(ent->position,position);
    ent->gravForce=-0.05;
    ent->type = 0;
    ent->npc = true;

    //ent->bounds = gfc_box(ent->position.x-0.2,ent->position.y-0.2,ent->position.z,2,1,2);
    ent->roundBounds = gfc_sphere(ent->position.x,ent->position.y,ent->position.z, 4);
    ent->bounds = gfc_box(0,0,0,0,0,0);
    ent->scale = vector3d(12,12,12);

    return ent;
}

void villager_update(Entity *self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    vector3d_add(self->position,self->position,self->velocity);
    entity_gravity(self);
    self->roundBounds = gfc_sphere(self->position.x,self->position.y,self->position.z,4);

    if(dialog)
    {
        if(strcmp(dialogText,"") == 0)
        {
            dialogText = firstStep;
        }

        if(hasKey && strcmp(dialogText,firstStep) == 0)
        {
            dialogText = secondStep;
        }
    }
    
    //self->rotation.z += 0.01;
}

void villager_think(Entity *self)
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