#include "simple_logger.h"
#include "enemy.h"
#include "player.h"

Entity *player = NULL;
float chaseDistance1 = 150;
uint32_t awareInterval1 = 2000;
float speed1 = 0.025;
float speed2 = 0.08;
uint32_t slowDuration = 4500;
uint32_t spriteLifeInterval = 3500; //amount of time it take for sprite to die out

float chaseDistance2 = 200;
float trackingDistance = 600;
uint32_t awareInterval2 = 1000;
uint32_t flameCDInterval = 10000;
float flameSpawnRadius = 20;

Sound *flameSpawnSound;
void enemy_update(Entity *self);

void enemy_think(Entity *self);

void sprite_think(Entity *self);

void mage_think(Entity *self);

Entity *enemy_new(Vector3D pos, Entity *passedPlayer, int enemyType)
{
    Entity *ent = NULL;
    if(passedPlayer && passedPlayer->player)
    {
        player = passedPlayer;
    }
    /*if(player)
    {
        slog("I have a player!");
    }*/
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no enemy for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1,1,0.1,1.0);
    ent->color = gfc_color(1,1,1,1);
    
    //ent->model = gf3d_model_load("models/kindred.model");
    ent->update = enemy_update;
    ent->player= false;
    ent->stunned = false;
    ent->aware = false;
    ent->type = enemyType;
    ent->awareThreshold = 0;
    ent->inRange = false;
    vector3d_copy(ent->position,pos);

    switch (enemyType)
    {
        case 1:
            ent->think = enemy_think;
            ent->gravForce = -0.05;
            ent->bounds = gfc_box(ent->position.x,ent->position.y,ent->position.z,5,5,5);
            ent->model = gf3d_model_load("models/centurion.model");
            ent->scale=vector3d(10,10,10);
            ent->roundBounds = gfc_sphere(0,0,0,0);
            ent->awareInterval = awareInterval1;
            ent->chaseDistance = chaseDistance1;
            ent->speed = speed1;
            slog("enemy type 1");
        //ent->velocity.x = 1;
            break;
        case 2:
            ent->think = sprite_think;
            ent->gravForce = 0;
            ent->bounds = gfc_box(ent->position.x,ent->position.y,ent->position.z,0.5,0.5,0.5);
            ent->scale = vector3d(3,3,3);
            ent->model = gf3d_model_load("models/fireSprite.model");
            ent->tracking = false;
            ent->speed = speed2;
            ent->spriteLife = SDL_GetTicks() + spriteLifeInterval;

            if(!flameSpawnSound)
            {
                flameSpawnSound = gfc_sound_load("audio/zapsplat_nature_fire_flames_blow_hard_very_short_001_90299.wav",1,7);
            }
            break;
        case 3:
            ent->think = mage_think;
            ent->gravForce = -0.05;
            ent->scale = vector3d(3,3,3);
            ent->bounds = gfc_box(ent->position.x,ent->position.y,ent->position.z,5,5,5);
            ent->model = gf3d_model_load("models/pyroClastia.model");
            ent->roundBounds = gfc_sphere(0,0,0,0);
            ent->flameNum = 0;
            ent->awareInterval = awareInterval2;
            ent->speed = speed1;
            ent->chaseDistance = chaseDistance2;
            ent->pinged = false;
            ent->flameCD = 0;
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
    switch(self->type)
    {
        case 1:
            self->bounds = gfc_box(self->position.x,self->position.y,self->position.z,5,5,5);
            break;
        case 2:
            self->bounds = gfc_box(self->position.x,self->position.y,self->position.z,0.5,0.5,0.5);
            break;
        case 3:
            self->bounds = gfc_box(self->position.x,self->position.y,self->position.z,5,5,5);
            break;
    }
    
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

    if(!self->stunned && !player->invisible && !player->hiding)
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

    if((player->invisible || self->stunned || player->hiding) && self->aware)
    {
        self->aware = false;
        self->velocity = vector3d(0,0,0);
    }
}

