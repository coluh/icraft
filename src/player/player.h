#ifndef _ICRAFT_player_player_h
#define _ICRAFT_player_player_h

#include "camera.h"

typedef struct Player Player;

Player *newPlayer(float x, float y, float z);
Camera *player_getCamera(Player *p);

#endif
