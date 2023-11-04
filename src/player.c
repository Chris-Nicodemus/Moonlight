#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"

static int thirdPersonMode = 1;
void player_think(Entity *self);
void player_update(Entity *self);

Entity *companion = NULL;
//companion highlight ability
uint32_t highlightInterval = 20000;
uint32_t highlightCooldown = 0;
uint32_t highlightDurationInterval = 1000;
uint32_t highlightDuration = 0;
float highlightRadius = 5000;
Bool highlighted = false;

//companion sacrifice ability
uint32_t sacrificeCooldown = 500000;
uint32_t sacrificeRevive = 0;
Bool sacrificed = false;

//item stuff
float itemRadius = 50;
Entity *item = NULL;
//jump ability
uint32_t fall = 0;
int jumpCost = 10;

//shade ability
uint32_t shadeDuration = 0;
uint32_t nextColorTick = 0;
int invisCost = 30;
Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    ent->model = gf3d_model_load("models/child.model");
    ent->think = player_think;
    ent->update = player_update;
    vector3d_copy(ent->position,position);
    ent->rotation.x = -GFC_PI;
    ent->rotation.y = GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    ent->hidden = 0;
    ent->player=true;
    ent->gravForce= -0.05;
    ent->bounds = gfc_box(ent->position.x,ent->position.y,ent->position.z,3,3,3);
    ent->roundBounds = gfc_sphere(0,0,0,0);
    ent->manaMax = 100;
    ent->mana = ent->manaMax;
    ent->type = 0;
    ent->stunned = false;
    ent->hiding = false;
    ent->invisible = false;
    ent->shadow = gfc_color8(114,0,182,179);
    ent->scale = vector3d(7.5,7.5,7.5);
    return ent;
}

Bool testPostion(Entity *self, Vector3D move)
{
    Vector3D testPos = self->position;
    vector3d_add(testPos,testPos,move);
    Box testBox = gfc_box(testPos.x,testPos.y,testPos.z,self->bounds.w,self->bounds.h,self->bounds.h);
    if(testPos.x > 2100) return false;
    if(testPos.x < -2100) return false;
    if(testPos.y > 2100) return false;
    if(testPos.y < -2100) return false;
    if(entity_checkBox(self,testBox)) 
    {
        slog("no move for collision reasons");
        return false;
    }
    if(entity_checkSphere(self,testPos))
    {
        slog("no move cuz circles");
        return false;
    }
    return true;
}

void transitionToShadow(Entity *self)
{
    if(!self)
        return;

    nextColorTick = SDL_GetTicks() + 10;

    if(self->color.r > self->shadow.r)
        self->color.r = self->color.r - 1;

    if(self->color.b > self->shadow.b)
        self->color.b = self->color.b - 1;

    if(self->color.a > self->shadow.a)
        self->color.a = self->color.a - 1;
}

