#ifndef __VILLAGER_H__
#define __VILLAGER_H__

#include "entity.h"

/**
 * @brief Create a new villager entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an vase entity pointer on success
 */
Entity *villager_new(Vector3D position);

#endif