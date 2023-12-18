#ifndef __key_H__
#define __key_H__

#include "entity.h"

/**
 * @brief Create a new key entity
 * @param position where to spawn the key at
 * @param player is the player that it tracks
 * @return NULL on error, or an vase entity pointer on success
 */
Entity *key_new(Vector3D position, Entity *player);

#endif