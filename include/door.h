#ifndef __DOOR_H__
#define __DOOR_H__

#include "entity.h"

/**
 * @brief Create a new door entity
 * @param position where to spawn the door at
 * @return NULL on error, or an vase entity pointer on success
 */
Entity *door_new(Vector3D position);

#endif