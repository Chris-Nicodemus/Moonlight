#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "gf3d_particle.h"
#include "player.h"

static int thirdPersonMode = 1;
void player_think(Entity *self);
void player_update(Entity *self);

//Entity *companion = NULL;
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
int sacrificeCap = 3;
int currentSacrifice = 0;
Bool sacrificed = false;

//item stuff
float itemRadius = 50;
Entity *item = NULL;
uint32_t fireworkFuse = 1500;

//jump ability
uint32_t fall = 0;
int jumpCost = 10;

//shade ability
uint32_t shadeDuration = 0;
uint32_t nextColorTick = 0;
int invisCost = 30;

//dash ability
Bool dash = false;
uint32_t dashCooldown = 0;
uint32_t dashCDInterval = 500;
int dashCost = 5;

//starry night ability
Particle stars[300];
int starCost = 40;
uint32_t starCooldown = 0;
uint32_t starsOff = 0;
uint32_t starCDInterval = 20000;
uint32_t starLife = 2000;
float starRadius = 200;

//shatter ability
float shatterRadius;
float shatterMultiplier = 5;
float shatterMaxCost = 50;

//sounds
Bool walking = false;
Sound *walkSound;
Sound *invisIn;
Sound *invisOut;
Sound *dashSound;
Sound *starSound;
Sound *jump;
Sound *glass;