void transitionToWhite(Entity *self)
{
    if(!self)
        return;

    nextColorTick = SDL_GetTicks() + 10;

    if(self->color.r < 255)
        self->color.r = self->color.r + 1;

    if(self->color.g < 255)
        self->color.g = self->color.g + 1;

    if(self->color.b < 255)
        self->color.b = self->color.b + 1;

    if(self->color.a < 255)
        self->color.a = self->color.a + 1;
}
void player_think(Entity *self)
{
    Vector3D forward = {0};
    Vector3D right = {0};
    Vector2D w,mouse;
    int mx,my;
    SDL_GetRelativeMouseState(&mx,&my);
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x * 0.1;
    forward.y = w.y * 0.1;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = w.x * 0.1;
    right.y = w.y * 0.1;

    self->bounds = gfc_box(self->position.x,self->position.y,self->position.z,5,5,5);

    if(self->gravForce != -0.05 && SDL_GetTicks() > fall)
    {
        self->gravForce = self->gravForce - 0.005;
        if(self->gravForce < -0.05)
        {
            self->gravForce = -0.05;
        }
    }

    if(self->invisible)
    {
        if(shadeDuration < SDL_GetTicks())
        {
            self->invisible = false;
            //self->color = gfc_color8(255,255,255,255);
        }

        if(gfc_color_cmp(self->color,self->shadow) == 0 && nextColorTick < SDL_GetTicks())
        {
            transitionToShadow(self);
        }
    }
    
    if(gfc_color_cmp(self->color,gfc_color8(255,255,255,255)) == 0 && !self->invisible)
    {
        transitionToWhite(self);
    }

    if(highlighted && SDL_GetTicks() > highlightDuration)
    {
        highlighted = false;
        entity_unhighlight();
    }

    if(sacrificed && SDL_GetTicks() > sacrificeRevive)
    {
        slog("triggering");
        if(!self->hiding)
        {
            companion->hidden = 0;
        }
        sacrificed = false;
    }


    if(!self->hiding && !self->stunned)
    {
        if (keys[SDL_SCANCODE_W])
        {   
            if(testPostion(self,forward))
                vector3d_add(self->position,self->position,forward);
        }
        if (keys[SDL_SCANCODE_S])
        {
            if(testPostion(self,vector3d(-forward.x,-forward.y,-forward.z)))
                vector3d_add(self->position,self->position,-forward);        
        }
        if (keys[SDL_SCANCODE_D])
        {
            if(testPostion(self,right))
                vector3d_add(self->position,self->position,right);
        }
        if (keys[SDL_SCANCODE_A])    
        {
            if(testPostion(self,vector3d(-right.x,-right.y,-right.z)))
                vector3d_add(self->position,self->position,-right);
        }
        if(gfc_input_command_pressed("jump") && self->mana >= jumpCost && self->position.z == 0)
        {
            slog("position: x:%f y:%f z:%f",self->position.x,self->position.y,self->position.z);
            self->mana = self->mana - jumpCost;
            fall = SDL_GetTicks() + 1000;
            self->gravForce =self->gravForce * -1;
        }

        if(gfc_input_command_pressed("shadow") && !self->invisible && self->mana >= invisCost)
        {
            self->mana = self->mana - invisCost;
            shadeDuration = SDL_GetTicks() + 5000;
            self->invisible = true;
            self->color.g = 0;
        }
    }
    if (keys[SDL_SCANCODE_SPACE])
    {
        //slog("Rotation:\nx: %f\ty: %f\tz: %f",self->rotation.x,self->rotation.y,self->rotation.z);
        //if(entity_checkBox(self,self->bounds)) 
        //    slog("something happening in the best way!");
        //companion->hidden = !companion->hidden;
        //self->color = self->shadow;
    }
    //if (keys[SDL_SCANCODE_Z])self->position.z -= 0.1;
    
    /*if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050;
    if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050;
    if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050;*/
    
    if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);

    /*if (keys[SDL_SCANCODE_F3])
    {
        thirdPersonMode = !thirdPersonMode;
        self->hidden = !self->hidden;
    }*/

    if(gfc_input_command_pressed("highlight"))
    {
        if(companion && !sacrificed)
        {
        //slog("1 is pressed!");
            if(SDL_GetTicks() > highlightCooldown)
            {
                highlighted = true;
                highlightCooldown = SDL_GetTicks() + highlightInterval;

                highlightDuration = SDL_GetTicks() + highlightDurationInterval;

                entity_highlight(self, companion, highlightRadius);
            }
        }
    }

    if(gfc_input_command_pressed("use"))
    {
        if(self->hiding && item)
        {
            self->position = item->exitPosition;
            self->hiding = false;
            self->hidden = 0;
            if(companion && !sacrificed)
            {
                companion->hidden = 0;
            }
            item = NULL;
        }
        else
        {
            item = entity_find_item(self,itemRadius);

            if(item)
            {
                if(item->vase)
                {
                    self->hiding = true;
                    self->hidden = 1;
                    if(companion)
                    {
                        companion->hidden = 1;
                    }
                }
            }
        }
    }

    entity_gravity(self);
}

void player_update(Entity *self)
{
    Vector3D forward = {0};
    Vector3D position;
    Vector3D rotation;
    Vector2D w;
    
    if (!self)return;
    
    vector3d_copy(position,self->position);
    vector3d_copy(rotation,self->rotation);
    if (thirdPersonMode)
    {
        position.z += 40;
        position.y += 10;
        rotation.x += M_PI*0.125;
        w = vector2d_from_angle(self->rotation.z);
        forward.x = w.x * 100;
        forward.y = w.y * 100;
        vector3d_add(position,position,-forward);

        //self->rotation.y = GFC_PI;
        self->rotation.x = -GFC_PI;
    }
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(rotation);
}

Bool player_getCompanion(Entity *player, Entity *passedCompanion)
{
    companion = passedCompanion;
    if(companion)
    {
        return true;
    }
    return false;
}

Bool player_touch(Entity *player, Entity *inflictor, int type)
{
    if(!player || !inflictor)
    {
        slog("player touch failed because missing pointer");
    }

    if(!companion)
    {
        slog("You die!");
        return true;
    }

    if(!sacrificed)
    {
        //companion sacrifice
        sacrificeRevive = SDL_GetTicks() + sacrificeCooldown;
        companion->hidden = 1;
        sacrificed = true;

        if(companion->hidden)
        {
            slog("successfully hidden");
        }
        else
        {
            slog("failure");
        }
        //send enemy backwards and stun
        Vector3D backwards;
        vector3d_add(backwards,inflictor->velocity,inflictor->velocity);
        vector3d_sub(inflictor->position, inflictor->position, backwards);
        inflictor->stunDuration = SDL_GetTicks() + 2000;
        inflictor->stunned = true;
        


        slog("companion save!");
        return false;
    }

    return true;
}
/*eol@eof*/
