#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_color.h"
#include "gfc_primitives.h"

#include "gf3d_model.h"
#include "gf3d_particle.h"

typedef enum
{
    ES_idle = 0,
    ES_hunt,
    ES_dead,
    ES_attack
}EntityState;


typedef struct Entity_S
{
    Uint8       _inuse;     /**<keeps track of memory usage*/
    Matrix4     modelMat;   /**<orientation matrix for the model*/
    Color       color;      /**<default color for the model*/
    Model      *model;      /**<pointer to the entity model to draw  (optional)*/
    Uint8       hidden;     /**<if true, not drawn*/
    Uint8       selected;
    Color       selectedColor;      /**<Color for highlighting*/
    
    Box         bounds; // for collisions
    int         team;  //same team dont clip
    int         clips;  // if false, skip collisions

    void       (*think)(struct Entity_S *self); /**<pointer to the think function*/
    void       (*update)(struct Entity_S *self); /**<pointer to the update function*/
    void       (*draw)(struct Entity_S *self); /**<pointer to an optional extra draw funciton*/
    void       (*damage)(struct Entity_S *self, float damage, struct Entity_S *inflictor); /**<pointer to the think function*/
    void       (*onDeath)(struct Entity_S *self); /**<pointer to an funciton to call when the entity dies*/
    
    EntityState state;
    
    Vector3D    position;  
    Vector3D    velocity;
    Vector3D    acceleration;
        
    Vector3D    scale;
    Vector3D    rotation;
    
    Uint32      health;     /**<entity dies when it reaches zero*/
    // WHATEVER ELSE WE MIGHT NEED FOR ENTITIES
    struct Entity_S *target;    /**<entity to target for weapons / ai*/
    
    void *customData;   /**<IF an entity needs to keep track of extra data, we can do it here*/

    //my stuff common
    float       gravForce; //force of gravity for entity
    Bool        player;    //true if entity is a player
    struct Entity_S    *playerEnt; //reference to player
    Sphere      roundBounds; //if I wanna use a round hitbox
    Bool        stunned;   //if you are in the stunned state
    uint32_t    stunDuration; //how long till you're un-stunned
    int         type;   //type of enemy you are. If not hostile, type is 0
    float       slowedSpeed;
    float       speed;
    Bool        slowed;
    uint32_t    slowDuration;

    //enemy specific
    Bool        aware;
    Bool        inRange;
    uint32_t    awareInterval;
    uint32_t    awareThreshold;
    float       chaseDistance;
    
    //srite only
    uint32_t    spriteLife;
    Bool        tracking;
    struct Entity_S *owner; //ref to the mage that created the flame sprite
    
    //mage only
    struct Entity_S *flames[6]; //array of flame sprites that the mage can have
    int         flameNum;
    Bool        pinged;
    Vector3D    pingPos;
    uint32_t    flameCD; //cooldown for more tracking flame sprites
    //item specific
    Vector3D    exitPosition; //where you show up when leave a vase
    Bool        vase; //proof that ent is a vase
    Bool        lamp;
    uint32_t    lampNextMana;
    Bool        firework;
    uint32_t    fireworkExplosion;
    Bool        used;

    //player specific
    int manaMax;
    int mana;
    Bool hiding;
    Bool invisible;
    Color shadow;
    Bool starsOn;
    struct Entity_S *companion;
}Entity;

typedef struct EntityManager_S
{
    Entity *entity_list;
    Uint32  entity_count;
}EntityManager;
/**
 * @brief initializes the entity subsystem
 * @param maxEntities the limit on number of entities that can exist at the same time
 */
void entity_system_init(Uint32 maxEntities);

/**
 * @brief provide a pointer to a new empty entity
 * @return NULL on error or a valid entity pointer otherwise
 */
Entity *entity_new();

/**
 * @brief free a previously created entity from memory
 * @param self the entity in question
 */
void entity_free(Entity *self);


/**
 * @brief Draw an entity in the current frame
 * @param self the entity in question
 */
void entity_draw(Entity *self);

/**
 * @brief draw ALL active entities
 */
void entity_draw_all();

/**
 * @brief Call an entity's think function if it exists
 * @param self the entity in question
 */
void entity_think(Entity *self);

/**
 * @brief run the think functions for ALL active entities
 */
void entity_think_all();

/**
 * @brief run the update functions for ALL active entities
 */
void entity_update_all();


/**
 * @brief gravity function meant to pair with update or think
 * @param self the entity in question
*/
void entity_gravity(Entity *self);

/**
 * @brief iterate through entity list to see if entity bounds collides with something else
 * @param self the entity in question
 * @param bounds is the actual box that is checked against the entity list
*/
Bool entity_checkBox(Entity *self, Box bounds);


/**
 * @brief iterate through ent list to see if an entity collides with any sphere hitboxes
 * @param self the entity in question
 * @param point point that is checked against ent list
*/
Bool entity_checkSphere(Entity *self, Vector3D point);

/**
 * @brief highlight all entities except for self and exclude within radius of self
 * @param self entity in question
 * @param exclude entity that will not highlight
 * @param radius distance of things that will highlight
*/
void entity_highlight(Entity *self, Entity *exclude, float radius);

/**
 * @brief iterate through ent list and unhighlight all ents
*/
void entity_unhighlight();

/**
 * @brief iterate through all entities to find closest usable item
 * @param self entity in question
 * @param radius distance where an item is usable
 * @return reference to closest usable item, NULL if none available
*/
Entity *entity_find_item(Entity* self, float radius);

/**
 * @brief iterate through all ents to stun soldiers + commanders and kill fire sprites within range
 * @param self entity in question
 * @param radius range in question
 * @param stunDuration duration of stun, duh
*/
void entity_stars(Entity* self, float radius, uint32_t stunDuration);

/**
 * @brief iterate through all ents and kill any enemies within range
 * @param self entity in question
 * @param radius range in question
*/
void entity_shatter(Entity *self, float radius);
#endif
