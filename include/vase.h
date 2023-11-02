#ifndef __VASE_H__
#define __VASE_H__

#include "entity.h"

/**
 * @brief Create a new vase entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an vase entity pointer on success
 */
Entity *vase_new(Vector3D position);

#endif
