#include "simple_logger.h"
#include "enemy.h"
#include "player.h"

Entity *player = NULL;
float chaseDistance1 = 50;
float speed1 = 0.025;

void enemy_update(Entity *self);

void enemy_think(Entity *self);

Entity *enemy_new(Vector3D pos, Entity *passedPlayer, int enemyType)
{
    Entity *ent = NULL;
    if(passedPlayer && passedPlayer->player)
    {
        player = passedPlayer;
    }
    if(player)
    {
        slog("I have a player!");
    }
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no enemy for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    
    //ent->model = gf3d_model_load("models/kindred.model");
    ent->think = enemy_think;
    ent->update = enemy_update;
    ent->gravForce = -0.05;
    ent->player= false;
    ent->stunned = false;
    ent->aware = false;
    ent->type = enemyType;
    ent->awareThreshold = 0;
    vector3d_copy(ent->position,pos);

    switch (enemyType)
    {
        case 1:
        ent->bounds = gfc_box(ent->position.x,ent->position.y,ent->position.z,5,5,5);
        ent->model = gf3d_model_load("models/centurion.model");
        ent->scale=vector3d(10,10,10);
        ent->roundBounds = gfc_sphere(0,0,0,0);
        ent->awareInterval = 3000;
        ent->chaseDistance = chaseDistance1;
        ent->speed = speed1;
        slog("enemy type 1");
        //ent->velocity.x = 1;
        break;
        default:
        slog("enemy switch");
    }
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
    entity_gravity(self);
    self->bounds = gfc_box(self->position.x,self->position.y,self->position.z,5,5,5);
    //self->rotation.z += 0.01;
}

void enemy_think(Entity *self)
{
    //slog("thinking");
    if (!self)return; 
    if(self->stunned && SDL_GetTicks() > self->stunDuration)
    {
        self->stunned = false;
    }

    if(!self->stunned && !player->invisible)
    {
        if(self->aware)
        {
            Vector3D dir;
            dir = vector3d(player->position.x - self->position.x, player->position.y - self->position.y,0);
            vector3d_normalize(&dir);
            self->velocity.x = dir.x * self->speed;
            self->velocity.y = dir.y * self->speed;

            if(gfc_box_overlap(self->bounds,player->bounds))
            {
                player_touch(player,self,self->type);
            }
        }

        if(vector3d_magnitude_between(self->position,player->position) <= self->chaseDistance)
        {
            if(!self->inRange)
            {
                self->inRange = true;
                self->awareThreshold = SDL_GetTicks() + self->awareInterval;
                slog("In Range!");
            }
        }
        else if(self->inRange || self->aware)
        {
            self->inRange = false;
            self->aware = false;
            self->velocity = vector3d(0,0,0);
        }

        if(self->inRange && !self->aware && SDL_GetTicks() > self->awareThreshold)
        {
            self->aware = true;
            slog("Aware!");
        }
    }

    if((player->invisible || self->stunned) && self->aware)
    {
        self->aware = false;
        self->velocity = vector3d(0,0,0);
    }
}