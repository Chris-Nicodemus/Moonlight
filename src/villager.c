#include "simple_logger.h"
#include "gfc_input.h"
#include "villager.h"

float safeDistance = 1000;

extern Bool dialog;
extern char* dialogText;
extern Bool hasKey;
extern char* options[];
extern Bool newDialog;
char *hello = "Hey Kid! I\'m hurt! Can you lure those soldiers away from me? I\'ll help you if you do!";
char *accept = "Thanks! Come back to me when they\'re gone!";
char *deny = "But... Please!";
char *reconsider = "Did you reconsider? Will you help me? Please?";
char *distractReminder = "I won\'t tell you where the key is until those soldiers are gone!";
char *thanks = "You got rid of them! Thanks! The key is hidden UNDER THE LAMP, but you need magic to see it!";
char *keyReminder = "Psst! Hey! If you\'re looking for the key, I saw the Mother hide it UNDER THE LAMP using magic!";
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
            dialogText = hello;
            options[0] = "1. I\'ll help you!";
            options[1] = "2. Sorry, it\'s too dangerous!";
        }

        if(strcmp(dialogText, hello) == 0)
        {
            if(gfc_input_command_pressed("shadow"))
            {
                dialogText = accept;
                options[0] = "";
                options[1] = "";
                newDialog = false;
            }

            if(gfc_input_command_pressed("star"))
            {
                dialogText = deny;
                options[0] = "";
                options[1] = "";
                newDialog = false;
            }
        }

        if(newDialog && strcmp(dialogText,deny) == 0)
        {
            dialogText = reconsider;
            options[0] = "1. Fine... I\'ll help.";
            options[1] = "2. Sorry, I can\'t.";
        }

        if(strcmp(dialogText, reconsider) == 0)
        {
            if(gfc_input_command_pressed("shadow"))
            {
                dialogText = accept;
                options[0] = "";
                options[1] = "";
                newDialog = false;
            }

            if(gfc_input_command_pressed("star"))
            {
                dialogText = deny;
                options[0] = "";
                options[1] = "";
                newDialog = false;
            }
        }

        if(newDialog && (strcmp(dialogText,accept) == 0 || strcmp(dialogText,distractReminder) == 0))
        {
            if(entity_enemies_in_radius(self, safeDistance))
            {
                dialogText = distractReminder;
                newDialog = false;
            }
            else
            {
                dialogText = thanks;
                newDialog = false;
            }
        }

        if(newDialog && strcmp(dialogText,thanks) == 0)
        {
            dialogText = keyReminder;
        }
        
        if(hasKey)
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