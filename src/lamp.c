#include "simple_logger.h"
#include "lamp.h"

void lamp_update(Entity *self);

void lamp_think(Entity *self);

uint32_t lampInterval = 100;
uint32_t musicInterval = 50;
uint32_t musicChange = 0;

//Bool playerIn = false;
Sound *lampCharging;

Mix_Music *music;

Entity *lamp_new(Vector3D position, Entity *player)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no lamp for you!");
        return NULL;
    }

    if(!lampCharging)
    {
        lampCharging = gfc_sound_load("audio/wind-chimes-with-wind-and-light-rain-171624.wav",1,6);
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
    ent->playerIn = false;


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

    if(gfc_point_in_sphere(self->playerEnt->position,self->roundBounds))
    {
        if(!self->playerIn)
        {
            self->playerIn = true;
            musicChange = SDL_GetTicks() + musicInterval;
            Mix_FadeInChannel(6,lampCharging->sound,-1,500);
        }

        if(Mix_VolumeMusic(-1) > 0 && musicChange < SDL_GetTicks())
        {
            musicChange = SDL_GetTicks() + musicInterval;
            Mix_VolumeMusic(Mix_VolumeMusic(-1) - 1);
        }

        if(self->playerEnt->mana < self->playerEnt->manaMax && SDL_GetTicks() > self->lampNextMana)
        {
            self->playerEnt->mana = self->playerEnt->mana + 1;
            self->lampNextMana = SDL_GetTicks() + lampInterval;
        }
    }
    else if(self->playerIn)
    {
        self->playerIn = false;
        Mix_FadeOutChannel(6,1000);
    }

    if(Mix_VolumeMusic(-1) < 32 && !self->playerIn && musicChange < SDL_GetTicks())
    {
        musicChange = SDL_GetTicks() + musicInterval;
        Mix_VolumeMusic(Mix_VolumeMusic(-1) + 1);
    }
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

Bool lamp_give_music(Mix_Music *lampMusic)
{
    music = lampMusic;

    if(music)
    {
        return true;
    }

    return false;
}
/*eol@eof*/