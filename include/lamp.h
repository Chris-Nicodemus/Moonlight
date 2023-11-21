#ifndef __LAMP_H__
#define __LAMP_H__

#include "gfc_audio.h"
#include "entity.h"

/**
 * @brief Create a new vase entity
 * @param position where to spawn the lamp at
 * @return NULL on error, or an vase entity pointer on success
 */
Entity *lamp_new(Vector3D position, Entity *player);


/**
 * @brief gives player a reference to the music it has to play when it is time do so
 * @param lampMusic is the music being given
 * @return true if it worked
*/
Bool lamp_give_music(Mix_Music *lampMusic);
#endif
