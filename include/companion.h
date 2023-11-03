#ifndef __COMPANION_H__
#define __COMPANION_H__

#include "entity.h"

/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an companion entity pointer on success
 */
Entity *companion_new(Vector3D position, Entity* passedPlayer);


#endif