void ping_mage (Entity *owner, Vector3D pos)
{
    if(!owner)
    {
        return;
    }
    if(!owner->aware)
    {
        owner->pingPos = pos;
        owner->pinged = true;
    }
}

void sprite_think(Entity *self)
{
    if(!self) return;

    if(self->spriteLife < SDL_GetTicks())
    {
        entity_free(self);
    }

    if(!self->tracking) return;

    Vector3D dir;
    dir = vector3d(player->position.x - self->position.x, player->position.y - self->position.y,player->position.z + 3 - self->position.z);
    vector3d_normalize(&dir);
    self->velocity.x = dir.x * self->speed;
    self->velocity.y = dir.y * self->speed;
    self->velocity.z = dir.z * self->speed;

    if(gfc_box_overlap(self->bounds,player->bounds))
    {
        if(player_touch(player,self,self->type))
        {
            player->slowDuration = SDL_GetTicks() + slowDuration;
            player->slowed = true;
        }
        if(self->owner)
        {
            ping_mage(self->owner, self->position);
        }
        entity_free(self);
    }
}

void mage_think(Entity *self)
{
    if (!self)return; 

    if(self->stunned)
    {
        self->velocity = vector3d(0,0,0);
    }
    if(self->stunned && SDL_GetTicks() > self->stunDuration)
    {
        self->stunned = false;
    }

    if(!self->stunned) 
    {
        if(self->flameNum == 0 && SDL_GetTicks() > self->flameCD && vector3d_magnitude_between(self->position,player->position) <= trackingDistance)
        {
            int i;
            for(i = 0; i < 6; i++)
            {
                float x,y,z;
                x = self->position.x + (gfc_crandom() * flameSpawnRadius);
                y = self->position.y + (gfc_crandom() * flameSpawnRadius);
                z = self->position.z + (gfc_random() + 2 * 8);
                Vector3D pos = vector3d(x,y,z);
                self->flames[i] = enemy_new(pos, player, 2);
                self->flames[i]->owner = self;
                self->flames[i]->tracking = true;
                self->flameNum = self->flameNum + 1;
                self->flameCD = SDL_GetTicks() + flameCDInterval;
                //gfc_sound_play(flameSpawnSound,0,1,i+7,0);
            }
            gfc_sound_play(flameSpawnSound,0,1,7,0);
        }

        if(self->pinged && !self->aware)
        {
            //slog("moving because of pings");
            Vector3D dir = vector3d(self->pingPos.x - self->position.x, self->pingPos.y - self->position.y,0);
            vector3d_normalize(&dir);
            self->velocity.x = dir.x * self->speed;
            self->velocity.y = dir.y * self->speed;
        }
        
        if(self->pinged && vector3d_magnitude_between(self->position,player->position) <= self->chaseDistance && !player->invisible && !player->hiding)
        {
            slog("This is triggering");
            self->inRange = true;
            self->aware = true;
            self->pinged = false;
        }

        if(self->pinged && vector3d_magnitude_between(self->position,self->pingPos) <= 5 && !self->aware)
        {
            slog("stopping because of this");
            self->velocity = vector3d(0,0,0);
            self->pinged = false;
        }
        
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
        //slog("getting here");
        if(!self->aware && vector3d_magnitude_between(self->position,player->position) <= self->chaseDistance)
        {
            //slog("distance: %f valid distance: %f", vector3d_magnitude_between(self->position,player->position), self->chaseDistance);
            if(!self->inRange)
            {
                self->inRange = true;
                self->awareThreshold = SDL_GetTicks() + self->awareInterval;
                slog("In Range!");
            }
        }
        else if(vector3d_magnitude_between(self->position,player->position) > self->chaseDistance && (self->inRange || self->aware))
        {
            self->inRange = false;
            self->aware = false;
            if(!self->pinged)
            {
                self->velocity = vector3d(0,0,0);
            }
        }

        if(self->inRange && !self->aware && SDL_GetTicks() > self->awareThreshold)
        {
            self->aware = true;
            slog("Aware!");
        }
    }
}