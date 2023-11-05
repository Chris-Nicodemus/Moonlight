#ifndef __LAMP_H__
#define __LAMP_H__

#include "entity.h"

/**
 * @brief Create a new vase entity
 * @param position where to spawn the lamp at
 * @return NULL on error, or an vase entity pointer on success
 */
Entity *lamp_new(Vector3D position, Entity *player);

#endif
