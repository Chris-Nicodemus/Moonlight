#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"

Entity *enemy_new(Vector3D pos, Entity *passedPlayer, int enemyType);

/**
 * Creates a new enemy at this position based on the number provided also passed a reference to the player
 * */

#endif