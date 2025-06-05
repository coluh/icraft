#ifndef _ICRAFT_entity_player_operation_h
#define _ICRAFT_entity_player_operation_h

#define PLAYER_DROP_SPEED 5.0f

typedef struct Entity Entity;
typedef struct World World;
typedef struct Item Item;
void player_destroying(Entity *player, World *world);
void player_place(Entity *player, World *world);

void player_pickup(Entity *self, Item *item);
void player_drop(Entity *self);

#endif
