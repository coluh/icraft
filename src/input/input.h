#ifndef _ICRAFT_input_input_h
#define _ICRAFT_input_input_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include "../player/camera.h"

void input_handle(SDL_Event *event, Camera *camera);
void input_update(Camera *camera);

#endif
