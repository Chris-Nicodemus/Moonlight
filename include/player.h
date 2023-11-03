#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/**
 * @brief Create a new player entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an player entity pointer on success
 */
Entity *player_new(Vector3D position);

/**
 * @brief set player companion variable to passedCompanion
 * @param player player in question
 * @param passedCompanion entity that will be bound to player
 * @return true if successful and false if not
*/
Bool player_getCompanion(Entity *player, Entity *passedCompanion);
#endif