//Dialog Tree
Bool dialog = false;
char *dialogText = "";
Bool hasKey = false;
char *options[2];
Bool newDialog = true;
Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }

    if(!walkSound)
    {
       walkSound = gfc_sound_load("audio/613260__thy-sfx__grass-running.wav",1,0);
       if(walkSound)
       {
           gfc_sound_play(walkSound,-1,0,0,0);
           Mix_Volume(0,0);
           //slog("playing");
       }
    }

    if(!invisIn)
    {
        invisIn = gfc_sound_load("audio/234799__richerlandtv__magic-pass-by.wav",1,1);
    }

    if(!invisOut)
    {
        invisOut = gfc_sound_load("audio/168180__speedenza__whoosh-woow-mk3.wav",1,1);
    }

    if(!dashSound)
    {
        dashSound = gfc_sound_load("audio/dash.wav",1,2);
    }

    if(!jump)
    {
        jump = gfc_sound_load("audio/zapsplat_magic_impact_hard_002_32007.wav",1,4);
    }

    if(!glass)
    {
        glass = gfc_sound_load("audio/zapsplat_impacts_short_whoosh_into_heavy_glass_and_material_smash_62177.wav",1,4);
    }

    if(!starSound)
    {
        starSound = gfc_sound_load("audio/zapsplat_sound_design_twinkle_mysterious_metallic_chime_003_48023.wav",1,5);
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
    ent->starsOn = false;
    ent->speed = 0.1;
    ent->slowedSpeed = 0.025;
    ent->slowed = false;
    ent->shadow = gfc_color8(114,0,182,179);
    ent->scale = vector3d(7.5,7.5,7.5);

    int i;
    for(i = 0; i < 300; i++)
    {
        stars[i].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),0);
        stars[i].size = gfc_random() * 30;
    }


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

    if(!self->slowed)
    {
        forward.x = w.x * self->speed;
        forward.y = w.y * self->speed;
    }
    else
    {
        forward.x = w.x * self->slowedSpeed;
        forward.y = w.y * self->slowedSpeed;
    }
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    if(!self->slowed)
    {
        right.x = w.x * self->speed;
        right.y = w.y * self->speed;
    }
    else
    {
        right.x = w.x * self->slowedSpeed;
        right.y = w.y * self->slowedSpeed;
    }
    if(dash && (vector3d_magnitude(forward) > 0 || vector3d_magnitude(right) > 0) )
    {
        //slog("forward, %f right, %f",vector3d_magnitude(forward),vector3d_magnitude(right));
        dash = false;
        forward.x = forward.x * 500;
        forward.y = forward.y * 500;
        right.x = right.x * 500;
        right.y = right.y * 500;
    }

    self->bounds = gfc_box(self->position.x,self->position.y,self->position.z,5,5,5);

    if(self->gravForce != -0.05 && SDL_GetTicks() > fall)
    {
        self->gravForce = self->gravForce - 0.005;
        if(self->gravForce < -0.05)
        {
            self->gravForce = -0.05;
        }
    }
    if(self->starsOn && starsOff < SDL_GetTicks())
    {
        int i;
        for(i = 0; i < 300; i++)
        {
            stars[i].color.a = 0;
        }
        self->starsOn = false;
    }

    if(self->invisible)
    {
        if(shadeDuration < SDL_GetTicks())
        {
            self->invisible = false;
            gfc_sound_play(invisOut,0,1,1,0);
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

    if(self->slowed && SDL_GetTicks() > self->slowDuration)
    {
        self->slowed = false;
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
            self->companion->hidden = 0;
        }
        sacrificed = false;
    }

    walking = false;

    if(!self->hiding && !self->stunned)
    {
        if(!dialog)
        {
        if (keys[SDL_SCANCODE_W])
        {   
            if(testPostion(self,forward))
            {
                vector3d_add(self->position,self->position,forward);
                walking = true;
            }
        }
        if (keys[SDL_SCANCODE_S])
        {
            if(testPostion(self,vector3d(-forward.x,-forward.y,-forward.z)))
            {
                vector3d_add(self->position,self->position,-forward);        
                walking = true;
            }
        }
        if (keys[SDL_SCANCODE_D])
        {
            if(testPostion(self,right))
            {
                vector3d_add(self->position,self->position,right);
                walking = true;
            }
        }
        if (keys[SDL_SCANCODE_A])    
        {
            if(testPostion(self,vector3d(-right.x,-right.y,-right.z)))
            {
                vector3d_add(self->position,self->position,-right);
                walking = true;
            }
        }
        }

        if(!walking)
        {
            Mix_Volume(0,0);
        }
        else
        {
            Mix_Volume(0,128);
        }

        if(!dialog)
        {
        if(gfc_input_command_pressed("jump") && self->mana >= jumpCost && self->position.z == 0)
        {
            //slog("position: x:%f y:%f z:%f",self->position.x,self->position.y,self->position.z);
            gfc_sound_play(jump,0,1,3,0);
            self->mana = self->mana - jumpCost;
            fall = SDL_GetTicks() + 1000;
            self->gravForce =self->gravForce * -1;
        }

        if(gfc_input_command_pressed("shadow") && !self->invisible && self->mana >= invisCost)
        {
            gfc_sound_play(invisIn,0,0.2,1,0);
            self->mana = self->mana - invisCost;
            shadeDuration = SDL_GetTicks() + 5000;
            self->invisible = true;
            self->color.g = 0;
        }

        if(gfc_input_command_pressed("dash") && self->position.z == 0 && !dash && dashCooldown < SDL_GetTicks() && self->mana >= dashCost)
        {
            gfc_sound_play(dashSound,0,0.5,2,0);
            dash = true;
            self->mana = self->mana - dashCost;
            dashCooldown = SDL_GetTicks() + dashCDInterval;
        }

        if(gfc_input_command_pressed("star") && starCooldown < SDL_GetTicks() && self->mana >= starCost)
        {
            gfc_sound_play(starSound,0,1,5,0);
            self->mana = self->mana - starCost;
            int i;
            for(i = 0; i < 300; i++)
            {
                float x,y,z;
                x = self->position.x + (gfc_crandom() * starRadius);
                y = self->position.y + (gfc_crandom() * starRadius);
                z = self->position.z + (gfc_crandom() * 50);
                stars[i].position = vector3d(x,y,z);
                stars[i].color.a = 1;
            }
            starCooldown = SDL_GetTicks() + starCDInterval;
            starsOff = SDL_GetTicks() + starLife;
            self->starsOn = true;

            entity_stars(self,starRadius,2000);
        }

        if(gfc_input_command_pressed("shatter") && self->mana > 0)
        {
            gfc_sound_play(glass,0,1,4,0);
            if(self->mana >= shatterMaxCost)
            {
                shatterRadius = shatterMaxCost * shatterMultiplier;
                self->mana = self->mana - shatterMaxCost;
            }
            else
            {
                shatterRadius = self->mana * shatterMultiplier;
                self->mana = 0;
            }

            entity_shatter(self,shatterRadius);
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
        if(self->companion && !sacrificed)
        {
        //slog("1 is pressed!");
            if(SDL_GetTicks() > highlightCooldown)
            {
                highlighted = true;
                highlightCooldown = SDL_GetTicks() + highlightInterval;

                highlightDuration = SDL_GetTicks() + highlightDurationInterval;

                entity_highlight(self, self->companion, highlightRadius);
            }
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
            if(self->companion && !sacrificed)
            {
                self->companion->hidden = 0;
            }
            item = NULL;
        }
        else if (dialog && strcmp(options[0],"") == 0)
        {
            dialog = false;
            newDialog = true;
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
                    if(self->companion)
                    {
                        self->companion->hidden = 1;
                    }
                    self->position = item->position;
                }
                else if(item->firework && !item->used)
                {
                    item->fireworkExplosion = SDL_GetTicks() + fireworkFuse;
                    item->used = true;
                }
                else if(item->npc)
                {
                    dialog = true;
                }
                else if(item->door)
                {
                    if(!item->unlocked && hasKey)
                    {
                        item->unlocked = true;
                    }

                    if(item->unlocked)
                    {
                        item->open = !item->open;
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
    player->companion = passedCompanion;
    if(player->companion)
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

    switch (type)
    {
        case 1:
            if(!player->companion)
            {
                slog("You die!");
                return true;
            }

            if(!sacrificed)
            {
                //companion sacrifice
                sacrificeRevive = SDL_GetTicks() + sacrificeCooldown;
                player->companion->hidden = 1;
                sacrificed = true;
                currentSacrifice = 0;

                if(player->companion->hidden)
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
            break;
        case 2:
            if(!player->companion)
            {
                slog("slowed!");
                return true;
            }

            if(!sacrificed)
            {
                currentSacrifice++;
                if(currentSacrifice == sacrificeCap)
                {
                    //companion sacrifice
                    sacrificeRevive = SDL_GetTicks() + sacrificeCooldown;
                    player->companion->hidden = 1;
                    sacrificed = true;
                    currentSacrifice = 0;

                    if(player->companion->hidden)
                    {
                        slog("successfully hidden");
                    }
                    else
                    {
                        slog("failure");
                    }
                }

                return false;
            }
            else
            {
                return true;
            }
            break;
        case 3:
            if(!player->companion)
            {
                slog("You die!");
                return true;
            }

            if(!sacrificed)
            {
                //companion sacrifice
                sacrificeRevive = SDL_GetTicks() + sacrificeCooldown;
                player->companion->hidden = 1;
                sacrificed = true;
                currentSacrifice = 0;

                if(player->companion->hidden)
                {
                    slog("successfully hidden");
                }
                else
                {
                    slog("failure");
                }
                //send enemy backwards and stun
                //Vector3D backwards;
                //vector3d_add(backwards,inflictor->velocity,inflictor->velocity);
                //vector3d_sub(inflictor->position, inflictor->position, backwards);
                inflictor->velocity = vector3d(0,0,0);
                inflictor->stunDuration = SDL_GetTicks() + 2000;
                inflictor->stunned = true;
        
                slog("companion save!");
                return false;
            }
            return true;
            break;
    }
    
    return false;
}

Vector3D player_getRight(Entity *player)
{
    Vector3D right = {0};
    Vector2D w,mouse;
    int mx,my;
    SDL_GetRelativeMouseState(&mx,&my);
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(player->rotation.z - GFC_HALF_PI);

    right.x = w.x;
    right.y = w.y;
    return right;
}
/*eol@eof*/
