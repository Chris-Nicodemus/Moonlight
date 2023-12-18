#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL.h>            
#include <SDL_mixer.h>

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gfc_audio.h"

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "world.h"
#include "enemy.h"
#include "companion.h"
#include "vase.h"
#include "lamp.h"
#include "firework.h"
#include "villager.h"


extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    //Sprite *mouse = NULL;
    Sprite *moon = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    //World *w;
    //Entity *enemy;
    //Particle particle[100];
    Matrix4 skyMat;
    Model *sky;
    extern Particle stars[300];
    World *forest;

    Entity *player;
    float manaRatio = 0.0;
    
    Bool edit = false;
    Bool custom = false;

    Bool mainMenu = true;
    Color spaceColor = gfc_color8(255,255,255,255);

    uint32_t spaceTextTime = 0;
    uint32_t spaceTextInterval = 50;
    Bool spaceTextUp = false;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }

        if (strcmp(argv[a],"--edit") == 0)
        {
            slog("edit mode");
            edit = true;
            custom = true;
        }

        if (strcmp(argv[a],"--custom") == 0)
        {
            slog("launching pre-built custom map");
            custom = true;
        }
    }
    
    if(edit)
    {
        FILE *custom;
        custom = fopen("config/custom.json","w");

        int type;
        float x,y,z;
        char *input = (char *)malloc(10);
        Bool done = false;
        fprintf(custom,"{\n\t\"world\":\n\t{\n\t\t");
        fprintf(custom,"\"model\":\"models/plain.model\",\n\t\t");
        fprintf(custom,"\"position\":[0,0,0],\n\t\t");
        fprintf(custom,"\"scale\":[1,1,1],\n\t\t");
        fprintf(custom,"\"rotation\":[0,0,0]\n\t},");
        fprintf(custom,"\n\n\t\"spawns\":\n\t[\n\t\t{");
        fprintf(custom,"\n\t\t\t\"name\":\"player\",\n\t\t\t");
        printf("Where would you like the player to be? (All on one line with a space in between)\n");
        scanf("%f %f %f", &x, &y, &z);
        slog("%f",x);
        //printf("y:");
        //scanf("%f", &y);
        slog("%f",y);
        //printf("z:");
        //scanf("%f", &z);
        slog("%f",z);
        fprintf(custom,"\"position\":[%f,%f,%f]\n\t\t}",x,y,z);

        while(!done)
        {
            printf("Enter name of entity to spawn in world, or \"done\" if finished\n");
            scanf("%s",input);
            slog(input);
            if(strcmp(input,"done")==0)
            {
                done = true;
            }
            else if(strcmp(input,"enemy") == 0)
            {
                printf("Enemy type? (1-3)\n");
                scanf("%i", &type);
                printf("Position? (All on one line with a space in between)\n");
                scanf("%f %f %f", &x, &y, &z);
                fprintf(custom,",\n\t\t{\n\t\t\t\"name\":\"enemy\",\n\t\t\t");
                fprintf(custom,"\"type\":\"%i\",\n\t\t\t",type);
                fprintf(custom,"\"position\":[%f,%f,%f]\n\t\t}",x,y,z);
                
            }
            else if(strcmp(input,"vase") == 0)
            {
                printf("Position? (All on one line with a space in between)\n");
                scanf("%f %f %f", &x, &y, &z);
                fprintf(custom,",\n\t\t{\n\t\t\t\"name\":\"vase\",\n\t\t\t");
                fprintf(custom,"\"position\":[%f,%f,%f],\n\t\t\t",x,y,z);
                printf("Exit position? (All on one line with a space in between)\n");
                scanf("%f %f %f", &x, &y, &z);
                fprintf(custom,"\"exit\":[%f,%f,%f]\n\t\t}",x,y,z);
            }
            else if(strcmp(input,"firework") == 0)
            {
                printf("Position? (All on one line with a space in between)\n");
                scanf("%f %f %f", &x, &y, &z);
                fprintf(custom,",\n\t\t{\n\t\t\t\"name\":\"firework\",\n\t\t\t");
                fprintf(custom,"\"position\":[%f,%f,%f]\n\t\t}",x,y,z);
            }
            else if(strcmp(input,"lamp") == 0)
            {
                printf("Position? (All on one line with a space in between)\n");
                scanf("%f %f %f", &x, &y, &z);
                fprintf(custom,",\n\t\t{\n\t\t\t\"name\":\"lamp\",\n\t\t\t");
                fprintf(custom,"\"position\":[%f,%f,%f]\n\t\t}",x,y,z);
            }
            else if(strcmp(input,"companion") == 0)
            {
                printf("Position? (All on one line with a space in between)\n");
                scanf("%f %f %f", &x, &y, &z);
                fprintf(custom,",\n\t\t{\n\t\t\t\"name\":\"companion\",\n\t\t\t");
                fprintf(custom,"\"position\":[%f,%f,%f]\n\t\t}",x,y,z);
            }
        }
        fprintf(custom,"\n\t]\n}");
        fclose(custom);
    }

    init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);
    
    slog_sync();
    
    entity_system_init(1024);
    
    gfc_audio_init(100,32,4,10,1,0);
    Mix_Music *awaken = gfc_sound_load_music("audio/awaken-136824.wav");
    Mix_PlayMusic(awaken, -1);
    Mix_VolumeMusic(32);
    slog("music volume: %i",Mix_VolumeMusic(-1));
    //mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    
