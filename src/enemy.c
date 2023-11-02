#include "simple_logger.h"
#include "enemy.h"

Entity *player = NULL;
float chaseDistance = 50;
float speed = 0.025;
Bool aware = false;
Bool inRange = false;
uint32_t awareInterval = 3000;
uint32_t awareThreshold = 0;

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
    ent->model = gf3d_model_load("models/kindred.model");
    ent->think = enemy_think;
    ent->update = enemy_update;
    ent->gravForce = -0.05;
    ent->player=false;
    vector3d_copy(ent->position,pos);

    switch (enemyType)
    {
        case 1:
        ent->bounds = gfc_box(ent->position.x,ent->position.y,ent->position.z,5,5,5);
        ent->roundBounds = gfc_sphere(0,0,0,0);
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

    if(aware)
    {
        Vector3D dir;
        dir = vector3d(player->position.x - self->position.x, player->position.y - self->position.y,0);
        vector3d_normalize(&dir);
        self->velocity.x = dir.x * speed;
        self->velocity.y = dir.y * speed;
    }

    if(vector3d_magnitude_between(self->position,player->position) <= chaseDistance)
    {
        if(!inRange)
        {
            inRange = true;
            awareThreshold = SDL_GetTicks() + awareInterval;
            slog("In Range!");
        }
    }
    else if(inRange || aware)
    {
        inRange = false;
        aware = false;
    }

    if(inRange && !aware && SDL_GetTicks() > awareThreshold)
    {
        aware = true;
        slog("Aware!");
    }
    //if(gfc_box_overlap(self->bounds,player->bounds))
    //slog("collision!");
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