#ifndef __FIREWORK_H__
#define __FIREWORK_H__

#include "entity.h"

/**
 * @brief Create a new vase entity
 * @param position where to spawn the firework at
 * @return NULL on error, or an vase entity pointer on success
 */
Entity *firework_new(Vector3D position);

#endif