//     if (agu)agu->selected = 1;
    //w = world_load("config/testworld.json");
    if(!custom)
    {
        forest = world_load("config/forest.json");
    }
    else
    {
        forest = world_load("config/custom.json");
    }
    player = gfc_list_get_nth(forest->entityList,0);
    lamp_give_music(awaken);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));

    a = 0;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));
    
    // main game loop
    slog("gf3d main loop begin");

    char text[10];

    Entity *villager = villager_new(vector3d(100,150,0));
    extern char* dialogText;
    extern Bool dialog;
    extern char* options[];

    while(!done)
    {
        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);
        
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        if(!mainMenu)
        {
        world_run_updates(forest);
        entity_think_all();
        entity_update_all();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                //world_draw(w);
                world_draw(forest);
                entity_draw_all();
            if(player->starsOn)
            {
                for (a = 0; a < 300; a++)
                {
                    gf3d_particle_draw(&stars[a]);
                }
            }

            if(dialog)
            {
                //gf2d_draw_rect_filled(gfc_rect(350 ,650,1300,64),gfc_color8(128,128,128,255));
                gf2d_font_draw_line_tag(dialogText,FT_H1,gfc_color8(105,25,139,255), vector2d(175,600),1.5);
                if(strcmp(options[0],"") != 0)
                {
                    gf2d_font_draw_line_tag(options[0],FT_H1,gfc_color8(105,25,139,255), vector2d(175,550),1.5);
                    gf2d_font_draw_line_tag(options[1],FT_H1,gfc_color8(105,25,139,255), vector2d(775,550),1.5);
                }
            }
            //2D draws
                gf2d_draw_rect_filled(gfc_rect(10 ,10,1000,32),gfc_color8(128,128,128,255));
                gf2d_font_draw_line_tag("Press q to exit",FT_H1,gfc_color(1,1,1,1), vector2d(10,10),0);
                //gf2d_draw_bezier_curve(Vector2D(0,0), Vector2D(1,1), Vector2D(2,0), Color(0,243,255,1));
                gf2d_draw_rect(gfc_rect(10 ,10,1000,32),gfc_color8(255,255,255,255));
                gf2d_draw_rect_filled(gfc_rect(10,650,(int)(manaRatio)*5,32),gfc_color8(0,245,255,170));
                manaRatio = (float)(player->mana)/(float)(player->manaMax)*100;
                //slog("mana ratio: %f",manaRatio);
                gf2d_draw_rect_filled(gfc_rect(10,650,500,32),gfc_color8(25,25,25,255));
                gf2d_draw_rect_filled(gfc_rect(10,650,(int)(manaRatio)*5,32),gfc_color8(0,245,255,170));    
                sprintf(text,"Mana: %i",player->mana);
                gf2d_font_draw_line_tag(text,FT_H1,gfc_color(1,1,1,1),vector2d(15,650), 0);
            
        }
        else
        {
            gf3d_camera_update_view();
            gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

            gf3d_vgraphics_render_start();
            //2534 1690
            moon = gf2d_sprite_load("images/ismael-gil-forest-night.jpg",0,0,1);
            gf2d_sprite_draw(moon,vector2d(0,0),vector2d(0.7,0.7),vector3d(0,0,0),gfc_color(1,1,1,1),1);

            //gf2d_draw_rect_filled(gfc_rect(10 ,10,1000,32),gfc_color8(128,128,128,255));
            gf2d_font_draw_line_tag("Moonlight",FT_H2,gfc_color(1,1,1,1), vector2d(480,80), 4);
            gf2d_font_draw_line_tag("Press space to begin",FT_H2,spaceColor, vector2d(420,580),3);
            
            if(SDL_GetTicks() > spaceTextTime)
            {
                spaceTextTime = SDL_GetTicks() + spaceTextInterval;
                //slog("triggering");
                if(!spaceTextUp && spaceColor.a - 5 <= 5)
                {
                    //spaceColor.a = spaceColor.a + 1;
                    spaceTextUp = true;
                    //slog("switch up");
                }
                else if(spaceTextUp && spaceColor.a + 5 >= 250)
                {
                    //spaceColor.a = spaceColor.a - 1;
                    spaceTextUp = false;
                    //slog("switch down");
                }

                if(spaceTextUp)
                {
                    spaceColor.a = spaceColor.a + 5;
                }
                else
                {
                    spaceColor.a = spaceColor.a - 5;
                }
                //slog("%f",spaceColor.a);
            }
            
            if(gfc_input_command_pressed("jump"))
            {
                mainMenu = false;
            }
        }
        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))done = 1; // exit condition
    }    
    
    //world_delete(w);
    //entity_free_all();
    world_delete(forest);
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
